#include "serial_port.h"
#include <unistd.h>
#include <signal.h>

int ser_hook = 4;
static Queue *send_queue, *received_queue, *packet_queue, *ack_queue;
xpm_object *coop_cursor;
extern gameState gs;
extern bool in_coop;
extern bool finished;
static bool hold_reg_empty = true;
bool swapped = false;
uint8_t pack_index = 0;

int ser_subscribe_int(uint8_t* bitno){
    *bitno = BIT(ser_hook);
    return sys_irqsetpolicy(COM1_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &ser_hook);
}

int ser_unsubscribe_int(){
    return sys_irqrmpolicy(&ser_hook);
}

int ser_config(){
    uint8_t reg;
    reg = (LCR_DLAB);
    bool res = write_to_port(LCR,reg);
        
    if(res) return !OK;
    res = write_to_port(DIVISOR_LATCH_MSB,0);

    if(res) return !OK;
    res = write_to_port(DIVISOR_LATCH_LSB,1);

    if(res) return !OK;
    reg = (LCR_8_BITS_PER_CHAR | LCR_ODD_PARITY);
    
    res = write_to_port(LCR, reg);

    if(res) return !OK;
    return OK;
}

void ser_init(){
    clear_fifo();
    send_queue = new_queue();
    received_queue = new_queue();
    ack_queue = new_queue();
    packet_queue = new_queue();
    coop_cursor = malloc(sizeof(xpm_object));
    coop_cursor->x = 0;
    coop_cursor->y = 0;
    ser_config();
}

bool ser_enable_int(){
    uint8_t reg;
    reg = (IER_RECEIVED_DATA_AVAILABLE_INT | IER_TRANSMIT_HOLD_REG_EMPTY_INT | IER_RECEIVER_LINE_STATUS_INT);
    return write_to_port(IER, reg);
}

void ser_exit(){
    disable_ser_int();
    free(send_queue->values);
    free(received_queue->values);
    free(send_queue);
    free(received_queue);
}

