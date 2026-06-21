#ifndef TASK_H
#define TASK_H

#include <stdint.h>

#include "config.h"

#define TASK_READY   0
#define TASK_RUNNING 1
#define TASK_DEAD    2
#define MAX_TASKS    CFG_MAX_TASKS
#define STACK_SIZE   CFG_STACK_SIZE

typedef struct {
    uint32_t esp;
    uint32_t* stack;
    int state;
    int id;
} task_t;

void      task_install(void);
void      task_create(void (*entry)(void));
void      schedule(void);
void      task_start(void);
int       task_count(void);
int       get_current_task(void);
void      set_current_task(int id);
int       get_task_state(int id);
void      set_task_state(int id, int s);
void      switch_context(uint32_t* prev_esp, uint32_t next_esp);
uint32_t  get_task_esp(int id);
uint32_t* get_prev_esp_ptr(void);

#endif