#include "pcnt_library.h"
#include "esp_log.h"

static const char *TAG = "MAIN";

#include "st7789.h"

#define ENCODER_GPIO_A 4
#define ENCODER_GPIO_B 5
pcnt_handle_t pcnt_handle;
int watch_points[] = {PCNT_LOW_LIMIT, -50, 0, 50, PCNT_HIGH_LIMIT};
int pulse_count = 0;
int event_count = 0;

void setup()
{
    // ENCODER
    ESP_ERROR_CHECK(pcnt_library_init(&pcnt_handle, ENCODER_GPIO_A, ENCODER_GPIO_B, watch_points, sizeof(watch_points) / sizeof(watch_points[0])));
}

void app_main(void)
{
    setup();

    while (1)
    {
        // ENCODER
        if (pcnt_library_wait_event(&pcnt_handle, &event_count, pdMS_TO_TICKS(1000)))
        {
            ESP_LOGI(TAG, "Watch point event, count: %d", event_count);
        }
        else
        {
            ESP_ERROR_CHECK(pcnt_library_get_count(&pcnt_handle, &pulse_count));
            ESP_LOGI(TAG, "Pulse count: %d", pulse_count);
        }
    }
}
