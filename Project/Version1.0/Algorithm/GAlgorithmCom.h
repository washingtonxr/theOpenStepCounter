/**
 * @File    GAlgorithmCom.h
 * @Date    Sunday, Jan. 5, 2019 at 14:21:09 PM BJT
 * @Author  Washington Ruan
 * @Email   washingtonxr@live.com
 *
 * This file contains the implementation of the gravity sensor device for motion detecting
 * module's head file.
 *
 * @bug No known bugs.
 **/
#ifndef GALGORITHMCOM_H
#define GALGORITHMCOM_H

//#define NOEXPORTLIB
#ifndef NOEXPORTLIB
#include "debug.h"
#endif

#if 0
#include "Peripherals.h"
#endif

/* Parameter for algorithm. */
#define GAL_VERSION                     0.15    /* Main algorithm version. */
#define ACTIVE_PRECISION      			1.060   /* Activity precision minimal window.*/
#define ARM_PRECISION                   5.000   /* ARM activity precision. */
#define R2W_PRECISION                   2.000   /* Raise activity precision. */
#define DYNAMIC_PRECISION     			1.000   /* Dynamic Window. */
#define SAMPLE_SIZE                     42      /* Sample size. */
#define SAMPLE_DEPTH                    1       /* Sample depth in second. */
#define STEPS_DETECTED_TH               05      /* Steps threshold value. */
#define STEPS_OVERLAY_TH                03      /* Overlay threshold value.*/
#define AXIS_NUM                        3       /* Number of Gravity Sensor Number. */
#define FILTER_CNT                      4       /* Average counter. */

/* Axis' informantion. */
typedef struct {
    float axis[AXIS_NUM];                       /* x,y,z axis' RAW data. */
}axis_info_t;

/* Original data unit. */
typedef struct {
    unsigned char counter;                      /* Max 256 bit in depth. */
    axis_info_t axia_data[SAMPLE_SIZE];         /* Sample in Axia. */
}axis_du_t;

/* Mean filtered data set structure. */
typedef struct {
    unsigned char counter;                          /* Max 256 bit in depth. */
    axis_info_t ave_data[SAMPLE_SIZE/FILTER_CNT];   /* Sample in Axia. */
    float vector_data[SAMPLE_SIZE/FILTER_CNT];      /* Vector direction data. */
    unsigned char ms_stamp[SAMPLE_SIZE/FILTER_CNT]; /* Sample in ms stamp. */
}axis_avefilter_t;

/* Core data set Structure. */
typedef struct {
    unsigned char counter;                      /* Data set size. */
    axis_du_t sample_block[SAMPLE_DEPTH];       /* Original input parameter. */
    axis_avefilter_t ave_block[SAMPLE_DEPTH];   /* Mean filtered value. */
    unsigned int steps;                         /* Step counter. (Range: 0x00000000 - 0xFFFFFFFF) */
    unsigned char arm_active;                   /* Raise to pay signal. (Yes: 1, No: 0) */
    unsigned char r2w_active;                   /* Raise to wake signal. (Yes: 1, No: 0) */
}axis_dblock_t;

#if 0
#include "GAlgorithm.h"
#endif
/**
 * Filter data and insert data to structure.
 */
void gdata_l1filter(axis_dblock_t *GDB);

/**
 * Initializing whole structure.
 */
void gdata_init(axis_dblock_t *GDB);

/**
 * Motion detecting.
 */
unsigned int gdata_motion_detect(axis_dblock_t *GDB);

/**
 * Show Galivity Motion Detector Alogorithm's Version.
 */
void gdata_version(void);

#endif
