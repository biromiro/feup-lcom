// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "serial_port.h"
#include "kbc.h"

extern uint8_t scancode;
int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab7/g06/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab7/g06/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
  int ipc_status,r,i=0;
  uint8_t irq_set_kbc, irq_set_serial, irq_set_timer;
  message msg;
  uint8_t bytes[2];

  ser_init();

  if(kbc_subscribe_int(&irq_set_kbc)!=0) {
    printf("Error subscribing timer\n");
    return 1;
  }
  if(ser_subscribe_int(&irq_set_serial) != 0){
    printf("Error subscribing serial\n");
    return 1;
  }

  if(timer_subscribe_int(&irq_set_timer)!=0) {
    printf("Error subscribing timer\n");
    return 1;
  }

  ser_enable_int();

  while(scancode != KBC_BRK_ESC_KEY) { /* Run until it has exceeeded time*/
    /* Get a request message */
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */
          if (msg.m_notify.interrupts & irq_set_kbc) { /* subscribed interrupt */
            kbc_ih();
            if(scancode == KBC_2BYTE_CODE){
              bytes[i] = scancode;
              i++;
              continue;
            }
            bytes[i] = scancode;
            i=0;
            if(scancode == KBC_MK_A_KEY){
              send_byte(0x01);
            }
          }
          if (msg.m_notify.interrupts & irq_set_serial) {
              ser_ih();
              if(is_empty(get_received_queue())) printf("empty queue");
              while(!is_empty(get_received_queue())){
                 uint8_t val = pop(get_received_queue());
                 printf("value = %x\n", val);
                 send_byte(val+1);
              } 
          }
          if (msg.m_notify.interrupts & irq_set_timer) { /* subscribed interrupt */
            timer_int_handler();
          }
          break;
        default:
          break; /* no other notifications expected: do nothing */
      }
    } else { /* received a standard message, not a notification */
      /* no standard messages expected: do nothing */
    }
  }
  timer_unsubscribe_int();
   ser_unsubscribe_int();
    kbc_unsubscribe_int();


  ser_exit();
  return 0;
}

int(video_test_controller)() {
  ser_init();
  send_byte(0x3);
   send_byte(0x1);
  send_byte(0x2);
  send_byte(0x4);
  send_byte(0x5);
  empty_send_queue();
  ser_exit();
  return 0;
}