void ser_ih(){
    uint8_t reg;
    read_port(IIR, &reg);
    while(!(reg & IIR_NO_INT)) {
        if(reg & IIR_RECEIVED_DATA_AVAILABLE){
            while(OK == read_byte());
            read_port(IIR, &reg);
        }
        if(reg & IIR_TRANSMIT_HOLD_REG_EMPTY){
            send_bytes_in_queue();
            read_port(IIR, &reg);
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
    reg = (FCR_CLEAR_RCVR_FIFO | FCR_CLEAR_XMIT_FIFO | FCR_ENABLE_BOTH_FIFO);
    return write_to_port(FCR, reg);
}

bool send_byte(uint8_t byte){
    bool pushed = push(send_queue,byte);
    if(hold_reg_empty){
        return send_bytes_in_queue();
    }else return pushed;
    
}

bool send_bytes_in_queue(){
    if(is_empty(send_queue)){
        hold_reg_empty = true;
        return false;
    }

    uint8_t empty_transmitter;

    while(!is_empty(send_queue)){
        write_to_port(THR,front(send_queue));
        pop(send_queue);
        read_port(LSR, &empty_transmitter);
        empty_transmitter &= LSR_TRANSMIT_HOLD_REG_EMPTY;
        hold_reg_empty = empty_transmitter;
        if(!empty_transmitter) return false;
    }
    return true;
}

bool read_byte(){
    uint8_t reg, byte;
    read_port(LSR, &reg);
    if(reg & LSR_RECEIVER_DATA){
        read_port(RBR, &byte);
        if(OK == (reg & (LSR_OVERRUN_ERR | LSR_PARITY_ERR | LSR_FRAMING_ERR))){
            push(received_queue, byte);
            return OK;
        }
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

    if(front(received_queue) == 0x53){
        send_byte(0x54);
    }else if(front(received_queue) == 0x54){
        send_byte(0x55);
    }else if(front(received_queue) == 0x55){
        uint8_t srandByte = 0;
        while(srandByte == ACK || srandByte == NACK || srandByte == END || srandByte == 0){
            srandByte = rand();
        }
        send_byte(0x56);
        send_byte(srandByte);
        srandom(srandByte);
    }else if(front(received_queue) == 0x56){
        pop(received_queue);
        uint8_t srandByte = pop(received_queue);
        while(srandByte == 0){
            read_byte();
            srandByte = pop(received_queue);
        }
        srandom(srandByte);
        swap_characters();
        swapped = true;
        gs = GAME;
        set_power_up_alarm(1);
        set_enemy_throw(0xF);
        in_coop=true;
        send_byte(0x57);
    }else if(front(received_queue) == 0x57){
        gs = GAME;
        set_power_up_alarm(1);
        set_enemy_throw(0xF);
        in_coop = true;
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
}

void send_mouse_info(xpm_object* cursor){
    //communication protocol specified in the "handle_received_info" function
    uint8_t mostSignificantPartY = 0, xByte = 0, yByte = 0, send_code = BIT(7);
    send_code |= ((cursor->x & 0x1F00) >> 8);
    mostSignificantPartY = ((cursor->y & 0x1F00)>>8);
    if(mostSignificantPartY == 0) mostSignificantPartY = BIT(5);
    xByte = (cursor->x & (0xFF));
    yByte = (cursor->y & (0xFF));
    if(xByte == 0) send_code |= BIT(5);
    if(yByte == 0) send_code |= BIT(6);
    send_byte(send_code);
    send_byte(mostSignificantPartY);
    send_byte(xByte);
    send_byte(yByte);
}

void handle_received_info(){
    
    /*
    The communication protocol is as follows:

    If bit 7 is up, it is a mouse packet (and it is expected that 4 bytes arrive)
    If bit 7 isn't up, it is a scancode or END code (0x40).

    The composition of the mouse packet is changed: 
    
    -The first byte has the following meaning:

        BIT(0) through BIT(4) -> the 5 most significant bits of the X position
        BIT(5) -> if byte 3 is 0 (avoid queue pop unwanted events)
        BIT(6) -> if byte 4 is 0 (avoid queue pop unwanted events)
        BIT(7) -> if it is mouse info

    -The second byte has the following meaning:

        BIT(0) through BIT(4) -> the 5 most sigifnicant bits of the Y position
        BIT(5) -> if bits 0 through 4 are all 0 (avoid queue pop unwanted events)
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

    When a mouse byte is first received, the following 3 packets are, by the queue logic, also from the packet, so they aren't checked.

    If the byte 0x40 == END is received at any given time, the game is finished.
    */

   if(is_empty(received_queue)) return;
    while(!is_empty(received_queue)){
        uint8_t curByte = pop(received_queue);

        if(!is_empty(packet_queue)){
            push(packet_queue, curByte);
            if(packet_queue->numElements == 4)
                mount_packet();
            continue;
        }

        if(curByte == END){
            gs = GAMEOVER;
            return;
        }

        if(curByte & BIT(7)){ //mouse packet
            push(packet_queue, curByte);
        }else{ //scancode
            uint8_t scancode;
            bool invalid = false;
            if(curByte & BIT(0)) //A make code
                scancode = KBC_MK_A_KEY;
            else if(curByte & BIT(1)) //A Break code
                scancode = KBC_BRK_A_KEY;
            else if(curByte & BIT(2)) //D make code
                scancode = KBC_MK_D_KEY;
            else if(curByte & BIT(3)) //D break code
                scancode = KBC_BRK_D_KEY;
            else invalid = true;
            if(!invalid) handle_button_presses(scancode, false);
        }
    }
}

void mount_packet(){
    uint8_t firstByte = pop(packet_queue), secondByte = pop(packet_queue), thirdByte = pop(packet_queue), fourthByte = pop(packet_queue);
    int x = 0, y = 0;
    if(secondByte & BIT(5)) secondByte = 0;
    if(firstByte & BIT(5)) thirdByte = 0;
    if(firstByte & BIT(6)) fourthByte = 0;

    x |= thirdByte;
    x |= ((firstByte & (BIT(0) | BIT(1) | BIT(2) | BIT(3))) << 8);

    y |= fourthByte;
    y |= ((secondByte & (BIT(0) | BIT(1) | BIT(2) | BIT(3))) << 8);

    coop_cursor->x = (firstByte & BIT(4)) ? (~(0xFFF) | x) : (x);
    coop_cursor->y = (secondByte & BIT(4)) ? (~(0xFFF) | y) : (y);
    struct packet pp;
    pp.lb = true;
    handle_mouse_packet(coop_cursor, &pp,  false);
}
