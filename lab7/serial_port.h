#include <lcom/lcf.h>
#include "serial_port_controller.h"
#include "queue.h"

void ser_init();

void ser_exit();

bool read_port(int port, uint8_t* val);

bool write_to_port(int port, uint8_t val);

bool disable_ser_int();

bool send_byte(uint8_t byte);

bool read_byte();

Queue* get_received_queue();

Queue* get_send_queue();
