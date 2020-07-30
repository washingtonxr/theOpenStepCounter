/**
 * @File    gs.c
 * @Date    Tuesday, Dec. 25, 2018 at 14:17:09 PM BJT
 * @Author  Washington Ruan
 * @Email   washingtonxr@live.com
 *
 * This file contains the implementation of the gravity sensor devicer
 * module's main file.
 *
 * @bug No known bugs.
 **/

#include <project.h>
#include "uart_debug.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "KX022.h"
#include "GAlgorithmCom.h"
#include "lcd/lcd_comm.h"
#include "lcd/lcd_interface.h"
#include "flash/spiflash.h"
#include "nfc_task.h"
#include "gs.h"
#include "comm/comm.h"
#if AK09918_NOTEST
#include "ak09918.h"
#endif
#ifndef NO_COMPASS
#include "Compass.h"
#endif
//#define ALLWAYS_NFC

//Sys_led_timer Red_LED, Orange_LED;  /* LED Database. */
axis_dblock_t Gsensor_DB;           /* GRAVITY Sensor Database. */
/* Just a test demo for compass. */
extern const unsigned char gImage_step_icon[];
extern unsigned char Compass_ReadEn;
#define GS_QUEUE_SIZE    5u
#if 0
#define gTimer_EN       1
#endif
#if AK09918_NOTEST
AK09918_dev_t *AK09918_DB;
#endif
static int enable = 0;
static uint32 cnt = 0;

static uint8_t set_steps_flag = 1;

    
#ifdef gTimer_EN
/* Creat a software timer for accelerometer ensor's time stamp. 
 * Modified by Washington Ruan @ Aug.7th, 2019. */
#include "timers.h"
TimerHandle_t gsTimer;
#endif

void clr_steps()
{
    dbg_info(">>>>>>>> clr_steps\n");
    Gsensor_DB.steps = 0;
}

uint32_t get_current_steps(void)
{
    return Gsensor_DB.steps;
}

void set_current_steps(uint32_t steps)
{
    dbg_info(">>>>>>>> steps = %d\n", steps);
    Gsensor_DB.steps = steps;
    set_steps_flag = 1;
}

void clr_step_with_time(void)
{
    static uint32_t last_date = 0xff;
    /* Variable used to store date and time information */
    cy_stc_rtc_config_t dateTime;

    /*Get the current RTC time and date */
    Cy_RTC_GetDateAndTime(&dateTime);

    if (last_date != dateTime.date)
    {
        last_date = dateTime.date;

        /*can not clr setp if restrore time and step*/
        if(!set_steps_flag || (dateTime.hour == 0 && dateTime.min == 0))
        {
            clr_steps();
        }
       
    }
    set_steps_flag = 0;
}

/*******************************************************************************
* Function Name: main()
********************************************************************************
* Summary:
*  Main function for the project.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Theory:
*  The main function initializes the PSoC 6 BLE device and runs a BLE process.
*
*******************************************************************************/
extern volatile int32_t activity_safe;

