// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "kbc.h"
#include "timer.h"

extern uint8_t packetByte;
extern int counter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (mouse_test_packet)(uint32_t cnt) {
  int ipc_status,r,counter_mouse=0;
  uint8_t irq_set;
  message msg;
  bool startPacket;
  struct packet pp;

  mouse_write_cmd(MOUSE_EN_DATA_REP);

  if(mouse_subscribe_int(&irq_set)!=0) {
    printf("Error subscribing timer\n");
    return 1;
  }

  while(cnt) { /* Run until it has exceeeded time*/
    /* Get a request message */
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */
          if (msg.m_notify.interrupts &irq_set) { /* subscribed interrupt */

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
              cnt--;
              counter_mouse = 0;
            }
          }
          break;
        default:
          break; /* no other notifications expected: do nothing */
      }
    } else { /* received a standard message, not a notification */
      /* no standard messages expected: do nothing */
    }
  }
  mouse_unsubscribe_int();
  mouse_write_cmd(MOUSE_DIS_DATA_REP);
  return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
  int ipc_status, r, counter_mouse=0;
  uint8_t irq_set_mouse, irq_set_timer;
  message msg;
  bool startPacket;
  struct packet pp;

  mouse_write_cmd(MOUSE_EN_DATA_REP);

  if(mouse_subscribe_int(&irq_set_mouse)!=0) {
    printf("Error subscribing timer\n");
    return 1;
  }

  if(timer_subscribe_int(&irq_set_timer) != 0 ){
    printf("Error subscribing timer\n");
    return 1;
  }

  while(counter/sys_hz() < idle_time) { /* Run until it has exceeeded time*/

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
          break;
        default:
          break; /* no other notifications expected: do nothing */
      }
    } else { /* received a standard message, not a notification */
      /* no standard messages expected: do nothing */
    }
  }

  timer_unsubscribe_int();
  mouse_unsubscribe_int();
  mouse_write_cmd(MOUSE_DIS_DATA_REP);
  return 0;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
  int ipc_status,r,counter_mouse=0;
  uint8_t irq_set;
  message msg;
  bool startPacket, gestureHandled = false;
  struct packet pp;
  struct mouse_ev* event;

  mouse_write_cmd(MOUSE_EN_DATA_REP);

  if(mouse_subscribe_int(&irq_set)!=0) {
    printf("Error subscribing timer\n");
    return 1;
  }

  while(!gestureHandled) { /* Run until it has exceeded time*/
    /* Get a request message */
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */
          if (msg.m_notify.interrupts &irq_set) { /* subscribed interrupt */
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

              event = mouse_detect_event(&pp);
              gestureHandled = mouse_handle_gesture(event,x_len,tolerance);

              mouse_print_packet(&pp);
              startPacket = false;
              counter_mouse = 0;
            }
          }
          break;
        default:
          break; /* no other notifications expected: do nothing */
      }
    } else { /* received a standard message, not a notification */
      /* no standard messages expected: do nothing */
    }
  }
  mouse_unsubscribe_int();
  mouse_write_cmd(MOUSE_DIS_DATA_REP);
  return 0;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
  bool startPacket;
  int counter_mouse = 0;
  struct packet pp;

  while(cnt){
    mouse_write_cmd(MOUSE_READ_DATA);
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
      cnt--;
    }
    tickdelay(micros_to_ticks(period*1000));
  }
  mouse_write_cmd(MOUSE_STREAM_MODE);
  mouse_write_cmd(MOUSE_DIS_DATA_REP);
  kbc_reset_commannd();
  return 0;
}
