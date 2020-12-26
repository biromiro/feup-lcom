#include <lcom/lcf.h>
#include "serial_port_controller.h"
#include "queue.h"

void ser_init();

int ser_subscribe_int(uint8_t* bitno);

int ser_unsubscribe_int();

void ser_exit();

void ser_ih();

bool ser_enable_int();

bool read_port(int port, uint8_t* val);

bool write_to_port(int port, uint8_t val);

bool disable_ser_int();

bool send_byte(uint8_t byte);

bool empty_send_queue();

bool read_byte();

Queue* get_received_queue();

Queue* get_send_queue();
