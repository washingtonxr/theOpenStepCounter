/**
 * @File    Peripherals.h
 * @Date    Tuesday, Dec. 25, 2018 at 14:17:09 PM BJT
 * @Author  Washington Ruan
 * @Email   washingtonxr@live.com
 *
 * This file contains the implementation of the gravity sensor devicer
 * module's main file.
 *
 * @bug No known bugs.
 **/

#ifndef _PERIPHERALS_H
#define _PERIPHERALS_H
#include "uart_debug.h"
#define I2CM_TIMEOUT        10

void I2C_1_Isr(void);
void I2C_1_Sysconfig(unsigned char slave_addr);
unsigned long I2C_1_WriteBytes(unsigned char Address, unsigned char RegisterAddr, \
                                    unsigned char RegisterLen, unsigned char *RegisterValue);
unsigned long I2C_1_ReadBytes(unsigned char Address, unsigned char RegisterAddr, \
                                    unsigned char RegisterLen, unsigned char *RegisterValue);
int SPI_1_WriteByte(unsigned char Value);

#ifndef NO_PI3
unsigned long I2C_1_Single_Write(unsigned char Address, unsigned char RegisterAddr, \
                                        unsigned char RegisterValue);
#else
int I2C_1_Single_Write(unsigned char Address, unsigned char RegisterAddr, \
                            unsigned char RegisterValue);
#endif

void I2C_1_Sysconfig(unsigned char slave_addr);

#endif
/* [] END OF FILE */
