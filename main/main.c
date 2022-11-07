#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "freertos/task.h"
#include "lcd_controller.h"
#include <stdio.h>
#include <memory.h>
#include <esp_log.h>
#include "mode.h"
#include "traffic_light.h"
#include "simulation.h"
#include "delayer.h"

_Noreturn void task_lcd_screen(void *params) {
  char buffer[16];
  for (;;) {
    lcd_home();
    lcd_clear();
    sprintf(buffer, "Mode %d", mode);
    lcd_write_string(buffer);
    lcd_write_string_from_end("0s", 0);
    lcd_cursor_next_line();
    uint8_t hours = time / 60;
    uint8_t minutes = time - (hours * 60);
    lcd_write_string("Temps");
    memset(&buffer[0], 0, sizeof(buffer));
    sprintf(buffer, "%02d:%02d", hours, minutes);
    lcd_write_string_from_end(buffer, 1);
    delay_s(1);
  }
}

_Noreturn void app_main(void)
{
  lcd_info_t info = {
      .address = 0x27,
      .sda = 21,
      .scl = 22,
      .max_rows = 2,
      .max_columns = 16,
  };

  lcd_init(&info);
  xTaskCreate(task_lcd_screen, "lcd screen", 2048, NULL, 2, NULL);

  gpio_reset_pin(14);
  gpio_reset_pin(12);
  gpio_reset_pin(13);
  gpio_reset_pin(27);
  gpio_reset_pin(32);
  gpio_reset_pin(33);
  gpio_reset_pin(15);
  gpio_reset_pin(4);

  gpio_set_direction(14, GPIO_MODE_OUTPUT);
  gpio_set_direction(12, GPIO_MODE_OUTPUT);
  gpio_set_direction(13, GPIO_MODE_OUTPUT);
  gpio_set_direction(27, GPIO_MODE_OUTPUT);
  gpio_set_direction(32, GPIO_MODE_OUTPUT);
  gpio_set_direction(33, GPIO_MODE_OUTPUT);
  gpio_set_direction(15, GPIO_MODE_INPUT);
  gpio_set_direction(4, GPIO_MODE_INPUT);

  intersection_t intersections[] = {
      {.green = 14, .yellow = 12, .red = 13, .level = 0},
      {.green = 27, .yellow = 32, .red = 33, .level = 1},
  };

  TaskHandle_t handles[] = {
      NULL,
      NULL,
  };

  for (int i = 0; i < sizeof(intersections) / sizeof(intersection_t); ++i) {
    xTaskCreate(task_intersection_light, "intersection light", 2048, &intersections[i], 1, &handles[i]);
  }

  xTaskCreate(task_simulation_time, "simulation time", 2048, NULL, 3, NULL);
  xTaskCreate(task_btn, "mode button", 2048, NULL, 3, NULL);
  xTaskCreate(task_simulation_btn, "simulation button switch", 2048, NULL, 2, NULL);

  for(;;) {
    if (simulation) {
      for (int i = 0; i < sizeof(handles) / sizeof(TaskHandle_t); ++i) {
        if (handles[i] != NULL) {
          vTaskDelete(handles[i]);
          handles[i] = NULL;
          gpio_set_level(14, 0);
          gpio_set_level(12, 0);
          gpio_set_level(13, 0);
          gpio_set_level(27, 0);
          gpio_set_level(32, 0);
          gpio_set_level(33, 0);
          intersections[i].level = intersections[i].level == 0 ? 1 : 0;
          xTaskCreate(task_intersection_light, "intersection light", 2048, &intersections[i], 1, &handles[i]);
        }
      }
      ESP_LOGI("ovior", "simulation button clicked, task supposed to end");
    }
    delay_ms(100);
  }
}
