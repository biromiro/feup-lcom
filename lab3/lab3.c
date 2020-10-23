#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>
#include "i8042.h"
#include "kbc.h"

extern int counter, cnt;
extern uint8_t scancode;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {
  /* To be completed by the students */

  int ipc_status,r,i=0;
  uint8_t irq_set;
  message msg;
  bool first = false;
  uint8_t bytes[2];

  if(kbc_subscribe_int(&irq_set)!=0) {
    printf("Error subscribing timer\n");
    return 1;
  }

  while(scancode != KBC_BRK_ESC_KEY) { /* Run until it has exceeeded time*/
    /* Get a request message */
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */
          if (msg.m_notify.interrupts &irq_set) { /* subscribed interrupt */
            kbc_ih();
            if(scancode == KBC_2BYTE_CODE){
              if(first)
                first = false;
              else
                first = true;
              scancode = 0;
              continue;
            }
            if(first){
              bytes[i] = scancode;
              i++;
              continue;
            }
            bytes[i] = scancode;
            kbd_print_scancode(!(scancode & KBC_MSB_SCNCD),i+1,bytes);
            i=0;
          }
          break;
        default:
          break; /* no other notifications expected: do nothing */
      }
    } else { /* received a standard message, not a notification */
      /* no standard messages expected: do nothing */
    }
  }

  kbc_unsubscribe_int();
  kbd_print_no_sysinb(cnt);
  return 0;
}

int(kbd_test_poll)() {

  int i=0;
  bool first = false;
  uint8_t bytes[2];

  do{
    if(kbc_read_data(&scancode) != 0){
      tickdelay(micros_to_ticks(DELAY_US));
      continue;
    }
    if(scancode == KBC_2BYTE_CODE){
      if(first)
        first = false;
      else
        first = true;
      scancode = 0;
      continue;
    }
    if(first){
      bytes[i] = scancode;
      i++;
      continue;
    }
    bytes[i] = scancode;
    kbd_print_scancode(!(scancode & KBC_MSB_SCNCD),i+1,bytes);
    i=0;

  }while(scancode != KBC_BRK_ESC_KEY);

  kbc_restore_interrupts();
  kbd_print_no_sysinb(cnt);

  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
  int ipc_status,r,i=0;
  uint8_t irq_set_kbc, irq_set_timer;
  message msg;
  bool first = false;
  uint8_t bytes[2];

  if(timer_subscribe_int(&irq_set_timer)){
    printf("Error subscribing timer\n");
    return 1;
  }

  if(kbc_subscribe_int(&irq_set_kbc)!=0) {
    printf("Error subscribing keyboard\n");
    return 1;
  }

  while((scancode != KBC_BRK_ESC_KEY) && (counter/60.0) < n ) { /* Run until it has exceeeded time*/
    /* Get a request message */
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */
          if (msg.m_notify.interrupts &irq_set_timer) { /* subscribed interrupt */
            timer_int_handler();   /* process it */
          }
          if (msg.m_notify.interrupts &irq_set_kbc) { /* subscribed interrupt */
            kbc_ih();
            if(scancode == KBC_2BYTE_CODE){
              if(first)
                first = false;
              else
                first = true;
              scancode = 0;
              continue;
            }
            if(first){
              bytes[i] = scancode;
              i++;
              continue;
            }
            bytes[i] = scancode;
            kbd_print_scancode(!(scancode & KBC_MSB_SCNCD),i+1,bytes);
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

  kbc_unsubscribe_int();
  timer_unsubscribe_int();
  kbd_print_no_sysinb(cnt);

  return 0;
}
