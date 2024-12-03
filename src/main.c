#include "pcnt_library.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "math.h"
#include "driver/adc.h"
#include "st7789.h"

static const char *TAG = "MAIN";

#define ENCODER_GPIO_A 4
#define ENCODER_GPIO_B 5
#define ENCODER_GPIO_BUTTON 2

pcnt_handle_t pcnt_handle;
int watch_points[] = {PCNT_LOW_LIMIT, -50, 0, 50, PCNT_HIGH_LIMIT};
int pulse_count = 0;
int event_count = 0;

// bool last_button_state = 0;
int menu_option = 0;
int last_pulse_count = 0;
int first_option = 0;
int last_option = 2;

void get_menu_option()
{

    // ENCODER
    if (pcnt_library_wait_event(&pcnt_handle, &event_count, pdMS_TO_TICKS(1000)))
    {
        // ESP_LOGI(TAG, "Watch point event, count: %d", event_count);
    }
    else
    {
        ESP_ERROR_CHECK(pcnt_library_get_count(&pcnt_handle, &pulse_count));
        // ESP_LOGI(TAG, "Pulse count: %d", pulse_count);
    }

    ESP_LOGI(TAG, "menu_option: %d", menu_option);

    if (pulse_count > last_pulse_count && menu_option != last_option)
    {
        menu_option++;
    }
    else if (pulse_count < last_pulse_count && menu_option != first_option)
    {
        menu_option--;
    }

    last_pulse_count = pulse_count;
}

// void get_button_click()
// {

//     int new_button_state = gpio_get_level(ENCODER_GPIO_BUTTON);

//     if (new_button_state == 0 && new_button_state != last_button_state)
//     {
//         ESP_LOGI(TAG, "button_state");
//     }
//     last_button_state = new_button_state;
// }

// NTC
#define NTC_GPIO 23
#define NTC_EXT 35

double Vs = 3.3;
double R1 = 10000;
double Beta = 3950;
double To = 298.15;
double Ro = 10000;
double adcMax = 4095.0;
double NTC_temp = 0;

void get_temperature_NTC() // leitura da temperatura do sensor extrusora
{
    const double Vs = 3.3;
    const double adcMax = 4095.0;
    const double R1 = 10000.0;
    const double Beta = 3950.0;
    const double Ro = 10000.0;
    const double To = 298.15;

    double Vout, Rt, T, Tc;
    int adc = 0;

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);

    adc = adc1_get_raw(ADC1_CHANNEL_0);

    if (adc < 0 || adc > adcMax)
    {
        ESP_LOGE(TAG, "Erro: Valor do ADC fora do intervalo: %d", adc);
        return;
    }

    Vout = (adc * Vs) / adcMax;
    Rt = R1 * Vout / (Vs - Vout);

    T = 1 / (1 / To + log(Rt / Ro) / Beta);
    Tc = T - 273.15;
    NTC_temp = Tc;

    // ESP_LOGI(TAG, "Temperatura: %.2f °C", Tc);
}

void set_NTC_EXT()
{
    get_temperature_NTC();

    gpio_set_level(NTC_EXT, NTC_temp > 250 ? 0 : 1); // liga e desliga a temperatura da extrusora para manter 250°C
}

// FAN
#define FAN_GPIO 18

void setup()
{

    // ENCODER
    ESP_ERROR_CHECK(pcnt_library_init(&pcnt_handle, ENCODER_GPIO_A, ENCODER_GPIO_B, watch_points, sizeof(watch_points) / sizeof(watch_points[0])));
    gpio_reset_pin(ENCODER_GPIO_BUTTON);
    gpio_set_direction(ENCODER_GPIO_BUTTON, GPIO_MODE_INPUT);

    // FAN
    gpio_reset_pin(FAN_GPIO);
    gpio_set_level(FAN_GPIO, 1);
}

void app_main(void)
{
    setup();

    while (1)
    {
        // get_button_click();
        get_menu_option();

        set_NTC_EXT();

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
