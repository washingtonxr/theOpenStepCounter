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
#if( HW_DEFINE == HW_B01)
#include "i2c_2_support.h"
#else
#include "I2C_1.h"
#endif
#include "comm/comm.h"

struct WriteReg1{
    unsigned char reg_addr;
    unsigned char data;
};

struct WriteReg1 kx022Init[] = {
/* register    operate data */
    {0x18   ,       0x00},          //bit7(operating_mode)=0,disable controller and start setup
    {0x1B   ,       0x03},          //bit7=1,close filter bit1=1,set ODR to 2:50Hz / 3:100Hz
//  {0x35   ,       0x4B},          //bit6/5/4 =100,16 samp_avg
    {0x3A   ,       0x54},          //8bit mode - max 84 0x54=84
    {0x3B   ,       0x80},          //sample buffer active & 8-bit FIFO mode
//  {0x18   ,       0x90},          //bit7(operating_mode)=1,enable controller and setup end  8g
    {0x18   ,       0x82},          //bit7(operating_mode)=1,enable controller and setup end  2g
};

unsigned long I2C_Gs_ReadBytes(unsigned char Address, unsigned char RegisterAddr, \
                                    unsigned char RegisterLen, unsigned char *RegisterValue)
{

#if( HW_DEFINE == HW_B01)
    return I2C_2_ReadBytes(Address, RegisterAddr, RegisterLen, RegisterValue);
#else
    return I2C_1_ReadBytes(Address, RegisterAddr, RegisterLen, RegisterValue);
#endif
}

unsigned long I2C_Gs_WriteBytes(unsigned char Address, unsigned char RegisterAddr, \
                                    unsigned char RegisterLen, unsigned char *RegisterValue)
{
#if( HW_DEFINE == HW_B01)
    I2C_2_WriteBytes(Address, RegisterAddr, RegisterLen, RegisterValue);
#else
    I2C_1_WriteBytes(Address, RegisterAddr, RegisterLen, RegisterValue);
#endif
    return 0;
}


/* KX022 module. */
void KX022_Init(axis_dblock_t *GDB)
{
    unsigned char i = 0;
    unsigned char IsReady = 0;
#if 0
    DEBUG_PRINTF("Info:Starting Initializing KX022.\n");
#endif
    do{
        I2C_Gs_ReadBytes(KX022_ADDR, KX022_creg, 1, &IsReady);
        i+=1;
    }while(IsReady!=0x55 && i<2);
#if 0
    DEBUG_PRINTF("Info: i = %d.\n",i);
#endif
    if(i < 2){
        for(i=0; i<sizeof(kx022Init)/((sizeof(unsigned char)) *2); i++) {
            I2C_Gs_WriteBytes(KX022_ADDR, (kx022Init[i].reg_addr), 1, &(kx022Init[i].data));
        }

        /* G_Algorithm Initializing. */
        gdata_init(GDB);
        memset(GDB, 0, sizeof(axis_dblock_t));

        /* Check algorithm's version. */
        gdata_version();
#if 0
        DEBUG_PRINTF("Info:The end of Initializing.\n");
#endif
    }else{
        DEBUG_PRINTF("Error:Initializing KX022 failed.\n");
    }

    return;
}

/**
 * Fill data to axis_dblock_t structure.
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
    int16 raw_data[42][3];
    unsigned char raw_depth;
    unsigned int i,j;

    if(length > 0){
        raw_depth = length / 6;
        if(raw_depth >= 42)raw_depth = 42;
        memset(raw_data, 0, sizeof(raw_data));
        memset(&GDB->sample_block[GDB->counter],0, sizeof(GDB->sample_block[GDB->counter]));
#if 0
        //DEBUG_PRINTF("RAW Data L1(%d-%d):\n", raw_depth, length);
        printf("RAW Data L1(%d-%d):\n", raw_depth, length);
#endif
        for(i = 0; i < raw_depth; i++){
#if 0
            raw_data[i][0] = (sensor_buf[6*i-5]<<8) + sensor_buf[6*i-6];        // X
            raw_data[i][1] = (sensor_buf[6*i-3]<<8) + sensor_buf[6*i-4];        // Y
            raw_data[i][2] = (sensor_buf[6*i-1]<<8) + sensor_buf[6*i-2];        // Z
#endif
            for(j = 0; j < 3; j++){
                raw_data[i][j] = (buffer[6*(i+1)-(5-2*j)]<<8) + buffer[6*(i+1)-(6-2*j)];
#if KX022_UPSDOWN
                /* Accelerometer is put in the back of PCBA. */
                if(j > 0)
                    GDB->sample_block[GDB->counter].axia_data[i].axis[j] = -((((float)raw_data[i][j]*KX022_LOCAL_GRAVITY)/KX022_RESOLUTION));
                else
                    GDB->sample_block[GDB->counter].axia_data[i].axis[j] = ((((float)raw_data[i][j]*KX022_LOCAL_GRAVITY)/KX022_RESOLUTION));
