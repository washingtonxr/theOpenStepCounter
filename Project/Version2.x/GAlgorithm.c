/**
 * @File    GAlgorithm.c
 * @Date    Sunday, Jan. 5, 2019 at 17:38:56 PM BJT
 * @Author  Washington Ruan
 * @Email   washingtonxr@live.com
 *
 * This file contains the implementation of the gravity sensor device for motion detecting
 * module.
 *
 * @bug No known bugs.
 **/
#include "GAlgorithm.h"
#include <string.h>
#include <math.h>

/* New structure. By Washington Ruan, Jul.17th,2019. */
static g_value_attribute_t GData_4_peak;
static unsigned char steps_cnt;
static unsigned char cnter_status;

/* Gravity data high frequency filter. */
static gslid_reg_t GData_flowslid;

/**
 * Initializing peak structure.
 */
static void gdata_peak_init(g_value_attribute_t *Data_Peak)
{
    memset(Data_Peak, 0, sizeof(g_value_attribute_t));
}

/**
 * Initializing whole structure.
 */
void gdata_init(axis_dblock_t *GDB)
{
    GDB->counter = 0;
    
    memset(GDB, 0, sizeof(axis_dblock_t));
    memset(&GData_flowslid, 0, sizeof(gslid_reg_t));
    
    gdata_peak_init(&GData_4_peak);
}

/**
 * Filter data and insert data to structure.
 */
void gdata_l1filter(axis_dblock_t *GDB)
{
    unsigned char i, j, m, n;

    i = GDB->counter - 1;
    {
        memset(&GDB->ave_block[i], 0, sizeof(GDB->ave_block[i]));
        /* Try 4 Filter counter. */
        GDB->ave_block[i].counter = GDB->sample_block[i].counter/FILTER_CNT;

        /* Set data block's counter. */
        for(j = 0; j < GDB->ave_block[i].counter; j++){
            
            /* Initializing Average Buffer. */
            for(m = 0; m < FILTER_CNT; m++){
                for(n = 0; n < AXIS_NUM; n++)
                    GDB->ave_block[i].ave_data[j].axis[n] = 0;
            }
            
            /* Fill realy time data inside Average Buffer. */
            for(m = 0; m < FILTER_CNT; m++){
                
                for(n = 0; n < AXIS_NUM; n++)
                    GDB->ave_block[i].ave_data[j].axis[n] += GDB->sample_block[i].axia_data[FILTER_CNT*j + m].axis[n];
            }

            /* Get average value of each axis. */
            GDB->ave_block[i].vector_data[j] = 0;

            for(n = 0; n < AXIS_NUM; n++){
                GDB->ave_block[i].ave_data[j].axis[n] = GDB->ave_block[i].ave_data[j].axis[n]/FILTER_CNT;
                //GDB->ave_block[i].vector_data[j] += pow(GDB->ave_block[i].ave_data[j].axis[n], 2);
            }
            
            /* Caculate vector value.*/
            //GDB->ave_block[i].vector_data[j] = (float)sqrt(GDB->ave_block[i].vector_data[j]);

            /* Set time stamp of each axis. */
            GDB->ave_block[i].ms_stamp[j] = j;
        }
    }
    return;
}

/**
 * Refresh new Peak to structure.
 */
static void gdata_PeakRefresh(g_value_attribute_t *Data_Peak, axis_info_t *Current_sample)
{
    unsigned char i;
    
    /* Get max & min value of peak. */
    for(i = 0; i < AXIS_NUM; i++){
        Data_Peak->new_peak.axis[i].axis_max = MAX(Data_Peak->new_peak.axis[i].axis_max, Current_sample->axis[i]);
        Data_Peak->new_peak.axis[i].axis_min = MIN(Data_Peak->new_peak.axis[i].axis_min, Current_sample->axis[i]);
    }
 #if 0   
    float vector4axis;
    vector4axis = sqrt(pow(Current_sample->axis[0],2)+ pow(Current_sample->axis[1],2)+pow(Current_sample->axis[2],2));
    Data_Peak->new_peak.vector_axis.axis_max = MAX(Data_Peak->new_peak.vector_axis.axis_max, vector4axis);
    Data_Peak->new_peak.vector_axis.axis_min = MIN(Data_Peak->new_peak.vector_axis.axis_min, vector4axis);
#endif
}

