// CARLOS PASQUALI, HENRIQUE JOCHEM
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/pcnt.h"
#include "driver/gpio.h"

// ENCODER
#define PCNT_INPUT_SIG_IO 4  // Pino de entrada do sinal de pulso
#define PCNT_CTRL_IO 5       // Pino de controle do contador
#define PCNT_H_LIM_VAL 1000  // Limite superior de contagem
#define PCNT_L_LIM_VAL -1000 // Limite inferior de contagem
int16_t count = 0;

void config_pulse_counter()
{
    // Configurações iniciais para o contador de pulsos
    pcnt_config_t pcnt_config = {
        .pulse_gpio_num = PCNT_INPUT_SIG_IO, // Pino de entrada de pulsos
        .ctrl_gpio_num = PCNT_CTRL_IO,       // Pino de controle
        .channel = PCNT_CHANNEL_0,           // Canal do contador
        .unit = PCNT_UNIT_0,                 // Unidade do contador
        .pos_mode = PCNT_COUNT_INC,          // Contagem positiva (borda de subida)
        .neg_mode = PCNT_COUNT_DIS,          // Desabilitar contagem negativa (borda de descida)
        .counter_h_lim = PCNT_H_LIM_VAL,     // Limite superior
        .counter_l_lim = PCNT_L_LIM_VAL,     // Limite inferior
        .lctrl_mode = PCNT_MODE_KEEP,        // Mantém o estado se o pino de controle estiver em nível baixo
        .hctrl_mode = PCNT_MODE_REVERSE      // Mantém o estado se o pino de controle estiver em nível alto
    };

    // Configuração do contador
    pcnt_unit_config(&pcnt_config);
    // Inicializa o contador, pausa, e reseta para zero
    pcnt_counter_pause(PCNT_UNIT_0); // Pausar o contador
    pcnt_counter_clear(PCNT_UNIT_0); // Limpar o valor do contador
    // Definir o limite de eventos do contador (opcional)
    pcnt_set_event_value(PCNT_UNIT_0, PCNT_EVT_H_LIM, PCNT_H_LIM_VAL); // Evento no limite superior
    // Reinicia a contagem
    pcnt_counter_resume(PCNT_UNIT_0); // Retomar a contagem de pulsos
}

// FAN
#define FAN 23 // Pino onde o LED está conectado

void setup()
{
    // FAN
    gpio_reset_pin(FAN);
    gpio_set_direction(FAN, GPIO_MODE_OUTPUT);

    // ENCODER
    config_pulse_counter();
}

void app_main()
{
    // ENCODER

    while (1)
    {
        // ENCODER
        //  Ler o valor atual do contador
        pcnt_get_counter_value(PCNT_UNIT_0, &count);
        printf("Valor do encoder: %d\n", count);
        // Delay para a próxima leitura (por exemplo, a cada segundo)
        vTaskDelay(pdMS_TO_TICKS(1000));

        // FAN
        gpio_set_level(FAN, 1);
        // vTaskDelay(5000 / portTICK_PERIOD_MS);
        // gpio_set_level(FAN, 0);
        // vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}