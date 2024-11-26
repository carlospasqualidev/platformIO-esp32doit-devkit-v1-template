#include "pcnt_library.h"
#include "esp_log.h"

static const char *TAG = "PCNT_LIB";

static bool pcnt_on_reach_callback(pcnt_unit_handle_t unit, const pcnt_watch_event_data_t *edata, void *user_ctx)
{
    BaseType_t high_task_wakeup;
    QueueHandle_t queue = (QueueHandle_t)user_ctx;
    xQueueSendFromISR(queue, &(edata->watch_point_value), &high_task_wakeup);
    return (high_task_wakeup == pdTRUE);
}

esp_err_t pcnt_library_init(pcnt_handle_t *handle, int gpio_a, int gpio_b, int *watch_points, size_t watch_point_count)
{
    if (!handle || !watch_points || watch_point_count == 0)
    {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOGI(TAG, "Install PCNT unit");
    pcnt_unit_config_t unit_config = {
        .high_limit = PCNT_HIGH_LIMIT,
        .low_limit = PCNT_LOW_LIMIT,
    };

    ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &handle->pcnt_unit));

    ESP_LOGI(TAG, "Set glitch filter");
    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = 1000,
    };
    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(handle->pcnt_unit, &filter_config));

    ESP_LOGI(TAG, "Install PCNT channels");
    pcnt_chan_config_t chan_a_config = {
        .edge_gpio_num = gpio_a,
        .level_gpio_num = gpio_b,
    };
    pcnt_channel_handle_t pcnt_chan_a = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(handle->pcnt_unit, &chan_a_config, &pcnt_chan_a));

    pcnt_chan_config_t chan_b_config = {
        .edge_gpio_num = gpio_b,
        .level_gpio_num = gpio_a,
    };
    pcnt_channel_handle_t pcnt_chan_b = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(handle->pcnt_unit, &chan_b_config, &pcnt_chan_b));

    ESP_LOGI(TAG, "Set edge and level actions for PCNT channels");
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_a, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_a, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_b, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_b, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));

    ESP_LOGI(TAG, "Add watch points");
    for (size_t i = 0; i < watch_point_count; i++)
    {
        ESP_ERROR_CHECK(pcnt_unit_add_watch_point(handle->pcnt_unit, watch_points[i]));
    }

    pcnt_event_callbacks_t cbs = {
        .on_reach = pcnt_on_reach_callback,
    };
    handle->event_queue = xQueueCreate(10, sizeof(int));
    ESP_ERROR_CHECK(pcnt_unit_register_event_callbacks(handle->pcnt_unit, &cbs, handle->event_queue));

    ESP_LOGI(TAG, "Enable and start PCNT unit");
    ESP_ERROR_CHECK(pcnt_unit_enable(handle->pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(handle->pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_start(handle->pcnt_unit));

    return ESP_OK;
}

esp_err_t pcnt_library_get_count(pcnt_handle_t *handle, int *count)
{
    if (!handle || !count)
    {
        return ESP_ERR_INVALID_ARG;
    }
    return pcnt_unit_get_count(handle->pcnt_unit, count);
}

bool pcnt_library_wait_event(pcnt_handle_t *handle, int *event_count, TickType_t timeout)
{
    if (!handle || !event_count)
    {
        return false;
    }
    return xQueueReceive(handle->event_queue, event_count, timeout);
}
