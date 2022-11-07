#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "delayer.h"
#include "mode.h"

uint8_t time = 0;
uint8_t btn_simulation;
uint8_t simulation = 0;

_Noreturn void task_simulation_btn(void *params) {
  for (;;) {
    btn_simulation = gpio_get_level(4);
    delay_ms(10);
    if (btn_simulation) {
      simulation = 1;
      delay_ms(100);
      simulation = 0;
      delay_s(1);
    }
  }
}

_Noreturn void task_simulation_time(void* params) {
  for(;;) {
    uint8_t cached_mode = mode;
    time++;
    delay_ms(1000 / cached_mode);
  }
}