#else
                GDB->sample_block[GDB->counter].axia_data[i].axis[j] = ((((float)raw_data[i][j]*KX022_LOCAL_GRAVITY)/KX022_RESOLUTION));
#endif
                //DEBUG_PRINTF("raw_data[%d][%d]:%d --> %f\n", i, j, raw_data[i][j], GDB->g_data[i][j]);
            }
            //GDB->gcom_data[i] = sqrt(pow(GDB->g_data[i][0],2) + pow(GDB->g_data[i][1],2) + pow(GDB->g_data[i][2],2));
            //DEBUG_PRINTF("com_data[%d]:%f\n", i, GDB->gcom_data[i]);
        }
        
        GDB->sample_block[GDB->counter].counter = raw_depth;
        //printf("raw_depth = %d\n", raw_depth);
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
static void KX022_Print(axis_dblock_t *GDB, unsigned char mode)
{
    unsigned char i,j,k;
    static unsigned int timestamp;
    
    if(mode == 0){
        printf("Info:TIM: %d\n", timestamp++);
        printf("Info: Gravity RAW data:\nx\ty\tz\t\n");
        for(i = 0; i< GDB->counter; i++){
            printf("GDB->counter = %d\n", i);
            for(j = 0; j < GDB->sample_block[i].counter; j++){
                printf("Info:ACC: ");
                for(k = 0; k < 3; k++){
                    printf("%f\t", GDB->sample_block[i].axia_data[j].axis[k]);
                }
                printf("\n");
            }
        }
    }else if(mode == 1){
        printf("Info:g-filed data:\nx\ty\tz\t\n");
        for(i = 0; i< GDB->counter; i++){
            printf("GDB->counter = %d\n", i);
            for(j = 0; j < GDB->ave_block[i].counter; j++){
                printf("Info:ACC: ");
                for(k = 0; k < 3; k++){
                    printf("%f\t", GDB->ave_block[i].ave_data[j].axis[k]);
                }
                printf("\n");
            }
        }
    }else if(mode == 2){    /* Serial assitant output mode. */
        unsigned char cmd[4] = {0x03,0xfc,0xfc,0x03};
        for(i = 0; i< GDB->counter; i++){
            for(j = 0; j < GDB->ave_block[i].counter; j++){
                printf("0x%02X,0x%02X",cmd[0], cmd[1]);
                for(k = 0; k < 3; k++){
                    printf("%f", GDB->ave_block[i].ave_data[j].axis[k]);
                }
                //printf(",");
                printf("0x%02X,0x%02X\n",cmd[2], cmd[3]);
            }
        }
    }
#if 0
    else if(mode == 3){    /* Push data to ble channel. */
        unsigned short putpage;
        static unsigned int index;
        static unsigned int timeindex;
        char *data2ble;
        
#define LEN_DATABUF 3072
        data2ble = (char *)pvPortMalloc(LEN_DATABUF);
        if(data2ble == NULL){
            dbg_err("Malloc for data2ble failed!\n");
        }
        memset(data2ble, 0, LEN_DATABUF);
#if 0
        //sprintf(data2ble, "Info:g-filed data:\nx\ty\tz\t\n");
        sprintf(data2ble, "\nInfo:TIM: %d-%d\n", index++, (GDB->sample_block[0].counter + GDB->sample_block[1].counter));
        
        for(i = 0; i< GDB->counter; i++){
            for(j = 0; j < GDB->sample_block[i].counter; j++){
                sprintf(data2ble + strlen(data2ble), "Info:ACC(%08X): ", timeindex++);
                for(k = 0; k < 3; k++){
                    sprintf(data2ble + strlen(data2ble), "%2.7f\t", GDB->sample_block[i].axia_data[j].axis[k]);
                }
                sprintf(data2ble + strlen(data2ble), "\n");
            }
        }
        /* Shoot data to the air. */
        if(strlen(data2ble) > 0){
            dbg_info("Info: data2ble(%d)\n" , strlen(data2ble));
#define SEND_LEN 256
            for(putpage = 0; putpage <= ceil(strlen(data2ble)/SEND_LEN); putpage++){
                ble_debug_print_arry(data2ble + putpage*SEND_LEN, SEND_LEN);
                //debug_print_arry(data2ble + putpage*SEND_LEN, SEND_LEN);
                vTaskDelay(30);
                //dbg_info("ble putpage = %d\n", putpage);
            }
#ifdef SEND_LEN
#undef SEND_LEN
#endif
        }
#ifdef LEN_DATABUF
#undef LEN_DATABUF
#endif
#else
        unsigned int ti;
        for(ti = 0; ti < 512; ti++){
            sprintf(data2ble + strlen(data2ble), "%d ", ti);
        }
        /* Shoot data to the air. */
        dbg_info("strlen(data2ble) = %d\n", strlen(data2ble));
        
        if(strlen(data2ble) > 0){
            dbg_info("Info: data2ble(%d)\n" , strlen(data2ble));
#define SEND_LEN 256
            for(putpage = 0; putpage <= ceil(strlen(data2ble)/SEND_LEN); putpage++){
                ble_debug_print_arry(data2ble + putpage*SEND_LEN, SEND_LEN);
                //debug_print_arry(data2ble + putpage*SEND_LEN, SEND_LEN);
                vTaskDelay(30);
                //dbg_info("ble putpage = %d\n", putpage);
            }
#ifdef SEND_LEN
#undef SEND_LEN
#endif
        }
#ifdef LEN_DATABUF
#undef LEN_DATABUF
#endif
#endif
        vPortFree(data2ble);
    }
#endif
}
#endif

