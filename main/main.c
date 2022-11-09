#include "freertos/FreeRTOS.h"
#include "lcd_controller.h"
#include "mode.h"
#include "traffic_light.h"
#include "simulation.h"
#include "delayer.h"
#include "sonar.h"

_Noreturn void app_main(void)
{
  create_lcd_tasks();
  create_mode_tasks();
  create_simulation_tasks();
  create_sonar_tasks();
  create_traffic_tasks();

  for(;;) {
    delay_ms(1000);
  }
}