#ifdef gTimer_EN
void vTimerCallback(TimerHandle_t xTimer)
{    
    vTaskSuspendAll();
    check_dispaly_suspend();
    xTaskResumeAll();
    cnt++;
    if(cnt > 15 && enable)
    {
        cnt = 0;
        enable  = 0;
        SendNfcCmd(NFC_DISABLE);
    }
    
    KX022_get_data(&Gsensor_DB);
    clr_step_with_time();
    
    /* Just a test demo for compass. */
#if 1
    /* Check ak09918's data. */
    ak09918_get_data(AK09918_DB);
    DebugPrintf("Info:MAG: %7.2lf\t%7.2lf\t%7.2lf\n", AK09918_DB->x, AK09918_DB->y, AK09918_DB->z);
#endif

#if 0
    printf("GS >> %d - %d\n", Gsensor_DB.arm_active, Gsensor_DB.r2w_active);

    if (Gsensor_DB.arm_active && activity_safe)
    {
        if(!enable)
        {
            enable = 1;
            cnt = 0;
            SendNfcCmd(NFC_ENABLE);
        }
    }
#endif
    /* Raising detected and light up LCD screen. */
    if(Gsensor_DB.r2w_active && !g_config.func_switch.close_lift_wrist_bright)
    {
        set_lcd_on();
        post_next_doing_service();
#if 0
        Cy_GPIO_Set(Motor_Pin_0_PORT, Motor_Pin_0_NUM);
        vTaskDelay(30);
        Cy_GPIO_Clr(Motor_Pin_0_PORT, Motor_Pin_0_NUM);
#endif
    }else
    /* Trun off the screen(dim) while detecting ARM's falling down. */
    if(Gsensor_DB.r4s_active && !get_display_wrist_lock()){
        set_dispaly_suspend();
    }
}
#endif
#ifndef NO_COMPASS
extern Compass_info_t Compass_Data;
static void Compass_routine(void)
{
    unsigned char ret;
    
    /* Get the angle from compass. */
    ret = Compass_Direction(Gsensor_DB.sample_block, &Compass_Data);
    if(ret != MAG_OK){
        dbg_err("Compass_Direction latch failed.\n");
    }
    
#if 0
    DebugPrintf("Info: True_azimuth(%d) = %4.4f\n", Compass_Data.data_state , Compass_Data.true_azimuth);
#endif

}
#endif
static void Motion_routine(void)
{
    cnt++;
#ifdef ALLWAYS_NFC        
    if(0)
#else
    if(cnt > 15 && enable)
#endif
    {
        cnt = 0;
        enable  = 0;
        SendNfcCmd(NFC_DISABLE);
    }
    
    KX022_get_data(&Gsensor_DB);

    KX022_data_mgmt(&Gsensor_DB);

#if 1
    //printf("GS >> %d - %d\n", Gsensor_DB.arm_active, Gsensor_DB.r2w_active);
#ifdef ALLWAYS_NFC  
    if(1)
#else
    if (Gsensor_DB.arm_active && activity_safe)
#endif            
    {
        if(!enable)
        {
            enable = 1;
            cnt = 0;
            SendNfcCmd(NFC_ENABLE);
        }
    }
#endif
    /* Raising detected and light up LCD screen. */
    if(Gsensor_DB.r2w_active && !g_config.func_switch.close_lift_wrist_bright)
    {
        dbg_info("r2w_active");
        set_lcd_on();
        post_next_doing_service();
#if 0
        Cy_GPIO_Set(Motor_Pin_0_PORT, Motor_Pin_0_NUM);
        vTaskDelay(30);
        Cy_GPIO_Clr(Motor_Pin_0_PORT, Motor_Pin_0_NUM);
#endif
    }else
    /* Trun off the screen(dim) while detecting ARM's falling down. */
    if(Gsensor_DB.r4s_active && !get_display_wrist_lock()){
        set_dispaly_suspend();
    }
}

