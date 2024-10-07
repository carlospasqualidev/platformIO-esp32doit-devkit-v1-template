#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#define LED_PIN 25
#define BUTTON_PIN 33

void setup() {
  gpio_reset_pin(LED_PIN);
  gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

  gpio_reset_pin(BUTTON_PIN);
  gpio_set_direction(BUTTON_PIN, GPIO_MODE_INPUT);
  gpio_set_pull_mode(BUTTON_PIN, GPIO_PULLUP_ONLY);


}

void IRAM_ATTR toggle_led(void *arg) {
  static bool led_on = false;
  gpio_set_level(LED_PIN, led_on);
  led_on = !led_on;
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

void app_main() {

  setup();

  const esp_timer_create_args_t timer_args = {
    .callback = &toggle_led,
    .name = "led_timer"
  };

  esp_timer_handle_t led_timer;
  esp_timer_create(&timer_args, &led_timer);

  while (true) {
    int buttonState = debounce(BUTTON_PIN, 50);
    printf("Button state: %d\n", buttonState);  // Print no terminal

    if (!buttonState)
    {
      // gpio_set_level(LED_PIN, 1);
    esp_timer_start_once(led_timer, 1000000); // 1 segundo
    }

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
