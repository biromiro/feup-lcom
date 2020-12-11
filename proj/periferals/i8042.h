//
// Created by biromiro on 23/10/20.
//

#ifndef LAB3_I8042_H
#define LAB3_I8042_H

/** @defgroup i8042 i8042
 * @{
 *
 * Constants for programming the i8042 Keyboard Controller (and useful i8254 Timer constants).
 * */

#define KBC_BRK_ESC_KEY 0x81 /**< @brief scancode (breakcode) of ESC key*/
#define KBC_MSB_SCNCD BIT(7) /**< @brief scancode diff (make or break) in BIT 7*/
#define KBC_MK_A_KEY 0x1e
#define KBC_BRK_A_KEY 0x9e
#define KBC_MK_S_KEY 0x1F
#define KBC_BRK_S_KEY 0x9f
#define KBC_MK_D_KEY 0x20
#define KBC_BRK_D_KEY 0xa0
#define KBC_MK_W_KEY 0x11
#define KBC_BRK_W_KEY 0x91
#define TIMER_FREQ 1193182 /**< @brief clock frequency for timer in PC and AT */
#define TIMER0_IRQ 0 /**< @brief Timer 0 IRQ line */
#define KBC_IRQ 1 /**< @brief KBC IRQ line */
#define MOUSE_IRQ 12 /**< @brief Mouse IRQ line */
#define RTC_IRQ 8

/* I/O port addresses */

#define TIMER_0    0x40 /**< @brief Timer 0 count register */
#define KBC_STAT_REG 0x64 /**< @brief KBC status register */
#define KBC_IN_BUF_CMD 0x64 /**< @brief KBC input buffer for commands */
#define KBC_IN_BUF_ARG 0x60 /**< @brief KBC input buffer for arguments */
#define KBC_OUT_BUF 0x60 /**< @brief KBC output buffer */

/* KBC Control */

/* Status Register Specification */

#define KBC_PARITY BIT(7) /**< @brief KBC parity error bit */
#define KBC_TIMEOUT BIT(6) /**< @brief KBC timeout error bit */
#define KBC_AUX BIT(5) /**< @brief KBC mouse data bit */
#define KBC_INH BIT(4) /**< @brief KBC inhibit flag bit */
#define KBC_A2 BIT(3) /**< @brief KBC a2 input line bit */
#define KBC_SYS BIT(2) /**< @brief KBC system flag bit */
#define KBC_IBF BIT(1) /**< @brief KBC input buffer full bit */
#define KBC_OBF BIT(0) /**< @brief KBC output buffer full bit */

/* KBC Commands for PC-AT/PS2 */

#define KBC_R_CMD_B 0x20 /**< @brief KBC command to read command byte */
#define KBC_W_CMD_B 0x60 /**< @brief KBC command to write command byte */
#define KBC_CHK 0xAA /**< @brief KBC command to self-check */
#define KBC_CHK_KBD_I 0xAB /**< @brief KBC command to check keyboard interface */
#define KBC_DIS_KBD_I 0XAD /**< @brief KBC command to disable KBD interface */
#define KBC_EN_KBD_I 0XAE /**< @brief KBC command to enable KBD interface */

/* KBC Command Byte */

#define KBC_DIS2 BIT(5) /**< @brief disable mouse interface */
#define KBC_DIS BIT(4) /**< @brief disable keyboard interface */
#define KBC_INT2 BIT(1) /**< @brief enable interrupt on OBF, from mouse */
#define KBC_INT BIT(0) /**< @brief enable interrupt on OBF, from keyboard */

/* KBC scancode type */

#define KBC_2BYTE_CODE 0xE0 /**< @brief the first byte of a 2 byte scancode*/

/* Mouse acknowledgment bytes*/

#define KBC_ACK 0xFA /** @brief acknowledgment byte if everything OK*/
#define KBC_NACK 0xFE /** @brief acknowledgment byte if invalid byte (may be because of a serial communication error)*/
#define KBC_ERROR 0xFC /** @brief acknowledgment byte if second consecutive invalid byte*/
#define MOUSE_EN_DATA_REP 0xF4
#define MOUSE_DIS_DATA_REP 0xF5
#define MOUSE_STREAM_MODE 0xEA
#define MOUSE_REMOTE_MODE 0xF0
#define MOUSE_READ_DATA 0xEB

/* RTC I/O adreses */

#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71
#define RTC_REG_SEC 0x00
#define RTC_REG_SEC_ALRM 0x01
#define RTC_REG_MIN 0x02
#define RTC_REG_MIN_ALRM 0x03
#define RTC_REG_HOUR 0x04
#define RTC_REG_HOUR_ALRM 0x05
#define RTC_REG_DAY_WEEK 0x06
#define RTC_REG_DAY_MONTH 0x07
#define RTC_REG_MONTH 0x08
#define RTC_REG_YEAR 0x09
#define RTC_REG_A 0x10
#define RTC_REG_B 0x11
#define RTC_REG_C 0x12
#define RTC_REG_D 0x13

/* RTC REGISTER FLAGS */

#define RTC_UIP BIT(7)
#define RTC_SET BIT(7)
#define RTC_PIE BIT(6)
#define RTC_AIE BIT(5)
#define RTC_UIE BIT(4)
#define RTC_SQWE BIT(3)
#define RTC_IRQF BIT(7)
#define RTC_PF BIT(6)
#define RTC_AF BIT(5)
#define RTC_UF BIT(4)

#define UPDATE_TIME_MICRO 244

#endif //LAB3_I8042_H
