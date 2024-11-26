#ifndef PCNT_LIBRARY_H
#define PCNT_LIBRARY_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "driver/pulse_cnt.h"

// Configurações padrão para os limites de contagem
#define PCNT_HIGH_LIMIT 100
#define PCNT_LOW_LIMIT -100

// Prototipagem das funções
typedef struct
{
    pcnt_unit_handle_t pcnt_unit;
    QueueHandle_t event_queue;
} pcnt_handle_t;

// Inicializa o PCNT
esp_err_t pcnt_library_init(pcnt_handle_t *handle, int gpio_a, int gpio_b, int *watch_points, size_t watch_point_count);

// Obtém a contagem atual do PCNT
esp_err_t pcnt_library_get_count(pcnt_handle_t *handle, int *count);

// Aguarda e retorna o próximo evento do PCNT (se houver)
bool pcnt_library_wait_event(pcnt_handle_t *handle, int *event_count, TickType_t timeout);

#endif // PCNT_LIBRARY_H
