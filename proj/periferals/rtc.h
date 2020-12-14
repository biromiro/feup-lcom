#pragma once

#include <lcom/lcf.h>
#include "i8042.h"

/**
 * @brief Enum defining the type of the RTC Interrupt
 */
enum {UPDATE, ALARM, PERIODIC} typedef interruptType;

/**
 * @brief Subscribes and enables RTC interrupts
 *
 * @param bit_no address of memory to be initialized with the
 *         bit number to be set in the mask returned upon an interrupt
 * @return Return 0 upon success and non-zero otherwise
 */
int (rtc_subscribe_int)(uint8_t* bit_no);

/**
 * @brief Unsubscribes RTC interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int (rtc_unsubscribe_int)();

/**
 * @brief Activates/Deactivates the given RTC Interrupt
 * 
 * @param interrupt the type of RTC Interrupt
 * @param value True to turn it on, False otherwise
 * 
 * @return Return 0 upon sucess and non-zero otherwise
 */
int set_rtc_interrupts(interruptType interrupt, bool value);

/**
 * @brief Waits until there is no active update on the RTC
 * 
 * @return Return 0 upon sucess and non-zero otherwise
 */
int (wait_until_finished_update)();

/**
 * @brief RTC Interrupt Handler
 */
void (rtc_ih)();

/**
 * @brief Disables RTC's updates
 * 
 * @return Return 0 upon sucess and non-zero otherwise
 */
int disable_update();

/**
 * @brief Enables RTC's updates
 * 
 * @return Return 0 upon sucess and non-zero otherwise
 */
int enable_update();

/**
 * @brief Reads the info of a given register and returns it throw the parameter
 * 
 * @param reg the register to read from
 * @param value the register to store the info in
 * 
 * @return Return 0 upon sucess and non-zero otherwise
 */
int (rtc_read_info)(uint8_t reg, uint8_t* value);

/**
 * @brief Sets up the alarm to power up in game
 * 
 * @param powerupDelay the relative time, in seconds, to set the alarm to
 */
void set_power_up_alarm(uint8_t powerupDelay);

/**
 * @brief Sets up the rate at which the periodic interrupts will send new enemies
 * 
 * @param div the 4 rate selection bits
 */
void set_enemy_throw(uint8_t div);

/**
 * @brief Converts a given BCD number to its corresponding decimal value
 * 
 * @return Returns the decimal value
 */
int (bcd_to_decimal)(uint8_t hex);

/**
 * @brief Converts a given decimal number to its corresponding BCD value
 * 
 * @return Returns the BCD value
 */
int (decimal_to_bcd)(uint8_t dec);

/**
 * @brief Updates date and time memory registers
 */
void (rtc_updater)();

/**
 * @brief Handles an alarm interrupt
 */
void (rtc_alarm)();

/**
 * @brief Handles a periodic interrupt
 */
void (rtc_periodic)();

/**
 * @brief Prints to the screen the current date and time
 */
char* (print_date)();