/**
 * Filter high frequency sensor data.
 */
static char gdata_slid_update(gslid_reg_t *slid, axis_info_t *Current_sample)
{
  	char res = 0;
    unsigned char i;
    float tmp;

    for(i = 0; i < AXIS_NUM; i++){
        
        /* Requre ABS value. */
        tmp = (float)ABS((Current_sample->axis[i] - slid->New_sample.axis[i]));
#if 0
        DEBUG_PRINTF("%f\t%f\t\n", Current_sample->axis[i], slid->New_sample.axis[i]);
        DEBUG_PRINTF("Info:gdata_slid(%d) = %f\n", i, tmp);
#endif
        if (tmp > (float)DYNAMIC_PRECISION) {
    		slid->Old_sample.axis[i] = slid->New_sample.axis[i];
    		slid->New_sample.axis[i] = Current_sample->axis[i];
    		res = 1;
    	} else {
    		slid->Old_sample.axis[i] = slid->New_sample.axis[i];
    	}
    }
	return res;
}

/**
 * Detate ARM motion axis.
 */
void gdata_detact_arm_active(axis_dblock_t *GDB)
{
    unsigned char i, j, len_g;
    unsigned char k;
    axis_info_t active_axis;

    

    typedef struct{
        float axis_max;
        unsigned char max_location;
        float axis_min;
        unsigned char min_location;
    }axis_feature;

    axis_feature axis_fd[AXIS_NUM];

    /* Check for each counter data block. */
    i = GDB->counter - 1;
    {     
        /* Check for each axis. */
        for(k = 0; k < AXIS_NUM; k++){
            
            /* Initializing. */
            axis_fd[k].axis_max = GDB->ave_block[i].ave_data[0].axis[k];
            axis_fd[k].max_location = 0;
            axis_fd[k].axis_min = GDB->ave_block[i].ave_data[0].axis[k];
            axis_fd[k].min_location = 0;
            
            /* Latch MAX axis. */
            for(j = 1; j<GDB->ave_block[i].counter; j++){
                axis_fd[k].axis_max = MAX(axis_fd[k].axis_max, GDB->ave_block[i].ave_data[j].axis[k]);
                if(axis_fd[k].axis_max == GDB->ave_block[i].ave_data[j].axis[k]){
                    axis_fd[k].max_location = GDB->ave_block[i].ms_stamp[j];
                }
            }
            
            /* Latch MIN axis. */
            for(j = 1; j<GDB->ave_block[i].counter; j++){
                axis_fd[k].axis_min = MIN(axis_fd[k].axis_min, GDB->ave_block[i].ave_data[j].axis[k]);
                if(axis_fd[k].axis_min == GDB->ave_block[i].ave_data[j].axis[k]){
                    axis_fd[k].min_location = GDB->ave_block[i].ms_stamp[j];
                }
            }
#if 0
            printf("%d\t%7.2f-%d\t%7.2f-%d\n", k, axis_fd[k].axis_max, axis_fd[k].max_location, axis_fd[k].axis_min, axis_fd[k].min_location);
#endif
        }
    }
    
    /* Compare each axis's value and define activity axis. */
    for(k = 0; k < AXIS_NUM; k++)
        active_axis.axis[k] = (float)ABS(axis_fd[k].axis_max - axis_fd[k].axis_min);

#ifndef NO_RAISEPAYMENT
    /* The mode for detect payment. */
    if(((active_axis.axis[0] > (float)ARM_PRECISION) && (active_axis.axis[1] > (float)ARM_PRECISION))\
        ||((active_axis.axis[1] > (float)ARM_PRECISION) && (active_axis.axis[2] > (float)ARM_PRECISION))\
        ||((active_axis.axis[2] > (float)ARM_PRECISION) && (active_axis.axis[0] > (float)ARM_PRECISION))){

        if((axis_fd[0].max_location > axis_fd[0].min_location)\
            &&(axis_fd[1].min_location > axis_fd[1].max_location)){
            if((axis_fd[0].max_location - axis_fd[0].min_location >= 2.0f)\
                &&((axis_fd[1].min_location - axis_fd[1].max_location >= 2.0f))){
                GDB->arm_active = 1;
                printf("Info: arm_active in right hand.\n");
            }
        }else 
        if((axis_fd[1].max_location > axis_fd[1].min_location)\
            &&(axis_fd[0].max_location > axis_fd[0].min_location)){
            if((axis_fd[1].max_location - axis_fd[1].min_location >= 2.0f)\
                &&((axis_fd[0].min_location - axis_fd[0].max_location >= 2.0f))){
                GDB->arm_active = 1;
                printf("Info: arm_active in left hand.\n");
            }
        }
    }
    else{
        GDB->arm_active = 0;
    }
#endif

  /* The mode for detect bright screen startup. */
    if((active_axis.axis[0] > (float)R2W_PRECISION) || \
        (active_axis.axis[1] > (float)R2W_PRECISION) || \
        (active_axis.axis[2] > (float)R2W_PRECISION)){

        j = GDB->ave_block[i].counter - 1;
        len_g = GDB->sample_block[i].counter - 1;
#if 0
        if((axis_fd[0].min_location > axis_fd[0].max_location)
        &&(axis_fd[0].min_location - axis_fd[0].max_location >= 1.0f)){
            printf("Prepare.\n");
            printf("Ymin = %f, Ymax = %f, YLMax %d, YLMin %d, Ylast = %f, YTLast = %f, YTFirst = %f\n", \
            axis_fd[1].axis_min, axis_fd[1].axis_max ,axis_fd[1].max_location ,\
            axis_fd[1].min_location, (float)GDB->ave_block[i].ave_data[j].axis[1],\
            (float)GDB->sample_block[i].axia_data[len_g].axis[1],\
            (float)GDB->sample_block[i].axia_data[0].axis[1]);

            printf("Zmin = %f, Zmax = %f, ZLMax %d, ZLMin %d, Zlast = %f, ZTLast = %f,ZTFirst = %f\n", \
            axis_fd[2].axis_min, axis_fd[2].axis_max ,axis_fd[2].max_location ,\
            axis_fd[2].min_location, (float)GDB->ave_block[i].ave_data[j].axis[2],\
             (float)GDB->sample_block[i].axia_data[len_g].axis[2],\
             (float)GDB->sample_block[i].axia_data[0].axis[2]);


            printf("Xmin = %f, Xmax = %f, XLMax %d, XLMin %d, Xlast = %f, XTLast = %f,XTFirst = %f\n", \
            axis_fd[0].axis_min, axis_fd[0].axis_max ,axis_fd[0].max_location ,\
            axis_fd[0].min_location, (float)GDB->ave_block[i].ave_data[j].axis[0],\
             (float)GDB->sample_block[i].axia_data[len_g].axis[0],\
             (float)GDB->sample_block[i].axia_data[0].axis[0]); 
        }
#endif
        /* Writhing*/
        if (fabsf((float)GDB->sample_block[i].axia_data[0].axis[2])>= 7.0f \
        && fabsf((float)GDB->sample_block[i].axia_data[0].axis[0]) <= 4.2f){
            //printf("Case 4-1.\n");
            if ((axis_fd[0].min_location > axis_fd[0].max_location) \
            && (active_axis.axis[2]>= 0.9f) \
            && (active_axis.axis[0]>=2.0f) \
            && ((float)GDB->sample_block[i].axia_data[len_g].axis[0] <= -1.5f)){
                //printf("Case 4.\n");
                GDB->r2w_active = 1;
            }else{
                GDB->r2w_active = 0;
                //printf("Case 4 -no.\n");
            }

        }
        else{
        if ((axis_fd[0].min_location > axis_fd[0].max_location) \
        && (active_axis.axis[1] <= 5.5f) && (axis_fd[1].axis_max >= -4.3f) \
        && (axis_fd[1].axis_max <= 3.5f) \
        && (fabsf((float)GDB->sample_block[i].axia_data[len_g].axis[0]- (float)GDB->sample_block[i].axia_data[0].axis[0])>= 1.0f) \
        && (fabsf((float)GDB->sample_block[i].axia_data[len_g].axis[1]) <= 3.5f) \
        && (active_axis.axis[2] >= 0.3f || abs(axis_fd[2].min_location-axis_fd[2].max_location)>=2) \
        && (active_axis.axis[0] >= 0.5f || axis_fd[0].min_location-axis_fd[0].max_location >= 2)){
            //printf("Case 1!!\n");
            //if((axis_fd[2].max_location > axis_fd[2].min_location)\
            &&((float)GDB->sample_block[i].axia_data[len_g].axis[2] >= 3.8f)){
                //printf("Case 1-1.\n");
                if (((float)GDB->sample_block[i].axia_data[len_g].axis[0] <= 3.5f)){
                    //printf("Case 1-2.\n");
                    GDB->r2w_active = 1;
                }else{
                GDB->r2w_active = 0;
                }
            }else
                /* Raise and writhing*/
                if ((axis_fd[0].min_location > axis_fd[0].max_location) \
                && (active_axis.axis[1] >= 4.0f || abs(axis_fd[1].min_location-axis_fd[1].max_location) >= 4) \
                && (active_axis.axis[0] >= 4.0f || (axis_fd[0].min_location-axis_fd[0].max_location) >= 2) \
                && (active_axis.axis[2] >= 3.3f || (float)GDB->sample_block[i].axia_data[len_g].axis[0] <= -3.5f)
                && (fabsf((float)GDB->sample_block[i].axia_data[len_g].axis[1]) <= 5.0f)
                && ((float)GDB->sample_block[i].axia_data[len_g].axis[0] <= 3.5f)){
                     //printf("Case 2.\n");
                    GDB->r2w_active = 1;
                }
                else
                /*Raise and down or Raise hand*/
                    if ((axis_fd[2].max_location > axis_fd[2].min_location) \
                    && (fabsf((float)GDB->sample_block[i].axia_data[len_g].axis[2]- (float)GDB->sample_block[i].axia_data[0].axis[2])>= 0.8f) \
                    && (fabsf((float)GDB->sample_block[i].axia_data[len_g].axis[1]- (float)GDB->sample_block[i].axia_data[0].axis[1])>=1.0f)\
                    && (fabsf((float)GDB->sample_block[i].axia_data[len_g].axis[1]) <= 3.5f)\
                    && (active_axis.axis[2] >= 4.0f || (axis_fd[2].max_location-axis_fd[2].min_location) >= 2) && 
                    (float)GDB->sample_block[i].axia_data[len_g].axis[0] <= 3.5f){
                        GDB->r2w_active = 1;
                        //printf("Case 3.\n");
                    }else{
                    GDB->r2w_active = 0;
                    }
        }
        }else{
                GDB->r2w_active = 0;
            }
            
#if 0
        /* Mode 1. */
        /* Determine the slope of data state curve of each axis. */
        if((axis_fd[2].max_location > axis_fd[2].min_location)\
            &&(axis_fd[0].min_location > axis_fd[0].max_location)){
            //DEBUG_PRINTF("Case 1: Check step 1.\n");
            if((axis_fd[2].max_location - axis_fd[2].min_location >= 1.0f)\
                &&((axis_fd[0].min_location - axis_fd[0].max_location >= 1.0f))){
                //DEBUG_PRINTF("Check step 2.\n");
                if((axis_fd[2].axis_max >= 9.8f - 6.0f)\
                    &&(axis_fd[2].axis_max - axis_fd[2].axis_min >= 1.0f)){
                    //DEBUG_PRINTF("Check step 3.\n");
                    if((axis_fd[0].axis_max - axis_fd[0].axis_min >= 0.6f)){
                        //DEBUG_PRINTF("Light up!\n");
                        GDB->r2w_active = 1;
                    }else{
                        GDB->r2w_active = 0;
                    }
                }else{
                    GDB->r2w_active = 0;
                }
            }else{
                GDB->r2w_active = 0;
            }
        }
        /* Mode 2. */
        else
        if((axis_fd[0].min_location > axis_fd[0].max_location) \
            &&(axis_fd[0].axis_max - axis_fd[0].axis_min >= 2.0f)){
            //DEBUG_PRINTF("Case 2: Check step 1.\n");
            if(axis_fd[2].axis_max >= 9.8f - 4.0f){
                //DEBUG_PRINTF("Case 2: Check step 2.\n");
                GDB->r2w_active = 1;
            }else{
                GDB->r2w_active = 0;
            }
        }else{
            GDB->r2w_active = 0;
        }
    }
    else{
        GDB->r2w_active = 0;
    }
