#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

void delay_ms(TickType_t ms) {
  vTaskDelay(ms / portTICK_PERIOD_MS);
}

void delay_s(TickType_t seconds) {
  delay_ms(seconds * 1000);
}
