#include <lcom/lcf.h>

/** @defgroup serial_port_controller serial_port_controller
 * @{
 *
 * Constants for programming the serial port
 */

#define COM1 0x3F8
#define COM1_IRQ 4

#define RBR 0x00
#define THR  0x00
#define IER 0x01
#define IIR 0x02
#define FCR 0x02
#define LCR 0x03
#define MCR 0x04
#define LSR 0x05
#define MSR 0x06
#define SCRATCHPAD_REG 0x07

#define DIVISOR_LATCH_LSB 0x00
#define DIVISOR_LATCH_MSB 0x01

#define LCR_8_BITS_PER_CHAR BIT(0) | BIT(1)
#define LCR_ODD_PARITY BIT(3)
#define LCR_DLAB BIT(7)

#define LSR_RECEIVER_DATA BIT(0)
#define LSR_OVERRUN_ERR BIT(1)
#define LSR_PARITY_ERR BIT(2)
#define LSR_FRAMING_ERR BIT(3)
#define LSR_BREAK_INT BIT(4)
#define LSR_TRANSMIT_HOLD_REG_EMPTY BIT(5)
#define LSR_TRANSMIT_EMPTY_REG BIT(6)
#define LSR_FIFO_ERR BIT(7)

#define IER_RECEIVED_DATA_AVAILABLE_INT BIT(0)
#define IER_TRANSMIT_HOLD_REG_EMPTY_INT BIT(1)
#define IER_RECEIVER_LINE_STATUS_INT BIT(2)
#define IER_MODEM_STATUS_INT BIT(3)

#define IIR_NO_INT BIT(0)
#define IIR_ID BIT(1) | BIT(2) | BIT(3)
#define IIR_RECEIVER_LINE_STATUS BIT(1) | BIT(2)
#define IIR_RECEIVED_DATA_AVAILABLE BIT(2)
#define IIR_FIFO_CHAR_TIMEOUT BIT(3) | BIT(2)
#define IIR_TRANSMIT_HOLD_REG_EMPTY BIT(1)
#define IIR_MODEM_STATUS 0x00


#define FCR_ENABLE_BOTH_FIFO BIT(0)
#define FCR_CLEAR_RCVR_FIFO BIT(1)
#define FCR_CLEAR_XMIT_FIFO BIT(2)
#define FCR_ENABLE_64_BYTE_FIFO BIT(5)
