#include "rtc.h"
#include "i8042.h"

int hook_rtc = 3;

static uint8_t hour = 0, minute = 0, second = 0, day_of_the_week_val = 0, day_month = 0, month_val = 0, year8bit = 0;
static uint32_t  year = 0;
static char day_of_the_week[9];
static char month[9];
static char date[] = "Wednesday, 31 of September of 2000, 23:59:59";

char * days_of_the_week[7] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
char * months[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

int (rtc_subscribe_int)(uint8_t *bit_no) {
  *bit_no=BIT(hook_rtc);
  return sys_irqsetpolicy(RTC_IRQ, IRQ_REENABLE, &hook_rtc);
}

int (rtc_unsubscribe_int)() {
  return sys_irqrmpolicy(&hook_rtc);
}

int (wait_until_finished_update)(){

  bool first = true;
  uint8_t reg;

  do{
      if(!first) tickdelay(micros_to_ticks(UPDATE_TIME_MICRO));
      if( OK != sys_outb(RTC_ADDR_REG,RTC_REG_A)) return !OK;
      if( OK != util_sys_inb(RTC_DATA_REG,&reg)) return !OK;
      first = false;
  }while(reg & RTC_UIP);

  return OK;
}

int set_interrupts(interruptType interrupt, bool value){
    uint8_t reg;
    sys_outb(RTC_ADDR_REG, RTC_REG_B);

    switch(interrupt){
        case UPDATE:
            util_sys_inb(RTC_DATA_REG, &reg);
            if(value) reg |= RTC_UIE;
            sys_outb(RTC_DATA_REG, reg);
            break;
        case ALARM:
            util_sys_inb(RTC_DATA_REG, &reg);
            if(value) reg |= RTC_AIE;
            sys_outb(RTC_DATA_REG, reg);
            break;
        case PERIODIC:
            util_sys_inb(RTC_DATA_REG, &reg);
            if(value) reg |= RTC_PIE;
            sys_outb(RTC_DATA_REG, reg);
            break;
    }
    return 0;
}


void (rtc_ih)(){
    uint8_t reg;

    sys_outb(RTC_ADDR_REG, RTC_REG_C);
    int interrupt_called = util_sys_inb(RTC_DATA_REG, &reg);

    if(interrupt_called & RTC_UF)
       rtc_updater();
    
    if(interrupt_called & RTC_AF)
        rtc_alarm();
    
    if(interrupt_called & RTC_PF)
        rtc_periodic();
}

int (rtc_read_info)(uint8_t reg, uint8_t* value){
    if(OK != wait_until_finished_update()){
        printf("Could not wait for the end of update!");
        return !OK;
    }
    if(OK != sys_outb(RTC_ADDR_REG, reg)){
        printf("Could not write to rtc's address register!");
        return !OK;
    }
    if( OK != util_sys_inb(RTC_DATA_REG, value)){
        printf("Could not read from rtc's data register!");
        return !OK;
    };

    return OK;
}

int (bcd_to_decimal)(uint8_t hex){
    if((((hex & 0xF0) >> 4) >= 10) || ((hex & 0x0F) >= 10)){
        printf("Invalid BCD value!");
        return !OK;
    }
    return ((hex & 0xF0) >> 4) * 10 + (hex & 0x0F);

}

void (rtc_updater)(){


    if(rtc_read_info(RTC_REG_HOUR, &hour) || rtc_read_info(RTC_REG_MIN, &minute) || rtc_read_info(RTC_REG_SEC, &second)){
        printf("Could not update the time!");
    }

    hour = bcd_to_decimal(hour);
    minute = bcd_to_decimal(minute);
    second = bcd_to_decimal(second);

    if(rtc_read_info(RTC_REG_DAY_WEEK, &day_of_the_week_val) || rtc_read_info(RTC_REG_DAY_MONTH, &day_month) || 
    rtc_read_info(RTC_REG_MONTH,&month_val) || rtc_read_info(RTC_REG_YEAR, &year8bit)){
        printf("Could not update the time!");
    }

    day_month = bcd_to_decimal(day_month);
    sprintf(month, "%s", months[bcd_to_decimal(month_val)-1]);
    year = bcd_to_decimal(year8bit) + 2000;
    sprintf(day_of_the_week, "%s", days_of_the_week[bcd_to_decimal(day_of_the_week_val)-1]);
}

void (rtc_alarm)(){
    wait_until_finished_update();
}

void (rtc_periodic)(){
    wait_until_finished_update();
}

char* (print_date)(){
    char day_ending[3];
    if(day_month == 1 || day_month == 21 || day_month == 31) sprintf(day_ending, "st");
    else if(day_month == 2 || day_month == 22) sprintf(day_ending, "nd");
    else if(day_month == 3 || day_month == 23) sprintf(day_ending, "rd");
    else sprintf(day_ending, "th");
    sprintf(date, "%s, %d%s of %s, %d, %d:%d:%d\n",day_of_the_week, day_month, day_ending, month, year, hour, minute, second);
    return date;
}
