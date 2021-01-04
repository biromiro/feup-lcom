#include <lcom/lcf.h>
#include <time.h>
#include "serial_port_controller.h"
#include "queue.h"
#include "rtc.h"
#include "../game/character_movement.h"


/** @defgroup serial_port serial_port
 * @{
 *
 * Functions for using the serial port
 */

/**
 * @brief initializes the serial port
 * 
 */
void ser_init();

/**
 * @brief configs the serial port communication settings
 * 
 * @return 0 upon success, 1 otherwise
 */
int ser_config();

/**
 * @brief subscribes the serial port interrupts
 * 
 * @param bitno the bit to set to identify this interrupt
 * @return 0 upon success, 1 otherwise 
 */
int ser_subscribe_int(uint8_t* bitno);

/**
 * @brief unsubscribes the serial port interrupts
 * 
 * @return 0 upon success, 1 otherwise
 */
int ser_unsubscribe_int();

/**
 * @brief Exits and restores the serial port
 * 
 */
void ser_exit();

/**
 * @brief Handles a serial port interrupt
 * 
 */
void ser_ih();

/**
 * @brief Handles an alarm of the unistd.h library
 * 
 */
void signal_handler(int signum);

/**
 * @brief Sets the up alarm handler of the unistd.h library
 * 
 */
void setup_handler();

/**
 * @brief Enables the serial port interrupts
 * 
 * @return true if successful, false otherwise
 */
bool ser_enable_int();
/**
 * @brief Reads the value of a given serial port port, adding its base address
 * 
 * @param port the port to read
 * @param val the pointer to store the value
 * @return true if successful, false otherwise
 */
bool read_port(int port, uint8_t* val);

/**
 * @brief Writes to a given port(adding its base address) a given value
 * 
 * @param port the port to write
 * @param val the value to write
 * @return true upon success, false otherwise
 */
bool write_to_port(int port, uint8_t val);

/**
 * @brief Disables the serial port interrupts
 * 
 * @return True if successful, false otherwise
 */
bool disable_ser_int();

/**
 * @brief clears the serial port fifos
 * 
 * @return True if successful, false otherwise
 */
bool clear_fifo();

/**
 * @brief Tries to send a byte to the serial port transmitter holding register, stores it in sending queue if not possible
 * 
 * @param byte the byte to send
 * @return True if successful, false otherwise 
 */
bool send_byte(uint8_t byte);

/**
 * @brief Tries to send all bytes in queue to serial port transmitter holding register until not possible
 * 
 * @return True if successful, false otherwise 
 */
bool send_bytes_in_queue();

/**
 * @brief Adds a byte to the send queue stack
 * 
 * @param byte the byte to add
 * @return True if successful, false otherwise 
 */
bool pack_byte_send_queue(uint8_t byte);

/**
 * @brief Sends all bytes in queue to serial port transmitter holding register (busy waiting)
 * 
 * @return True if successful, false otherwise 
 */
bool empty_send_queue();

/**
 * @brief Reads a byte from the serial port receiver buffer register
 * 
 * @param in_timeout true if the read byte call is in the timeout loop
 * 
 * @return True if successful, false otherwise 
 */
bool read_byte(bool in_timeout);

/**
 * @brief Get the received queue object
 * 
 * @return the received queue 
 */
Queue* get_received_queue();

/**
 * @brief Get the send queue object
 * 
 * @return the send queue 
 */
Queue* get_send_queue();

/**
 * @brief clears all fifos (ser port queues and program queues)
 * 
 * @return True if successful, false otherwise 
 */
bool ser_clear();

/**
 * @brief Handles the co-op game start, syncing both players
 * 
 * @return True if successful, false otherwise 
 */
bool handle_coop_start();

/**
 * @brief Sends a scancode to the other player
 * 
 * @param scancode the scancode to send 
 */
void send_scancode(uint8_t scancode);

/**
 * @brief Sends the needed mouse info upon a magic blast throw to the other player
 * 
 * @param cursor the cursor of local the player
 */
void send_mouse_info(xpm_object* cursor);

/**
 * @brief Received info (either mouse or kbd) handler
 * 
 */
void handle_received_info();

/**
 * @brief Mounts the received packet bytes
 * 
 */
void mount_packet();
