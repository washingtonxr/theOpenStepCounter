/**
 * @File    Peripherals.c
 * @Date    Tuesday, Dec. 25, 2018 at 14:17:09 PM BJT
 * @Author  Washington Ruan
 * @Email   washingtonxr@live.com
 *
 * This file contains the implementation of the gravity sensor devicer
 * module's main file.
 *
 * @bug No known bugs.
 **/

#include <math.h>
#include "Peripherals.h"
#include "uart_debug.h"
#include "I2C_1.h"

/* Allocate buffer */
#define BUFFER_SIZE (128UL)
unsigned char tx_buf[BUFFER_SIZE],rx_buf[BUFFER_SIZE];
cy_stc_scb_i2c_master_xfer_config_t transaction;
static uint32 status;



/* I2C Initializing Processing. */
/* Implement ISR for I2C_1 */
void I2C_1_Isr(void) {
	Cy_SCB_I2C_Interrupt(I2C_1_HW, &I2C_1_context);
}

void I2C_1_Sysconfig(unsigned char slave_addr)
{
    /* Configure transaction */
    transaction.slaveAddress = slave_addr;
    transaction.buffer		 = tx_buf;
    transaction.bufferSize	 = sizeof(tx_buf);
    transaction.xferPending  = false;
    return;
}

#ifndef NO_PI3
unsigned long I2C_1_Single_Write(unsigned char Address, unsigned char RegisterAddr, unsigned char RegisterValue)
{
    /* Send Start with Address + Write */
    status = I2C_1_MasterSendStart((uint32)Address, CY_SCB_I2C_WRITE_XFER, I2CM_TIMEOUT);

    /* Send register address */
    if(status == CY_SCB_I2C_SUCCESS)
    {
        status |= I2C_1_MasterWriteByte((uint32)RegisterAddr, I2CM_TIMEOUT);
    }

    if(status == CY_SCB_I2C_SUCCESS)
    {
        /* Send data one byte at a time */
        status |= I2C_1_MasterWriteByte(RegisterValue, I2CM_TIMEOUT);
    }
    /* Send Stop */
    status |= I2C_1_MasterSendStop(I2CM_TIMEOUT);
    return status;
#else
int I2C_1_Single_Write(unsigned char Address, unsigned char RegisterAddr, unsigned char RegisterValue)
{
    unsigned char WriteBuf[2] = {0x00,0x00};
    int rc;

    bcm2835_i2c_begin();
    bcm2835_i2c_setSlaveAddress(SlaveAddress);
    bcm2835_i2c_set_baudrate(Boundrate);

    WriteBuf[0] = Reg;
    WriteBuf[1] = Com;
    rc = bcm2835_i2c_write( WriteBuf, 2);
    if(!rc == 0){
        dbg_info("Write Command failed.\n");
        return rc;
    }
    bcm2835_i2c_end();
    return 0;

#endif
}

unsigned long I2C_1_WriteBytes(unsigned char Address, unsigned char RegisterAddr, \
                                    unsigned char RegisterLen, unsigned char *RegisterValue)
{
    uint8 i = RegisterLen;

    /* Send Start with Address + Write */
    status = I2C_1_MasterSendStart((uint32)Address, CY_SCB_I2C_WRITE_XFER, I2CM_TIMEOUT);

    /* Send register address */
    if(status == CY_SCB_I2C_SUCCESS)
    {
        status |= I2C_1_MasterWriteByte((uint32)RegisterAddr, I2CM_TIMEOUT);
    }
    if(status == CY_SCB_I2C_SUCCESS)
    {
        /* Send data one byte at a time */
        while(i-- && (status == CY_SCB_I2C_SUCCESS))
        {
            status |= I2C_1_MasterWriteByte((uint32)*RegisterValue++, I2CM_TIMEOUT);
        }
    }
    /* Send Stop */
    status |= I2C_1_MasterSendStop(I2CM_TIMEOUT);

    return status;
}

unsigned long I2C_1_ReadBytes(unsigned char Address, unsigned char RegisterAddr, \
                                    unsigned char RegisterLen, unsigned char *RegisterValue)
{
    uint8 i = 0;

    /* Send Start with Address + Write */
    status = I2C_1_MasterSendStart((uint32)Address, CY_SCB_I2C_WRITE_XFER, I2CM_TIMEOUT);
    if(status == CY_SCB_I2C_SUCCESS)
    {
        /* Send register address */
        status |= I2C_1_MasterWriteByte((uint32)RegisterAddr, I2CM_TIMEOUT);
    }
    if(status == CY_SCB_I2C_SUCCESS)
    {
        /* Send Start with Address + Read */
        status |= I2C_1_MasterSendReStart((uint32)Address, CY_SCB_I2C_READ_XFER, I2CM_TIMEOUT);
    }
    if(status == CY_SCB_I2C_SUCCESS)
    {
        /* Read in data bytes */
        while(i < (RegisterLen - 1))
        {
            status = I2C_1_MasterReadByte(CY_SCB_I2C_ACK, RegisterValue+i, I2CM_TIMEOUT);
            // Delay to allow DMP time to setle
            i++;
        }
        /* Read and NAK the final byte */
        I2C_1_MasterReadByte(CY_SCB_I2C_NAK, RegisterValue+i, I2CM_TIMEOUT);
    }

    /* Send Stop to indicate end of communciation */
    status |= I2C_1_MasterSendStop(I2CM_TIMEOUT);

    return status;
}

int SPI_1_WriteByte(unsigned char Value)
{
	uint8 tx_data[1];
    //uint8 rx_data[1];
    cy_en_scb_spi_status_t errorStatus;
    uint32 timeOut = 1000;

    tx_data[0] = Value;

    do{
        errorStatus = Cy_SCB_SPI_Transfer(Screen_HW, &tx_data, NULL, sizeof(tx_data), &Screen_context);
        timeOut--;
    }while ((0UL != (errorStatus & CY_SCB_SPI_TRANSFER_ACTIVE)) && (timeOut > 0UL));
    /* Blocking wait for transfer completion */
    while (0UL != (CY_SCB_SPI_TRANSFER_ACTIVE & Cy_SCB_SPI_GetTransferStatus(Screen_HW, &Screen_context)))
    {
    }
    return 0;
}

/* [] END OF FILE */
