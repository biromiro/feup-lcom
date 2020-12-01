//
// Created by biromiro on 23/10/20.
//

#include "kbc.h"

int hook_kbc = 1;
int hook_mouse = 2;
uint8_t scancode = 0;
uint8_t packetByte = 0;
static enum state currentState = INITIAL;
int16_t delta_x = 0, delta_y = 0;

int(kbc_subscribe_int)(uint8_t *bit_no) {
  *bit_no = BIT(hook_kbc);
  sys_irqsetpolicy(KBC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_kbc);
  return 0;
}

int(kbc_unsubscribe_int)() {
  sys_irqrmpolicy(&hook_kbc);
  return 0;
}

int(kbc_get_status)(uint8_t *st) {
  //checks if it was possible to get the configuration of the kbc
  return util_sys_inb(KBC_STAT_REG, st);
}

int(kbc_read_buffer)(int port, uint8_t *data) {
  return util_sys_inb(port, data);
}

int(kbc_write_cmd)(int port, uint8_t cmd) {
  uint8_t st = 0, timeout_cnt = 0;


  /* loops while 8042 input buffer is not empty */
  do {

    /* checks if it is possible to read the status of KBC */
    if (kbc_get_status(&st) != 0) {
      printf("Error getting KBC status");
      return -1;
    }

    /* checks if the input buffer is empty */
    if ((KBC_IBF & st) == 0) {
      sys_outb(port, cmd);
      return 0;
    }

    /* delays the next iteration and increases the timeout counter */
    tickdelay(micros_to_ticks(DELAY_US));
    timeout_cnt++;
  } while (timeout_cnt < 5);

  return -1;
}


int(kbc_read_data)(uint8_t *data) {
  uint8_t st = 0;

  /* checks if it is possible to read the status of KBC */
  if (kbc_get_status(&st) != 0) {
    printf("Error getting KBC status");
    return -1;
  }

  /* checks if the output buffer is full */
  if (KBC_OBF & st) {
    if (kbc_read_buffer(KBC_OUT_BUF, data) != 0) {
      printf("Error getting KBC data with full buffer");
      return -1;
    }

    if (((st & (KBC_PARITY | KBC_TIMEOUT)) == 0) || ((st & KBC_AUX) == 0))
      return 0;
    else
      return -1;
  }

  return -1;
}

int (kbc_restore_interrupts)() {
  uint8_t cmd;
  kbc_write_cmd(KBC_IN_BUF_CMD, KBC_R_CMD_B);
  kbc_read_buffer(KBC_OUT_BUF, &cmd);
  cmd |= KBC_INT;
  kbc_write_cmd(KBC_IN_BUF_CMD, KBC_W_CMD_B);
  kbc_write_cmd(KBC_IN_BUF_ARG, cmd);
  return 0;
}

int (kbc_reset_commannd)(){
  kbc_write_cmd(KBC_IN_BUF_CMD, KBC_W_CMD_B);
  kbc_write_cmd(KBC_IN_BUF_ARG,minix_get_dflt_kbc_cmd_byte());
  return 0;
}

void(kbc_ih)(void) {
  if (kbc_read_data(&scancode) != 0)
    printf("Unable to read scancode!");
}

int(mouse_subscribe_int)(uint8_t *bit_no) {
  *bit_no = BIT(hook_mouse);
  sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_mouse);
  return 0;
}

int(mouse_unsubscribe_int)() {
  sys_irqrmpolicy(&hook_mouse);
  return 0;
}

int (mouse_read_data)(uint8_t *data){
  uint8_t st = 0;

  /* checks if it is possible to read the status of KBC */
  if (kbc_get_status(&st) != 0) {
    printf("Error getting KBC status");
    return -1;
  }

  /* checks if the output buffer is full */
  if (KBC_OBF & st) {
    if (kbc_read_buffer(KBC_OUT_BUF, data) != 0) {
      printf("Error getting KBC data with full buffer");
      return -1;
    }

    if ((st & (KBC_PARITY | KBC_TIMEOUT)) == 0 && (st & KBC_AUX) == KBC_AUX)
      return 0;
    else {
      printf("Error parity, timeout or not from mouse");
      return -1;
    }
  }
  printf("Output buffer not full");
  return -1;
}

