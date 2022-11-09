#pragma once

#include "stdint.h"

void task_intersection_light(void *param);

typedef struct intersection {
    uint8_t green;
    uint8_t yellow;
    uint8_t red;
    uint8_t level;
} intersection_t;

void create_traffic_tasks(void);

void bus_task(void *param);