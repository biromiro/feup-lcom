#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>
#include "i8042.h"
#include "kbc.h"

extern int counter, cnt;
extern uint8_t scancode;
extern bool make;

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
              bytes[i] = KBC_2BYTE_CODE;
              i++;
              continue;
            }
            if((scancode & KBC_MSB_SCNCD) == 0){
              make = false;
            }
            bytes[i] = scancode;
            kbd_print_scancode(make,i,bytes);
            i=0;
            make = true;
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
  kbc_restore_interrupts();

  return 1;
}

int(kbd_test_poll)() {
  /* To be completed by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int(kbd_test_timed_scan)(uint8_t n) {
  /* To be completed by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}