void task_gs(void* pvParameters)
{   
#ifndef gTimer_EN
    TickType_t tickstart, tickend, tickdeta;
#endif

    (void)pvParameters;
    
	I2C_1_Start();

    vTaskDelay(100);

    /* Gravity Sensor. */
    KX022_Init(&Gsensor_DB);

    vTaskDelay(100);
#ifndef NO_COMPASS
    /* Initialize the compass. */
    Compass_Init();
#endif
#if 0    
    /* Just a test demo for compass. */
#if AK09918_NOTEST
    int ret;    

    AK09918_DB = (AK09918_dev_t *)pvPortMalloc(sizeof(AK09918_dev_t));
    if(AK09918_DB == NULL){
        DebugPrintf("Error: Memory allocate failed - AK09918_DB.\n");
    }
    
    /* Initializing AK09918. */
    ret = ak09918_init(AK09918_DB);
    if(ret != AK09918_ERR_OK){
        DebugPrintf("Error: Initialize ak09918 failed(%d).\n", ret);
    }
#endif
#endif

#ifdef gTimer_EN
    /* Initilizing xTimer. */
    gsTimer = xTimerCreate
           ( /* Just a text name, not used by the RTOS
             kernel. */
             "Accelerometer sensor Timer",
             /* The timer period in ticks, must be
             greater than 0. */
             500,
             /* The timers will auto-reload themselves
             when they expire. */
             pdTRUE,
             /* The ID is used to store a count of the
             number of times the timer has expired, which
             is initialised to 0. */
             ( void * ) 0,
             /* Each timer calls the same callback when
             it expires. */
             vTimerCallback
           );
    
    if(gsTimer == NULL){
        printf("Error: Create gTimer failed.\n");
    }else{
        if(xTimerStart(gsTimer, 0) != pdPASS){
            printf("Error: gTimer could not be set into the active state.\n");
        }
    }
#endif

    vTaskDelay(1000);

	/* Restore step number after an abnormal reset. */
	restore_step();

    while(1)
    {

#ifndef gTimer_EN
        tickstart = xTaskGetTickCount();

        vTaskSuspendAll();
        check_dispaly_suspend();
        xTaskResumeAll();

        /* Motion routine. */
        Motion_routine();
#ifndef NO_COMPASS
        dbg_info("Compass_ReadEn = %d\n", Compass_ReadEn);
        /* Compass routine. */
        if(Compass_ReadEn){
            
            /* Continue light up screen. */
            update_dispaly_timeout();
            set_display_wrist_lock(1);
        
            /* Check compass. */
            Compass_routine();
        }
#endif
        tickend = xTaskGetTickCount();
#if 0
        dbg_info("%08x,%08x,%d \n", tickstart, tickend, (tickend - tickstart));
#endif
        if(tickend >= tickstart){
            tickdeta = (tickend - tickstart);
            if(tickdeta < LATCH_NORMAL_PERIOD){
                vTaskDelay(LATCH_NORMAL_PERIOD - tickdeta);
            }
        }else{
            tickdeta = (tickstart - tickend);
            if( tickdeta < LATCH_NORMAL_PERIOD){
                vTaskDelay(LATCH_NORMAL_PERIOD - tickdeta);
            }
        }
#else
        vTaskDelay(1000);
#endif

    }
}



#if 1
int8_t show_step(activity_t * para)
{
#define SETP_ICON_X 2
#define SETP_ICON_Y 204
#define SETP_DATA_X 28


    LCD_DispIcon(SETP_ICON_X, SETP_ICON_Y, gImage_step_icon, RGB888ToRGB565(0x0CDD9E));
#if 0

    LCD_DispDigit(DIGIT_12_ADDR, (Gsensor_DB.steps/100000)%10, SETP_DATA_X, SETP_ICON_Y);
    LCD_DispDigit(DIGIT_12_ADDR, (Gsensor_DB.steps/10000)%10, SETP_DATA_X+DIGIT_12_WIDE, SETP_ICON_Y);
    LCD_DispDigit(DIGIT_12_ADDR, (Gsensor_DB.steps/1000)%10, SETP_DATA_X+DIGIT_12_WIDE*2, SETP_ICON_Y);
    LCD_DispDigit(DIGIT_12_ADDR, (Gsensor_DB.steps/100)%10, SETP_DATA_X+DIGIT_12_WIDE*3, SETP_ICON_Y);
    LCD_DispDigit(DIGIT_10_ADDR, (Gsensor_DB.steps/10)%10, SETP_DATA_X+DIGIT_12_WIDE*4, SETP_ICON_Y);
    LCD_DispDigit(DIGIT_10_ADDR, (Gsensor_DB.steps/1)%10, SETP_DATA_X+DIGIT_12_WIDE*5, SETP_ICON_Y);
#else
    DispOneChar((Gsensor_DB.steps/100000)%10, SETP_DATA_X,SETP_ICON_Y,RGB888ToRGB565(0x0CDD9E),BLACK);

    DispOneChar((Gsensor_DB.steps/10000)%10, SETP_DATA_X+CHAR_MIN_FONT_W,SETP_ICON_Y,RGB888ToRGB565(0x0CDD9E),BLACK);

    DispOneChar((Gsensor_DB.steps/1000)%10, SETP_DATA_X+CHAR_MIN_FONT_W*2,SETP_ICON_Y,RGB888ToRGB565(0x0CDD9E),BLACK);

    DispOneChar((Gsensor_DB.steps/100)%10, SETP_DATA_X+CHAR_MIN_FONT_W*3,SETP_ICON_Y,RGB888ToRGB565(0x0CDD9E),BLACK);

    DispOneChar((Gsensor_DB.steps/10)%10,  SETP_DATA_X+CHAR_MIN_FONT_W*4,SETP_ICON_Y,RGB888ToRGB565(0x0CDD9E),BLACK);

    DispOneChar(Gsensor_DB.steps%10,  SETP_DATA_X+CHAR_MIN_FONT_W*5,SETP_ICON_Y,RGB888ToRGB565(0x0CDD9E),BLACK);
#endif
    return 0;
}
#endif