#endif
    /* The mode for detect off screen. */
    if (0 == GDB->r2w_active ){
        if((active_axis.axis[0] > (float)R4S_PRECISION) || \
        (active_axis.axis[1] > (float)R4S_PRECISION) || \
        (active_axis.axis[2] > (float)R4S_PRECISION)){

#if 0
            if((axis_fd[2].max_location < axis_fd[2].min_location) && \
            (axis_fd[2].min_location - axis_fd[2].max_location >= 1) && \
            (axis_fd[2].axis_max - axis_fd[2].axis_min >= 0.8f) && \
            (axis_fd[2].axis_min < 9.8f - 4.0f) &&){

                //printf("axis_fd[2].axis_min = %4.4f===================================\n", axis_fd[2].axis_min);
                if (((float)GDB->ave_block[i].ave_data[j].axis[2] < 4.2f) && \
                ((float)GDB->ave_block[i].ave_data[j].axis[0] > 1.5f)){
                    vTaskDelay(250);
                     //printf("Light off!\n");
                    GDB->r4s_active = 1;
#if 1
                    printf("Light off.\n");
                    printf("Ymin = %f, Ymax = %f, YLMax %d, YLMin %d, Ylast = %f\n", \
                    axis_fd[1].axis_min, axis_fd[1].axis_max ,axis_fd[1].max_location ,\
                    axis_fd[1].min_location, (float)GDB->ave_block[i].ave_data[j].axis[1]);
                    printf("Zmin = %f, Zmax = %f, ZLMax %d, ZLMin %d, Zlast = %f\n", \
                    axis_fd[2].axis_min, axis_fd[2].axis_max ,axis_fd[2].max_location ,\
                    axis_fd[2].min_location, (float)GDB->ave_block[i].ave_data[j].axis[2]);
                    printf("Xmin = %f, Xmax = %f, XLMax %d, XLMin %d, Xlast = %f\n", \
                    axis_fd[0].axis_min, axis_fd[0].axis_max ,axis_fd[0].max_location ,\
                    axis_fd[0].min_location, (float)GDB->ave_block[i].ave_data[j].axis[0]); 
#endif
                }
                }
#endif
#if 0
        DEBUG_PRINTF("Screen light off....\n");
#endif
            if((axis_fd[0].max_location > axis_fd[0].min_location) &&
            ((float)GDB->ave_block[i].ave_data[j].axis[0] >= 6.5f) \
            && axis_fd[2].axis_min >= -5.0f 
            && ((float)GDB->ave_block[i].ave_data[j].axis[2] <= 4.0f)){
                vTaskDelay(250);
                // printf("Light off. Case 1\n");
                // printf("Ymin = %f, Ymax = %f, YLMax %d, YLMin %d, Ylast = %f\n", \
                // axis_fd[1].axis_min, axis_fd[1].axis_max ,axis_fd[1].max_location ,\
                // axis_fd[1].min_location, (float)GDB->ave_block[i].ave_data[j].axis[1]);
                // printf("Zmin = %f, Zmax = %f, ZLMax %d, ZLMin %d, Zlast = %f\n", \
                // axis_fd[2].axis_min, axis_fd[2].axis_max ,axis_fd[2].max_location ,\
                // axis_fd[2].min_location, (float)GDB->ave_block[i].ave_data[j].axis[2]);
                // printf("Xmin = %f, Xmax = %f, XLMax %d, XLMin %d, Xlast = %f\n", \
                // axis_fd[0].axis_min, axis_fd[0].axis_max ,axis_fd[0].max_location ,\
                // axis_fd[0].min_location, (float)GDB->ave_block[i].ave_data[j].axis[0]); 
                GDB->r4s_active = 1;
            }

            else
            if ((axis_fd[1].axis_min)< -5.8f || (axis_fd[1].axis_max) > 5.8f \
            && (axis_fd[0].max_location > axis_fd[0].min_location)){
                //printf("Ymin = %f, Ymax = %f, YLMax %d, YLMin %d, Ylast = %f\n", \
                axis_fd[1].axis_min, axis_fd[1].axis_max ,axis_fd[1].max_location );
                vTaskDelay(250);
                // printf("Light off. Case 2\n");
                // printf("Ymin = %f, Ymax = %f, YLMax %d, YLMin %d, Ylast = %f\n", \
                // axis_fd[1].axis_min, axis_fd[1].axis_max ,axis_fd[1].max_location ,\
                // axis_fd[1].min_location, (float)GDB->ave_block[i].ave_data[j].axis[1]);

                // printf("Zmin = %f, Zmax = %f, ZLMax %d, ZLMin %d, Zlast = %f\n", \
                // axis_fd[2].axis_min, axis_fd[2].axis_max ,axis_fd[2].max_location ,\
                // axis_fd[2].min_location, (float)GDB->ave_block[i].ave_data[j].axis[2]);

                // printf("Xmin = %f, Xmax = %f, XLMax %d, XLMin %d, Xlast = %f\n", \
                // axis_fd[0].axis_min, axis_fd[0].axis_max ,axis_fd[0].max_location ,\
                // axis_fd[0].min_location, (float)GDB->ave_block[i].ave_data[j].axis[0]); 

                GDB->r4s_active = 1;
            }else{
            GDB->r4s_active = 0;
            }
        
    }else{
        GDB->r4s_active = 0;
    }
    }else{
        GDB->r4s_active = 0;
    }

    return;
}


