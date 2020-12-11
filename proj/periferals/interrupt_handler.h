#include <lcom/lcf.h>

#include "kbc.h"
#include "timer.h"
#include "video_gr.h"
#include "rtc.h"
#include "../game/character_movement.h"
#include "../game/magic_blast.h"
#include "../game/enemies.h"
#include "../img/cursor.xpm"
#include "../img/background.xpm"

enum {MOUSE,TIMER,KBD,RTC} typedef irq_type;

/**
 * @brief Subscribes all the peripherals
 * Timer, Keyboard and Timer
 * @return 0 on success, 1 on error
 */
int subscribe_interrupts();

/**
 * @brief Calls the functions to set up the program
 * 
 * @return 0 on success 
 */
int initialize();
/**
 * @brief Unsubscribes all the peripherals
 * 
 * @return 0 on success, 1 on error
 */
int unsubscribe_interrupts();

/**
 * @brief Calls the functions to terminate the program
 * 
 * @return 0 on success, 1 on error
 */
int finish();

/**
 * @brief Get the irq set, based on the type of interruption
 * 
 * @param type Type of interruption
 * @return interrupt
 */
uint8_t get_irq_set(irq_type type);

/**
 * @brief Interrupt receiver
 * 
 */
void interrupt_call_receiver();

/**
 * @brief Updates the screen every frame
 * 
 */
void timer_handler();

/**
 * @brief Builds the mouse packet and handles it
 * 
 */
void mouse_handler();

/**
 * @brief Builds the keyboard packet and handles it
 * 
 */
void kbd_handler();

/**
 * @brief Handles the real time clock 
 * 
 */
void rtc_handler();
