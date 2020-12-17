#include "game_state.h"

static int thrown = 0;
static int rtcPeriodicMultiplier = 6;
static uint8_t currentDiv = 0xF;

void handle_rtc_ingame_changes(bool* alarmInterrupt){
    if(*alarmInterrupt){

        *alarmInterrupt = false;
        set_power_up_alarm(15); //next rate change will be in 15 seconds
        set_enemy_throw(currentDiv); 

        /*
        * Enemy wave rate generator
        * 
        * Due to the RTC's low periodic interrupt range, added another parameter "rtcPeriodicMUltiplier",
        * allowing a bigger time range (instead of the 500ms max in the RTC, 6*500 = 3s is the new max)
        * As new alarm interrupts arrive, this function decreases the interval between periodic interrupts
        * for a bigger enemy spawning frequency (increased difficulty).
        */
        if(rtcPeriodicMultiplier > 3){
            rtcPeriodicMultiplier--;
        }else if(currentDiv > 0xC){
            currentDiv--;
            rtcPeriodicMultiplier = 6;
        }
    }

    // Only sends the new enemy if the rtcPeriodMultiplier is 0, and resets it (working as a new timer, of sorts)    
    if(thrown == 0){
        throw_enemies();
        thrown = rtcPeriodicMultiplier;
    }else thrown--;
}

