//
// Created by biromiro on 23/10/20.
//

#ifndef LAB3_KBC_H
#define LAB3_KBC_H
#include <lcom/lcf.h>
#include "i8042.h"
#include <minix/sysutil.h>

#define DELAY_US 20000

enum state{
  INITIAL,
  FIRST_SLOPE,
  VERTEX,
  SECOND_SLOPE,
  COMPLETE
};

/**
 * @brief Subscribes and enables Timer 0 interrupts
 *
 * @param bit_no address of memory to be initialized with the
 *         bit number to be set in the mask returned upon an interrupt
 * @return Return 0 upon success and non-zero otherwise
 */
int (timer_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unsubscribes Timer 0 interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int (timer_unsubscribe_int)();

/**
 * @brief Timer 0 interrupt handler
 *
 * Increments tick counter
 */
void (timer_int_handler)();

/**
 * @brief Subscribes and enables KBC interrupts
 *
 * @param bit_no address of memory to be initialized with the
 *         bit number to be set in the mask returned upon an interrupt
 * @return Return 0 upon success and non-zero otherwise
 */
int (kbc_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unsubscribes KBC interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int (kbc_unsubscribe_int)();

/**
 * @brief Reads the KBC configuration (status)
 *
 * @param st    Address of memory position to be filled with the kbc config
 * @return Return 0 upon success and non-zero otherwise
 */
int (kbc_get_status)(uint8_t *st);

/**
 * @brief Reads the output buffer of KBC
 *
 * @param port  Buffer to read
 * @param data    Address of memory position to be filled with the output buffer byte
 * @return Return 0 upon success and non-zero otherwise
 */
int (kbc_read_buffer)(int port, uint8_t *data);

/**
 * @brief Writes (issues) a command to the KBC
 *
 * @param port Buffer to write
 * @param cmd The command to output to the KBC
 * @return Return 0 upon success and non-zero otherwise
 */
int (kbc_write_cmd)(int port, uint8_t cmd);

/**
 * @brief Reads the value of the OBF of the KBC
 *
 * @param data The variable to store the read data
 * @return Return 0 upon success and non-zero otherwise
 */
int (kbc_read_data)(uint8_t* data);

/**
 * @brief Restores the Minix Interrupt Handler for KBC
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int (kbc_restore_interrupts)();

/**
 * @brief Resets the KBC to Minix's default
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int (kbc_reset_commannd)();

/**
 * @brief Subscribes mouse interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int (mouse_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unsubscribes mouse interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int (mouse_unsubscribe_int)();

/**
 * @brief Reads a data byte from the mouse controller
 *
 * @param data the pointer to which the data is to be returned by
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int (mouse_read_data)(uint8_t *data);


/**
 * @brief Writes a mouse command to the controller
 *
 * @param cmd the command to write
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int (mouse_write_cmd)(uint8_t cmd);

/**
 * @brief Handles a V shaped mouse gesture
 *
 * @param event defines the current state of the gesture
 * @param x_len the length of the gesture
 * @param tolerance the maximum deviation the gesture can have
 * 
 * @return Returns true upon completion
 */
bool (mouse_handle_gesture)(struct mouse_ev *event,uint8_t x_len, uint8_t tolerance);

/**
 * @brief Parses a given packet for clearer output
 * 
 * @param pp the packet to parse
 */
void (mouse_parse_packet)(struct packet* pp);

#endif //LAB3_KBC_H
