#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#define LED1_PIN 25
#define ADC_CHANNEL 0 // Potenciômetro conectado ao ADC1_CH0 (GPIO 36)

// Função para configurar o PWM
void configure_led_pwm()
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

void app_main(void)
{
    configure_led_pwm();

    // Configurar ADC para leitura do potenciômetro
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN_DB_11);

    while (true)
    {
        // Ler valor do ADC (potenciômetro)
        int adc_value = adc1_get_raw(ADC_CHANNEL);
        printf("adc: %d\n", adc_value);

        // Mapear o valor do ADC para o ciclo de trabalho do PWM (0 a 8191)
        int duty = adc_value * (8191 / 4095);

        // Ajustar o brilho do LED
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);

        // mostrar o valor de duty no serial monitor
        printf("Duty: %d\n", duty);

        vTaskDelay(100 / portTICK_PERIOD_MS); // Delay para suavizar a leitura
    }
}

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#define LED1_PIN 25
#define ADC_CHANNEL 0 // Potenciômetro conectado ao ADC1_CH0 (GPIO 36)

// Função para configurar o PWM
void configure_led_pwm()
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

// nova void app main que lê o valor do potenciômetro conectado a um pino adc do esp32 e ajusta o brilho do LED e o valor lido deve ser mapeado para variar o duty cycle do pwm dos leds

void app_main(void)
{
    // lê o valor do potenciômetro conectado a um pino adc do esp32 e ajusta o brilho do LED e o valor lido deve ser mapeado para variar o duty cycle do pwm dos leds

    configure_led_pwm();

    // Configurar ADC para leitura do potenciômetro
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN_DB_11);

    // Calibrar ADC
    esp_adc_cal_characteristics_t *adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, adc_chars);

    while (1)
    {
        // Ler valor do potenciômetro
        uint32_t adc_reading = 0;
        for (int i = 0; i < 32; i++)
        {
            adc_reading += adc1_get_raw(ADC_CHANNEL);
        }
        adc_reading /= 32;

        // Mapear valor lido para o duty cycle do PWM
        uint32_t duty = (adc_reading * 8191) / 4095;
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    }

    free(adc_chars);

    vTaskDelay(1000 / portTICK_PERIOD_MS);
}
