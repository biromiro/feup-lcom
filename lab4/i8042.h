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
#define TIMER_FREQ 1193182 /**< @brief clock frequency for timer in PC and AT */
#define TIMER0_IRQ 0 /**< @brief Timer 0 IRQ line */
#define KBC_IRQ 1 /**< @brief KBC IRQ line */

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

#endif //LAB3_I8042_H
