#pragma once

#include "stdint.h"

typedef struct lcd_info {
    uint8_t max_rows;
    uint8_t max_columns;
    uint8_t sda;
    uint8_t scl;
    uint8_t address;
} lcd_info_t;

typedef struct lcd_position {
    uint8_t row;
    uint8_t column;
} lcd_position_t;

void lcd_init(lcd_info_t *info);

void lcd_cursor_next_line(void);

void lcd_cursor_to(lcd_position_t *position);

void lcd_clear(void);

void lcd_home(void);

void lcd_write_string(char *str);

void lcd_write_string_from_end(char *str, uint8_t row);

void lcd_write_char(char c);
