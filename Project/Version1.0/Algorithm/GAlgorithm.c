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

/* Gravity data feature. */
static g_peakd_t GData_feature;

/* Gravity data high frequency filter. */
static gslid_reg_t GData_flowslid;

/**
 * Initializing peak structure.
 */
static void gdata_peak_init(g_peakd_t *Data_Peak)
{
    memset(Data_Peak, 0, sizeof(g_peakd_t));
}

/**
 * Initializing whole structure.
 */
void gdata_init(axis_dblock_t *GDB)
{
    GDB->counter = 0;
    
    memset(GDB, 0, sizeof(axis_dblock_t));
    
    memset(&GData_feature, 0, sizeof(g_peakd_t));
    memset(&GData_flowslid, 0, sizeof(gslid_reg_t));
    
    gdata_peak_init(&GData_feature);
}

/**
 * Filter data and insert data to structure.
 */
void gdata_l1filter(axis_dblock_t *GDB)
{
    unsigned char i, j, m, n;
    
    for(i = 0; i< GDB->counter; i++){
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
                GDB->ave_block[i].vector_data[j] += pow(GDB->ave_block[i].ave_data[j].axis[n], 2);
            }
            
            /* Caculate vector value.*/
            GDB->ave_block[i].vector_data[j] = (float)sqrt(GDB->ave_block[i].vector_data[j]);
                
            /* Set time stamp of each axis. */
            GDB->ave_block[i].ms_stamp[j] = j*FILTER_CNT;
        }
    }
    return;
}

/**
 * Refresh new Peak to structure.
 */
