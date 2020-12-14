#include "game_state.h"

//need boolean to double the range of the powerups (since the periodic interrupt of the RTC)
//greatest period is only 500ms (increasing it, as such, to 1s)
static int thrown = 0;
static int rtcPeriodicMultiplier = 6;
static uint8_t currentDiv = 0xF;

void handle_rtc_ingame_changes(bool* alarmInterrupt){
    if(*alarmInterrupt){
        *alarmInterrupt = false;
        set_power_up_alarm(15);
        set_enemy_throw(currentDiv);
        if(rtcPeriodicMultiplier > 3){
            rtcPeriodicMultiplier--;
        }else if(currentDiv > 0xC){
            currentDiv--;
            rtcPeriodicMultiplier = 6;
        }
    }
    if(thrown == 0){
        throw_enemies();
        thrown = rtcPeriodicMultiplier;
    }else thrown--;
}

