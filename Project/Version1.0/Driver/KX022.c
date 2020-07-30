/**
 * @File    KX022.c
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
#include "KX022.h"
#include "I2C_1.h"

struct WriteReg1{
    unsigned char reg_addr;
    unsigned char data;
};

struct WriteReg1 kx022Init[] = {
/* register    operate data */
    {0x18   ,       0x00},          //bit7(operating_mode)=0,disable controller and start setup
    {0x1B   ,       0x03},          //bit7=1,close filter bit1=1,set ODR to 2:25Hz / 3:50Hz
//  {0x35   ,       0x4B},          //bit6/5/4 =100,16 samp_avg
    {0x3A   ,       0x54},          //8bit mode - max 84 0x54=84
    {0x3B   ,       0x80},          //sample buffer active & 8-bit FIFO mode
//  {0x18   ,       0x90},          //bit7(operating_mode)=1,enable controller and setup end  8g
    {0x18   ,       0x82},          //bit7(operating_mode)=1,enable controller and setup end  2g
};

/* KX022 module. */
void KX022_Init(axis_dblock_t *GDB)
{
    unsigned char i = 0;
    unsigned char IsReady = 0;

    DEBUG_PRINTF("Info:Starting Initializing KX022.\n");

    do{
        I2C_1_ReadBytes(KX022_ADDR, KX022_creg, 1, &IsReady);
        i+=1;
    }while(IsReady!=0x55 && i<2);
#if 0
    DEBUG_PRINTF("Info: i = %d.\n",i);
#endif
    if(i < 2){
        for(i=0; i<sizeof(kx022Init)/((sizeof(unsigned char)) *2); i++) {
            I2C_1_WriteBytes(KX022_ADDR, (kx022Init[i].reg_addr), 1, &(kx022Init[i].data));
        }

        /* G_Algorithm Initializing. */
        gdata_init(GDB);
        memset(GDB, 0, sizeof(axis_dblock_t));

        /* Check algorithm's version. */
        gdata_version();

        DEBUG_PRINTF("Info:The end of Initializing.\n");

    }else{
         DEBUG_PRINTF("Error:Initializing KX022 failed.\n");
    }

    return;
}

/**
 * Fill data to axis_dblock_t structure. Transfer RAW gravigy data into real value for each axis.
 */
static unsigned char KX022_db2axis(unsigned char *buffer, unsigned char length, axis_dblock_t *GDB)
{
#if 0
    unsigned char i;
    enum{x,y,z};
    
    for(i=0; i<length; i++){
        if(i%3 == x){
            axis[x][i/3] = (char)buffer[i];
            DEBUG_PRINTF("%d;",axis[x][i/3]);
        }else if(i%3 == y){
            axis[y][i/3] = (char)buffer[i];
            DEBUG_PRINTF("%d;",axis[y][i/3]);
        }else if(i%3 == z){
            axis[z][i/3] = (char)buffer[i];
            DEBUG_PRINTF("%d\n",axis[z][i/3]);
        }
    }
    return  (i/3);
#endif
    int16 raw_data[KX022_ddepth][KX022_axis];
    unsigned char raw_depth;
    unsigned int i,j;

    if(length > 0){
        raw_depth = length / 6;
        if(raw_depth >= KX022_ddepth)raw_depth = KX022_ddepth;
        memset(raw_data, 0, sizeof(raw_data));
#if 0
        DEBUG_PRINTF("RAW Data L1(%d-%d):\n", raw_depth, length);
#endif
        for(i = 0; i < raw_depth; i++){
#if 0
            raw_data[i][0] = (sensor_buf[6*i-5]<<8) + sensor_buf[6*i-6];        // X
            raw_data[i][1] = (sensor_buf[6*i-3]<<8) + sensor_buf[6*i-4];        // Y
            raw_data[i][2] = (sensor_buf[6*i-1]<<8) + sensor_buf[6*i-2];        // Z
#endif
            for(j = 0; j < KX022_axis; j++){
                raw_data[i][j] = (buffer[6*(i+1)-(5-2*j)]<<8) + buffer[6*(i+1)-(6-2*j)];
                GDB->sample_block[GDB->counter].axia_data[i].axis[j] = ((((float)raw_data[i][j]*KX022_NG)/KX022_RESOLUTION));
                //DEBUG_PRINTF("raw_data[%d][%d]:%d --> %f\n", i, j, raw_data[i][j], GDB->g_data[i][j]);
            }
            //GDB->gcom_data[i] = sqrt(pow(GDB->g_data[i][0],2) + pow(GDB->g_data[i][1],2) + pow(GDB->g_data[i][2],2));
            //DEBUG_PRINTF("com_data[%d]:%f\n", i, GDB->gcom_data[i]);
        }
        
        GDB->sample_block[GDB->counter].counter = raw_depth;
        GDB->counter++;

        if( GDB->counter >= SAMPLE_DEPTH){
            return KX022_CHE;
        }else{
            return KX022_OK;
        }
    }else{
        DEBUG_PRINTF("Error: No sensor data exist.\n");
        return KX022_ERR;
    }
}

