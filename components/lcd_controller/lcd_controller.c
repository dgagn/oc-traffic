#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <memory.h>
#include "sdkconfig.h"
#include "rom/ets_sys.h"
#include "lcd_controller.h"
#include "driver/i2c.h"

#define LCD_BACKLIGHT 0x08
#define LCD_ENABLE 0x04
#define LCD_COMMAND 0x00
#define LCD_WRITE 0x01

#define LCD_ENTRY_MODE 0x06

#define LCD_CLEAR 0x01
#define LCD_HOME 0x02
#define LCD_DISPLAY_ON 0x0C
#define LCD_FUNCTION_RESET 0x30
#define LCD_SET_4BIT 0x28

static char *tag = "ovior";

lcd_info_t lcd_initial_info;

int init_i2c_driver(void) {
  i2c_config_t i2c_conf = {
      .mode = I2C_MODE_MASTER,
      .sda_io_num = lcd_initial_info.sda,
      .scl_io_num = lcd_initial_info.scl,
      .sda_pullup_en = GPIO_PULLUP_ENABLE,
      .scl_pullup_en = GPIO_PULLUP_ENABLE,
      .master.clk_speed = 100000
  };

  i2c_param_config(I2C_NUM_0, &i2c_conf);
  i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);

  return 0;
}

void lcd_pulse(uint8_t data) {
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  ESP_ERROR_CHECK(i2c_master_start(cmd));
  ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (lcd_initial_info.address << 1) | I2C_MASTER_WRITE, 1));
  ESP_ERROR_CHECK(i2c_master_write_byte(cmd, data | LCD_ENABLE, 1));
  ESP_ERROR_CHECK(i2c_master_stop(cmd));
  ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS));
  i2c_cmd_link_delete(cmd);
  ets_delay_us(1);

  cmd = i2c_cmd_link_create();
  ESP_ERROR_CHECK(i2c_master_start(cmd));
  ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (lcd_initial_info.address << 1) | I2C_MASTER_WRITE, 1));
  ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (data & ~LCD_ENABLE), 1));
  ESP_ERROR_CHECK(i2c_master_stop(cmd));
  ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS));
  i2c_cmd_link_delete(cmd);
  ets_delay_us(500);
}

void lcd_write_nibble(uint8_t nibble, uint8_t mode) {
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();

  uint8_t data = (nibble & 0xF0) | mode | LCD_BACKLIGHT;

  ESP_ERROR_CHECK(i2c_master_start(cmd));
  ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (lcd_initial_info.address << 1) | I2C_MASTER_WRITE, 1));
  ESP_ERROR_CHECK(i2c_master_write_byte(cmd, data, 1));
  ESP_ERROR_CHECK(i2c_master_stop(cmd));
  ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS));
  i2c_cmd_link_delete(cmd);

  lcd_pulse(data);
}

void lcd_write_byte(uint8_t data, uint8_t mode) {
  lcd_write_nibble(data & 0xf0, mode);
  lcd_write_nibble((data << 4) & 0xf0, mode);
}

void lcd_write_char(char c) {
  lcd_write_byte(c, LCD_WRITE);
}

void lcd_write_string(char *str) {
  while (*str) lcd_write_char(*str++);
}

void lcd_command(uint8_t data) {
  lcd_write_byte(data, LCD_COMMAND);
}

void lcd_home(void) {
  lcd_command(LCD_HOME);
  vTaskDelay(2 / portTICK_PERIOD_MS);
}

void lcd_clear(void) {
  lcd_command(LCD_CLEAR);
  vTaskDelay(2 / portTICK_PERIOD_MS);
}

void lcd_cursor_to(lcd_position_t *position) {
  if (position->row > lcd_initial_info.max_rows - 1) {
    ESP_LOGE(tag, "Error writing to row %d because the max row is %d.", position->row, lcd_initial_info.max_rows - 1);
    position->row = lcd_initial_info.max_rows - 1;
  }
  uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
  lcd_command(0x80 | (position->column + row_offsets[position->row]));
}

void lcd_cursor_next_line(void) {
  lcd_position_t position = {
      .row = 1,
      .column = 0
  };
  lcd_cursor_to(&position);
}

void lcd_reset(void) {
  lcd_write_nibble(LCD_FUNCTION_RESET, LCD_COMMAND);
  vTaskDelay(10 / portTICK_PERIOD_MS);
  lcd_write_nibble(LCD_FUNCTION_RESET, LCD_COMMAND);
  ets_delay_us(200);
  lcd_write_nibble(LCD_FUNCTION_RESET, LCD_COMMAND);
  lcd_write_nibble(LCD_SET_4BIT, LCD_COMMAND);
  ets_delay_us(80);
}


void lcd_init(lcd_info_t *info) {
  lcd_initial_info.max_columns = info->max_columns;
  lcd_initial_info.max_rows = info->max_rows;
  lcd_initial_info.address = info->address;
  lcd_initial_info.scl = info->scl;
  lcd_initial_info.sda = info->sda;

  init_i2c_driver();

  vTaskDelay(100 / portTICK_PERIOD_MS);

  lcd_reset();

  lcd_command(LCD_SET_4BIT);
  ets_delay_us(80);

  lcd_clear();

  lcd_command(LCD_ENTRY_MODE);
  ets_delay_us(80);

  lcd_command(LCD_DISPLAY_ON);
}

void lcd_write_string_from_end(char *str, uint8_t row) {
  lcd_position_t position = {
      .column = lcd_initial_info.max_columns - strlen(str),
      .row = row,
  };
  lcd_cursor_to(&position);

  lcd_write_string(str);
}
