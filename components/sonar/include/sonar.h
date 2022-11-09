#pragma once

#include "stdint.h"

extern uint8_t car_waiting;

void tx_task();

void rx_task();

void create_sonar_tasks();