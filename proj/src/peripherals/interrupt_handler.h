#include <lcom/lcf.h>

#include "kbc.h"
#include "timer.h"
#include "video_gr.h"
#include "rtc.h"
#include "serial_port.h"
#include "../game/game_state.h"
#include "../game/hud.h"
#include "../img/cursor.xpm"
#include "../img/background.xpm"
#include "../img/mainmenu.xpm"
#include "../img/mainmenu_start.xpm"
#include "../img/mainmenu_instructions.xpm"
#include "../img/mainmenu_coop.xpm"
#include "../img/mainmenu_exit.xpm"
#include "../img/instructionsmenu.xpm"
#include "../img/gameover.xpm"
#include "../img/coop.xpm"

/** @defgroup interrupt_handler interrupt_handler
 * @{
 *
 * Functions for using the interrupt handlers
 */

enum {MOUSE,TIMER,KBD,RTC,SER} typedef irq_type;

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

/**
 * @brief Reads the information on the serial port and handles it
 * 
 */
void ser_handler();

/**
 * @brief Resets all game assets for a possible restart
 * 
 */
void reset_game();
