#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/rmt.h"
#include "sdkconfig.h"

#define RMT_TX_CHANNEL 1
#define RMT_TX_GPIO_NUM 16
#define RMT_RX_CHANNEL 0
#define RMT_RX_GPIO_NUM 17

#define RMT_CLK_DIV 100
#define RMT_TICK_10_US (80000000/RMT_CLK_DIV/100000)

#define HCSR04_MAX_TIMEOUT_US 25000

#define US2TICKS(us) (us / 10 * RMT_TICK_10_US)
#define TICKS2US(ticks) (ticks * 10.0 / RMT_TICK_10_US)

uint8_t car_waiting = false;

inline void set_item_edge(rmt_item32_t *item, int low_us, int high_us) {
  item->level0 = 0;
  item->duration0 = US2TICKS(low_us);
  item->level1 = 1;
  item->duration1 = US2TICKS(high_us);
}

void nec_tx_init() {
  rmt_config_t rmt_tx;
  rmt_tx.channel = RMT_TX_CHANNEL;
  rmt_tx.gpio_num = RMT_TX_GPIO_NUM;
  rmt_tx.mem_block_num = 1;
  rmt_tx.clk_div = RMT_CLK_DIV;
  rmt_tx.tx_config.loop_en = false;

  rmt_tx.tx_config.carrier_duty_percent = 50;
  rmt_tx.tx_config.carrier_freq_hz = 38000;
  rmt_tx.tx_config.carrier_level = 1;
  rmt_tx.tx_config.carrier_en = 0;  // off

  rmt_tx.tx_config.idle_level = 0;
  rmt_tx.tx_config.idle_output_en = true;
  rmt_tx.rmt_mode = 0;
  rmt_config(&rmt_tx);
  rmt_driver_install(rmt_tx.channel, 0, 0);
}

void nec_rx_init() {
  rmt_config_t rmt_rx;
  rmt_rx.channel = RMT_RX_CHANNEL;
  rmt_rx.gpio_num = RMT_RX_GPIO_NUM;
  rmt_rx.clk_div = RMT_CLK_DIV;
  rmt_rx.mem_block_num = 1;
  rmt_rx.rmt_mode = RMT_MODE_RX;
  rmt_rx.rx_config.filter_en = false;
  rmt_rx.rx_config.filter_ticks_thresh = 100;
  rmt_rx.rx_config.idle_threshold = US2TICKS(HCSR04_MAX_TIMEOUT_US);
  rmt_config(&rmt_rx);
  rmt_driver_install(rmt_rx.channel, 1000, 0);
}

void rx_task() {
  int channel = RMT_RX_CHANNEL;

  nec_rx_init();

  RingbufHandle_t rb = NULL;

  //get RMT RX ringbuffer
  rmt_get_ringbuf_handle(channel, &rb);
  rmt_rx_start(channel, 1);

  while (rb) {
    size_t rx_size = 0;
    //try to receive data from ringbuffer.
    //RMT driver will push all the data it receives to its ringbuffer.
    //We just need to parse the value and return the spaces of ringbuffer.
    rmt_item32_t *item = (rmt_item32_t *) xRingbufferReceive(rb, &rx_size, 1000);
    if (item) {
      for (int i = 0; i < rx_size / sizeof(rmt_item32_t); ++i) {
        float value = TICKS2US(item[i].duration0) / 58.2f; // NOLINT(bugprone-integer-division)

        if (value > 0 && value < 7) {
          car_waiting = true;
          ESP_LOGI("ovior", "VOITURE LA WOOOOOO %f", value);
        } else {
          car_waiting = false;
        }
      }

      //after parsing the data, return spaces to ringbuffer.
      vRingbufferReturnItem(rb, (void *) item);
    } else {
      vTaskDelay(10);
    }
  }

  vTaskDelete(NULL);
}


_Noreturn void tx_task() {
  vTaskDelay(10);
  nec_tx_init();

  int channel = RMT_TX_CHANNEL;

  int item_num = 1;
  rmt_item32_t item[item_num];
  for (int i = 0; i < item_num; ++i)
    set_item_edge(&item[i], 20, 180);

  for (;;) {
    rmt_write_items(channel, item, item_num, true);
    rmt_wait_tx_done(channel, portMAX_DELAY);

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void create_sonar_tasks() {
  xTaskCreate(rx_task, "rx_task", 2048, NULL, 10, NULL);
  xTaskCreate(tx_task, "tx_task", 2048, NULL, 10, NULL);
}