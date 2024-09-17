#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED_PIN GPIO_NUM_2
#define BUTTON_PIN GPIO_NUM_15

bool debounce(int pin, unsigned long delay)
{
    bool currentState = gpio_get_level(BUTTON_PIN);

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

void app_main(void)
{
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    gpio_reset_pin(BUTTON_PIN);
    gpio_set_direction(BUTTON_PIN, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_PIN, GPIO_PULLUP_ONLY);

    while (1)
    {

        gpio_set_level(LED_PIN, debounce(BUTTON_PIN, 20) ? 0 : 1);

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}