#if 0
static void KX022_Print(unsigned char length, axis_dblock_t *GDB)
{
    unsigned int i,j;
    //DEBUG_PRINTF("g-raw data:\nx\ty\tz\t\n");
    if(length > 0){
        for(i = 0; i < length / 6; i++){
            for(j = 0; j < 3; j++){
                DEBUG_PRINTF("%f\t", GDB->g_data[i][j]);
            }
            DEBUG_PRINTF("\n");
            //DEBUG_PRINTF("com_data[%d]:%f\n", i, GDB->gcom_data[i]);
        }
    }
}
#else
/**
 * Print data from axis_dblock_t structure.
 */
static void KX022_Print(axis_dblock_t *GDB)
{
    unsigned char i,j,k;
#if 0
    static unsigned int Print_CNT;
#endif
#if 0 
    DEBUG_PRINTF("Info:g-raw data:\nx\ty\tz\t\n");
#endif
#if 0
    Print_CNT = 0;
#endif
    for(i = 0; i< GDB->counter; i++){
        for(j = 0; j < GDB->sample_block[i].counter; j++){
#if 0
            DEBUG_PRINTF("%02d=>", Print_CNT++);
#endif
            for(k = 0; k < 3; k++){
                DEBUG_PRINTF("%f\t", GDB->sample_block[i].axia_data[j].axis[k]);
            }
            DEBUG_PRINTF("\n");
        }
    }
}
#endif
int KX022_get_data(axis_dblock_t *GDB)
{
    unsigned char data_length;
    unsigned char sensor_buf[255];
    unsigned char clear = 0;
    unsigned char ret;
    /* Get FIFO depth. */
    I2C_1_ReadBytes(KX022_ADDR, KX022_blen, 1, &data_length);
    //DEBUG_PRINTF("Info:Buffer length = %d\n",data_length);
    if( data_length > 0){
        memset(sensor_buf, 0, sizeof(sensor_buf));
        /* Get data from buffer. */
        I2C_1_ReadBytes(KX022_ADDR, KX022_dreg, data_length, sensor_buf);
        /* Clear buffer. */
        I2C_1_ReadBytes(KX022_ADDR, KX022_dclc, 1, &clear);
        /* Coordinate data. */
        ret = KX022_db2axis(sensor_buf, data_length, GDB);
        switch(ret){
            case KX022_OK:
                DEBUG_PRINTF("Info:KX022_OK(%d).\n", GDB->counter);
            break;
            case KX022_CHE:
#if 1
                KX022_Print(GDB);
#endif
                gdata_l1filter(GDB);
                gdata_motion_detect(GDB);
                /* Global Counter Clear. */
                GDB->counter = 0;
            break;
            case KX022_ERR:
                DEBUG_PRINTF("Error:KX022_db2axis(%d).\n",ret);
            break;
            default:
                DEBUG_PRINTF("Error:KX022_db2axis(%d).\n",ret);
            break;
        }
        return KX022_OK;
    }else{
        return KX022_ERR;
    }
}


