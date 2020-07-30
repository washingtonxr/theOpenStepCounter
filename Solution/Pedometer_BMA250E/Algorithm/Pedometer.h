#ifndef __PEDOMETER_H__
#define __PEDOMETER_H__
/** \mainpage Interface for pedometer algorithm
* 
*       \section intro_sec Introduction
*		Interface for pedometer algorithm
*
*       \section disclaimer_sec Disclaimer
*
* Common:
* Bosch Sensortec products are developed for the consumer goods industry. They may only be used
* within the parameters of the respective valid product data sheet.  Bosch Sensortec products are
* provided with the express understanding that there is no warranty of fitness for a particular purpose.
* They are not fit for use in life-sustaining, safety or security sensitive systems or any system or device
* that may lead to bodily harm or property damage if the system or device malfunctions. In addition,
* Bosch Sensortec products are not fit for use in products which interact with motor vehicle systems.
* The resale and/or use of products are at the purchaser’s own risk and his own responsibility. The
* examination of fitness for the intended use is the sole responsibility of the Purchaser.
*
* The purchaser shall indemnify Bosch Sensortec from all third party claims, including any claims for
* incidental, or consequential damages, arising from any product use not covered by the parameters of
* the respective valid product data sheet or not approved by Bosch Sensortec and reimburse Bosch
* Sensortec for all costs in connection with such claims.
*
* The purchaser must monitor the market for the purchased products, particularly with regard to
* product safety and inform Bosch Sensortec without delay of all security relevant incidents.
*
* Engineering Samples are marked with an asterisk (*) or (e). Samples may vary from the valid
* technical specifications of the product series. They are therefore not intended or fit for resale to third
* parties or for use in end products. Their sole purpose is internal client testing. The testing of an
* engineering sample may in no way replace the testing of a product series. Bosch Sensortec
* assumes no liability for the use of engineering samples. By accepting the engineering samples, the
* Purchaser agrees to indemnify Bosch Sensortec from all claims arising from the use of engineering
* samples.
*
* Special:
* This software module (hereinafter called "Software") and any information on application-sheets
* (hereinafter called "Information") is provided free of charge for the sole purpose to support your
* application work. The Software and Information is subject to the following terms and conditions:
*
* The Software is specifically designed for the exclusive use for Bosch Sensortec products by
* personnel who have special experience and training. Do not use this Software if you do not have the
* proper experience or training.
*
* This Software package is provided `` as is `` and without any expressed or implied warranties,
* including without limitation, the implied warranties of merchantability and fitness for a particular
* purpose.
*
* Bosch Sensortec and their representatives and agents deny any liability for the functional impairment
* of this Software in terms of fitness, performance and safety. Bosch Sensortec and their
* representatives and agents shall not be liable for any direct or indirect damages or injury, except as
* otherwise stipulated in mandatory applicable law.
*
* The Information provided is believed to be accurate and reliable. Bosch Sensortec assumes no
* responsibility for the consequences of use of such Information nor for any infringement of patents or
* other rights of third parties which may result from its use. No license is granted by implication or
* otherwise under any patent or patent rights of Bosch. Specifications mentioned in the Information are
* subject to change without notice.
*
* It is not allowed to deliver the source code of the Software to any third party without permission of
* Bosch Sensortec.
*               
*/


