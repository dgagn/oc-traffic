#include <freertos/FreeRTOS.h>
#include <esp_log.h>
#include <freertos/task.h>
#include "traffic_light.h"
#include "delayer.h"
#include "driver/gpio.h"
#include "mode.h"
#include "simulation.h"

#define GPIO_BUS 5

#define GPIO_GREEN_1 14
#define GPIO_YELLOW_1 12
#define GPIO_RED_1 13

#define GPIO_GREEN_2 27
#define GPIO_YELLOW_2 32
#define GPIO_RED_2 33

uint8_t is_open = false;

void toggle_gpio_with_delay(gpio_num_t gpio, TickType_t seconds) {
  gpio_set_level(gpio, 1);
  delay_s(seconds);
  gpio_set_level(gpio, 0);
}

void full_reset_pin(uint8_t gpio, uint8_t direction) {
  gpio_reset_pin(gpio);
  gpio_set_direction(gpio, direction);
}

_Noreturn void task_intersection_light(void *param) {
  intersection_t *intersection = (intersection_t *) param;

  for (;;) {
    uint8_t cached_mode = mode;
    uint8_t rest = (5 / cached_mode + 1) - (6 / cached_mode);

    if (intersection->level == 1) {
      toggle_gpio_with_delay(intersection->red, 3 + rest / cached_mode);
    }
    if (intersection->level == 1) {
      is_open = true;
    }
    toggle_gpio_with_delay(intersection->green, 2 + rest / cached_mode);
    if (intersection->level == 1) {
      is_open = false;
    }
    toggle_gpio_with_delay(intersection->yellow, 1);
    if (intersection->level == 0) {
      toggle_gpio_with_delay(intersection->red, 3 + rest / cached_mode);
    }
  }
}

_Noreturn void bus_task(void *param) {
  for(;;) {
    if ((time > 60 * 6 && time < 60 * 9) && is_open) {
      gpio_set_level(GPIO_BUS, 1);
    } else {
      gpio_set_level(GPIO_BUS, 0);
    }
    delay_ms(100);
  }
}

_Noreturn void p_create_traffic_tasks(void *param) {
  TaskHandle_t handles[] = {
      NULL,
      NULL,
  };

  intersection_t intersections[] = {
      {.green = GPIO_GREEN_1, .yellow = GPIO_YELLOW_1, .red = GPIO_RED_1, .level = 0},
      {.green = GPIO_GREEN_2, .yellow = GPIO_YELLOW_2, .red = GPIO_RED_2, .level = 1},
  };

  for (int i = 0; i < sizeof(intersections) / sizeof(intersection_t); ++i) {
    xTaskCreate(task_intersection_light, "intersection light", 2048, &intersections[i], 1, &handles[i]);
  }

  for (;;) {
    if (simulation == 0) {
      delay_ms(100);
      continue;
    }

    for (int i = 0; i < sizeof(handles) / sizeof(TaskHandle_t); ++i) {
      if (handles[i] != NULL) {
        vTaskDelete(handles[i]);
        handles[i] = NULL;
        gpio_set_level(GPIO_GREEN_1, 0);
        gpio_set_level(GPIO_YELLOW_1, 0);
        gpio_set_level(GPIO_RED_1, 0);
        gpio_set_level(GPIO_GREEN_2, 0);
        gpio_set_level(GPIO_YELLOW_2, 0);
        gpio_set_level(GPIO_RED_2, 0);
        intersections[i].level = intersections[i].level == 0 ? 1 : 0;
        xTaskCreate(task_intersection_light, "intersection light", 2048, &intersections[i], 1, &handles[i]);
      }
    }
    delay_ms(100);
  }
}

void create_traffic_tasks(void) {
  full_reset_pin(GPIO_GREEN_1, GPIO_MODE_OUTPUT);
  full_reset_pin(GPIO_YELLOW_1, GPIO_MODE_OUTPUT);
  full_reset_pin(GPIO_RED_1, GPIO_MODE_OUTPUT);
  full_reset_pin(GPIO_GREEN_2, GPIO_MODE_OUTPUT);
  full_reset_pin(GPIO_YELLOW_2, GPIO_MODE_OUTPUT);
  full_reset_pin(GPIO_RED_2, GPIO_MODE_OUTPUT);
  full_reset_pin(GPIO_BUS, GPIO_MODE_OUTPUT);
  xTaskCreate(p_create_traffic_tasks, "traffic light", 2048, NULL, 1, NULL);
  xTaskCreate(bus_task, "bus light", 2048, NULL, 1, NULL);
}
