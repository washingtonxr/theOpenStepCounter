/**
 * @File    GAlgorithm.h
 * @Date    Sunday, Jan. 5, 2019 at 14:34:15 PM BJT
 * @Author  Washington Ruan
 * @Email   washingtonxr@live.com
 *
 * This file contains the implementation of the gravity sensor device for motion detecting
 * module's head file.
 *
 * @bug No known bugs.
 **/
#ifndef GALGORITHM_H
#define GALGORITHM_H

#include "GAlgorithmCom.h"

#define MOST_ACTIVE_NULL      			0       /* Sign of stable. */
#define MOST_ACTIVE_X					1       /* X axis is activity. */	
#define MOST_ACTIVE_Y					2       /* Y axis is activity. */	
#define MOST_ACTIVE_Z					3       /* Z axis is activity. */	

#define ABS(a)                          (0 - (a)) > 0 ? (-(a)) : (a)
#if 1
#define MAX(a,b)                        ((a) > (b) ? (a) : (b)) 
#define MIN(a,b)                        ((a) < (b) ? (a) : (b))
#else
/* Get MAX data. */
#define MAX(a, b)   ((a>0)&(b>0))?(a>b?a:b):(((a>0)&(b<0))?a:(((a<0)&(b>0))?b:((-(a)>-(b))?a:b))) 
#endif

#define TIME_STAMP                      1/SAMPLE_SIZE   /* Minimul time stamp. */

typedef struct {
	axis_info_t NewMax;
	axis_info_t NewMin;
	axis_info_t OldMax;
	axis_info_t OldMin;
}g_peakd_t;

typedef struct{
	axis_info_t New_sample;
	axis_info_t Old_sample;
}gslid_reg_t;

/*
const unsigned char Filter_table[] = {"
    RAW Data L1(42-252):
    Info:g-raw data:
    x   y   z   
    -1.875000   0.585938    -10.007324  
    -1.875000   0.583496    -9.382324   
    -1.875000   0.585938    -9.382324   
    -1.875000   0.583496    -10.007324  
    -1.875000   0.585938    -9.382324   
    -1.875000   0.583496    -9.382324   
    -1.875000   0.583496    -9.382324   
    -1.875000   0.585938    -9.382324   
    -1.875000   0.583496    -9.382324   
    -1.875000   0.585938    -10.007324  
    -1.875000   0.583496    -9.382324   
    -1.875000   0.585938    -10.007324  
    -1.875000   0.585938    -9.382324   
    -1.875000   0.585938    -9.382324   
    -1.875000   0.585938    -10.007324  
    -1.875000   0.585938    -9.382324   
    -1.875000   0.585938    -10.007324  
    -1.875000   0.583496    -9.382324   
    -1.875000   0.585938    -9.382324   
    -1.875000   0.585938    -9.382324   
    -1.875000   0.585938    -9.382324   
    -1.875000   0.583496    -9.382324   
    -1.875000   0.583496    -10.007324  
    -1.875000   0.583496    -9.382324   
    -1.875000   0.585938    -9.382324   
    -1.875000   0.583496    -10.007324  
    -1.875000   0.583496    -9.382324   
    -1.875000   0.583496    -9.382324   
    -1.875000   0.583496    -10.007324  
    -1.875000   0.585938    -10.007324  
    -1.875000   0.583496    -9.382324   
    -1.875000   0.585938    -10.007324  
    -1.875000   0.583496    -9.382324   
    -1.875000   0.583496    -10.007324  
    -1.875000   0.583496    -10.007324  
    -1.875000   0.585938    -10.007324  
    -1.875000   0.585938    -9.382324   
    -1.875000   0.583496    -9.382324   
    -1.875000   0.585938    -9.382324   
    -1.875000   0.585938    -10.007324  
    -1.875000   0.585938    -10.007324  
    -1.875000   0.585938    -10.007324  

    Info:axis_change(0) = 2
    Info:axis_change(1) = 2
    Info:axis_change(2) = 12
    Info:gdata_detact_active_axis(3)
    Info:gdata_slid = 2.494547
    Info:gdata_slid = 1.845643
    Info:gdata_slid = 0.781860
    Info:gdata_slid_update(0)
    Info:axis_change(0) = 2
    Info:axis_change(1) = 2
    Info:axis_change(2) = 12
    Info:gdata_detact_active_axis(3)
    Info:gdata_slid = 2.495019
    Info:gdata_slid = 1.719341
    Info:gdata_slid = 0.781862
    Info:gdata_slid_update(0)
    Info:axis_change(0) = 2
    Info:axis_change(1) = 2
    Info:axis_change(2) = 12
    Info:gdata_detact_active_axis(3)
    Info:gdata_slid = 2.495739
    Info:gdata_slid = 1.810872
    Info:gdata_slid = 0.781860
    Info:gdata_slid_update(0)
    Info:axis_change(0) = 2
    Info:axis_change(1) = 2
    Info:axis_change(2) = 12
    Info:gdata_detact_active_axis(3)
    "
};
*/


#endif

