#include "task.h"
#include "vga.h"
#include "config.h"

#define STACK_BASE CFG_STACK_BASE

static task_t tasks[MAX_TASKS];
static int    task_count_val = 0;
static int    current_task   = 0;
static int    prev_task      = 0;

void task_install(void) {
    for (int i = 0; i < MAX_TASKS; i++)
        tasks[i].state = TASK_DEAD;
}

void task_create(void (*entry)(void)) {
    if (task_count_val >= MAX_TASKS) return;

    task_t* t = &tasks[task_count_val];
    t->id    = task_count_val;
    t->state = TASK_READY;

    uint8_t* stack_mem = (uint8_t*)(STACK_BASE + task_count_val * STACK_SIZE);
    t->stack = (uint32_t*)stack_mem;

    volatile uint32_t* sp = (uint32_t*)(stack_mem + STACK_SIZE);

    /* return address — where task starts */
    *--sp = (uint32_t)entry;

    /* callee saved regs: ebx, esi, edi, ebp — all zero */
    *--sp = 0; /* ebx */
    *--sp = 0; /* esi */
    *--sp = 0; /* edi */
    *--sp = 0; /* ebp */

    t->esp = (uint32_t)sp;
    task_count_val++;
}

/* accessors for pit.c */
int      task_count(void)              { return task_count_val; }
int      get_current_task(void)        { return current_task; }
void     set_current_task(int id)      { prev_task = current_task; current_task = id; }
int      get_task_state(int id)        { return tasks[id].state; }
void     set_task_state(int id, int s) { tasks[id].state = s; }
uint32_t get_task_esp(int id)          { return tasks[id].esp; }

/* returns pointer to prev task's esp field so asm can save into it */
uint32_t* get_prev_esp_ptr(void) { return &tasks[prev_task].esp; }

void schedule(void) {
    if (task_count_val < 2) return;

    int next = current_task;
    for (int i = 0; i < task_count_val; i++) {
        next = (next + 1) % task_count_val;
        if (tasks[next].state == TASK_READY ||
            tasks[next].state == TASK_RUNNING)
            break;
    }

    if (next == current_task) return;

    int prev = current_task;
    tasks[prev].state = TASK_READY;
    tasks[next].state = TASK_RUNNING;
    current_task = next;

    switch_context(&tasks[prev].esp, tasks[next].esp);
}

void task_start(void) {
    current_task = 0;
    tasks[0].state = TASK_RUNNING;

    __asm__ volatile (
        "mov %0, %%esp\n"
        "pop %%ebp\n"
        "pop %%edi\n"
        "pop %%esi\n"
        "pop %%ebx\n"
        "ret\n"
        : : "m"(tasks[0].esp)
    );
}