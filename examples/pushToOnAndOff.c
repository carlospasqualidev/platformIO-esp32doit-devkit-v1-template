#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define LED_PIN GPIO_NUM_2
#define BUTTON_PIN GPIO_NUM_15

const static char *TAG = "TESTE BOTAO: ";

void app_main(void)
{
    bool button_state = 1;
    bool i = 0;

    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    gpio_reset_pin(BUTTON_PIN);
    gpio_set_direction(BUTTON_PIN, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_PIN, GPIO_PULLUP_ONLY);

    while (true)
    {
        int new_state = gpio_get_level(BUTTON_PIN);

        if (new_state != button_state)
        {
            button_state = new_state;
            if (button_state == 0)
            {
                gpio_set_level(LED_PIN, i ^= 1);
                ESP_LOGI(TAG, "Botao apertado");
            }
            else
            {
                ESP_LOGI(TAG, "Botao solto");
            }
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
