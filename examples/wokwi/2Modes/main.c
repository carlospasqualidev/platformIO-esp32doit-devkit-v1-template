#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#define LED1_PIN 25
#define LED2_PIN 26
#define ADC_CHANNEL 0
#define BUTTON 27

void configure_led_pwm1()
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = LEDC_TIMER_13_BIT,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK};
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
        .gpio_num = LED1_PIN,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0};
    ledc_channel_config(&ledc_channel);
}

void configure_led_pwm2()
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_1,
        .duty_resolution = LEDC_TIMER_13_BIT,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK};
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
        .gpio_num = LED2_PIN,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_1,
        .timer_sel = LEDC_TIMER_1,
        .duty = 0,
        .hpoint = 0};
    ledc_channel_config(&ledc_channel);
}

bool debounce(int pin, unsigned long delay)
{
    bool currentState = gpio_get_level(pin);

    static unsigned long lastTime = 0;
    static bool lastState = 0;
    unsigned long currentMillis = xTaskGetTickCount() * portTICK_PERIOD_MS;

    if (currentState != lastState)
    {
        lastTime = 0;
    }

    if ((currentMillis - lastTime) > delay)
    {
        lastState = currentState;
    }

    return lastState;
}

void mode1()
{
    int adc_value = adc1_get_raw(ADC_CHANNEL);

    int duty = adc_value * (8191 / 4095);
    duty = duty / 32;
    printf("Duty 1: %d\n", duty);

    // LED 1
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);

    int inverseDuty = 255 - duty;
    printf("Duty 2: %d\n", inverseDuty);

    // LED 2
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, inverseDuty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
}


void mode2(){
  // Aumenta o brilho do LED gradualmente
        for (int duty = 0; duty <= 8191; duty += 256) {
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
            vTaskDelay(pdMS_TO_TICKS(30));
        }

        // Diminui o brilho do LED gradualmente
        for (int duty = 8191; duty >= 0; duty -= 256) {
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, duty);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
            vTaskDelay(pdMS_TO_TICKS(30));
        }
}


void app_main(void)
{
    configure_led_pwm1();
    configure_led_pwm2();

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN_DB_11);

    bool buttonState = 1;
    int mode = 0;

    while (true)
    {

        // o botão precisa ficar pressionado por um momento
        int newButtonState = debounce(BUTTON, 50);
        if (newButtonState == 0 && newButtonState != buttonState)
        {
            mode = !mode;
        }
    
        printf("Mode: %d\n", mode);

        switch (mode)
        {
        case 0:
            mode1();
            break;

        case 1:
            mode2();
            break;

        default:
            break;
        }
        buttonState = newButtonState;
        vTaskDelay(100 / portTICK_PERIOD_MS); 
    }
}
