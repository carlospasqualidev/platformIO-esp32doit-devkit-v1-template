#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "bmp180.h"

// CARLOS PASQUALI, HENRIQUE JOCHEM
#include "ssd1306.h"

static const char *TAG = "BMP180 I2C Read";

#define REFERENCE_PRESSURE 101325l

// SENSOR
#define I2C_PIN_SDA 21
#define I2C_PIN_SCL 22

float altitude;
float temperature;
char lineChar[20];

// OLED
#define OLED_SDA_GPIO 21   // define the SDA GPIO
#define OLED_SCL_GPIO 22   // define the SCL GPIO
#define OLED_RESET_GPIO -1 // define the RESET GPIO
// OLED CONFIGURATION
SSD1306_t oled;

/*void bmp180_task(void *pvParameter)
{
}*/

void app_main()
{

    ESP_LOGI(TAG, "Panel is 128x64");
    ESP_LOGI(TAG, "INTERFACE is i2c");
    ESP_LOGI(TAG, "CONFIG_SDA_GPIO=%d", OLED_SDA_GPIO);
    ESP_LOGI(TAG, "CONFIG_SCL_GPIO=%d", OLED_SCL_GPIO);
    ESP_LOGI(TAG, "CONFIG_RESET_GPIO=%d", OLED_RESET_GPIO);
    i2c_master_init(&oled, OLED_SDA_GPIO, OLED_SCL_GPIO, OLED_RESET_GPIO); // initialize the i2c master driver

    // initialize the OLED
    ssd1306_init(&oled, 128, 64);                          // initialize the OLED
    ssd1306_clear_screen(&oled, false);                    // clear the OLED
    ssd1306_contrast(&oled, 0xFF);                         // set the contrast
    ssd1306_display_text_x3(&oled, 0, "BMP180", 5, false); // display the text
    vTaskDelay(pdMS_TO_TICKS(3000));                       // delay

    ssd1306_clear_screen(&oled, false); // clear the OLED
    ssd1306_contrast(&oled, 0xFF);      // set the contrast

    esp_err_t err; // init error

    err = bmp180_init(I2C_PIN_SDA, I2C_PIN_SCL);
    if (err == ESP_OK)
    {
        // xTaskCreate(&bmp180_task, "bmp180_task", 1024 * 4, NULL, 5, NULL);
        while (1)
        {
            esp_err_t err;
            uint32_t pressure;

            err = bmp180_read_pressure(&pressure);
            if (err != ESP_OK)
            {
                ESP_LOGE(TAG, "Reading of pressure from BMP180 failed, err = %d", err);
            }
            err = bmp180_read_altitude(REFERENCE_PRESSURE, &altitude);
            if (err != ESP_OK)
            {
                ESP_LOGE(TAG, "Reading of altitude from BMP180 failed, err = %d", err);
            }
            err = bmp180_read_temperature(&temperature);
            if (err != ESP_OK)
            {
                ESP_LOGE(TAG, "Reading of temperature from BMP180 failed, err = %d", err);
            }
            ESP_LOGI(TAG, "Pressure %lu Pa, Altitude %.1f m, Temperature : %.1f degC", pressure, altitude, temperature);

            // OLED
            snprintf(lineChar, sizeof(lineChar), "Alt:  %.1f m", altitude);
            ssd1306_display_text(&oled, 0, lineChar, sizeof(lineChar), false);

            snprintf(lineChar, sizeof(lineChar), "Temp: %.1f C", temperature);
            ssd1306_display_text(&oled, 1, lineChar, sizeof(lineChar), false);

            snprintf(lineChar, sizeof(lineChar), "Pres: %lu Pa", pressure);
            ssd1306_display_text(&oled, 2, lineChar, sizeof(lineChar), false);

            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
    else
    {
        ESP_LOGE(TAG, "BMP180 init failed with error = %d", err);
    }
}