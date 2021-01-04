#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int counter = 0;
int hook = 0;

int(timer_set_frequency)(uint8_t timer, uint32_t freq) {

  //checking for valid frequency
  if (freq < 19 || freq > TIMER_FREQ)
    return 1;

  uint16_t div = TIMER_FREQ / freq;

  uint8_t msb = 0;
  uint8_t lsb = 0;

  //loading msb and lsb of frequency to the respective variables
  util_get_MSB(div, &msb);
  util_get_LSB(div, &lsb);

  uint8_t conf_timer = 0;

  //checks if it was not possible to get the timer's configuration
  if (timer_get_conf(timer, &conf_timer) == 1) {
    printf("Invalid configuration on timer");
    return 1;
  }

  //builds the control word (maintaining the 4 lsbits of the timer's config)
  switch (timer) {
    case 0:
      conf_timer = ((conf_timer & 0x0F) | TIMER_LSB_MSB | TIMER_SEL0);
      break;
    case 1:
      conf_timer = ((conf_timer & 0x0F) | TIMER_LSB_MSB | TIMER_SEL1);
      break;
    case 2:
      conf_timer = ((conf_timer & 0x0F) | TIMER_LSB_MSB | TIMER_SEL2);
      break;
  }

  //checks if it was possible to send the control word to the control port
  //as well as sending the desired frequency to the desired timer
  return (sys_outb(TIMER_CTRL, conf_timer) || sys_outb(TIMER_0 + timer, lsb) || sys_outb(TIMER_0 + timer, msb));
}

int(timer_subscribe_int)(uint8_t *bit_no) {
  *bit_no = BIT(hook);
  sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook);
  return 0;
}

int(timer_unsubscribe_int)() {
  sys_irqrmpolicy(&hook);
  return 0;
}

void(timer_int_handler)() {
  counter++;
}

int(timer_get_conf)(uint8_t timer, uint8_t *st) {

  //checks if the timer chosen is valid
  if (timer > 2 || timer < 0)
    return 1;

  //building the readback command and checking if it was possible to send it to the control port
  uint8_t readBackComm = (TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer));
  if (sys_outb(TIMER_CTRL, readBackComm))
    return 1;

  //checks if it was possible to get the configuration of the timer
  return util_sys_inb(TIMER_0 + timer, st);
}

enum timer_init initializationMode(uint8_t *st) {

  *st = *st >> 4;
  *st &= 0x03;

  //defines the init mode
  switch (*st) {
    case 0x01:
      return LSB_only;
    case 0x02:
      return MSB_only;
    case 0x03:
      return MSB_after_LSB;
    default:
      return INVAL_val;
  }
}

uint8_t countMode(uint8_t st) {

  //defines the count mode (0 through 5)
  switch (st) {
    case 0x06:
      return 2;
    case 0x07:
      return 3;
    default:
      return st;
  }
}

int(timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {

  union timer_status_field_val status;

  //selects the field member selected and updates the union parameter accordingly
  switch (field) {
    case tsf_all:
      status.byte = st;
      break;
    case tsf_initial:
      status.in_mode = initializationMode(&st);
      break;
    case tsf_mode:
      st = st >> 1;
      st &= 0x07;
      status.count_mode = countMode(st);
      break;
    case tsf_base:
      st &= 0x01;
      status.bcd = (st == 1);
  }

  //checks if it was possible to print the configuration to the user
  return timer_print_config(timer, field, status);
}
