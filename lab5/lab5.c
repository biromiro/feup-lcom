// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

extern uint8_t scancode;
extern int counter;
// Any header files included below this line should have been created by you
#include "kbc.h"
#include "video_gr.h"

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

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
  
  vg_init(mode);
  sleep(delay);
  vg_exit();

  return 1;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {
  
  int ipc_status,r,i=0;
  uint8_t irq_set;
  message msg;
  uint8_t bytes[2];

  if(kbc_subscribe_int(&irq_set)!=0) {
    printf("Error subscribing timer\n");
    return 1;
  }

  vg_init(mode);


  vg_draw_rectangle(x, y, width, height, color);

  if(OK != swap_buffer()){
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
              bytes[i] = scancode;
              i++;
              continue;
            }
            bytes[i] = scancode;
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
  vg_exit();
  kbc_unsubscribe_int();

  return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  int ipc_status,r,i=0;
  uint8_t irq_set;
  message msg;
  uint8_t bytes[2];

  if(kbc_subscribe_int(&irq_set)!=0) {
    printf("Error subscribing timer\n");
    return 1;
  }

  vg_init(mode);

  vg_print_matrix(mode == 0x105, no_rectangles,first,step);
  
  if(OK != swap_buffer()){
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
              bytes[i] = scancode;
              i++;
              continue;
            }
            bytes[i] = scancode;
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
  vg_exit();
  kbc_unsubscribe_int();

  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  int ipc_status,r,i=0;
  uint8_t irq_set;
  message msg;
  uint8_t bytes[2];
  xpm_image_t img;
  if(kbc_subscribe_int(&irq_set)!=0) {
    printf("Error subscribing timer\n");
    return 1;
  }

  vg_init(0x105);

  uint8_t* map = loadXPM(xpm, XPM_INDEXED, &img);

  print_xpm(x,y,map,&img);

  if(OK != swap_buffer()){
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
              bytes[i] = scancode;
              i++;
              continue;
            }
            bytes[i] = scancode;
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
  vg_exit();
  kbc_unsubscribe_int();

  return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {
  
  int ipc_status,r,i=0,frameCounter=0;
  uint8_t kbc_irq, timer_irq;
  message msg;
  uint8_t bytes[2];
  uint16_t interruptFrames = sys_hz()/fr_rate;
  xpm_image_t img;

  if(kbc_subscribe_int(&kbc_irq)!=0) {
    printf("Error subscribing keyboard\n");
    return 1;
  }

  if(timer_subscribe_int(&timer_irq)!=0) {
    printf("Error subscribing timer\n");
    return 1;
  }
  vg_init(0x105);
  uint8_t* map = loadXPM(xpm, XPM_INDEXED, &img);

  print_xpm(xi,yi,map,&img);

  if(OK != swap_buffer()){
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
          if (msg.m_notify.interrupts &kbc_irq) { /* subscribed interrupt */
            kbc_ih();
            if(scancode == KBC_2BYTE_CODE){
              bytes[i] = scancode;
              i++;
              continue;
            }
            bytes[i] = scancode;
            i=0;
          }
          if (msg.m_notify.interrupts &timer_irq) { /* subscribed interrupt */
            timer_int_handler();
            if(yi>yf || xi>xf)break;
            if(counter%interruptFrames==0){
              frameCounter++;
              if(speed>0){
                erase_xpm(xi,yi,map,&img);
                if(xi==xf){
                  if(yi+speed>yf)
                    yi=yf;
                  else
                    yi+=speed;
                }
                else if(yi==yf){
                  if(xi+speed>xf)
                    xi=xf;
                  else
                    xi+=speed;
                }
                
                
                print_xpm(xi,yi,map,&img);
              }
              else{
                if(frameCounter==abs(speed)){
                  erase_xpm(xi,yi,map,&img);
                  if(xi==xf){
                    if(yi+1>yf)
                      yi=yf;
                    else
                      yi++;
                  }
                  else if(yi==yf){
                    if(xi+1>xf)
                      xi=xf;
                    else
                      xi++;
                  }
                  
                  print_xpm(xi,yi,map,&img);
                  frameCounter=0;
                }
              }
              if(OK != swap_buffer()){
                return 1;
              }
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
  vg_exit();
  timer_unsubscribe_int();
  kbc_unsubscribe_int();

  return 1;
}

int(video_test_controller)() {
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}
