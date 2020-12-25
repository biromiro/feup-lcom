#include "interrupt_handler.h"

static uint8_t irq_set_mouse, irq_set_timer, irq_set_kbc, irq_set_rtc, irq_set_ser, i = 0;
extern int counter, cnt;
extern uint8_t scancode, packetByte;
bool finished;
static int ipc_status, r, counter_mouse = 0;
static message msg;
static bool startPacket;
static struct packet pp;
static uint8_t bytes[2];
static xpm_object *igcursor, *background_img, *mainMenu, *instructionsMenu, *gameOver, *coopWaitingMenu;
extern bool alarmInterrupt;

extern gameState gs;

int subscribe_interrupts() {

  if (rtc_subscribe_int(&irq_set_rtc) != 0){
    printf("Error subscribing rtc\n");
    return 1;
  }

  mouse_write_cmd(MOUSE_EN_DATA_REP);

  if (kbc_subscribe_int(&irq_set_kbc) != 0) {
    printf("Error subscribing kbc\n");
    return 1;
  }

  if (mouse_subscribe_int(&irq_set_mouse) != 0) {
    printf("Error subscribing mouse\n");
    return 1;
  }

  if (timer_subscribe_int(&irq_set_timer) != 0) {
    printf("Error subscribing timer\n");
    return 1;
  }

  if (ser_subscribe_int(&irq_set_ser) != 0){
    printf("Error subscribing serial port\n");
    return 1;
  }

  return 0;
}

int initialize() {

  ser_init();

  if (OK != subscribe_interrupts()) {
    printf("Could not subscribe all interrupts!\n");
    return 1;
  }

  ser_enable_int();

  set_rtc_interrupts(ALARM, true);
  set_rtc_interrupts(UPDATE, false);
  set_rtc_interrupts(PERIODIC, true);

  vg_init(0x14C);
  timer_set_frequency(0, 120);
  
  create_game_objects();
  set_magic_blasts_available();
  set_enemies_available();

  igcursor = create_sprite(cursor, "cursor", 200, 200);
  background_img = create_sprite(background, "background", 0, 0);
  mainMenu = create_sprite(mainmenu,"mainMenu",0,0);
  instructionsMenu = create_sprite(instructionsmenu,"instructionsMenu",0,0);
  gameOver = create_sprite(gameover,"gameOver",0,0);
  coopWaitingMenu = create_sprite(coop, "coopWaitingMenu", 300,300);
  set_background(background_img);

  set_power_up_alarm(0);
  set_enemy_throw(0xF);

  gs = START;
  return 0;
}

int unsubscribe_interrupts() {

  if (ser_unsubscribe_int() != 0){
    printf("Error unsubscribing serial port\n");
    return 1;
  }

  if (timer_unsubscribe_int() != 0) {
    printf("Error unsubscribing timer\n");
    return 1;
  }

  if (mouse_unsubscribe_int() != 0) {
    printf("Error unsubscribing mouse\n");
    return 1;
  }

  if (kbc_unsubscribe_int() != 0) {
    printf("Error unsubscribing kbc\n");
    return 1;
  }

  mouse_write_cmd(MOUSE_DIS_DATA_REP);

  if (rtc_unsubscribe_int() != 0){
    printf("Error unsubscribing rtc\n");
    return 1;
  }

  return 0;
}

int finish() {

  vg_exit();

  free_game_objects();

  free_enemies();

  free_magic_blasts();

  timer_set_frequency(0, 60);

  set_rtc_interrupts(ALARM, false);
  set_rtc_interrupts(UPDATE, false);
  set_rtc_interrupts(PERIODIC, false);

  ser_exit();

  if (OK != unsubscribe_interrupts()) {
    printf("Could not unsubscribe all interrupts!\n");
    return 1;
  }

  return 0;
}

uint8_t get_irq_set(irq_type type) {
  switch (type) {
    case MOUSE:
      return irq_set_mouse;
    case TIMER:
      return irq_set_timer;
    case KBD:
      return irq_set_kbc;
    case RTC:
      return irq_set_rtc;
    case SER:
      return irq_set_ser;
  }
}

void interrupt_call_receiver() {
  /* Get a request message */
  if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
    printf("driver_receive failed with: %d", r);
    return;
  }
  if (is_ipc_notify(ipc_status)) { /* received notification */
    switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE: /* hardware interrupt notification */
        if (msg.m_notify.interrupts & get_irq_set(TIMER)) {
          timer_handler();
        }
        if (msg.m_notify.interrupts & get_irq_set(MOUSE)) { /* subscribed interrupt */
          mouse_handler();
        }
        if (msg.m_notify.interrupts & get_irq_set(KBD)) {
          kbd_handler();
        }
        if (msg.m_notify.interrupts & get_irq_set(RTC)) {
          rtc_handler();
        }
        if (msg.m_notify.interrupts & get_irq_set(SER)) {
          ser_handler();
        }
        break;
      default:
        break; /* no other notifications expected: do nothing */
    }
  }
  else { /* received a standard message, not a notification */
    /* no standard messages expected: do nothing */
  }
  return;
}

void timer_handler() {
  timer_int_handler();

  if (counter % 2 == 0) {
    print_background();
    if(gs==START){
      print_xpm(mainMenu);
    } else if(gs==INSTRUCTIONS){
      print_xpm(instructionsMenu);
    } else if(gs == COOP){
      print_xpm(coopWaitingMenu);
    }else if(gs==GAME) {
      if(checking_collision(get_magic_blasts())) finished=true;
      if (OK != update_character_movement(counter))
        finished = true;
      print_magic_blasts();
      print_enemies();
      draw_current_hud();

    }else if(gs==GAMEOVER) {
      print_xpm(gameOver);
    }
    
    print_xpm(igcursor);
    if (OK != swap_buffer()) {
      printf("Unable to swap buffers!");
      return;
    }
  }
}

void mouse_handler() {
  mouse_ih();
  if (packetByte & BIT(3)) {
    startPacket = true;
  }
  if (startPacket) {
    pp.bytes[counter_mouse] = packetByte;
    counter_mouse++;
  }
  if (counter_mouse == 3) {
    mouse_parse_packet(&pp);
    startPacket = false;
    counter_mouse = 0;
    igcursor->x += pp.delta_x;
    igcursor->y -= pp.delta_y;
  }
  handle_mouse_packet(igcursor, &pp, true);
  if(gs==EXIT)
    finished=true;
}

void kbd_handler() {
  kbc_ih();
  if (scancode == KBC_2BYTE_CODE) {
    bytes[i] = scancode;
    i++;
    return;
  }
  bytes[i] = scancode;

  handle_button_presses(scancode, true);
  i = 0;
  if (scancode == KBC_BRK_ESC_KEY )
    finished = true;
}

void rtc_handler(){
  rtc_ih();
  handle_rtc_ingame_changes(&alarmInterrupt);
}

void ser_handler(){
  ser_ih();
  if (gs == COOP) handle_coop_start();
  else if (gs == GAME) handle_received_info();
}
