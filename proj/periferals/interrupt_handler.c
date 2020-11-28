#include "interrupt_handler.h"

static uint8_t irq_set_mouse, irq_set_timer, irq_set_kbc, i=0;
extern int counter, cnt;
extern uint8_t scancode, packetByte;
bool finished;
static int ipc_status, r, counter_mouse=0, x = 0, y = 0;
static message msg;
static bool startPacket;
static struct packet pp;
static uint8_t bytes[2];
static struct xpm_object *objects;  

int subscribe_interrupts(){

  mouse_write_cmd(MOUSE_EN_DATA_REP);

  if(kbc_subscribe_int(&irq_set_kbc)!=0) {
    printf("Error subscribing kbc\n");
    return 1;
  }

  if(mouse_subscribe_int(&irq_set_mouse)!=0) {
    printf("Error subscribing mouse\n");
    return 1;
  }

  if(timer_subscribe_int(&irq_set_timer) != 0 ){
    printf("Error subscribing timer\n");
    return 1;
  }

  return 0;
}

int initialize(){
  if(OK != subscribe_interrupts()){
    printf("Could not subscribe all interrupts!\n");
    return 1;
  }
  vg_init(0x14C);

  objects = loadXPMs();
  return 0;
}

int unsubscribe_interrupts(){

  if(timer_unsubscribe_int() != 0 ){
    printf("Error unsubscribing timer\n");
    return 1;
  }

  if(mouse_unsubscribe_int()!=0) {
    printf("Error unsubscribing mouse\n");
    return 1;
  }

  mouse_write_cmd(MOUSE_DIS_DATA_REP);

  if(kbc_unsubscribe_int()!=0) {
    printf("Error unsubscribing kbc\n");
    return 1;
  }
  
  return 0;
}

int finish(){
  
  vg_exit();
  
  if(OK != unsubscribe_interrupts()){
    printf("Could not unsubscribe all interrupts!\n");
    return 1;
  }

  return 0;
}

uint8_t get_irq_set(irq_type type){
  switch(type){
    case MOUSE:
      return irq_set_mouse;
    case TIMER:
      return irq_set_timer;
    case KBD:
      return irq_set_kbc;
  }
}

void interrupt_call_receiver(){
  /* Get a request message */
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      return;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */
          if (msg.m_notify.interrupts & get_irq_set(TIMER)){
            timer_handler();
          }
          if (msg.m_notify.interrupts & get_irq_set(MOUSE)) { /* subscribed interrupt */
            mouse_handler();
          }
          if (msg.m_notify.interrupts & get_irq_set(KBD)){
            kbd_handler(); 
          }
          break;
        default:
          break; /* no other notifications expected: do nothing */
      }
    } else { /* received a standard message, not a notification */
      /* no standard messages expected: do nothing */
    }
    return;
}

void timer_handler(){
  timer_int_handler();
  print_xpm(x,y,objects[0].map,&objects[0].img);
  if(OK != swap_buffer()){
     printf("Unable to swap buffers!");
     finished = true;;
  }
}

void mouse_handler(){
  mouse_ih();
  if(packetByte & BIT(3)) {
    startPacket = true;
  }
  if(startPacket) {
    pp.bytes[counter_mouse] = packetByte;
    counter_mouse++;
  }
  if(counter_mouse == 3){
    mouse_parse_packet(&pp);
    mouse_print_packet(&pp);
    startPacket = false;
    counter_mouse = 0;
  }
  counter = 0;
}

void kbd_handler(){
  kbc_ih();
  if(scancode == KBC_2BYTE_CODE){
    bytes[i] = scancode;
    i++;
    return;
  }
  bytes[i] = scancode;
  kbd_print_scancode(!(scancode & KBC_MSB_SCNCD),i+1,bytes);
  if(scancode == KBC_D_KEY){
      x+=10;
  }else if(scancode == KBC_A_KEY){
      x-=10;
  }else if(scancode == KBC_S_KEY){
      y+=10;
  }else if(scancode == KBC_W_KEY){
      y-=10;
  }
  i=0;
  counter = 0;
  if(scancode == KBC_BRK_ESC_KEY) finished = true;
}
