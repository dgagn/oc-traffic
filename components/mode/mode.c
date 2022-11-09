#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include "mode.h"
#include "delayer.h"

#define GPIO_BTN 15

uint8_t mode = 1;

uint8_t btn;

_Noreturn void task_btn(void *params) {
  for (;;) {
    btn = gpio_get_level(GPIO_BTN);
    vTaskDelay(10);
    if (btn) {
      mode = mode == 1 ? 2 :
             mode == 2 ? 3 : 1;
      delay_s(1);
    }
  }
}

void create_mode_tasks() {
  gpio_reset_pin(GPIO_BTN);
  gpio_set_direction(GPIO_BTN, GPIO_MODE_INPUT);
  xTaskCreate(task_btn, "mode button", 2048, NULL, 3, NULL);
}
