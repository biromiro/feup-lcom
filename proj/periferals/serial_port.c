#include "serial_port.h"

int ser_hook = 4;
static Queue *send_queue, *received_queue;
xpm_object *coop_cursor;
extern gameState gs;
extern bool coop;

int ser_subscribe_int(uint8_t* bitno){
    *bitno = BIT(ser_hook);
    return sys_irqsetpolicy(COM1_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &ser_hook);
}

int ser_unsubscribe_int(){
    return sys_irqrmpolicy(&ser_hook);
}

void ser_init(){
    disable_ser_int();
    clear_fifo();
    send_queue = new_queue();
    received_queue = new_queue();
    coop_cursor = malloc(sizeof(xpm_object));
    coop_cursor->x = 0;
    coop_cursor->y = 0;
}

bool ser_enable_int(){
    uint8_t reg;
    reg = (IER_RECEIVED_DATA_AVAILABLE_INT);
    return write_to_port(IER, reg);
}

void ser_exit(){
    free(send_queue->values);
    free(received_queue->values);
    free(send_queue);
    free(received_queue);
}

void ser_ih(){
    uint8_t reg,byte;
    read_port(IIR, &reg);
    if(!(reg & IIR_NO_INT)) {
        switch( reg &  IIR_ID) {
            case IIR_RECEIVED_DATA_AVAILABLE:
                if(read_byte(&byte) == 0)
                    push(received_queue, byte);
                break;
            case IIR_FIFO_CHAR_TIMEOUT:
                while(OK == read_byte());
        }
    }
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

bool clear_fifo(){
    uint8_t reg;
    read_port(FCR, &reg);
    reg |= (FCR_CLEAR_RCVR_FIFO | FCR_CLEAR_XMIT_FIFO | FCR_ENABLE_BOTH_FIFO);
    return write_to_port(FCR, reg);
}

bool send_byte(uint8_t byte){
    uint8_t empty_transmitter, maxTries = 5;
    push(send_queue,byte);
    do{
        read_port(LSR, &empty_transmitter);
        empty_transmitter &= LSR_TRANSMIT_HOLD_REG_EMPTY;
        maxTries--;
    }while(empty_transmitter && maxTries);
    if(!empty_transmitter) return false;
    return write_to_port(THR,pop(send_queue));
}

bool pack_byte_send_queue(uint8_t byte){
    return !push(send_queue,byte);
}

bool empty_send_queue(){
    uint8_t empty_transmitter;
    while(!is_empty(send_queue)){
        do{
           read_port(LSR, &empty_transmitter);
           empty_transmitter &= LSR_TRANSMIT_HOLD_REG_EMPTY;
        }while(!empty_transmitter);
        if(!OK == write_to_port(THR,pop(send_queue))) return false;
    }
    return true;
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

bool ser_clear(){
    clear_fifo();
    while(pop(received_queue) != 0);
    return true;
}

bool handle_coop_start(){
    //clock_t start, end;
    //double time_spent;
    if(front(received_queue) == 0x53){
        printf("received1");
        ser_clear();
        send_byte(0x54);
    }else if(front(received_queue) == 0x54){
        printf("received2");
        ser_clear();
        send_byte(0x55);
    }else if(front(received_queue) == 0x55){
        printf("received3");
        uint8_t srandByte = time(NULL);
        send_byte(0x56);
        send_byte(srandByte);
        empty_send_queue();
        srandom(srandByte);
        printf("srandByte (sender) = %d\n", srandByte);
    }else if(front(received_queue) == 0x56){
        printf("received4");
        pop(received_queue);
        uint8_t srandByte = pop(received_queue);
        while(srandByte == 0){
            read_byte();
            srandByte = pop(received_queue);
        }
        srandom(srandByte);
        swap_characters();
        gs = GAME;
        printf("srandByte = %d\n", srandByte);
        coop=true;
        send_byte(0x57);
        micro_delay(3*8333);
    }else if(front(received_queue) == 0x57){
        printf("received5");
        gs = GAME;
        coop = true;
        printf("started game");
    }
    pop(received_queue);
    return true;
}

void send_scancode(uint8_t scancode){
    //communication protocol specified in the "handle_received_info" function
    uint8_t send_code = 0;
    if(scancode == KBC_MK_A_KEY){
        send_code |= BIT(0);
    }else if(scancode == KBC_BRK_A_KEY){
        send_code |= BIT(1);
    }else if(scancode == KBC_MK_D_KEY){
        send_code |= BIT(2);
    }else if(scancode == KBC_BRK_D_KEY){
        send_code |= BIT(3);
    }
    send_byte(send_code);
    empty_send_queue();
    printf("sent scancode!\n");
}

void send_mouse_info(xpm_object* cursor){
    //communication protocol specified in the "handle_received_info" function
    uint8_t mostSignificantPartY = 0, xByte = 0, yByte = 0, send_code = BIT(7);
    if(cursor->x == 0) send_code |= BIT(5);
    if(cursor->y == 0) send_code |= BIT(6);
    send_code |= ((cursor->x & 0x1F00) >> 8);
    mostSignificantPartY = ((cursor->y & 0x1F00)>>8);
    if(mostSignificantPartY == 0) mostSignificantPartY = BIT(5);
    xByte = (cursor->x & (0xFF));
    yByte = (cursor->y & (0xFF));

    pack_byte_send_queue(send_code);
    pack_byte_send_queue(mostSignificantPartY);
    pack_byte_send_queue(xByte);
    pack_byte_send_queue(yByte);
    empty_send_queue(); 
    printf("sent mouse info!, x = %d, y=%d, 1: %x, 2:%x, 3:%x, 4:%x\n", cursor->x, cursor->y, send_code, mostSignificantPartY, xByte, yByte);
}

void handle_received_info(){

    /*
    The communication protocol is as follows:

    If bit 7 is up, it is a mouse packet (and it is expected that 4 bytes arrived.)
    If bit 7 isn't up, it is a scancode.

    The composition of the mouse packet is changed: 
    
    -The first byte has the following meaning:

        BIT(0) through BIT(4) -> the 5 most significant bits of the X position
        BIT(5) -> if byte 3 is 0
        BIT(6) -> if byte 4 is 0
        BIT(7) -> if it is mouse info

    -The second byte has the following meaning:

        BIT(0) through BIT(4) -> the 4 most sigifnicant bits of the Y position
        BIT(5) -> if the 4 most significant bits are all 0
        BIT(6) and BIT(7) -> unused

    -The third and fourth bytes represent the same:
        
        Byte 3 -> 8 least significant bits of the X position
        Byte 4 -> 8 least significant bits of the Y position

    It must be remembered that the mouse packet is only sent when the lb is pressed (as it is only important to know
    when the co-op player sent a magic blast), and that is assumed by the info receiver.

    The composition of a scancode is also changed, and is as follows:

    BIT(0) up -> A make code
    BIT(1) up -> A break code
    BIT(2) up -> D make code
    BIT(3) up -> D break code
    BITS 4 through 6 -> unused
    BIT(7) -> it is a scancode
    */
   printf("received info!\n");

    while(!is_empty(received_queue)){
        read_byte(); //just trying to retrieve another byte in case of it arriving after the ih or mid handling
        uint8_t curByte = pop(received_queue);
        if(curByte & BIT(7)){ //mouse packet
            uint8_t secondByte = pop(received_queue), thirdByte = pop(received_queue), fourthByte = pop(received_queue);
            int x = 0, y = 0;
            while(secondByte == 0){
                read_byte();
                secondByte = pop(received_queue);
            }
            if(secondByte & BIT(5)) secondByte = 0;
            while(thirdByte == 0 && !(curByte & BIT(5))){
                read_byte();
                thirdByte = pop(received_queue);
            }
            while(fourthByte == 0 && !(curByte & BIT(6))){
                read_byte();
                thirdByte = pop(received_queue);
            }

            printf("1:%x, 2:%x, 3:%x, 4:%x\n", curByte, secondByte, thirdByte, fourthByte);
            x |= thirdByte;
            x |= ((curByte & (BIT(0) | BIT(1) | BIT(2) | BIT(3))) << 8);

            y |= fourthByte;
            y |= ((secondByte & (BIT(0) | BIT(1) | BIT(2) | BIT(3))) << 8);

            coop_cursor->x = (curByte & BIT(4)) ? (~x + 1) : (x);
            coop_cursor->y = (secondByte & BIT(4)) ? (~y + 1) : (y);
            printf("x = %d, y = %d\n", coop_cursor->x, coop_cursor->y);
            struct packet pp;
            pp.lb = true;
            handle_mouse_packet(coop_cursor, &pp,  false);
        }else{ //scancode
            uint8_t scancode;
            bool invalid = false;
            if(curByte & BIT(0)) //A make code
               scancode = KBC_MK_A_KEY;
            else if(curByte & BIT(1))
                scancode = KBC_BRK_A_KEY;
            else if(curByte & BIT(2))
                scancode = KBC_MK_D_KEY;
            else if(curByte & BIT(3))
                scancode = KBC_BRK_D_KEY;
            else invalid = true;
            if(!invalid) handle_button_presses(scancode, false);
        }
    }
}