/**
 * Detect active axis.
 */
static char gdata_detect_active_axis(g_value_attribute_t *Data_Peak)
{
	unsigned char ret = MOST_ACTIVE_NULL;
    unsigned char i;
    float axis_change[AXIS_NUM];
    // float vector_threshold = 1.5;
    
    for(i = 0; i < AXIS_NUM; i++){
        axis_change[i] = (float)ABS(Data_Peak->new_peak.axis[i].axis_max - Data_Peak->new_peak.axis[i].axis_min);
#if 0
        DEBUG_PRINTF("Info:axis_change(%d) = %f\n", i, axis_change[i]);
#endif
    }
   
    if(axis_change[0] > axis_change[1] && axis_change[0] > axis_change[2] && axis_change[0] >= ACTIVE_PRECISION){
        // printf("vector_change is %f.\n",Data_Peak->new_peak.vector_axis.axis_max - Data_Peak->new_peak.vector_axis.axis_min);
        ret = MOST_ACTIVE_X;
    }
    else
    if(axis_change[1] > axis_change[0] && axis_change[1] > axis_change[2] && axis_change[1] >= ACTIVE_PRECISION){
        // printf("vector_change is %f.\n",Data_Peak->new_peak.vector_axis.axis_max - Data_Peak->new_peak.vector_axis.axis_min);
        ret = MOST_ACTIVE_Y;
    }
    else
    if(axis_change[2] > axis_change[0] && axis_change[2] > axis_change[1] && axis_change[2] >= ACTIVE_PRECISION*2){
        // printf("vector_change is %f.\n",Data_Peak->new_peak.vector_axis.axis_max - Data_Peak->new_peak.vector_axis.axis_min);
        ret = MOST_ACTIVE_Z;
    }
	return ret; 
} 

