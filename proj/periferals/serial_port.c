#include "serial_port.h"
#include <unistd.h>
#include <signal.h>

int ser_hook = 4;
static Queue *send_queue, *received_queue;
xpm_object *coop_cursor;
extern gameState gs;
extern bool in_coop;
extern bool finished;
static bool ualarm_set = false;

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
        printf("%d",res);
    if(res) return !OK;
    res = write_to_port(DIVISOR_LATCH_MSB,0);
            printf("%d",res);

    if(res) return !OK;
    res = write_to_port(DIVISOR_LATCH_LSB,1);
            printf("%d",res);

    if(res) return !OK;
    reg = (LCR_8_BITS_PER_CHAR | LCR_ODD_PARITY);
    
    res = write_to_port(LCR, reg);
            printf("%d",res);

    if(res) return !OK;
    printf("fully configured");
    return OK;
}

void ser_init(){
    clear_fifo();
    send_queue = new_queue();
    received_queue = new_queue();
    coop_cursor = malloc(sizeof(xpm_object));
    coop_cursor->x = 0;
    coop_cursor->y = 0;
    ser_config();
    signal(SIGALRM,setup_handler);
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
    printf("%x\n",reg);
    if(!(reg & IIR_NO_INT)) {
        if(reg & IIR_RECEIVED_DATA_AVAILABLE){
            printf("received info\n");
            while(OK == read_byte());
            read_port(IIR, &reg);
        }
        printf("%x!!!!!!!!!!",reg);
        if(reg & IIR_TRANSMIT_HOLD_REG_EMPTY){
            printf("UFE\n");
            send_bytes_in_queue();
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

void signal_handler(int signum){
   ualarm_set = true; 
   printf("alarm fired");
}

void setup_handler() {
    struct sigaction alarm;
    alarm.sa_handler = signal_handler;
    sigemptyset(&alarm.sa_mask);
    alarm.sa_flags = 0;
    sigaction(SIGALRM, &alarm, NULL);
}

bool send_byte(uint8_t byte){
    uint8_t empty_transmitter;
    push(send_queue,byte);
    read_port(LSR, &empty_transmitter);
    empty_transmitter &= LSR_TRANSMIT_HOLD_REG_EMPTY;
    if(!empty_transmitter) return false;
    while(OK != write_to_port(THR,front(send_queue)));
    printf("sent byte : %x\n", front(send_queue));
    if(front(send_queue) == 0xFE || front(send_queue) == 0xDE){
        pop(send_queue);
        return true;
    }
    bool timeout = false;

    alarm(1);

    while(!timeout){
        read_byte();
        uint8_t answer = pop(received_queue);
        if(answer == 0xFE){
            alarm(0);
            pop(send_queue);
            return true;
        }
         
        if(answer == 0xDE || ualarm_set){
            write_to_port(THR,front(send_queue));

            if(ualarm_set){
                printf("alarm");
                ualarm_set = false;
                alarm(1);
                timeout = true;
            }
        }
        if(answer == 0){
            continue;
        }
    }
    gs = INSTRUCTIONS;
    alarm(0);
    return false;
    
}

bool send_bytes_in_queue(){
    uint8_t empty_transmitter;
    if(is_empty(send_queue)) return false;
    while(!is_empty(send_queue)){

        read_port(LSR, &empty_transmitter);

        empty_transmitter &= LSR_TRANSMIT_HOLD_REG_EMPTY;

        if(!empty_transmitter) return false;

        while(OK != write_to_port(THR,front(send_queue)));

        printf("sent byte : %x\n", front(send_queue));

        if(front(send_queue) == 0xFE || front(send_queue) == 0xDE){
            pop(send_queue);
            return true;
        }
            bool timeout = false;

        alarm(1);

        while(!timeout){
            read_byte();
            uint8_t answer = pop(received_queue);
            if(answer == 0xFE){
                alarm(0);
                pop(send_queue);
                return true;
            }
            
            if(answer == 0xDE || ualarm_set){
                write_to_port(THR,front(send_queue));

                if(ualarm_set){
                    printf("alarm");
                    ualarm_set = false;
                    alarm(1);
                    timeout = true;
                }
            }
            if(answer == 0){
                continue;
            }
        }
        gs = INSTRUCTIONS;
        alarm(0);
        return false;
    }
    return true;
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

        while(OK != write_to_port(THR,front(send_queue)));
        printf("sent byte : %x\n", front(send_queue));
        if(front(send_queue) == 0xFE || front(send_queue) == 0xDE){
            pop(send_queue);
            printf("ack or nack sent");
            return true;
        }
        bool timeout = false;

        alarm(1);

        while(!timeout){
            read_byte();
            uint8_t answer = pop(received_queue);
            if(answer == 0xFE){
                alarm(0);
                pop(send_queue);
                return true;
            }
            
            if(answer == 0xDE || ualarm_set){
                write_to_port(THR,front(send_queue));

                if(ualarm_set){
                    printf("alarm");
                    ualarm_set = false;
                    alarm(1);
                    timeout = true;
                }
            }
            if(answer == 0){
                continue;
            }
        }
        gs = INSTRUCTIONS;
        alarm(0);
        return false;
    }
    return true;
}

bool read_byte(){
    uint8_t reg, byte;
    read_port(LSR, &reg);
    if(reg & LSR_RECEIVER_DATA){
        read_port(RBR, &byte);
        printf("received byte: %x\n", byte);
        if(OK == (reg & (LSR_OVERRUN_ERR | LSR_PARITY_ERR | LSR_FRAMING_ERR))){
            push(received_queue,byte);
            if(byte != 0xFE && byte != 0xDE) send_byte(0xFE);
            return OK;
        }else{
            if(byte != 0xFE && byte != 0xDE) send_byte(0xDE);
            return !OK;
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
        printf("\n%d",send_byte(0x56));
        printf("\n%d", send_byte(srandByte));
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
        set_power_up_alarm(1);
        set_enemy_throw(0xF);
        printf("srandByte = %d\n", srandByte);
        in_coop=true;
        send_byte(0x57);
    }else if(front(received_queue) == 0x57){
        printf("received5");
        gs = GAME;
        set_power_up_alarm(1);
        set_enemy_throw(0xF);
        in_coop = true;
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

    send_byte(send_code);
    send_byte(mostSignificantPartY);
    send_byte(xByte);
    send_byte(yByte);
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

    while(!is_empty(received_queue)){
        read_byte(); //just trying to retrieve another byte in case of it arriving after the ih or mid handling
        uint8_t curByte = pop(received_queue);
        if(curByte == 0xFF){
            gs = GAMEOVER;
            return;
        }
        if(curByte & BIT(7)){ //mouse packet
            uint8_t secondByte = pop(received_queue), thirdByte = pop(received_queue), fourthByte = pop(received_queue);
            printf("mouse");
            printf("1:%x, 2:%x, 3:%x, 4:%x\n", curByte, secondByte, thirdByte, fourthByte);
            int x = 0, y = 0;
            while(secondByte == 0){
                read_byte();
                secondByte = pop(received_queue);
                printf("second_byte");
            }
            printf("1:%x, 2:%x, 3:%x, 4:%x\n", curByte, secondByte, thirdByte, fourthByte);
            if(secondByte & BIT(5)) secondByte = 0;
            while(thirdByte == 0 && !(curByte & BIT(5))){
                read_byte();
                thirdByte = pop(received_queue);
                printf("third_byte");
            }
            printf("1:%x, 2:%x, 3:%x, 4:%x\n", curByte, secondByte, thirdByte, fourthByte);
            while(fourthByte == 0 && !(curByte & BIT(6))){
                read_byte();
                fourthByte = pop(received_queue);
            }
            
            printf("1:%x, 2:%x, 3:%x, 4:%x\n", curByte, secondByte, thirdByte, fourthByte);
            x |= thirdByte;
            x |= ((curByte & (BIT(0) | BIT(1) | BIT(2) | BIT(3))) << 8);

            y |= fourthByte;
            y |= ((secondByte & (BIT(0) | BIT(1) | BIT(2) | BIT(3))) << 8);

            coop_cursor->x = (curByte & BIT(4)) ? (~(0xFFF) | x) : (x);
            coop_cursor->y = (secondByte & BIT(4)) ? (~(0xFFF) | y) : (y);
            printf("x = %d, y = %d\n", coop_cursor->x, coop_cursor->y);
            struct packet pp;
            pp.lb = true;
            handle_mouse_packet(coop_cursor, &pp,  false);

        }else{ //scancode
            printf("kbd");
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


