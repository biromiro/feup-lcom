#include <lcom/lcf.h>

#include "kbc.h"
#include "timer.h"
#include "video_gr.h"

enum {MOUSE,TIMER,KBD} typedef irq_type;

int subscribe_interrupts();

int initialize();

int unsubscribe_interrupts();

int finish();

uint8_t get_irq_set(irq_type type);

void interrupt_call_receiver();

void timer_handler();

void mouse_handler();

void kbd_handler();
