#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "delayer.h"
#include "mode.h"
#include "sonar.h"

#define GPIO_SIMULATION 4

uint32_t time = 0;
uint8_t btn_simulation;
uint8_t simulation = 0;

uint8_t car_time = 0;

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
    uint8_t hours = time / 60;
    if (hours == 24) {
      time = 0;
    }
    car_time = car_waiting ? car_time + 1 : 0;
    delay_ms(1000 / cached_mode);
  }
}

void create_simulation_tasks() {
  gpio_reset_pin(GPIO_SIMULATION);
  gpio_set_direction(GPIO_SIMULATION, GPIO_MODE_INPUT);

  xTaskCreate(task_simulation_time, "simulation time", 2048, NULL, 3, NULL);
  xTaskCreate(task_simulation_btn, "simulation button switch", 2048, NULL, 2, NULL);
}