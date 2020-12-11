#include <lcom/lcf.h>
#include "i8042.h"

enum {UPDATE, ALARM, PERIODIC} typedef interruptType;

int (rtc_subscribe_int)(uint8_t* bit_no);

int (rtc_unsubscribe_int)();

int set_rtc_interrupts(interruptType interrupt, bool value);

int (wait_until_finished_update)();

void (rtc_ih)();

int (rtc_read_info)(uint8_t reg, uint8_t* value);

int (bcd_to_decimal)(uint8_t hex);

void (rtc_updater)();

void (rtc_alarm)();

void (rtc_periodic)();

char* (print_date)();
