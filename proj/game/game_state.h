#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <lcom/lcf.h>
#include "character_movement.h"
#include "enemies.h"
#include "magic_blast.h"
#include "../periferals/rtc.h"

/**
 * @brief Matches an RTC interrupt to a corresponding in game change
 * 
 * @param alarmInterrupt True if an Alarm interrupt is being handled, false otherwise
 */
void handle_rtc_ingame_changes(bool* alarmInterrupt);

uint8_t get_wave();

void reset_rtc_ingame_changes();

#endif