static void gdata_PeakRefrash(g_peakd_t *Data_Peak, axis_info_t *Current_sample)
{
  	static unsigned char sample_size = 0;
    unsigned char i;
    
    /* Refrash in each SAMPLE_SIZE time. */
	if (sample_size++ > SAMPLE_SIZE) {
		sample_size = 1;
        
        /* Packup new maximul and minimul data to old buffer. */
        Data_Peak->OldMax = Data_Peak->NewMax;
		Data_Peak->OldMin = Data_Peak->NewMin;
        
        /* Initializing new buffer. */
#if 0
        memset(&Data_Peak->NewMax, 0, sizeof(axis_info_t));
        memset(&Data_Peak->NewMin, 0, sizeof(axis_info_t));
      	//gdata_peak_init(Data_Peak);
#else
        unsigned char m;

        for(m = 0; m < AXIS_NUM; m++){
            Data_Peak->NewMax.axis[m] = Current_sample->axis[m];
            Data_Peak->NewMin.axis[m] = Current_sample->axis[m];
        }
#endif
	}
    
    for(i = 0; i < AXIS_NUM; i++){
	    Data_Peak->NewMax.axis[i] = MAX(Data_Peak->NewMax.axis[i], Current_sample->axis[i]);
    }
    
    for(i = 0; i < AXIS_NUM; i++){
	    Data_Peak->NewMin.axis[i] = MIN(Data_Peak->NewMin.axis[i], Current_sample->axis[i]);
    }
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
            DEBUG_PRINTF("Info:gdata_slid = %f\n", tmp);
            DEBUG_PRINTF("%f\t%f\t\n",Current_sample->axis[i],slid->New_sample.axis[i]);
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
static void gdata_detact_arm_active(axis_dblock_t *GDB)
{
	//unsigned char ret = MOST_ACTIVE_NULL;
    unsigned char i, j;
    unsigned char k;
    axis_info_t active_axis;

    float axis_min[AXIS_NUM] = {0,};
    float axis_max[AXIS_NUM] = {0,};
#if 0
        unsigned char m;
        for(i = 0; i < GDB->counter; i++){
            for(j = 0; j< GDB->ave_block[i].counter; j++){
                DEBUG_PRINTF("%d\t",GDB->ave_block[i].ms_stamp[j]);
                for(m = 0; m < 3; m++){
                    DEBUG_PRINTF("%f\t",GDB->ave_block[i].ave_data[j].axis[m]);
                }
                DEBUG_PRINTF("\n");
            }
        }
#endif
    /* Check for each counter data block. */
    for(i = 0; i<GDB->counter; i++){
        
        /* Check for each axis. */
        for(k = 0; k < AXIS_NUM; k++){
            
            /* Initializing. */
            axis_max[k] = GDB->ave_block[i].ave_data[0].axis[k];
            axis_min[k] = GDB->ave_block[i].ave_data[0].axis[k];
        
            /* Latch MAX axis. */
            for(j = 0; j<GDB->ave_block[i].counter - 1; j++){
                axis_max[k] = MAX(MAX(axis_max[k], GDB->ave_block[i].ave_data[j].axis[k]), GDB->ave_block[i].ave_data[j].axis[k]);
            }
            
            /* Latch MIN axis. */
            for(j = 0; j<GDB->ave_block[i].counter - 1; j++){
                axis_min[k] = MIN(MIN(axis_min[k], GDB->ave_block[i].ave_data[j].axis[k]), GDB->ave_block[i].ave_data[j].axis[k]);
            }
        }
    }
#if 0
    DEBUG_PRINTF("KEY:");
    for(k = 0; k < AXIS_NUM; k++)
        DEBUG_PRINTF("%f\t%f\t", axis_max[k], axis_min[k]);
    DEBUG_PRINTF("\n");
#endif
    /* Compare each axis' value. */
    for(k = 0; k < AXIS_NUM; k++)
        active_axis.axis[k] = (float)ABS(axis_max[k] - axis_min[k]);
#if 0
    DEBUG_PRINTF("ABS:%f\t%f\t%f\t\n", active_axis.axis[0], active_axis.axis[1], active_axis.axis[2]);
#endif
#if 1
#if 0 
    if((active_axis.axis[0] > (float)ARM_PRECISION)||(active_axis.axis[1] > (float)ARM_PRECISION)||(active_axis.axis[2] > (float)ARM_PRECISION)){
#else
    if(((active_axis.axis[0] > (float)ARM_PRECISION) && (active_axis.axis[1] > (float)ARM_PRECISION))\
        ||((active_axis.axis[1] > (float)ARM_PRECISION) && (active_axis.axis[2] > (float)ARM_PRECISION))\
        ||((active_axis.axis[2] > (float)ARM_PRECISION) && (active_axis.axis[0] > (float)ARM_PRECISION))){
#endif
        GDB->arm_active = 1;
    }
    else{
        GDB->arm_active = 0;
    }

    if((active_axis.axis[0] > (float)R2W_PRECISION)||(active_axis.axis[1] > (float)R2W_PRECISION)||(active_axis.axis[2] > (float)R2W_PRECISION)){
        GDB->r2w_active = 1;
    }
    else{
        GDB->r2w_active = 0;
    }
#else
    if(active_axis.axis[0] > active_axis.axis[1] && active_axis.axis[0] > active_axis.axis[2] && active_axis.axis[0] >= (float)ARM_PRECISION){
        GDB->arm_active = MOST_ACTIVE_X;
    }
    else
    if(active_axis.axis[1] > active_axis.axis[0] && active_axis.axis[1] > active_axis.axis[2] && active_axis.axis[1] >= (float)ARM_PRECISION){
        GDB->arm_active = MOST_ACTIVE_Y;
    }
    else
    if(active_axis.axis[2] > active_axis.axis[0] && active_axis.axis[2] > active_axis.axis[1] && active_axis.axis[2] >= (float)ARM_PRECISION){
        GDB->arm_active = MOST_ACTIVE_Z;
    }
    else{
        GDB->arm_active = MOST_ACTIVE_NULL;
    }
#endif
    return;
}

/**
 * Detate active axis.
 */
static char gdata_detact_active_axis(g_peakd_t *Data_Peak)
{
	unsigned char ret = MOST_ACTIVE_NULL;
    unsigned char i;
    float axis_change[AXIS_NUM];
    
    for(i = 0; i < AXIS_NUM; i++){
        axis_change[i] = (float)ABS(Data_Peak->NewMax.axis[i] - Data_Peak->NewMin.axis[i]);
#if 0
        DEBUG_PRINTF("Info:axis_change(%d) = %f\n", i, axis_change[i]);
#endif
    }
    
    if(axis_change[0] > axis_change[1] && axis_change[0] > axis_change[2] && axis_change[0] >= (float)ACTIVE_PRECISION){
        ret = MOST_ACTIVE_X;
    }
    else
    if(axis_change[1] > axis_change[0] && axis_change[1] > axis_change[2] && axis_change[1] >= (float)ACTIVE_PRECISION){
        ret = MOST_ACTIVE_Y;
    }
    else
    if(axis_change[2] > axis_change[0] && axis_change[2] > axis_change[1] && axis_change[2] >= (float)ACTIVE_PRECISION){
        ret = MOST_ACTIVE_Z;
    }
	return ret;
}

/**
 * Detate which axis is most activity.
 */
static unsigned char gdata_detect_processing(g_peakd_t *Data_Peak, gslid_reg_t *slid, axis_info_t *Current_sample)
{
    unsigned char Step_counter = 0;
    unsigned char ret = gdata_detact_active_axis(Data_Peak);
    float threshold[AXIS_NUM];
#if 0
    DEBUG_PRINTF("Info:gdata_detact_active_axis = %d.\n", ret);
#endif
    switch (ret) {
        
        case MOST_ACTIVE_X:
            threshold[0] = (Data_Peak->OldMax.axis[0] + Data_Peak->OldMin.axis[0]) / 2;
#if 0
            DEBUG_PRINTF("X:%f\t%f\t%f\t\n",Data_Peak->OldMax.axis[0],Data_Peak->OldMin.axis[0],threshold[0]);
            DEBUG_PRINTF("%f\t%f\t\n",slid->Old_sample.axis[0],slid->New_sample.axis[0]);
#endif
            if (slid->Old_sample.axis[0] > threshold[0] && slid->New_sample.axis[0] < threshold[0]) {
                Step_counter ++;
            }
        break;
            
        case MOST_ACTIVE_Y:
            threshold[1] = (Data_Peak->OldMax.axis[1] + Data_Peak->OldMin.axis[1]) / 2;
#if 0
            DEBUG_PRINTF("Y:%f\t%f\t%f\t\n",Data_Peak->OldMax.axis[1],Data_Peak->OldMin.axis[1],threshold[1]);
            DEBUG_PRINTF("%f\t%f\t\n",slid->Old_sample.axis[1],slid->New_sample.axis[1]);
#endif
            if (slid->Old_sample.axis[1] > threshold[1] && slid->New_sample.axis[1] < threshold[1]) {
                Step_counter ++;
            }
        break;
            
        case MOST_ACTIVE_Z:
            threshold[2] = (Data_Peak->OldMax.axis[2] + Data_Peak->OldMin.axis[2]) / 2;
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
static unsigned char steps_cnt;
static unsigned char cnter_status;
unsigned int gdata_motion_detect(axis_dblock_t *GDB)
{
    unsigned char i, j;
    unsigned char ret;
    unsigned char steps_temp = 0;

    /* Processing for steps counter. */
    for(i = 0; i< GDB->counter; i++){
        
        for(j = 0; j < GDB->ave_block[i].counter; j++){
            gdata_PeakRefrash(&GData_feature, &GDB->ave_block[i].ave_data[j]);
#if 0
            DEBUG_PRINTF("NewMax:%f\t%f\t%f\t\n", GData_feature.NewMax.axis[0], GData_feature.NewMax.axis[1], GData_feature.NewMax.axis[2]);
            DEBUG_PRINTF("NewMin:%f\t%f\t%f\t\n", GData_feature.NewMin.axis[0], GData_feature.NewMin.axis[1], GData_feature.NewMin.axis[2]);
            DEBUG_PRINTF("OldMax:%f\t%f\t%f\t\n", GData_feature.OldMax.axis[0], GData_feature.OldMax.axis[1], GData_feature.OldMax.axis[2]);
            DEBUG_PRINTF("OldMin:%f\t%f\t%f\t\n", GData_feature.OldMin.axis[0], GData_feature.OldMin.axis[1], GData_feature.OldMin.axis[2]);

            DEBUG_PRINTF("Current:%f\t%f\t%f\t\n\n", GDB->ave_block[i].ave_data[j].axis[0], GDB->ave_block[i].ave_data[j].axis[1], GDB->ave_block[i].ave_data[j].axis[2]);
#endif
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
            ret = gdata_detect_processing(&GData_feature, &GData_flowslid, &GDB->ave_block[i].ave_data[j]);
            if(ret){
                steps_temp++;
            }
#if 0
            DEBUG_PRINTF("Info:Steps(%d)\n", ret);
#endif
        }
    }
    
    if(0 == cnter_status){
        if(steps_temp > 0){
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
    
        if(steps_temp > 0){
            GDB->steps += steps_temp;
        }else{
        /* STEPS_OVERLAY_TH. */
            cnter_status = 0;
        }
    }

    /* Detecting ARM motion. */
    gdata_detact_arm_active(GDB);
    
    return 0;
}

void gdata_version(void){
    DEBUG_PRINTF("Info: GAlgorithm Version:%f\n", GAL_VERSION);
}

