#include "serial_port.h"

int ser_hook = 4;
static Queue *send_queue, *received_queue;

int ser_subscribe_int(uint8_t* bitno){
    *bitno = BIT(ser_hook);
    return sys_irqsetpolicy(COM1_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &ser_hook);
}

int ser_unsubscribe_int(){
    return sys_irqrmpolicy(&ser_hook);
}

void ser_init(){
    disable_ser_int();
    send_queue = new_queue();
    received_queue = new_queue();
}

void ser_exit(){
    free(send_queue->values);
    free(received_queue->values);
    free(send_queue);
    free(received_queue);
}

void ser_ih(){
    uint8_t byte;
    read_byte(&byte);
}

bool read_port(int port, uint8_t* val){
    return util_sys_inb(port + COM1, val);
}

bool write_to_port(int port, uint8_t val){
    return sys_outb(COM1 + port,val);
}

bool disable_ser_int(){
    uint8_t reg;
    read_port(IER,&reg);
    reg &= ~(IER_RECEIVED_DATA_AVAILABLE_INT | IER_TRANSMIT_HOLD_REG_EMPTY_INT | IER_RECEIVER_LINE_STATUS_INT | IER_MODEM_STATUS_INT);
    return write_to_port(IER, reg);
}

bool send_byte(uint8_t byte){
    uint8_t empty_transmitter;
    do{
        read_port(LSR, &empty_transmitter);
        empty_transmitter &= LSR_TRANSMIT_HOLD_REG_EMPTY;
    }while(!empty_transmitter);
    return write_to_port(THR,byte);
}

bool read_byte(){
    uint8_t reg, byte;
    read_port(LSR, &reg);
    if(reg & LSR_RECEIVER_DATA){
        read_port(RBR, &byte);
        if(OK == (reg & (LSR_OVERRUN_ERR | LSR_PARITY_ERR | LSR_FRAMING_ERR))){
            push(received_queue,byte);
            return OK;
        }else return !OK;
    }
    return !OK;
}

Queue* get_received_queue(){
    return received_queue;
}

Queue* get_send_queue(){
    return send_queue;
}