int(mouse_get_status)(uint8_t *st) {
  //checks if it was possible to get the configuration of the kbc
  return util_sys_inb(KBC_IN_BUF_ARG, st);
}

int(mouse_write_cmd)(uint8_t cmd) {
  uint8_t ack_flag;
  do {
    kbc_write_cmd(0x64,0xD4);
    kbc_write_cmd(0x60,cmd);
    mouse_get_status(&ack_flag);
  }while (ack_flag != KBC_ACK);
  return 0;
}

void (mouse_parse_packet)(struct packet* pp){
  pp->rb = pp->bytes[0] & BIT(1);
  pp->mb = pp->bytes[0] & BIT(2);
  pp->lb = pp->bytes[0] & BIT(0);
  pp->delta_x = (pp->bytes[0] & BIT(4)) ? (0xFF00 | pp->bytes[1]) : (pp->bytes[1]);
  pp->delta_y = (pp->bytes[0] & BIT(5)) ? (0xFF00 | pp->bytes[2]) : (pp->bytes[2]);
  pp->x_ov = pp->bytes[0] & BIT(6);
  pp->y_ov = pp->bytes[0] & BIT(7);
}

void (mouse_ih)(void) {
  if (mouse_read_data(&packetByte) != 0)
    printf("Unable to read packetByte!");
}

bool (mouse_handle_gesture)(struct mouse_ev *event,uint8_t x_len, uint8_t tolerance){
  switch (currentState) {
    case INITIAL:
      delta_x = 0;
      delta_y = 0;
      printf("\nINITIAL\n");
      if(event->type == LB_PRESSED)
        currentState = FIRST_SLOPE;
      break;
    case FIRST_SLOPE:
      printf("\nFIRST_SLOPE\n");
      if(event->type == LB_RELEASED){
        if((delta_x >= x_len) && (delta_y/delta_x) > 1) {
          delta_x = 0;
          delta_y = 0;
          currentState = VERTEX;
        }
        else
          currentState = INITIAL;
      }else if(event->type == MOUSE_MOV){
        if(event->delta_x <= 0 || event->delta_y <= 0)
          if(abs(event->delta_x) > tolerance || abs(event->delta_y) > tolerance){
            currentState = INITIAL;
            break;
          }
        if(event->delta_x != 0 && (event->delta_y)/(event->delta_x) <= 1)
          currentState = INITIAL;
        else {
          delta_x += event->delta_x;
          delta_y += event->delta_y;
        }
      }else
        currentState = INITIAL;
      break;
    case VERTEX:
      printf("\nVERTEX\n");
      if(event->type == RB_PRESSED){
        if(abs(delta_x) > tolerance || abs(delta_y) > tolerance)
          currentState = INITIAL;
        else {
          delta_x = 0;
          delta_y = 0;
          currentState = SECOND_SLOPE;
        }
      }
      else if(event->type == MOUSE_MOV){
        delta_x += event->delta_x;
        delta_y += event->delta_y;
      }
      else if(event->type == LB_PRESSED) {
        delta_x = 0;
        delta_y = 0;
        currentState = FIRST_SLOPE;
      }
      else{
        currentState = INITIAL;
      }
      break;
    case SECOND_SLOPE:
      printf("\nSECOND_SLOPE\n");
      if(event->type == RB_RELEASED){
        if((delta_x >= x_len) && (delta_y/delta_x) < -1) {
          currentState = COMPLETE;
          return true;
        }else
          currentState = INITIAL;
      }else if(event->type == MOUSE_MOV){
        if(event->delta_x <= 0 || event->delta_y >= 0)
          if(abs(event->delta_x) > tolerance || abs(event->delta_y) > tolerance){
            currentState = INITIAL;
            break;
          }
        if(event->delta_x != 0 && (event->delta_y)/(event->delta_x) >= -1)
          currentState = INITIAL;
        else {
          delta_x += event->delta_x;
          delta_y += event->delta_y;
        }
      }else
        currentState = INITIAL;
      break;
    case COMPLETE:
      printf("\nCOMPLETE\n");
      return true;
    default:
      break;
  }
  return false;
}
