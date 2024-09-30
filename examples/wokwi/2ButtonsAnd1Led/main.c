#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define led 25

#define buttonOn 32
#define buttonOff 33

bool debounceOn(int pin, unsigned long delay)
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

bool debounceOff(int pin, unsigned long delay)
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

void app_main()
{

  gpio_reset_pin(led);
  gpio_set_direction(led, GPIO_MODE_OUTPUT);

  gpio_reset_pin(buttonOn);
  gpio_set_direction(buttonOn, GPIO_MODE_INPUT);
  gpio_set_pull_mode(buttonOn, GPIO_PULLUP_ONLY);

  gpio_reset_pin(buttonOff);
  gpio_set_direction(buttonOff, GPIO_MODE_INPUT);
  gpio_set_pull_mode(buttonOff, GPIO_PULLUP_ONLY);

  bool buttonStateOn = 0;
  bool buttonStateOff = 0;

  while (true)
  {
    // BUTTON ON
    int newButtonStateOn = debounceOn(buttonOn, 50);

    if (newButtonStateOn == 0 && newButtonStateOn != buttonStateOn)
    {
      gpio_set_level(led, 1);
    }

    //BUTTON OFF
    int newButtonStateOff = debounceOff(buttonOff, 50);

    if (newButtonStateOff == 0 && newButtonStateOff != buttonStateOff)
    {
      gpio_set_level(led, 0);
    }

    buttonStateOn = newButtonStateOn;
    buttonStateOff = newButtonStateOff;

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
