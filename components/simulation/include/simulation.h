#pragma once

extern uint32_t time;
extern uint8_t simulation;
extern uint8_t car_time;

void task_simulation_time(void* params);

void task_simulation_btn(void *params);

void create_simulation_tasks();
