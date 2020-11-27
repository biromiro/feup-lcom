// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/proj.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "i8042.h"
#include "i8254.h"
#include "kbc.h"
#include "timer.h"
#include "video_gr.h"

extern int counter, cnt;
extern uint8_t scancode, packetByte; 

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/proj/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/proj/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}
int (proj_main_loop)(int argc, char *argv[]) {
  int ipc_status, r, counter_mouse=0, x = 0, y = 0;
  uint8_t irq_set_mouse, irq_set_timer, irq_set_kbc, i=0;
  message msg;
  bool startPacket;
  struct packet pp;
  uint8_t bytes[2];
  struct xpm_object *objects = loadXPMs();  

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

  vg_init(0x14C);

  while(scancode != KBC_BRK_ESC_KEY) { /* Run until it has exceeeded time*/

    /* Get a request message */
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */
          if (msg.m_notify.interrupts & irq_set_timer){
            timer_int_handler();
            print_xpm(x,y,objects[0].map,&objects[0].img);
            if(OK != swap_buffer()){
                return 1;
            }
          }
          if (msg.m_notify.interrupts &irq_set_mouse) { /* subscribed interrupt */
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
          if (msg.m_notify.interrupts & irq_set_kbc){
            kbc_ih();
            if(scancode == KBC_2BYTE_CODE){
              bytes[i] = scancode;
              i++;
              continue;
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
          }
          break;
        default:
          break; /* no other notifications expected: do nothing */
      }
    } else { /* received a standard message, not a notification */
      /* no standard messages expected: do nothing */
    }
  }

  vg_exit();

  timer_unsubscribe_int();

  mouse_unsubscribe_int();

  mouse_write_cmd(MOUSE_DIS_DATA_REP);

  kbc_unsubscribe_int();

  return 0;
}