/**
 * Detect which axis is most activity.
 */
static unsigned char gdata_detect_processing(g_value_attribute_t *Data_Peak, gslid_reg_t *slid, axis_info_t *Current_sample)
{
    unsigned char Step_counter = 0;
    unsigned char ret = 0;
    float threshold[AXIS_NUM];
    
#if 0
    DEBUG_PRINTF("Info:gdata_detact_active_axis = %d.\n", ret);
#endif
    
    /* Latch active axis. */
    ret = gdata_detect_active_axis(Data_Peak);

    switch (ret) {
        case MOST_ACTIVE_X:
            threshold[0] = (Data_Peak->eld_peak.axis[0].axis_max + Data_Peak->eld_peak.axis[0].axis_min) / 2;
#if 0
            DEBUG_PRINTF("X:%f\t%f\t%f\t\n",Data_Peak->OldMax.axis[0],Data_Peak->OldMin.axis[0],threshold[0]);
            DEBUG_PRINTF("%f\t%f\t\n",slid->Old_sample.axis[0],slid->New_sample.axis[0]);
#endif
            if (slid->Old_sample.axis[0] > threshold[0] && slid->New_sample.axis[0] < threshold[0]) {
                Step_counter ++;
               
            }
        break;
            
        case MOST_ACTIVE_Y:
            threshold[1] = (Data_Peak->eld_peak.axis[1].axis_max + Data_Peak->eld_peak.axis[1].axis_min) / 2;
#if 0
            DEBUG_PRINTF("Y:%f\t%f\t%f\t\n",Data_Peak->OldMax.axis[1],Data_Peak->OldMin.axis[1],threshold[1]);
            DEBUG_PRINTF("%f\t%f\t\n",slid->Old_sample.axis[1],slid->New_sample.axis[1]);
#endif
            if (slid->Old_sample.axis[1] > threshold[1] && slid->New_sample.axis[1] < threshold[1]) {
                Step_counter ++;
            }
        break;
            
        case MOST_ACTIVE_Z:
            threshold[2] = (Data_Peak->eld_peak.axis[2].axis_max + Data_Peak->eld_peak.axis[2].axis_min) / 2;
#if 0
            DEBUG_PRINTF("Z:%f\t%f\t%f\t\n",Data_Peak->OldMax.axis[2],Data_Peak->OldMin.axis[2],threshold[2]);
            DEBUG_PRINTF("%f\t%f\t\n",slid->Old_sample.axis[2],slid->New_sample.axis[2]);
#endif
            if (slid->Old_sample.axis[2] > threshold[2] && slid->New_sample.axis[2] < threshold[2]) {
                Step_counter ++;
            }
        break;
            
        case MOST_ACTIVE_NULL:
        break;
        
        default: 
        break;
    }
    return Step_counter;
}

