#include <freertos/FreeRTOS.h>
#include <esp_log.h>
#include "traffic_light.h"
#include "delayer.h"
#include "driver/gpio.h"
#include "mode.h"
#include "simulation.h"


void toggle_gpio_with_delay(gpio_num_t gpio, TickType_t seconds) {
  gpio_set_level(gpio, 1);
  delay_s(seconds);
  gpio_set_level(gpio, 0);
}

_Noreturn void task_intersection_light(void *param) {
  intersection_t *intersection = (intersection_t *) param;

  for (;;) {
    uint8_t cached_mode = mode;
    uint8_t rest = (5 / cached_mode + 1) - (6 / cached_mode);

    if (intersection->level == 1) {
      toggle_gpio_with_delay(intersection->red, 3 + rest / cached_mode);
    }
    toggle_gpio_with_delay(intersection->green, 2 + rest / cached_mode);
    toggle_gpio_with_delay(intersection->yellow, 1);
    if (intersection->level == 0) {
      toggle_gpio_with_delay(intersection->red, 3 + rest / cached_mode);
    }
  }
}