/* Module Includes */
#include "define.h"
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ( 101785
   #Define Constants */
#define C_AXIS_IS_COMPOSITE_U8X     1  /**< Axis is Composite */
#define C_AXIS_IS_SINGLE_U8X        2  /**< Axis is Single */

/* Varibles for Single Axis Acceleration Features functions */

#define C_SINGLE_AXIS_ACCEL_VALUE_BUFSIZE_U8R (U8)3 /**< Acceleration buffer Size */
#define C_SINGLE_AXIS_TREND_BUFSIZE_U8R       (U8)4 /**< Trend type buffer size (Keep it strictly 4) */
#define C_STRIDE_INTERVAL_BUFSIZE_U8R         (U8)5 /**< Stride interval buffer size */
#define C_STRIDE_INTERVAL_COUNT_MAXLIMIT_U8R  (U8)60 /**< Maximum limit for stride interval allowed */
#define C_STRIDE_SHORTPEAK_TO_PP_RATIO_CUTOFF_F32R (F32)0.15 /**< Strides short peaks values to Peak-To-Peak values ratio */

#define C_AXIS_UNIFORMITY_PASSED_NO_CUTOFF_U8R      (U8)4 /**< Minimum numbers of stride uniformity tests needs to be passed out of 5 tests */
#define C_AXIS_STRIDE_INTERVAL_PASSED_NO_CUTOFF_U8R (U8)3 /**< Minimum numbers of stride interval tests needs to be passed out of 5 tests */
#define C_STRIDE_INTERVAL_DIFF_MAX_LIMIT_U8R        (U8)5 /**< Maximum stride interval difference allowed b/w two successive strides */
#define C_DOMINATING_AXIS_BUFSIZE_U8R               (U8)5 /**< Ist and IInd dominant axis buffer size */
#define C_NO_DOMINANT_AXIS_OCC_MIN_LIMIT            (U8)4 /**< Minimum numbers of dominant occurences (out of 5) required for an axis to be dominant */
#define STABLE       (U8)1      /**< Indicated Stablility of an axis pattern */
#define UNSTABLE     (U8)0      /**< Indicated Unstablility of an axis pattern */
/* EasyCASE ) */
/* EasyCASE ( 101786
   ENUM and struct Definitions */
/* EasyCASE ( 101791
   Structure to hold pedometer output parameters */
/* EasyCASE < */
typedef struct 
{
  U8  V_PedoActivity_U8R;            /**< Pedometer Activity Type */
  U8  V_CadenceX10_U8R;              /**< Cadence (steps/sec) multiple of 10 */
  U16 V_StepLengthInCm_U16R;         /**< Step Length (in cm) */  
  U16 V_SpeedCmPerSec_U16R;          /**< Speed (in cm/sec) */
  
  U8  V_DeltaSteps_U8R;              /**< Delta Steps */
  U16 V_StepCountLastActivity_U16R;  /**< Steps for Last User Activity */
  U16 V_StepCountCurrActivity_U16R;  /**< Steps for Current User Activity */  
  U16 V_TotalStepCount_U16X;         /**< Total Step Count */

  F32 V_DeltaDistInMet_F32R;         /**< Delta Distance (In Meters) */
  U16 V_DistLastActivityInMet_U16R;  /**< Distance for Last User Activity (In Meters) */
  U16 V_DistCurrActivityInMet_U16R;  /**< Distance for Last User Activity (In Meters) */
  F32 V_TotalDistInMet_F32R;         /**< Total Distance (In Meters) */

}ts_PedoParam;
/* EasyCASE > */
/* EasyCASE ) */
/* EasyCASE ( 101792
   Structure to hold the data buffers and variables of the Single Axis */
/* EasyCASE < */
typedef struct
{
   /*FIFOs required for individual axis to save useful parameters for analysis */
   
       S16 A_AccelBuf_S16X[C_SINGLE_AXIS_ACCEL_VALUE_BUFSIZE_U8R];             /**< Buffer to hold the acceleration values */
       S8  A_AccelTrendBuf_S8X[C_SINGLE_AXIS_TREND_BUFSIZE_U8R];               /**< Buffer to hold the trend type */
       S16 A_AccelTrendValueBuf_S16X[C_SINGLE_AXIS_TREND_BUFSIZE_U8R];         /**< Buffer to hold the accel values at trends */
       U8  A_AccelSuccecciveTrendTimeBuf_U8X[C_SINGLE_AXIS_TREND_BUFSIZE_U8R]; /**< Buffer to hold the time count between successive trends */

       U16 A_StridePeakToPeakValBuf_U16X[C_STRIDE_INTERVAL_BUFSIZE_U8R]; /**< Buffer to hold the stride peak-to-peak (Max val-Min val)*/
       U8  A_StrideIntervalBuf_U8X[C_STRIDE_INTERVAL_BUFSIZE_U8R];       /**< Buffer to hold the Stride Interval */
       S8  A_StrideIntervalDiffBuf_U8X[C_STRIDE_INTERVAL_BUFSIZE_U8R];   /**< Buffer to hold the Stride Interval difference */
       U8  A_UniformityTestBuf_U8X[C_STRIDE_INTERVAL_BUFSIZE_U8R];       /**< Buffer to hold the result of uniformity test */

   /*Counts required for individual axis to update the FIFOs*/ 
   
       U8  V_AccelBufDataCount_U8X;             /**< Acceleration value buffer data packet count */
       U8  V_AccelTrendBufDataCount_U8X;        /**< Acceleration trend buffer data packet count*/
       U8  V_StrideIntervalBufDataCount_U8X;    /**< Stride Interval buffer data packet count */
       U8  V_SuccessiveTrendTimeCount_U8X;      /**< Time count between successive trends */

   /* Flags and other variables */   
       
       S8  V_InstTrendType_S8X;                     /**< Instantaneous trend type */      
       U8  V_SuccessiveTrendTimeValidityFlag_U8X;   /**< Time count between successive trends validity flag */
       U8  V_ValidStridePatternOccuredFlag_U8X;     /**< Flag for valid stride patterns */
       U8  V_AxisIsStableFlag_U8X;                  /**< Axis stability flag */
       U16 V_CadenceX100_U16X;                      /**< single axis cadence(steps/sec) (in multiple of 100)*/
                   
}ts_SingleAxis;
/* EasyCASE > */
/* EasyCASE ) */
/* EasyCASE ( 101793
   Structure to hold the Extra Variables */
/* EasyCASE < */
typedef struct 
{
U16  InstStep_PP_Val;      /* Instantaneous step peak-to-peak value 
                                                     (Acceleration value difference at Negetive trend and Positive trend) */
U8   InstStep_PP_Time;     /* Instantaneous step peak-to-peak time
                                                     (Time count difference at Negetive trend and Positevee trend)*/
U16  InstStepRatio;        /* Instantaneous Activity detectin ratio */
U8   InstActivity;         /* Instantaneous Activity */

U16  InstCadence;          /* Instantaneous cadence (steps per sec) */
F32  LpfInstCadence;       /* LPF cadence (steps per sec)*/
F32  StableStepCadence;    /* Stable step cadence(steps/sec) */
F32  InstStepLength;       /* Instantaneous step length (in cm)*/
F32  InstSpeed;            /* Instantaneous Speed (cm/sec) */
F32  LpfSpeed;             /*  LPF Speed (cm/sec)*/

S16  RawCompAccel;             /* Raw composite acceleration */
S16  v_FiltAccel4Axis_s16r[4]; /* Filtered acceleration for all four axis */

U8   v_TrendDominantAxis_u8r;         /* Trend dominant axis */
U8   v_SingleOrCompositeAxisFlag_u8r; /* Single or Composite axis selection flag */

/*Added for axis switching */

U16 X_StrideRatio; /* Stride Ratio(Stride_PP_Val/(StrideInterval/2)) for X-Axis */
U16 Y_StrideRatio; /* Stride Ratio(Stride_PP_Val/(StrideInterval/2)) for Y-Axis */
U16 Z_StrideRatio; /* Stride Ratio(Stride_PP_Val/(StrideInterval/2)) for Z-Axis */
U16 C_StrideRatio; /* Stride Ratio(Stride_PP_Val/(StrideInterval/2)) for Composite-Axis */

U8 IstDomAxis;   /* First dominant axis */
U8 IIndDomAxis;  /* Second dominant axis */
U8 IIIrdDomAxis; /* Third dominant axis */

U8 X_Stability;  /* X-axis acceleration pattern stability flag */
U8 Y_Stability;  /* Y-axis acceleration pattern stability flag */
U8 Z_Stability;  /* Z-axis acceleration pattern stability flag */
U8 C_Stability;  /* Composite-axis acceleration pattern stability flag */
U8 CurrentStableAxis; /* Selected stable axis to find pedometer parameters */

U16 X_CadenceX100;  /* Stride based Cadence(Steps/sec) (multiple of 100) for X-axis */
U16 Y_CadenceX100;  /* Stride based Cadence(Steps/sec) (multiple of 100) for Y-axis */
U16 Z_CadenceX100;  /* Stride based Cadence(Steps/sec) (multiple of 100) for Z-axis */
U16 C_CadenceX100;  /* Stride based Cadence(Steps/sec) (multiple of 100) for C-axis */
U16 StableStrideCadenceX100; /* Stride based Cadence(Steps/sec) (multiple of 100) for stable axis */
U8 PedoActivity;             /* Pedometer Activity */

}ts_ExtraVar;
/* EasyCASE > */
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ( 70
   Public API Declarations */
/* EasyCASE ( 101478
   PEDO_GetStepCount */
/*******************************************************************************
 * Description: *//**\brief This API should be called to get Total no.of steps
 * counted.
 *
 *  
 *
 *
 *
 *  \param
 *   NONE
 *
 *
 *  \return 
 *   U16 V_StepCount_U16R --> Total no. of steps counted. 
 *            
 *
 ******************************************************************************/
/* Scheduling: 
 * 
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
unsigned long PEDO_GetStepCount(void);
/* EasyCASE ) */
/* EasyCASE ( 101486
   PEDO_StopDetection */
/*******************************************************************************
 * Description: *//**\brief This function will put the Pedometer Algorithm into 
 * Sleep mode.
 *  
 *
 *
 *
 *  \param
 *   NONE
 *
 *
 *  \return 
 *   NONE 
 *            
 *
 ******************************************************************************/
/* Scheduling: 
 * 
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
void PEDO_StopDetection(void);
/* EasyCASE ) */
/* EasyCASE ( 101488
   PEDO_StartDetection */
/*******************************************************************************
 * Description: *//**\brief This API should be used to start the Pedometer Algorithm
 * if pedometer is in sleep mode. If Pedometer Algorithm is running already, Call this
 * API to get Pedometer algorthm restarted. Step count will be reset. By default the normal
 * human activity detection is enabled.
 *
 *  
 *
 *
 *
 *  \param
 *  NONE
 *
 *
 *  \return 
 *  NONE
 *            
 *
 ******************************************************************************/
/* Scheduling: 
 * 
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
void PEDO_StartDetection(void);
/* EasyCASE ) */
/* EasyCASE ( 101490
   PEDO_ResetStepCount */
/*******************************************************************************
 * Description: *//**\brief This API should be used to reset the step count to zero.
 * Pedometer Algorithm mode is not affected by this function. 
 *  
 *
 *
 *
 *  \param
 *   NONE
 *   
 *
 *
 *  \return 
 *   NONE
 *            
 *
 ******************************************************************************/
/* Scheduling: 
 * 
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
void PEDO_ResetStepCount(void);
/* EasyCASE ) */
/* EasyCASE ( 101492
   PEDO_GetActivity */
/*******************************************************************************
 * Description: *//**\brief This API should be called to get the User's Activity
 * Type (Nature of the Step) i.e. STAITIONARY(0x00), WALK(0x01)or RUN(0x03).
 *
 *  
 *
 *
 *
 *  \param
 *  NONE
 *
 *
 *  \return 
 *  U8 --> User Activity Type 
 *            
 *
 ******************************************************************************/
/* Scheduling: 
 * 
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
unsigned char PEDO_GetActivity(void);
/* EasyCASE ) */
/* EasyCASE ( 101496
   PEDO_InitAlgo */
/*******************************************************************************
 * Description: *//**\brief This API should be used to initialze the variables 
 * that are used in the Pedometer Algorithm. It should be called device Power On
 * to initialise the Pedometer Algorithm.
 *
 *
 *
 *
 *  \param
 *   U8 v_GRange_u8r :  Parameter used to set the division factor for threshold
 *                      depending on the Accelerometer range.
 *                     (Use 0-->2G;  1-->4G;  2-->8G acceleration range of the sensor).  
 *
 *  \return 
 *   NONE
 *            
 *
 ******************************************************************************/
/* Scheduling: 
 * Call the function after giving a delay (10msec) after power on of the device.
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
void PEDO_InitAlgo(unsigned char v_GRange_u8r);
/* EasyCASE ) */
/* EasyCASE ( 101497
   PEDO_Enable_Robustness */
/*******************************************************************************
 * Description: *//**\brief This API should be called to start the Pedometer
 * Algorithm with robustness feature enabled. By default robustness feature
 * will be enabled in the pedometer algorithm.If any time the robustness 
 * feature is disabled then this API needs to be called again to enable the
 * robustness feature. 
 *  
 *
 *
 *
 *  \param
 *   NONE
 *
 *
 *  \return 
 *   NONE 
 *            
 *
 ******************************************************************************/
/* Scheduling: 
 * 
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
void PEDO_EnableRobustness(void);
/* EasyCASE ) */
/* EasyCASE ( 101498
   PEDO_SetInterStepCount */
/*******************************************************************************
 * Description: *//**\brief This API should be called to set/change the inter step
 * count limit depending on the robust/non-robust pedometer applications.
 *
 *  
 *
 *
 *
 *  \param
 *   U8 v_InterStepCountLimit_u8r --> Inter-Step count limit for pedometer algorithm.
 *  
 *
 *
 *  \return 
 *   NONE
 *            
 *
 ******************************************************************************/
/* Scheduling: 
 * 
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
void PEDO_SetInterStepCount(U8 v_InterStepCountLimit_u8r );
/* EasyCASE ) */
/* EasyCASE ( 101499
   PEDO_Disable_Robustness */
/*******************************************************************************
 * Description: *//**\brief This API should be called to start the Pedometer
 * Algorithm with robustness feature disabled. By default robustness feature 
 * will be enabled in the algorithm. If any time the robustness feature needs
 * to be disabled then this API has to be called. 
 *
 *
 *
 * \param
 * NONE  
 *   
 *
 *
 * \return 
 * NONE    
 *            
 *
 ******************************************************************************/
/* Scheduling: 
 * 
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
void PEDO_DisableRobustness(void);
/* EasyCASE ) */
/* EasyCASE ( 101500
   PEDO_ResetAlgo */
/*******************************************************************************
 * Description: *//**\brief This API should be called to reset the Pedometer Algorithm.
 * It will clear/reset the variables used into Pedometer Algorithm.
 *
 *  
 *
 *
 *
 *  \param
 *  NONE 
 *   
 *
 *
 *  \return 
 *  NONE
 *            
 *
 ******************************************************************************/
/* Scheduling: 
 * *NONE
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
void PEDO_ResetAlgo(void);
/* EasyCASE ) */
/* EasyCASE ( 101502
   PEDO_GetBandwidthSwitchInfo */
/*******************************************************************************
 * Description: *//**\brief This API should be called to get the information of
 * Axis_Select Flag to decide whether to use 10Hz or 300Hz Bandwidth of accelerometer
 * for pedometer algorithm.
 * a)-->IF(Activity==WALK) ==> (V_AxisSelectFlag_U8R = COMPOSITE AXIS) ==> BW=300Hz.
 * b)-->IF(Activity==RUN) ==> (V_AxisSelectFlag_U8R = SINGLE AXIS) ==> BW=10Hz.
 *
 *  
 *
 *
 *
 *  \param
 *   NONE
 *
 *
 *  \return 
 *   U8 V_AxisSelectFlag_U8R -->  Selected Axis for Step Counting (Single or Composite Axis ?)
 *      based on the type of the Activity.
 *            
 *
 ******************************************************************************/
/* Scheduling: 
 * 
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
U8 PEDO_GetBandwidthSwitchInfo(void);
/* EasyCASE ) */
/* EasyCASE ( 101504
   PEDO_ProcessAccelarationData */
/*******************************************************************************
 * Description: *//**\brief This API is the Heart of the Pedometer Algorothm.
 * It should be called to process the acceleration data along X.Y and Z axis
 *  to calculate the Composite Acceleration value, Total No. of Steps, Step nature
 * (STATIONARY/WALK/RUN) etc.
 *
 *  
 *
 *
 *
 *  \param
 *   short v_RawAccelX_s16r --> Acceleration Value along X-axis in LSBs
 *   short v_RawAccelY_s16r --> Acceleration Value along Y-axis in LSBs
 *   short v_RawAccelZ_s16r --> Acceleration Value along Z-axis in LSBs
 *
 *
 *  \return 
 *   F32 --> Value of the Composite Aceleration (|x|+|y|+|z|). 
 *            
 *
 ******************************************************************************/
/* Scheduling: 
 * Call this API after initialising the pedometer algorithm by calling API "PEDO_InitAlgo" atleast once.
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
short PEDO_ProcessAccelarationData(short v_RawAccelX_s16r, short v_RawAccelY_s16r, short v_RawAccelZ_s16r);
/* EasyCASE ) */
/* EasyCASE ( 101505
   PEDO_UpdatePedometerParameters */
/*******************************************************************************
 * Description: *//**\brief This API should be called to calculate and update
 * the pedometer data such as StepCounts( Delta, Total, Last Activity, Current Activity ),
 * Distance ( Delta, Total, Last Activity, Current Activity ), cadence, Speed, Activity Type.
 *
 *  
 *
 *
 *
 *  \param
 *  ts_PedoParam* s_PedometerData_xxr --> Structure to hold basic and derived parameters
 *  of the Pedometer like StepCounts and distance(Delta, Total, Last Activity, Current Activity),
 *  Cadence, Speed, Activity Type etc.
 *
 *
 *
 *  \return 
 *   NONE
 *            
 *
 ******************************************************************************/
/* Scheduling: 
 * Call this API after processing acceleration data by calling API "PEDO_ProcessAccelarationData". 
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
void  PEDO_UpdatePedometerParameters(ts_PedoParam* s_PedometerData_xxr);
/* EasyCASE ) */
/* EasyCASE ( 101525
   PEDO_MedianFilter */
/*******************************************************************************
 * Description: *//**\brief This function finds the median of the input array of
 * the given size. 
 *  
 *
 *
 *
 *  \param
 *     S32 *p_InputArray_s32r   : Input Array for which median has to be found.
 *     U8   v_SizeInputArray_u8 : Size of the input array.
 *  \return 
 *     U32 :It returns median value of the input array.  
 *
 ******************************************************************************/
/* Scheduling:
 * none
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/

 U32 PEDO_MedianFilter( U32 *p_InputArray_u32r, U8 v_SizeInputArray_u8r);
/* EasyCASE ) */
/* EasyCASE ) */
#endif
/* EasyCASE ) */