/**
 * Data management unit.
 * 1, L1 filter rountine.
 * 2, Mothion detect.
 * 3, ARM active detect.
 * 4, Log's info. management.
 */
int KX022_data_mgmt(axis_dblock_t *GDB)
{
            
    /* RAW data filer process. */
    gdata_l1filter(GDB);

    /* Motion detecting. */
    gdata_motion_detect(GDB);
    
    /* Detecting ARM motion. */
    gdata_detact_arm_active(GDB);

    /* Global Counter Clear. */
    if(GDB->counter >= 2){
        /* Leave for debuging. */
#if 0
        /* Print RAW data. */
        KX022_Print(GDB, 0);
#endif
        /* Reset global counter. */
        GDB->counter = 0;
    }

#if 0
    /* Shoot RAW to BLE channel. By Washington Ruan. */
    /* Just for debuging. Send RAW data to BLE channel. */
    static unsigned char shot_time;
    /* Shot data to the air. */
    sensor_buf[256 - 3] = shot_time++;
    sensor_buf[256 - 2] = data_length;
    ble_debug_print_arry(sensor_buf, 256);
#endif
#if 0
    /* Leave for debuging. */
    switch(ret){
        case KX022_OK:
            //DEBUG_PRINTF("Info:KX022_OK(%d).\n", GDB->counter);
        break;
        case KX022_CHE:
#if 0
            /* Print RAW data. */
            KX022_Print(GDB, 0);
#endif
            /* RAW data filer process. */
            gdata_l1filter(GDB);
#if 1
            /* Print data after l1filter. */
            KX022_Print(GDB, 1);
#endif
            /* Motion detecting. */
            gdata_motion_detect(GDB);

            /* Detecting ARM motion. */
            gdata_detact_arm_active(GDB);
            
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
#endif
    return KX022_OK;
}

/**
 * Accquire data from hardware.
 * Check data's length from FIFO and convert in to float type.
 */
int KX022_get_data(axis_dblock_t *GDB)
{
    unsigned char data_length;
    unsigned char sensor_buf[256];
    unsigned char clear = 0;
    unsigned char ret;

    /* Read RAW data length from register. */
    I2C_Gs_ReadBytes(KX022_ADDR, KX022_blen, 1, &data_length);
    //DEBUG_PRINTF("Info:Buffer length = %d\n",data_length);

    if(data_length > 0){
        
        /* Reset buffer. */
        memset(sensor_buf, 0, sizeof(sensor_buf));
        
        /* Get RAW data from FIFO. */
        I2C_Gs_ReadBytes(KX022_ADDR, KX022_dreg, data_length, sensor_buf);

        /* Clear FIFO register. */
        I2C_Gs_ReadBytes(KX022_ADDR, 0x3e, 1, &clear);       /* Clear buffer. */

        /* Convert data into axis float mode. */
        ret = KX022_db2axis(sensor_buf, data_length, GDB);
        if(ret == KX022_ERR){
            DEBUG_PRINTF("Error:KX022_db2axis(%d).\n",ret);
            
            /* Global Counter Clear. */
            GDB->counter = 0;
        
            memset(GDB, 0, sizeof(GDB));
        }
        return KX022_OK;
    }else{
        /* There is noting from FIFO. Not suppose to be here actually.
           Or maybe causeing by the error of hardware. */
        dbg_err("KX022_get_data EMPTY.\n");
        return KX022_ERR;
    }
}

/* End of this file. */