#define STEP_FLAG "setp"
int8_t save_step(void)
{
    uint8_t buf[64];
    uint32_t len = 0;
    
    cy_stc_rtc_config_t dateTime;

    memset(buf,0,sizeof(buf));
    
    /*Get the current RTC time and date */
    Cy_RTC_GetDateAndTime(&dateTime);

    spi_flash_write(TODAY_STEP_ADDR, STEP_FLAG, strlen(STEP_FLAG));

    memcpy(buf, (uint8_t*)&dateTime, sizeof(cy_stc_rtc_config_t));
    len += sizeof(cy_stc_rtc_config_t);
    
    memcpy(buf+len, (uint8_t*)&(Gsensor_DB.steps), sizeof(uint32_t));
    len += sizeof(uint32_t);

    dbg_dbg("save  step begin \n")
    spi_flash_write(TODAY_STEP_ADDR+strlen(STEP_FLAG), buf, len);
    dbg_info("save  step ok\n")

    return 0;
}



bool is_need_to_restore_step(void)
{
    uint8_t buf[10]= {0};

    spi_flash_read(TODAY_STEP_ADDR,  buf, sizeof(STEP_FLAG));
    if(0 == memcmp(buf, STEP_FLAG , strlen(STEP_FLAG)))
    {
        return true;
    }
    return false;
}

int8_t restore_step(void)
{
    cy_stc_rtc_config_t dateTimeSave;
    cy_stc_rtc_config_t dateTime;
    uint32_t steps;

    if(is_need_to_restore_step())
    {
        spi_flash_read(TODAY_STEP_ADDR+strlen(STEP_FLAG), 
        (uint8_t*)&dateTimeSave, sizeof(cy_stc_rtc_config_t));
        
        dbg_info("\r\n <<--restore_step-->>\r\n");
        DebugPrintf("Date %02u/%02u/%02u\r\n",(uint16_t) dateTimeSave.date, 
                (uint16_t) dateTimeSave.month, (uint16_t) dateTimeSave.year);
        DebugPrintf("Time %02u:%02u:%02u\r\n", (uint16_t) dateTimeSave.hour, 
                (uint16_t) dateTimeSave.min, (uint16_t) dateTimeSave.sec);
 
        /*Get the current RTC time and date */
        Cy_RTC_GetDateAndTime(&dateTime);

        if(dateTime.year == dateTimeSave.year && 
            dateTime.month == dateTimeSave.month &&
             dateTime.date == dateTimeSave.date &&
             
             (dateTime.hour > dateTimeSave.hour ||
             (dateTime.hour == dateTimeSave.hour && 
             dateTime.min >= dateTimeSave.min)))

        {
            spi_flash_read(TODAY_STEP_ADDR+strlen(STEP_FLAG)+sizeof(cy_stc_rtc_config_t),
            (uint8_t *)&steps, sizeof(uint32_t));
            dbg_info("restore step begin\n");
            vTaskSuspendAll();
            spi_flash_write(TODAY_STEP_ADDR, "\x00\x00\x00\x00", strlen(STEP_FLAG));
            xTaskResumeAll();
            dbg_info("restore step ok!\n");

            if(get_current_steps() < steps)
            {
                set_current_steps(steps);
            }
            
            return 0;
        }
        
    }
    
    return -1;
}


/* [] END OF FILE */