/**
 * Motion detecting.
 */
unsigned int gdata_motion_detect(axis_dblock_t *GDB)
{
    unsigned char i, j;
    unsigned char ret;
    unsigned char steps_temp = 0;

    if(GDB->counter >= DATA_ROUND){
#if 0
        DEBUG_PRINTF("Start motion detect.\n");
#endif   
        /* Packup new maximul and minimul data to old buffer. */
        GData_4_peak.eld_peak = GData_4_peak.new_peak;
    
        /* Set new peak to default value. */
        for(i = 0; i < AXIS_NUM; i++){
            GData_4_peak.new_peak.axis[i].axis_max = -100.0;
            GData_4_peak.new_peak.axis[i].axis_min = 100.0;
        }
#if 0
        GData_4_peak.new_peak.vector_axis.axis_max = -100.0;
        GData_4_peak.new_peak.vector_axis.axis_min = 100.0;
#endif
        /* Processing for steps counter. */
        for(i = 0; i< GDB->counter; i++){
            for(j = 0; j < GDB->ave_block[i].counter; j++){
                
                gdata_PeakRefresh(&GData_4_peak, &GDB->ave_block[i].ave_data[j]);
#if 0
                ret = gdata_slid_update(&GData_flowslid, &GDB->ave_block[i].ave_data[j]);
#if 0
                DEBUG_PRINTF("Info:gdata_slid_update(%d)\n", ret);
#endif
#if 0
                ret = gdata_detact_active_axis(&GData_frature);
                DEBUG_PRINTF("Info:gdata_detact_active_axis(%d)\n", ret);
#endif
                ret = gdata_detect_processing(&GData_feature, &GData_flowslid, &GDB->ave_block[i].ave_data[j]);
                if(ret)GDB->steps++;
#if 0
                DEBUG_PRINTF("Info:Steps(%d)\n", ret);
#endif
                gdata_detact_arm_active(&GData_feature, GDB);
#endif
            }
        }
/* For debugging. */
#if 0
        for(i = 0; i < AXIS_NUM; i++){
            DEBUG_PRINTF("New_peak-%d, %f\t%f\n", i, GData_4_peak.new_peak.axis[i].axis_max, GData_4_peak.new_peak.axis[i].axis_min);
        }
        for(i = 0; i < AXIS_NUM; i++){
            DEBUG_PRINTF("Eld_peak-%d, %f\t%f\n", i, GData_4_peak.eld_peak.axis[i].axis_max, GData_4_peak.eld_peak.axis[i].axis_min);
        }
#endif
        for(i = 0; i< GDB->counter; i++){
            for(j = 0; j <GDB->ave_block[i].counter; j++){
                ret = gdata_slid_update(&GData_flowslid, &GDB->ave_block[i].ave_data[j]);
#if 0
                DEBUG_PRINTF("Info:gdata_slid_update(%d)\n", ret);
#endif
#if 0
                ret = gdata_detact_active_axis(&GData_frature);
                DEBUG_PRINTF("Info:gdata_detact_active_axis(%d)\n", ret);
#endif
                ret = gdata_detect_processing(&GData_4_peak, &GData_flowslid, &GDB->ave_block[i].ave_data[j]);
                if(ret){
                    steps_temp++;
#if 0
                    GDB->step_location = j;
                    printf("This step is found,the location is %d.\n",GDB->step_location);
                    printf("Steps temp is %d.\n",steps_temp);
#endif
                }
#if 0
                DEBUG_PRINTF("Info:Steps(%d)\n", ret);
#endif
            }
        }
        
        if(0 == cnter_status){
            if(steps_temp > 0 && steps_temp <= STEP_NUMPERSEC){
                steps_cnt += steps_temp;
                
                if(steps_cnt >= STEPS_DETECTED_TH){
                    GDB->steps += steps_cnt;
                    steps_cnt = 0;
                    cnter_status = 1;
                }
            }else{
                steps_cnt = 0;
            }
        }else{
            if(steps_temp > 0 && steps_temp <= STEP_NUMPERSEC){
                GDB->steps += steps_temp;
            }else{
            /* STEPS_OVERLAY_TH. */
                cnter_status = 0;
            }
        }
    }
    return 0;
}

void gdata_version(void){
    DEBUG_PRINTF("Info: GAlgorithm Version:%f\n", GAL_VERSION);
}
/* End of this file. */
