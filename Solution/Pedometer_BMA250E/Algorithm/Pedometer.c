 /** \mainpage Pedometer API
* 
*       \section intro_sec Introduction:  
*		Pedometer algorithm to find the step count
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
#include "Pedometer.h"
#include "PedoSupport.h"

/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ( 63
   Local Declarations and Definitions */
/* EasyCASE ( 322
   Protected Data Interfaces (PDIs) */
/* EasyCASE ) */
/* EasyCASE ( 101093
   #Define Constants */
/* PATTERN RECOGNITION ( ACCELERATION LOWER-PEAK, UPPER-PEAK THRESOLD LIMITS ) */

#define WALKING_LOWER_LIMIT                     100>>V_DivideFactor_U8R     /**< Stable Axis Lower Threshold for Walk step */
#define WALKING_UPPER_LIMIT                     400>>V_DivideFactor_U8R     /**< Stable Axis Upper Threshold for Walk step */
#define LAZY_WALKING_LOWER_LIMIT_Robust         50>>V_DivideFactor_U8R      /**< Stable Axis Lower Threshold for Lazy Walk step(Robust) */
#define LAZY_WALKING_LOWER_LIMIT_NoRobust       32>>V_DivideFactor_U8R      /**< Stable Axis Lower Threshold for Lazy Walk step(Non-Robust) */
#define LAZY_WALKING_UPPER_LIMIT                160>>V_DivideFactor_U8R     /**< Stable Axis Upper Threshold for Lazy Walk step */
#define JOG_LOWER_LIMIT                         100>>V_DivideFactor_U8R     /**< Stable Axis Lower Threshold for Jog step */
#define JOG_UPPER_LIMIT                         17000>>V_DivideFactor_U8R   /**< Stable Axis Upper Threshold for Jog step */
#define C_SENSOR_NOISE_LEVEL_U8X                20>>V_DivideFactor_U8R /**< Variation of Acceleration Value When kept idle? */

/* PATTERN RECOGNITION ( TREND CYCLE LIMITS FOR DIFF ACTIVITIES ) */

#define WALKING_CYCLE_LOWER_LIMIT               5       /**< Trend cycle count lower limit for walking */
#define WALKING_CYCLE_UPPER_LIMIT               21      /**< Trend cycle count upper limit for walking */
#define LAZY_WALKING_CYCLE_LOWER_LIMIT          10      /**< Trend cycle count lower limit for lazy walking */
#define LAZY_WALKING_CYCLE_UPPER_LIMIT          32      /**< Trend cycle count upper limit for lazy walking */
#define JOG_CYCLE_LOWER_LIMIT                   4       /**< Trend cycle count lower limit for jogging */
#define JOG_CYCLE_UPPER_LIMIT                   15      /**< Trend cycle count upper limit for jogging */
#define MAX_COUNT_DIFFERENCE                    3       /**< Difference between 2 adjacent steps*/

/* PATTERN RECOGNITION ( STEP-TO-STEP INTERVAL LIMITS FOR DIFF ACTIVITIES ) */

#define C_LazyWalkStepToStepLowerLimit_U8X      18      /**< Step-To-Step count lower limit for lazy walk */
#define C_LazyWalkStepToStepUpperLimit_U8X      30      /**< Step-To-Step count upper limit for lazy walk */
#define C_WalkStepToStepLowerLimit_U8X          7       /**< Step-To-Step count lower limit for walk */
#define C_WalkStepToStepUpperLimit_U8X          17      /**< Step-To-Step count upper limit for walk */
#define C_JogStepToStepLowerLimit_U8X           7       /**< Step-To-Step count lower limit for Jog */
#define C_JogStepToStepUpperLimit_U8X           17      /**< Step-To-Step count upper limit for Jog */

/* MODE DEFINITIONS */

#define MODE_DETECTION                          0       /**< First 4 steps yet to be made */
#define MODE_COUNTING                           1       /**< Walking mode (made 4 steps) */
#define MODE_SLEEPING                           2       /**< Sleeping mode */

#define M_Walk_U8X                              (char)0x01    /**< Mask Bit for Walk*/
#define M_SlowWalk_U8X                          (char)0x02    /**< Mask Bit for Slow Walk*/
#define M_Jog_U8X                               (char)0x04    /**< Mask Bit for Jog*/

#define M_ModeDetection_U8X                     (char)0x01    /**< Mask Bit for Detection Mode*/
#define M_ModeCounting_U8X                      (char)0x02    /**< Mask Bit for Counting Mode*/
#define M_AlgoReset_U8X                         (char)0x04    /**< Mask Bit for Algo reset*/

#define M_Qualified_U8X                         (char)0x08    /**< Mask Bit for Qualified Step*/
#define M_UnQualified_U8X                       (char)0x10    /**< Mask Bit for UnQualified Step*/
#define M_PositiveTrend_U8X                     (char)0x20    /**< Mask Bit for Positive Trend Change*/
#define M_NegativeTrend_U8X                     (char)0x40    /**< Mask Bit for Negative Trend Change*/

#define M_DisableRobustness_U8X                 (char)0x80    /**< Mask Bit for Disable robustness feature*/

/* MACROS DECIDING THE STEP NATURE */

/**\ brief Macro Deciding Lazy walk step */

#define IS_LAZY_WALK_STEP(resultDiff_i16) \
        ((resultDiff_i16 >= V_LazyWalkLowerLimit_U8R &&  \
          resultDiff_i16 <= LAZY_WALKING_UPPER_LIMIT) && \
        (V_SampleCountForCycle_U8R >= LAZY_WALKING_CYCLE_LOWER_LIMIT && \
         V_SampleCountForCycle_U8R <= LAZY_WALKING_CYCLE_UPPER_LIMIT))

/**\ brief Macro deciding Walk step */

#define IS_WALK_STEP(resultDiff_i16) \
        ((resultDiff_i16 >= WALKING_LOWER_LIMIT &&  \
          resultDiff_i16 <= WALKING_UPPER_LIMIT) && \
        (V_SampleCountForCycle_U8R >= WALKING_CYCLE_LOWER_LIMIT && \
         V_SampleCountForCycle_U8R <= WALKING_CYCLE_UPPER_LIMIT))

/**\ brief Macro deciding Jog step */

#define IS_JOG_STEP(resultDiff_i16) \
        ((resultDiff_i16 >= JOG_LOWER_LIMIT &&  \
          resultDiff_i16 <= JOG_UPPER_LIMIT) && \
        (V_SampleCountForCycle_U8R >= JOG_CYCLE_LOWER_LIMIT && \
         V_SampleCountForCycle_U8R <= JOG_CYCLE_UPPER_LIMIT))

/* PEDOMETER OTHER CONSTANTS */

#define C_FilterTaps_U8X                   (char)24     /**< Filter taps */
#define C_Q15ConversionFactor_U8X          (char)15     /**< Division Factor for Q15 */
#define C_Clear_U8X                        (char)0      /**< For clearing to 0 */

#define C_CountZero_U8X                    (char)0      /**< For Counter purpose 0 */
#define C_CountOne_U8X                     (char)1      /**< For Counter purpose 1 */

#define C_DetectionModeTimeOut_U8X         (char)70     /**< Detection Mode time out upper limit (2.8 seconds) */
#define C_CountingModeTimeOut_U8X          (char)100    /**< Counting Mode time out upper limit (4 seconds) */
#define C_ErrorCountLimit_U8X              (char)3//3   /**< Limit for Error count */
#define C_CorrectionCountLimit_U8X         (char)3      /**< Limit for Correction count */
#define C_InterStepCountLimit_U8X          (char)3      /**< Limit for Inter Step count (With Robustness) */
#define C_InterStepCountLimitNoRobustness  (char)3      /**< Limit for Inter Step count (Without Robustness) */

#define C_RunLowerRange_U8X                (char)36>>V_DivideFactor_U8R /**< Run Activity Step Peak-To-Peak to Step-Interval Ratio */
#define C_WalkLowerRange_U8X               (char)4      /**< Walk Activity Step Peak-To-Peak to Step-Interval Ratio */

#define C_ActivityFilterSize_U8X           (char)8      /**< Activity Filter Size */
#define C_RestTimeThreshold_U8R            (char)125    /**< Idle time Without any motion */
#define C_40ms_U8R                         (char)40     /**< Pedometer algorithm calling interval */

/* ACTIVITY TYPE */
#define STATIONARY                         (char)0      /**< STATIONARY */
#define WALK                               (char)1      /**< WALK */
#define RUN                                (char)3      /**< RUN */
/* EasyCASE - */
/* SPEED and CADENCE LPF CONSTANTS */
#define C_SpeedCadenceLpfInitialCountLimit_U8R  (char) 20     /**< No of Initial counts for which LPF for Speed and Cadence is relaxed */
#define C_CADENCE_ALPHA_LPF                     (float)0.125  /**< Alpha for Cadence Low Pass Filter */
#define C_SPEED_ALPHA_LPF                       (float)0.125  /**< Alpha for Speed Low Pass Filter */
/* EasyCASE - */
#define C_Xaxis_U8X                        (char)0      /**< X_Axis */
#define C_Yaxis_U8X                        (char)1      /**< Y_Axis */
#define C_Zaxis_U8X                        (char)2      /**< Z_Axis */
#define C_CompositeAxis_U8X                (char)3      /**< Composite_Axis */
/* EasyCASE - */
#define C_InstP2PTimeBufSize_U8R           (char)10                 /**< Instantaneous Peak-To-Peak time buffer size */
#define C_InstCadenceFluatuationLimit_U8R  (char)3                  /**< Once FIFO is filled, Maximum allowed fluctuations in The P2P time */
#define C_COMPOSITE_AXIS_CADENCE100_CUTOFF_U16R (unsigned short)250 /**< Lower cadence limit above which activity is considered as RUN */

/* EasyCASE ) */
/* EasyCASE ( 222
   ENUM Definitions */
/* EasyCASE ) */
/* EasyCASE ( 101281
   Variable Definitions */
unsigned char    V_InterStepCount_U8R=0;       /**< Holds the steps counted while not in counting */
unsigned char    V_SampleCountForCycle_U8R=0;  /**< Holds the sample counts between trend changes */
unsigned char    V_Activity_U8R;               /**< Holds the pedometer activity */
unsigned char    V_DivideFactor_U8R;           /**< Holds the division factor for the threshold */
unsigned char    V_StatusFlags_U8R;            /**< Holds the Status Flags used */
static unsigned short V_StepCount_U16R;        /**< Holds the Total StepCount */

static short  A_Raw_AccelC_S16R[C_FilterTaps_U8X];   /**< Holds the 24 input values of Composite-Axis to the filter */
static short  A_Raw_AccelX_S16R[C_FilterTaps_U8X];   /**< Holds the 24 input values of X-Axis to the filter */
static short  A_Raw_AccelY_S16R[C_FilterTaps_U8X];   /**< Holds the 24 input values of Y-Axis to the filter */
static short  A_Raw_AccelZ_S16R[C_FilterTaps_U8X];   /**< Holds the 24 input values of Z-Axis to the filter */
unsigned char V_LazyWalkLowerLimit_U8R =0;           /**< Holds the lazy walk lower limit threshold */
/* EasyCASE - */
U8 V_PreActivity_U8R = 0;                /**< Holds the previous activity type */
U8 V_ActivityDetected_U8R=0;             /**< Holds the current activity type */

static U8 V_InterStepCountLimit_U8R = 0; /**< Holds the interstepcount limit (User selectable for robust and non-robust cases) */
/* EasyCASE - */
/* Holds the Filter coefficients for 0.24 in Q15 format */
/* EasyCASE - */
/* EasyCASE < */
/* OLD COEFFICIENTS: FIR LPF FILTER(Fc = xHz to Fs = yHz )*/
static const short gc_CompositeFilterCoeff[C_FilterTaps_U8X] =
{
        58, 134, 173, 45, -324, -802, -999, -422, 1192, 3614, 6078, 7637,
        7637, 6078, 3614, 1192, -422, -999, -802, -324, 45, 173, 134, 58
};
/* EasyCASE > */
/* EasyCASE - */
/* EasyCASE < */
/* NEW COEFFICIENTS: FIR LPF FILTER(Fc = 4Hz to Fs = 6Hz )*/
static const short gc_HighSpeedFilterCoeff[C_FilterTaps_U8X] =
{
     290, 288, -353, -597, -26, 1110, 857, -1154, -2584, 10, 6498, 12235,
     12235, 6498, 10, -2584, -1154, 857, 1110, -26, -597, -353, 288, 290
};
/* EasyCASE > */
/* EasyCASE - */
/* Variables Specific for Internal and External Buffers - Refer to PedoSupport.c */
U8 A_TempActivityBuffer_U8R[INTERNAL_FIFO_MAX_SIZE];       /**< Temporary instantaneous activity buffer */
U8 A_TempP2PCountBuffer_U8R[INTERNAL_FIFO_MAX_SIZE];       /**< Temporary instantaneous peak-to-peak time count buffer */
U8 A_TempS2SCountBuffer_U8R[INTERNAL_FIFO_MAX_SIZE];       /**< Temporary instantaneous step-to-step time count buffer */
U8 V_ActivityInstateneous_U8R = 0;                         /**< Instantaneous Activity */
static U8 V_AxisSelectFlag_U8R = C_AXIS_IS_COMPOSITE_U8X;  /**< Axis Selection flag (Single/Composite)? */
/* EasyCASE - */
ts_SingleAxis S_Param_Xaxis;  /**< Structure variable to hold X-axis acceleration features  */
ts_SingleAxis S_Param_Yaxis;  /**< Structure variable to hold Y-axis acceleration features  */
ts_SingleAxis S_Param_Zaxis;  /**< Structure variable to hold Z-axis acceleration features  */
ts_SingleAxis S_Param_Caxis;  /**< Structure variable to hold C-axis acceleration features  */
ts_LPF S_ObjCadence;          /**< Structure variable to hold low pass filter values for Cadence */
ts_LPF S_ObjSpeed;            /**< Structure variable to hold low pass filter values for Speed */
/* EasyCASE - */
U8  V_TempFIFOSize_U8R = 0;            /**< Temporary FIFO size */
U16 V_MovAvgStableCadenceX100_U16R=0;  /**< Step based stable cadence (Moving Averaged) */
U16 V_SingleAxisStableCadence_U16R;    /**< Stride based cadence for stable axis */
U16 V_CompositeAxis_StrideRatio_U16R;  /**< Stride Peak-to-Peak value to Stride Interval ratio for composite axis */
U16 V_CompositeAxis_CadenceX100_U16R;  /**< Cadence for Composite axis */
/* EasyCASE - */
F32 V_CurrentDist_F32R = 0.0;          /**< Current Activity Distance (in Meters) */
/* EasyCASE - */
ts_ExtraVar ExtraVar;  /**< ( Extra structure variables to be removed after analysis ) */
/* EasyCASE ) */
/* EasyCASE ( 223
   Constant- and Table-Definitions in ROM/EEPROM */
/* EasyCASE ) */
/* EasyCASE ( 252
   Functions Prototype Declaration */
static void  PEDO_TrendFinder( short, short* ); 
static short PEDO_GetAbsoluteShort( short );
static void  PEDO_Get_SingleAxisAccelFeatures( S16, ts_SingleAxis* );
static void  PEDO_Reset_SingleAxisAccelFeatures( ts_SingleAxis* );
static void  PEDO_Find_StableSingleAxis_StableCadenceX100( ts_SingleAxis*, ts_SingleAxis*, ts_SingleAxis*, ts_SingleAxis*, U8, U8, U8, U8*, U16*);
static void  PEDO_FindDominantAxisOrder( ts_SingleAxis*, ts_SingleAxis*, ts_SingleAxis*, U8, U8*,  U8*, U8* );
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ( 70
   API-Interface functions */
/* EasyCASE ( 101478
   PEDO_GetStepCount */
/* EasyCASE F */
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
unsigned long PEDO_GetStepCount(void)
   {
   return (V_StepCount_U16R );
   }
/* EasyCASE ) */
/* EasyCASE ( 101486
   PEDO_StopDetection */
/* EasyCASE F */
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
void PEDO_StopDetection(void)
   {
   /* Reset all flags except Robustness Flag */    
   V_StatusFlags_U8R = (V_StatusFlags_U8R & M_DisableRobustness_U8X);
   }
/* EasyCASE ) */
/* EasyCASE ( 101488
   PEDO_StartDetection */
/* EasyCASE F */
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
void PEDO_StartDetection(void)
   {
   /* This function will set the mode as detection mode. If this function is called
      in between the program this function will reset the step count value. */
   /* EasyCASE - */
   /* All flags except robustness cleared */
   V_StatusFlags_U8R = (V_StatusFlags_U8R & M_DisableRobustness_U8X);
   /* Set the Previous data count as 1 */
   V_StatusFlags_U8R |= M_AlgoReset_U8X;;
   /* Set the mode as Detection Mode */
   V_StatusFlags_U8R |= M_ModeDetection_U8X;;
   }
/* EasyCASE ) */
/* EasyCASE ( 101490
   PEDO_ResetStepCount */
/* EasyCASE F */
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
void PEDO_ResetStepCount(void)
   {
   /* Reset the number of steps counted to 0 */
   V_StepCount_U16R = C_Clear_U8X;
   /* Reset all flags except Robustness flag */
   V_StatusFlags_U8R = (V_StatusFlags_U8R & M_DisableRobustness_U8X);
   /* Set the previous data as 1 */
   V_StatusFlags_U8R|=(M_AlgoReset_U8X|M_ModeDetection_U8X);
   /* Reset activity */
   V_Activity_U8R=C_Clear_U8X;
   }
/* EasyCASE ) */
/* EasyCASE ( 101492
   PEDO_GetActivity */
/* EasyCASE F */
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
unsigned char PEDO_GetActivity(void)
   {
   /*Activities are identified based on the acceleration pattern features */
   return (V_ActivityDetected_U8R);
   }
/* EasyCASE ) */
/* EasyCASE ( 101496
   PEDO_InitAlgo */
/* EasyCASE F */
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
void PEDO_InitAlgo(unsigned char v_GRange_u8r)
   { 
      /* Initialze the variables that are being used in the Pedometer Algorithm. */
      /* EasyCASE - */
      V_Activity_U8R = C_Clear_U8X;  /* Clear User Activity Type */
      
      /* Clear Speed and Cadence Filter Variables */
      S_ObjCadence.V_FilterValue_F32R = 0; 
      S_ObjSpeed.V_FilterValue_F32R = 0;
      
      /* Initialise the LPF_ALPHA for Speed and Cadence Filter Variables */
      S_ObjCadence.V_ALPHA_F32R = C_CADENCE_ALPHA_LPF;
      S_ObjSpeed.V_ALPHA_F32R = C_SPEED_ALPHA_LPF;
      
      /* Reset the step count */
      V_StepCount_U16R = C_Clear_U8X;
      
      /* Clear the Status Flag and Set the MODE as Detection to startc algo afresh */
      V_StatusFlags_U8R = C_Clear_U8X;
      V_StatusFlags_U8R |= (M_ModeDetection_U8X|M_AlgoReset_U8X);
      V_DivideFactor_U8R=v_GRange_u8r;
      
      /*Default lower limit for lazy walk is limit with robustness enabled */
      V_LazyWalkLowerLimit_U8R=LAZY_WALKING_LOWER_LIMIT_Robust;
      
      /* Enable Robustness feature */
      PEDO_EnableRobustness();
      
      /* Set InterStepCount limit */
      PEDO_SetInterStepCount(5);
   }
/* EasyCASE ) */
/* EasyCASE ( 101497
   PEDO_Enable_Robustness */
/* EasyCASE F */
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
void PEDO_EnableRobustness(void)
   {
   /* Enable Robustness feature of the Pedometer */
   V_StatusFlags_U8R&=~(M_DisableRobustness_U8X);
   /*Lower threshold for robust mode*/
   V_LazyWalkLowerLimit_U8R=LAZY_WALKING_LOWER_LIMIT_Robust;
   }
/* EasyCASE ) */
/* EasyCASE ( 101498
   PEDO_SetInterStepCount */
/* EasyCASE F */
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
void PEDO_SetInterStepCount(U8 v_InterStepCountLimit_u8r )
   {
   V_InterStepCountLimit_U8R = v_InterStepCountLimit_u8r;
   }
/* EasyCASE ) */
/* EasyCASE ( 101499
   PEDO_Disable_Robustness */
/* EasyCASE F */
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
void PEDO_DisableRobustness(void)
   {
   /* Disable robustness feature for Pedometer Algorithm */
   V_StatusFlags_U8R|=(M_DisableRobustness_U8X);
    /*Set Lasy walk lower thresold for non_robust mode*/
   V_LazyWalkLowerLimit_U8R=LAZY_WALKING_LOWER_LIMIT_NoRobust;
   }
/* EasyCASE ) */
/* EasyCASE ( 101500
   PEDO_ResetAlgo */
/* EasyCASE F */
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
void PEDO_ResetAlgo(void)
   {
   V_PreActivity_U8R = 0;  /* Clear the Previous activity */
   
   S_ObjCadence.V_FilterValue_F32R = 0; /* Clear the LPF filtered value for Cadence */
   S_ObjSpeed.V_FilterValue_F32R = 0; /* Clear the LPF filtered value for Speed */
   
   S_ObjCadence.V_ALPHA_F32R = C_CADENCE_ALPHA_LPF; /* Set the LPF_ALPHA value for Cadence */
   S_ObjSpeed.V_ALPHA_F32R = C_SPEED_ALPHA_LPF; /* Set the LPF_ALPHA value for Speed */
   
   PEDO_ResetStepCount(); /* Reset the Step Count */
   
   /* Reset single axis parameters for the X, Y, Z and composite axis. */
   PEDO_Reset_SingleAxisAccelFeatures( &S_Param_Xaxis );
   PEDO_Reset_SingleAxisAccelFeatures( &S_Param_Yaxis );
   PEDO_Reset_SingleAxisAccelFeatures( &S_Param_Zaxis );
   PEDO_Reset_SingleAxisAccelFeatures( &S_Param_Caxis );
   }
/* EasyCASE ) */
/* EasyCASE ( 101502
   PEDO_GetBandwidthSwitchInfo */
/* EasyCASE F */
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
U8 PEDO_GetBandwidthSwitchInfo(void)
   {
   return V_AxisSelectFlag_U8R;
   }
/* EasyCASE ) */
/* EasyCASE ( 101504
   PEDO_ProcessAccelarationData */
/* EasyCASE F */
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
short PEDO_ProcessAccelarationData(short v_RawAccelX_s16r, short v_RawAccelY_s16r, short v_RawAccelZ_s16r)
   {
   /* EasyCASE ( 101532
      Variable Declerations */
   signed char  v_Index_u8r = 0; /* Local Index variable */
   static unsigned char  v_CorrectionCount_u8r=0; /* Correction Count */
   static unsigned char  v_ErrorCount_u8r=0; /* Error Count between two steps */
   /* EasyCASE - */
   static unsigned char   v_CountStepToStep_u8r=0; /* Time count b/w last and current step */
   static unsigned char   v_CountStepToStepPrev1_u8r=0; /* Time count b/w second last and last step */
   static unsigned char   v_CountStepToStepPrev2_u8r=0; /* Time count b/w third last and second last step */
   static unsigned char   v_ModeTimeoutCount_u8r=0; /* Detection/Counting Mode timeout counter */
   /* EasyCASE - */
   short  v_RawAccelC_s16r=0; /* Raw Composite Acceleration value */
   short  v_FiltAccelTrendAxis_s16r=0;  /* Filtered acceleration value for stable single axis */
   short  v_FiltAccel4Axis_s16r[4]; /* Filtered acceleration value for all 4 axis (X, Y, Z and Composite) */
   
   /* Initialise the filter result value to 0 */
   long  v_FilterResultAxisX_u32r=C_Clear_U8X; /* Filtered X-Axis Accel Value */
   long  v_FilterResultAxisY_u32r=C_Clear_U8X; /* Filtered Y-Axis Accel Value */
   long  v_FilterResultAxisZ_u32r=C_Clear_U8X; /* Filtered Z-Axis Accel Value */
   long  v_FilterResultAxisC_u32r=C_Clear_U8X; /* Filtered Composite-Axis Accel Value */
   static char v_PreStatusFlags_u8r=0; /* Previous status */
   /* EasyCASE - */
   static short v_NegTrendAccelVal_s16r=0x7FFF; /* Acceleration value at Negetive Trend */
   /* Previous acceleration value difference from negetive to positive trends */
   static short v_PrevPosNegTrendAccelValDiff_s16r=0;
   /* EasyCASE - */
   /* EasyCASE < */
   unsigned short v_AvgActivityDetectionRatio_u16r=0; /* Average step peak-to-peak value versus step-interval ratio */
   static unsigned short a_AvgActivityDetectionRatioBuf_u16r[C_ActivityFilterSize_U8X]={0}; /* step peak-to-peak value versus step-interval ratio buffer */
   static unsigned char v_TrendDominantAxis_u8r=C_CompositeAxis_U8X; /* Trend dominant axis out of 4 axis (X, Y, Z and Composite ) */
   /* EasyCASE > */
   /* EasyCASE - */
   /* Variables for the single axis accel features */
   static unsigned char v_XaxisStrideDetectedFlag_u8r=0; /* X-axis stride detection flag */
   static unsigned char v_YaxisStrideDetectedFlag_u8r=0; /* Y-axis stride detection flag */
   static unsigned char v_ZaxisStrideDetectedFlag_u8r=0; /* Z-axis stride detection flag */
   static unsigned char v_CaxisStrideDetectedFlag_u8r=0; /* Composite-axis stride detection flag */
   
   unsigned char v_IstDomAxis_u8r=0; /* First dominant axis */
   unsigned char v_IIndDomAxis_u8r=0; /* Second dominant axis */
   unsigned char v_IIIrdDomAxis_u8r=0; /* Third dominant axis */
   unsigned char v_Activity_u8r=0; /* User activity */
   static   char v_PreviousActivity_u8r=0; /* Previous Activity */
   unsigned char v_SingleStableAxis_u8r = C_CompositeAxis_U8X; /* Stable axis out of 4 axis ( X, Y, Z and Composite ) */
   unsigned long v_TempU32_u32r=0; /* Temp unsigned int variable */
   /* EasyCASE ) */
   /* EasyCASE ( 101533
      Mode is sleeping? */
   /* Check whether the mode is sleeping mode */
   if (V_StatusFlags_U8R & (M_ModeDetection_U8X|M_ModeCounting_U8X))
      {
      }
   else
      {
      return 0;
      }
   /* EasyCASE ) */
   /* EasyCASE ( 101534
      Reset Previous data */
   if (V_StatusFlags_U8R & M_AlgoReset_U8X)
      {
      /* Set the reset the previous data as 0 */
      V_StatusFlags_U8R &= ~M_AlgoReset_U8X;
      /*Holds the previous activity type*/
      v_PreviousActivity_u8r=C_Clear_U8X;
      
      /* Holds the steps counted while not in counting */
      V_InterStepCount_U8R=C_Clear_U8X;
      /* 0 if there is no correction. correction value if there needs to be a correction */
      v_CorrectionCount_u8r=C_Clear_U8X;
      /*Holds error count between 2 steps*/
      v_ErrorCount_u8r=C_Clear_U8X;
      /*Holds the count between two consecutive counts*/
      v_ModeTimeoutCount_u8r=C_Clear_U8X;
      /* Holds the sample counts between trend changes */
      V_SampleCountForCycle_U8R=C_Clear_U8X;
      /* Holds the time count between last & this step */
      v_CountStepToStep_u8r=C_Clear_U8X;
      /* Holds the time count between second last & last step */
      v_CountStepToStepPrev1_u8r=C_Clear_U8X;
      /* Holds the time count between third last & second last step */
      v_CountStepToStepPrev2_u8r=C_Clear_U8X;
      /*Initialise interstep count*/
      V_InterStepCountLimit_U8R = C_InterStepCountLimit_U8X;
      
      /* Holds the composite high for the step cycle */
      v_NegTrendAccelVal_s16r=0x7FFF;
      /*Holds prev Result Diff*/
      v_PrevPosNegTrendAccelValDiff_s16r=C_Clear_U8X;
      /*The variable holding filter result are cleared*/
      v_FilterResultAxisC_u32r=C_Clear_U8X;
      
      /* Clear Internal FIFO buffer data size counter */
      ResetInternalFIFO();
      /* Clear External FIFO buffer data size counter */
      ResetExternalFIFO();
      /* EasyCASE - */
      /* Clear all axis accel buffers */
      for (v_Index_u8r=0;v_Index_u8r<C_FilterTaps_U8X;v_Index_u8r++)
         {
         A_Raw_AccelX_S16R[v_Index_u8r] = C_Clear_U8X;
         A_Raw_AccelY_S16R[v_Index_u8r] = C_Clear_U8X;
         A_Raw_AccelZ_S16R[v_Index_u8r] = C_Clear_U8X;
         A_Raw_AccelC_S16R[v_Index_u8r] = C_Clear_U8X;
         }
      /* Clear activity detection ratio buffer */
      for (v_Index_u8r=(C_ActivityFilterSize_U8X-1);v_Index_u8r>=C_CountZero_U8X;v_Index_u8r--)
         {
         a_AvgActivityDetectionRatioBuf_u16r[v_Index_u8r]=0;
         }
      V_ActivityDetected_U8R = (U8) STATIONARY; /* Default activity is Rest */
      v_TrendDominantAxis_u8r = C_CompositeAxis_U8X; /* Default trend dominating axis is Composite */
      V_AxisSelectFlag_U8R = C_AXIS_IS_COMPOSITE_U8X; /* Default axis selection is "AxisIsComposite" */
      }
   /* EasyCASE ) */
   /* EasyCASE ( 101535
      Check whether any change in Robstness feature? */
   if (/*Check whether robustness feature status changed*/
       ((V_StatusFlags_U8R & M_DisableRobustness_U8X)!=
        (v_PreStatusFlags_u8r & M_DisableRobustness_U8X)))
      {
      /*Robustness feature got changed So Clear Temporary counts */
      v_CorrectionCount_u8r=0;
      V_InterStepCount_U8R=0;
      V_StatusFlags_U8R &=~(M_Qualified_U8X|M_UnQualified_U8X);
      v_ModeTimeoutCount_u8r = 0;
      V_StatusFlags_U8R |=M_ModeDetection_U8X;
      V_StatusFlags_U8R &=~M_ModeCounting_U8X;
      ResetInternalFIFO(); /* Clear Internal FIFO buffer data size variable */
      }
   /* EasyCASE ) */
   /* EasyCASE ( 101536
      Count Updation */
   V_SampleCountForCycle_U8R++; /* Increment the count for cycle */
   v_CountStepToStep_u8r++;     /* Increment the step to step count */
   v_ModeTimeoutCount_u8r++;    /* Increment Count Mode Elapse Timer*/
   /* EasyCASE ) */
   /* EasyCASE ( 101537
      Find Trend Properties */
   /* EasyCASE ( 101538
      Calculate the Composite Acceleration */
   v_RawAccelC_s16r = PEDO_GetAbsoluteShort(v_RawAccelX_s16r)+ PEDO_GetAbsoluteShort(v_RawAccelY_s16r) + PEDO_GetAbsoluteShort(v_RawAccelZ_s16r);
   /* EasyCASE ) */
   /* EasyCASE ( 101539
      Calculate the FIR-LPF Filtered Acceleration Values */
   /* shift the acceleration values into respective FIFOs for X, Y, Z and Composite axis  */
   for (v_Index_u8r=C_CountZero_U8X; v_Index_u8r<(C_FilterTaps_U8X-1); v_Index_u8r++)
      {
      A_Raw_AccelX_S16R[C_FilterTaps_U8X-v_Index_u8r-1] = A_Raw_AccelX_S16R[C_FilterTaps_U8X-v_Index_u8r-2];
      A_Raw_AccelY_S16R[C_FilterTaps_U8X-v_Index_u8r-1] = A_Raw_AccelY_S16R[C_FilterTaps_U8X-v_Index_u8r-2];
      A_Raw_AccelZ_S16R[C_FilterTaps_U8X-v_Index_u8r-1] = A_Raw_AccelZ_S16R[C_FilterTaps_U8X-v_Index_u8r-2];
      A_Raw_AccelC_S16R[C_FilterTaps_U8X-v_Index_u8r-1] = A_Raw_AccelC_S16R[C_FilterTaps_U8X-v_Index_u8r-2];
      }
   A_Raw_AccelX_S16R[C_CountZero_U8X] = v_RawAccelX_s16r;
   A_Raw_AccelY_S16R[C_CountZero_U8X] = v_RawAccelY_s16r;
   A_Raw_AccelZ_S16R[C_CountZero_U8X] = v_RawAccelZ_s16r;
   A_Raw_AccelC_S16R[C_CountZero_U8X] = v_RawAccelC_s16r;
   /* EasyCASE - */
   /* Clear all the variables holding filtered acceleration value for X, Y, Z and Composite axis */
   /* EasyCASE - */
   v_FilterResultAxisX_u32r=C_Clear_U8X;
   v_FilterResultAxisY_u32r=C_Clear_U8X;
   v_FilterResultAxisZ_u32r=C_Clear_U8X;
   v_FilterResultAxisC_u32r=C_Clear_U8X;
   /* EasyCASE - */
   /* Calculate the FIR LPF filtered values for X, Y, Z and Composite axis */
   for (v_Index_u8r=C_CountZero_U8X;v_Index_u8r<C_FilterTaps_U8X;v_Index_u8r++)
      {
      v_FilterResultAxisX_u32r = v_FilterResultAxisX_u32r + 
      (long)((long)A_Raw_AccelX_S16R[(C_FilterTaps_U8X-C_CountOne_U8X)-v_Index_u8r]
      *(long)gc_HighSpeedFilterCoeff[v_Index_u8r]);
      /* EasyCASE - */
      v_FilterResultAxisY_u32r = v_FilterResultAxisY_u32r + 
      (long)((long)A_Raw_AccelY_S16R[(C_FilterTaps_U8X-C_CountOne_U8X)-v_Index_u8r]
      *(long)gc_HighSpeedFilterCoeff[v_Index_u8r]);
      /* EasyCASE - */
      v_FilterResultAxisZ_u32r = v_FilterResultAxisZ_u32r + 
      (long)((long)A_Raw_AccelZ_S16R[(C_FilterTaps_U8X-C_CountOne_U8X)-v_Index_u8r]
      *(long)gc_HighSpeedFilterCoeff[v_Index_u8r]);
      /* EasyCASE - */
      v_FilterResultAxisC_u32r = v_FilterResultAxisC_u32r +
       (long)((long)A_Raw_AccelC_S16R[(C_FilterTaps_U8X-C_CountOne_U8X)-v_Index_u8r]
      *(long)gc_CompositeFilterCoeff[v_Index_u8r]);
      }
   /* Divide by 32768 to compensate Q15 format multiplication.*/
   /* EasyCASE - */
   v_FiltAccel4Axis_s16r[C_Xaxis_U8X] = (short)(v_FilterResultAxisX_u32r>>C_Q15ConversionFactor_U8X);
   v_FiltAccel4Axis_s16r[C_Yaxis_U8X] = (short)(v_FilterResultAxisY_u32r>>C_Q15ConversionFactor_U8X);
   v_FiltAccel4Axis_s16r[C_Zaxis_U8X] = (short)(v_FilterResultAxisZ_u32r>>C_Q15ConversionFactor_U8X);
   v_FiltAccel4Axis_s16r[C_CompositeAxis_U8X] = (short)(v_FilterResultAxisC_u32r>>C_Q15ConversionFactor_U8X);
   /* EasyCASE ) */
   /* EasyCASE ( 101540
      Find Stable Single Axis ( X, Y, Z or Composite Axis ) */
   /* Find all the axis (X, Y, Z and Composite Axis) properties */ 
   PEDO_Get_SingleAxisAccelFeatures( v_FiltAccel4Axis_s16r[C_Xaxis_U8X], &S_Param_Xaxis);
   PEDO_Get_SingleAxisAccelFeatures( v_FiltAccel4Axis_s16r[C_Yaxis_U8X], &S_Param_Yaxis);
   PEDO_Get_SingleAxisAccelFeatures( v_FiltAccel4Axis_s16r[C_Zaxis_U8X], &S_Param_Zaxis);
   PEDO_Get_SingleAxisAccelFeatures( v_FiltAccel4Axis_s16r[C_CompositeAxis_U8X], &S_Param_Caxis);
   /* EasyCASE - */
   /* Get the PedoActivity --> STATIONARY, WALK or RUN */ 
   v_Activity_u8r = PEDO_GetActivity();
   /* EasyCASE - */
   /* EasyCASE < */
   /* Check for X, Y, Z and Composite Axis if any stride pattern got detected?? */ 
    if (S_Param_Xaxis.V_ValidStridePatternOccuredFlag_U8X==1)
            v_XaxisStrideDetectedFlag_u8r = 1; /* For X-axis Stride Pattern got detected */
    
    if (S_Param_Yaxis.V_ValidStridePatternOccuredFlag_U8X==1)
            v_YaxisStrideDetectedFlag_u8r = 1; /* For Y-axis Stride Pattern got detected */
    
    if (S_Param_Zaxis.V_ValidStridePatternOccuredFlag_U8X==1)
            v_ZaxisStrideDetectedFlag_u8r = 1; /* For Z-axis Stride Pattern got detected */
    
    if (S_Param_Caxis.V_ValidStridePatternOccuredFlag_U8X==1)
            v_CaxisStrideDetectedFlag_u8r = 1; /* For Composite-axis Stride Pattern got detected */
    
    /* Check atleast for 2 out of 3 axis whether stride pattern got detected or not ?? */
    if( ((v_XaxisStrideDetectedFlag_u8r==1)&&(v_YaxisStrideDetectedFlag_u8r==1))||
        ((v_YaxisStrideDetectedFlag_u8r==1)&&(v_ZaxisStrideDetectedFlag_u8r==1))||
        ((v_ZaxisStrideDetectedFlag_u8r==1)&&(v_XaxisStrideDetectedFlag_u8r==1))
      )
    {                                
          PEDO_FindDominantAxisOrder( &S_Param_Xaxis, &S_Param_Yaxis, &S_Param_Zaxis, v_Activity_u8r, &v_IstDomAxis_u8r, &v_IIndDomAxis_u8r, &v_IIIrdDomAxis_u8r );
          v_XaxisStrideDetectedFlag_u8r=0;
          v_YaxisStrideDetectedFlag_u8r=0;
          v_ZaxisStrideDetectedFlag_u8r=0;
          v_CaxisStrideDetectedFlag_u8r=0;
    }
   /* EasyCASE > */
   /* EasyCASE - */
    /* Call PEDO_Find_StableSingleAxis_StableCadenceX100_StableCadenceX100API, every 40 msec cycle,
       to find the Stable axis out of 4-axis X,Y,Z and Composite axis and Stable Cadence for the selected axis */
   PEDO_Find_StableSingleAxis_StableCadenceX100( &S_Param_Xaxis, &S_Param_Yaxis, &S_Param_Zaxis, &S_Param_Caxis, v_IstDomAxis_u8r, v_IIndDomAxis_u8r,
                                                  v_Activity_u8r, &v_SingleStableAxis_u8r, &V_SingleAxisStableCadence_U16R  );
   V_CompositeAxis_StrideRatio_U16R = (U16)(2*S_Param_Caxis.A_StridePeakToPeakValBuf_U16X[0]/(F32)S_Param_Caxis.A_StrideIntervalBuf_U8X[0]);
   V_CompositeAxis_CadenceX100_U16R = S_Param_Caxis.V_CadenceX100_U16X;
   /* EasyCASE ) */
   /* EasyCASE ( 101541
      Axis Selection --> Single/Composite Axis ? */
   /* Do the Axis selection Single/Composite based on the activity detected */
   if (v_Activity_u8r == RUN)
      {
      V_AxisSelectFlag_U8R=C_AXIS_IS_SINGLE_U8X;
      }
   else
      {
      V_AxisSelectFlag_U8R=C_AXIS_IS_COMPOSITE_U8X;
      }
   /* EasyCASE ) */
   /* EasyCASE ( 101542
      Assign Trend Dominating Axis and filtered acceleration value of Trend Dominating Axis */
   /* Assign Trend Dominating Axis based on the axis selection (Single Axis or Composite Axis) */
   if (V_AxisSelectFlag_U8R==C_AXIS_IS_SINGLE_U8X)
      {
      v_TrendDominantAxis_u8r=v_SingleStableAxis_u8r;
      }
   else
      {
      v_TrendDominantAxis_u8r=C_CompositeAxis_U8X;
      }
   /* Assign acceleration value of Trend Dominating Axis */
   /* EasyCASE - */
   v_FiltAccelTrendAxis_s16r=v_FiltAccel4Axis_s16r[v_TrendDominantAxis_u8r];
   /* EasyCASE ) */
   /* EasyCASE ( 101543
      Find Trends (Positive/Negetive)? */
   PEDO_TrendFinder(v_FiltAccelTrendAxis_s16r, &v_FiltAccelTrendAxis_s16r);
   /* EasyCASE ) */
   /* EasyCASE ( 101544
      Extra Variables (To be removed) */
   ExtraVar.RawCompAccel = v_RawAccelC_s16r; // To be removed after filtering
   ExtraVar.v_FiltAccel4Axis_s16r[C_Xaxis_U8X]=v_FiltAccel4Axis_s16r[C_Xaxis_U8X];
   ExtraVar.v_FiltAccel4Axis_s16r[C_Yaxis_U8X]=v_FiltAccel4Axis_s16r[C_Yaxis_U8X];
   ExtraVar.v_FiltAccel4Axis_s16r[C_Zaxis_U8X]=v_FiltAccel4Axis_s16r[C_Zaxis_U8X];
   ExtraVar.v_FiltAccel4Axis_s16r[C_CompositeAxis_U8X]=v_FiltAccel4Axis_s16r[C_CompositeAxis_U8X];
   
   ExtraVar.X_StrideRatio = (U16)(2*S_Param_Xaxis.A_StridePeakToPeakValBuf_U16X[0]/(F32)S_Param_Xaxis.A_StrideIntervalBuf_U8X[0]);
   ExtraVar.Y_StrideRatio = (U16)(2*S_Param_Yaxis.A_StridePeakToPeakValBuf_U16X[0]/(F32)S_Param_Yaxis.A_StrideIntervalBuf_U8X[0]);
   ExtraVar.Z_StrideRatio = (U16)(2*S_Param_Zaxis.A_StridePeakToPeakValBuf_U16X[0]/(F32)S_Param_Zaxis.A_StrideIntervalBuf_U8X[0]);
   ExtraVar.C_StrideRatio = (U16)(2*S_Param_Caxis.A_StridePeakToPeakValBuf_U16X[0]/(F32)S_Param_Caxis.A_StrideIntervalBuf_U8X[0]);
   
   ExtraVar.IstDomAxis = v_IstDomAxis_u8r;
   ExtraVar.IIndDomAxis = v_IIndDomAxis_u8r;
   ExtraVar.IIIrdDomAxis = v_IIIrdDomAxis_u8r;
   
   ExtraVar.X_Stability = S_Param_Xaxis.V_AxisIsStableFlag_U8X;
   ExtraVar.Y_Stability = S_Param_Yaxis.V_AxisIsStableFlag_U8X;
   ExtraVar.Z_Stability = S_Param_Zaxis.V_AxisIsStableFlag_U8X;
   ExtraVar.C_Stability = S_Param_Caxis.V_AxisIsStableFlag_U8X;
   
   ExtraVar.CurrentStableAxis = v_SingleStableAxis_u8r;
   
   ExtraVar.X_CadenceX100 = S_Param_Xaxis.V_CadenceX100_U16X;
   ExtraVar.Y_CadenceX100 = S_Param_Yaxis.V_CadenceX100_U16X;
   ExtraVar.Z_CadenceX100 = S_Param_Zaxis.V_CadenceX100_U16X;
   ExtraVar.C_CadenceX100 = S_Param_Caxis.V_CadenceX100_U16X;
   
   ExtraVar.StableStrideCadenceX100 = V_SingleAxisStableCadence_U16R;
   ExtraVar.v_TrendDominantAxis_u8r=v_TrendDominantAxis_u8r;
   ExtraVar.v_SingleOrCompositeAxisFlag_u8r=V_AxisSelectFlag_U8R;
   /* EasyCASE ) */
   /* EasyCASE ) */
   /* EasyCASE ( 101545
      Human Activity */
   /* Check whether the trend is Positive or Negative */
   if (V_StatusFlags_U8R & M_NegativeTrend_U8X)
      {
      /* Save the value of acceleration at Negetive Trend */
      v_NegTrendAccelVal_s16r = v_FiltAccelTrendAxis_s16r;
      }
   else
      {
      if (V_StatusFlags_U8R & M_PositiveTrend_U8X)
         {
         /* Set the status as Step Qualified or Unqualified ? */
         V_StatusFlags_U8R &=~(M_Qualified_U8X|M_UnQualified_U8X);
         
         /* Calculate the acceleration value difference for successive trends 
            (Negetive trend value - Positive trend value) */
         v_FiltAccelTrendAxis_s16r = v_NegTrendAccelVal_s16r - v_FiltAccelTrendAxis_s16r;
         
         /* Clear the Activity */
         V_Activity_U8R = C_Clear_U8X;
         /* EasyCASE - */
         /* Check whether the step is Walk, Jog or Run? */
         /* EasyCASE ( 101546
            Lazy Walk */
         /* Check whether the step is lazy walk step */
         if (IS_LAZY_WALK_STEP(v_FiltAccelTrendAxis_s16r))
            {
            if (((v_CountStepToStep_u8r > C_LazyWalkStepToStepLowerLimit_U8X) 
                && (v_CountStepToStep_u8r < C_LazyWalkStepToStepUpperLimit_U8X)))
               {
               V_StatusFlags_U8R |= M_Qualified_U8X;
               }
            else
               {
               V_StatusFlags_U8R |= M_UnQualified_U8X;
               }
            /* Slow activity */
            V_Activity_U8R |= M_SlowWalk_U8X;
            }
         /* EasyCASE ) */
         /* EasyCASE ( 101547
            Walk */
         if (IS_WALK_STEP(v_FiltAccelTrendAxis_s16r))
            {
            if (((PEDO_GetAbsoluteShort(v_CountStepToStep_u8r -  v_CountStepToStepPrev1_u8r) <= MAX_COUNT_DIFFERENCE) ||
                ( PEDO_GetAbsoluteShort(v_CountStepToStep_u8r -  v_CountStepToStepPrev2_u8r) <=MAX_COUNT_DIFFERENCE) )&&
                ((v_CountStepToStep_u8r > C_WalkStepToStepLowerLimit_U8X) && (v_CountStepToStep_u8r < C_WalkStepToStepUpperLimit_U8X)))
               {
               V_StatusFlags_U8R |= M_Qualified_U8X;
               }
            else
               {
               V_StatusFlags_U8R |= M_UnQualified_U8X;
               }
            /* Medium activity */
            V_Activity_U8R |= M_Walk_U8X;
            }
         /* EasyCASE ) */
         /* EasyCASE ( 101548
            Jog */
         /* Check whether the step is Jog step */
         if (IS_JOG_STEP(v_FiltAccelTrendAxis_s16r))
            {
            if (((PEDO_GetAbsoluteShort(v_CountStepToStep_u8r -  v_CountStepToStepPrev1_u8r) <= MAX_COUNT_DIFFERENCE) ||
                ( PEDO_GetAbsoluteShort(v_CountStepToStep_u8r -  v_CountStepToStepPrev2_u8r) <=MAX_COUNT_DIFFERENCE) )&&
                ((v_CountStepToStep_u8r > C_JogStepToStepLowerLimit_U8X) && (v_CountStepToStep_u8r < C_JogStepToStepUpperLimit_U8X)))
               {
               V_StatusFlags_U8R |= M_Qualified_U8X;
               }
            else
               {
               V_StatusFlags_U8R |= M_UnQualified_U8X;
               }
            /* Brisk activity */
            V_Activity_U8R |= M_Jog_U8X;
            }
         /* EasyCASE ) */
         /* EasyCASE ( 101549
            Step counting */
         /* EasyCASE ( 101550
            Time out in Detection */
         if ((v_ModeTimeoutCount_u8r>C_DetectionModeTimeOut_U8X)&&( V_StatusFlags_U8R&M_ModeDetection_U8X)
             /* Check whether steps ate detected for 2.8 seconds in Detection Mode or not? */)
            {
            /* Clear the mode timeout count */
            v_ModeTimeoutCount_u8r=C_Clear_U8X;
            if (((V_StatusFlags_U8R & M_DisableRobustness_U8X)==0))
               {
               /* No activity in detection mode; so clear the Temporary step count*/
               v_CorrectionCount_u8r=C_Clear_U8X;
               V_InterStepCount_U8R=C_Clear_U8X;
               V_StatusFlags_U8R &=~(M_Qualified_U8X|M_UnQualified_U8X);
               v_ErrorCount_u8r=C_Clear_U8X;
               /* Clear Internal FIFO buffer data size variable */
                ResetInternalFIFO();
               }
            }
         /* EasyCASE ) */
         /* EasyCASE ( 101551
            Time out in Counting */
         if ((v_ModeTimeoutCount_u8r>C_CountingModeTimeOut_U8X)&&( V_StatusFlags_U8R&M_ModeCounting_U8X)
             /* No steps for 4 seconds in Count Mode*/)
            {
            /* Clear the mode timeout count */
            v_ModeTimeoutCount_u8r=C_Clear_U8X;
            if (((V_StatusFlags_U8R & M_DisableRobustness_U8X)==0))
               {
               /* No activity in counting mode; so clear the Temporary step count*/
               v_CorrectionCount_u8r=C_Clear_U8X;
               V_StatusFlags_U8R &=~(M_Qualified_U8X|M_UnQualified_U8X);
               v_ErrorCount_u8r=C_Clear_U8X;
                V_StatusFlags_U8R|=M_ModeDetection_U8X;
                V_StatusFlags_U8R&=~M_ModeCounting_U8X;
               /* Clear Internal FIFO buffer data size variable */
                ResetInternalFIFO();
               }
            }
         /* EasyCASE ) */
         if (/*Check whether step is valid or not*/
             ((V_StatusFlags_U8R & (M_Qualified_U8X|M_UnQualified_U8X))!=0))
            {
            /* If there is change in activity and the current result diff are greater than certain "Threshold" then
               temporary counts are cleared.Threshold=(Largest of Current Result Diff and previous Result Diff)/2.
               This is applicable in detection mode */
            /* EasyCASE ( 101552
               Activity Monitor */
            if (((v_PreviousActivity_u8r & V_Activity_U8R)==C_Clear_U8X) && (v_PreviousActivity_u8r !=C_Clear_U8X) &&
                ( V_StatusFlags_U8R&M_ModeDetection_U8X) && ((V_StatusFlags_U8R & M_DisableRobustness_U8X)==0) &&
                (((PEDO_GetAbsoluteShort(v_FiltAccelTrendAxis_s16r-v_PrevPosNegTrendAccelValDiff_s16r))<<C_CountOne_U8X)>
                ((v_FiltAccelTrendAxis_s16r>v_PrevPosNegTrendAccelValDiff_s16r)?v_FiltAccelTrendAxis_s16r:v_PrevPosNegTrendAccelValDiff_s16r)))
               {
               /* Activities differs in Detection state;  So, clear the temporary step count */
               V_InterStepCount_U8R = C_Clear_U8X;
               v_CorrectionCount_u8r=C_Clear_U8X;
               V_StatusFlags_U8R &=~(M_Qualified_U8X|M_UnQualified_U8X);
               
               /* Clear Internal FIFO buffer data size variable */
               ResetInternalFIFO();
               }
            /* EasyCASE ) */
            v_PreviousActivity_u8r=V_Activity_U8R; /*Stores the current Activity Type*/
            v_PrevPosNegTrendAccelValDiff_s16r=v_FiltAccelTrendAxis_s16r; /* Stores the current Negetive-To-Positive Trend Value Difference */
            v_ErrorCount_u8r=C_Clear_U8X; /*Error Count cleared*/
            v_ModeTimeoutCount_u8r=C_Clear_U8X; /* Reset the Mode Timeout Count */
            /* EasyCASE - */
            /* Find instantaneous activity */
            /* EasyCASE - */
            v_AvgActivityDetectionRatio_u16r = v_FiltAccelTrendAxis_s16r/V_SampleCountForCycle_U8R;
            if (/*If Activity threshold ratio > Lower threshold for run*/
                v_AvgActivityDetectionRatio_u16r>C_RunLowerRange_U8X)
               {
               V_ActivityInstateneous_U8R = (U8)RUN;
               }
            else
               {
               if (/*If threshold is <= Run Lower range and != 0*/
                   (v_AvgActivityDetectionRatio_u16r>C_WalkLowerRange_U8X))
                  {
                  V_ActivityInstateneous_U8R = (U8)WALK;
                  }
               else
                  {
                  V_ActivityInstateneous_U8R = (U8)STATIONARY;
                  }
               }
            /* Extra Variables (To be removed ) */
            ExtraVar.InstStep_PP_Val = v_FiltAccelTrendAxis_s16r;
            ExtraVar.InstStep_PP_Time = V_SampleCountForCycle_U8R;
            ExtraVar.InstStepRatio = (unsigned int)((F32)v_FiltAccelTrendAxis_s16r/(F32)ExtraVar.InstStep_PP_Time);
            ExtraVar.InstActivity = (U8)V_ActivityInstateneous_U8R;
            /* EasyCASE - */
            /* Check whether the  step is Qualified */
            if (V_StatusFlags_U8R & M_Qualified_U8X)
               {
               /* Check whether the mode is counting mode */
               /* EasyCASE ( 101553
                  Activity Detection Block - FIFO */
               for (v_Index_u8r=(C_ActivityFilterSize_U8X-1);v_Index_u8r>C_CountZero_U8X;v_Index_u8r--)
                  {
                  a_AvgActivityDetectionRatioBuf_u16r[v_Index_u8r]=a_AvgActivityDetectionRatioBuf_u16r[v_Index_u8r-1];
                  }
               a_AvgActivityDetectionRatioBuf_u16r[C_CountZero_U8X]= (U16)((F32)v_FiltAccelTrendAxis_s16r/(F32)V_SampleCountForCycle_U8R);
               /* EasyCASE - */
               /* Calculate the sum of the ActivityDetectionRatio buffer values */
               v_TempU32_u32r=0;
               for (v_Index_u8r=(C_ActivityFilterSize_U8X-1);v_Index_u8r>=C_CountZero_U8X;v_Index_u8r--)
                  {
                  v_TempU32_u32r += a_AvgActivityDetectionRatioBuf_u16r[v_Index_u8r];
                  }
               /*Taking Moving average of ratio*/
               v_AvgActivityDetectionRatio_u16r=(U16)((F32)v_TempU32_u32r/(F32)C_ActivityFilterSize_U8X);
               /* EasyCASE - */
               /* Check the current activity using Average Activity Detection Ratio */
               if (/*If Activity threshold ratio > Lower threshold for run*/
                   (v_AvgActivityDetectionRatio_u16r>C_RunLowerRange_U8X))
                  {
                  V_ActivityDetected_U8R = (U8)RUN;
                  }
               else
                  {
                  if (/*If threshold is <= Run Lower range and != 0*/
                      (v_AvgActivityDetectionRatio_u16r>C_WalkLowerRange_U8X))
                     {
                     /* Check Composite Axis Stride Peak-To-Peak Value and Stride Interval Time Ratio to varify whether activity is RUN or WALK? */
                     /* EasyCASE - */
                     V_ActivityDetected_U8R = (U8)WALK;
                     }
                  else
                     {
                     V_ActivityDetected_U8R = (U8) STATIONARY;
                     }
                  }
               /* EasyCASE ) */
               if (V_StatusFlags_U8R&M_ModeCounting_U8X)
                  {
                  /* EasyCASE ( 101554
                     Counting Mode */
                  if (/*Check whether correction count >3 in Counting Mode*/
                      v_CorrectionCount_u8r>C_CorrectionCountLimit_U8X)
                     {
                     /* Increment Step Count with Correction Count added into it */
                     V_StepCount_U16R+=(v_CorrectionCount_u8r+C_CountOne_U8X);
                     
                     /* Reset the Correction Count */
                     v_CorrectionCount_u8r = C_Clear_U8X;
                     
                     /* Push the current step parameters into external buffer */
                     SetExternalBuffer(&v_CountStepToStep_u8r,&V_SampleCountForCycle_U8R,&V_ActivityInstateneous_U8R,1);
                     
                     /* Get the internal buffer data size */
                     V_TempFIFOSize_U8R = GetInternalFIFOSize();
                     
                     /* Get internal buffer Parameters (i.e. Correction steps Paramaters ) */
                     GetInternalBuffer(A_TempS2SCountBuffer_U8R, A_TempP2PCountBuffer_U8R, A_TempActivityBuffer_U8R);
                     
                     /* Push the Correction Steps parameters into External Buffer */
                     SetExternalBuffer(A_TempS2SCountBuffer_U8R, A_TempP2PCountBuffer_U8R, A_TempActivityBuffer_U8R, V_TempFIFOSize_U8R);
                     }
                  else
                     {
                     /* Increment the Step Count */
                      V_StepCount_U16R++;
                      
                     /* Push the current step parameters into external buffer */
                     SetExternalBuffer(&v_CountStepToStep_u8r,&V_SampleCountForCycle_U8R,&V_ActivityInstateneous_U8R,1);
                     }
                  /* EasyCASE ) */
                  }
               else
                  {
                  /*Check whether current mode is Detection Mode*/
                  /* EasyCASE ( 101555
                     Detection Mode */
                  if (V_StatusFlags_U8R&M_ModeDetection_U8X)
                     {
                     if (/* Correction count is added to InterStepCount when correction count > 3 in detection mode */
                         v_CorrectionCount_u8r>C_CorrectionCountLimit_U8X)
                        {
                        /* Increment the InterStepCount with CorrectionCount added into it. */
                        V_InterStepCount_U8R+=(v_CorrectionCount_u8r+1);
                        
                        /* Reset the Correction Count */
                        v_CorrectionCount_u8r = C_Clear_U8X;
                        
                        /* Push the Detection mode Step Parameters into internal buffer */
                        SetInternalBuffer(v_CountStepToStep_u8r,V_SampleCountForCycle_U8R,V_ActivityInstateneous_U8R);
                        }
                     else
                        {
                        /* Increment the InterStepCount */
                         V_InterStepCount_U8R++;
                         
                        /* Push the Detection mode Step Parameters into internal buffer */
                        SetInternalBuffer(v_CountStepToStep_u8r,V_SampleCountForCycle_U8R,V_ActivityInstateneous_U8R);
                        }
                     if (/* When interstep count > 9 mode changed to counting in case if Robustness feature enabled;
                            When interstep count > 3 mode changed to counting in case if Robustness feature disabled */
                         ((V_InterStepCount_U8R > V_InterStepCountLimit_U8R)&&((V_StatusFlags_U8R & M_DisableRobustness_U8X)==0))||
                         ((V_InterStepCount_U8R > V_InterStepCountLimit_U8R)&&((V_StatusFlags_U8R & M_DisableRobustness_U8X)==M_DisableRobustness_U8X)))
                        {
                        /* Set the mode to MODE_COUNTING */
                        V_StatusFlags_U8R|=M_ModeCounting_U8X;
                        V_StatusFlags_U8R&=~M_ModeDetection_U8X;
                        
                        /* Increment the StepCount */
                        V_StepCount_U16R += (V_InterStepCount_U8R+v_CorrectionCount_u8r);
                        
                        /* Reset the InterStepCount */
                        V_InterStepCount_U8R = C_Clear_U8X;
                        
                        /* Reset the CorrectionCount */
                         v_CorrectionCount_u8r = C_Clear_U8X;
                         
                         /* Get the internal buffer data size */ 
                        V_TempFIFOSize_U8R = GetInternalFIFOSize();
                        
                        /* Get internal buffer Parameters (i.e. InterStep & CorrectionSteps  Paramaters ) */
                        GetInternalBuffer(A_TempS2SCountBuffer_U8R, A_TempP2PCountBuffer_U8R, A_TempActivityBuffer_U8R);
                        
                        /* Push all the internal buffer parameters into External Buffer */
                        SetExternalBuffer(A_TempS2SCountBuffer_U8R, A_TempP2PCountBuffer_U8R, A_TempActivityBuffer_U8R, V_TempFIFOSize_U8R);
                        }
                     }
                  /* EasyCASE ) */
                  }
               }
            else
               {
               /* EasyCASE ( 101556
                  Correction Count */
               if (/*Check whether Step is unqualified*/
                   V_StatusFlags_U8R & M_UnQualified_U8X)
                  {
                  /* Increment the Correction Count */
                  v_CorrectionCount_u8r++;
                  
                  /* Push the Detection mode Step Parameters into internal buffer */
                  SetInternalBuffer(v_CountStepToStep_u8r,V_SampleCountForCycle_U8R,V_ActivityInstateneous_U8R);
                  }
               /* EasyCASE ) */
               }
            }
         else
            {
            /* EasyCASE ( 101557
               Error Count */
            if (/*Error count is incremented if the step is not valid and not due to noise*/
                (v_FiltAccelTrendAxis_s16r>C_SENSOR_NOISE_LEVEL_U8X))
               {
               /*Error Count is incremented*/
               v_ErrorCount_u8r++;
               }
            if (/*When the error count becomes greater than 3 the temporary counts are cleared*/
                v_ErrorCount_u8r>C_ErrorCountLimit_U8X)
               {
               /*The mode changed to detection and counts are cleared*/
               V_StatusFlags_U8R|=M_ModeDetection_U8X;
               V_StatusFlags_U8R&=~M_ModeCounting_U8X;
               v_ErrorCount_u8r=C_Clear_U8X;
               v_CorrectionCount_u8r=C_Clear_U8X;
               V_InterStepCount_U8R=C_Clear_U8X;
               V_StatusFlags_U8R &=~(M_Qualified_U8X|M_UnQualified_U8X);
               v_ModeTimeoutCount_u8r = C_Clear_U8X;
               
               /* Reset the Internal Buffer data Size Counter */
                ResetInternalFIFO();
               }
            /* EasyCASE ) */
            }
         /* EasyCASE ( 101558
            Step to Step Count Updation */
         if (/*Count step to step is updated if the trend change is not due to noise*/
             v_FiltAccelTrendAxis_s16r>C_SENSOR_NOISE_LEVEL_U8X)
            {
            /* Update the last, secondlast and thridlast count variables */
            v_CountStepToStepPrev2_u8r = v_CountStepToStepPrev1_u8r;
            v_CountStepToStepPrev1_u8r = v_CountStepToStep_u8r;
            v_CountStepToStep_u8r = C_Clear_U8X;
            }
         /* EasyCASE ) */
         /* EasyCASE ) */
         /* Reset the sample count for cycle */
         V_SampleCountForCycle_U8R = C_Clear_U8X;
         }
      else
         {
         /* EasyCASE ( 101559
            Time out in Detection */
         if (/* No steps for 2.8 seconds in Detection Mode */
             /* No steps for 4 seconds in Counting Mode */
             ((v_ModeTimeoutCount_u8r>C_DetectionModeTimeOut_U8X)&&( V_StatusFlags_U8R&M_ModeDetection_U8X))||
             ((v_ModeTimeoutCount_u8r>C_CountingModeTimeOut_U8X)&&( V_StatusFlags_U8R&M_ModeCounting_U8X)))
            {
            v_AvgActivityDetectionRatio_u16r=0;
            V_ActivityDetected_U8R = (U8) STATIONARY;
            for (v_Index_u8r=(C_ActivityFilterSize_U8X-1);v_Index_u8r>=C_CountZero_U8X;v_Index_u8r--)
               {
               a_AvgActivityDetectionRatioBuf_u16r[v_Index_u8r]=0;
               }
            }
         /* EasyCASE ) */
         }
      }
   /* EasyCASE ) */
   /*Current status are stored*/
   v_PreStatusFlags_u8r=V_StatusFlags_U8R;
   /* EasyCASE - */
   /* return the composite value */
   return v_FiltAccelTrendAxis_s16r;
   }
/* EasyCASE ) */
/* EasyCASE ( 101525
   PEDO_MedianFilter */
/* EasyCASE F */
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

 U32 PEDO_MedianFilter( U32 *p_InputArray_u32r, U8 v_SizeInputArray_u8r)
   {
   /* EasyCASE ( 101526
      Local Variables */
   /* EasyCASE < */
   U8  v_index1_u8r=0, v_index2_u8r=0;
   U32 a_InputArrayCopy_u32r[20]={0};
   U32 v_Temp_u32r=0;
   F32 v_Temp_f32r=0;
   /* EasyCASE > */
   /* EasyCASE ) */
   if ((v_SizeInputArray_u8r > 0) && (v_SizeInputArray_u8r <= 20)/* Check whether input array size is proper or not?? */)
      {
      /* EasyCASE ( 101527
         Do sorting of the input data in ascending order */
      /* Copy the values of input array */
      for (v_index1_u8r=0; v_index1_u8r < v_SizeInputArray_u8r; v_index1_u8r++)
         {
         a_InputArrayCopy_u32r[v_index1_u8r] = *(p_InputArray_u32r + v_index1_u8r);
         }
      /* Do bubble sorting */
      for (v_index1_u8r=0; v_index1_u8r < (v_SizeInputArray_u8r-1); v_index1_u8r++)
         {
         for (v_index2_u8r = (v_index1_u8r+1); v_index2_u8r < v_SizeInputArray_u8r; v_index2_u8r++)
            {
            if (a_InputArrayCopy_u32r[v_index1_u8r] > a_InputArrayCopy_u32r[v_index2_u8r])
               {
               v_Temp_u32r = a_InputArrayCopy_u32r[v_index1_u8r];
               a_InputArrayCopy_u32r[v_index1_u8r] = a_InputArrayCopy_u32r[v_index2_u8r];
               a_InputArrayCopy_u32r[v_index2_u8r] = v_Temp_u32r;
               }
            }
         }
      /* EasyCASE ) */
      /* EasyCASE ( 101528
         Find the Median Value for input data */
      /* Find the median of the input array */
      if ((v_SizeInputArray_u8r%2)==0)
         {
         /* Input Buffer length is even */
          v_Temp_f32r = (F32)( a_InputArrayCopy_u32r[(v_SizeInputArray_u8r/2)-1] + a_InputArrayCopy_u32r[(v_SizeInputArray_u8r/2)] )/2;
         }
      else
         {
         /* Input Buffer length is odd */
         v_Temp_f32r = (F32)a_InputArrayCopy_u32r[(v_SizeInputArray_u8r-1)/2];
         }
      /* EasyCASE ) */
      }
   else
      {
      /* Input Buffer length is not in proper range so return 0 */
      }
   return (U32) v_Temp_f32r;
   }
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ( 100504
   Local functions */
/* EasyCASE ( 101586
   PEDO_GetAbsoluteShort */
/* EasyCASE F */
/*******************************************************************************
 * Description: *//**\brief This function should be called to get the absolute value
 * of the input value of short type. 
 * 
 *
 *
 *
 *  \param
 *  short v_Val_s16r --> Input value for which absolute value needs to be calculated.
 *
 *
 *
 *  \return 
 *   short --> Absolute value of the Input value.
 *            
 *
 ******************************************************************************/
/* Scheduling: 
 *  
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
static short PEDO_GetAbsoluteShort(short v_Val_s16r)
   {
   return (v_Val_s16r < 0)? -v_Val_s16r : v_Val_s16r;
   }
/* EasyCASE ) */
/* EasyCASE ( 101610
   PEDO_TrendFinder */
/* EasyCASE F */
/*******************************************************************************
 * Description: *//**\brief This function should be called to find the current trend
 * (Positive/Negetive) and optimum value of the acceleration at the current trend. 
 *
 *  
 *
 *
 *
 *  \param
 *  short  v_AccelVal_s16r        : Input Acceleration value (in LSBs)
 *  short *v_OptimalAccelVal_s16r : Optimum value of acceleration at the current trend
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
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
static void PEDO_TrendFinder(short v_AccelVal_s16r, short *v_OptimalAccelVal_s16r)
   {
   /* EasyCASE ( 101627
      Local Variables */
   static signed short     v_Prev1AccelVal_s16r = -1;  /* Previous Acceleration value */
   static signed short     v_Prev2AccelVal_s16r = -1;  /* Previous to previous Acceleration value */
   static signed char      v_CurrTrend_s8r = -1;     /* Current trend (default trend in Negetive trend )*/
   /* EasyCASE ) */
   /* EasyCASE ( 101778
      Find Positive/Negetive trends and value at trends */
   /* Clear the Positive nad Negetive trend bit in the Status Flag */
   /* EasyCASE - */
   V_StatusFlags_U8R &=~(M_NegativeTrend_U8X|M_PositiveTrend_U8X); /*Status flag to hold the change in trend */
   if (v_CurrTrend_s8r == 1)
      {
      /* EasyCASE ( 101628
         Current trend is Positive, Check for Negetive trend */
      if ((v_AccelVal_s16r < v_Prev1AccelVal_s16r) && (v_AccelVal_s16r < v_Prev2AccelVal_s16r))
         {
         v_CurrTrend_s8r = -1; /* Set the current trend as negative */
         V_StatusFlags_U8R |= M_NegativeTrend_U8X; /* Set the change in trend as negative */
         
         /* Return the optimal value of acceleration at trend change
          (in this case maximum of prev and prevToPrev acceleration values) */
         *v_OptimalAccelVal_s16r = (v_Prev1AccelVal_s16r > v_Prev2AccelVal_s16r)?
                                    v_Prev1AccelVal_s16r : v_Prev2AccelVal_s16r;
         }
      /* EasyCASE ) */
      }
   else
      {
      /* EasyCASE ( 101629
         Current trend is Negetive, Check for Positive trend */
      if ((v_AccelVal_s16r > v_Prev1AccelVal_s16r) && (v_AccelVal_s16r > v_Prev2AccelVal_s16r))
         {
         v_CurrTrend_s8r = 1;    /* Set the current trend as Positive */
         V_StatusFlags_U8R |= M_PositiveTrend_U8X;  /* Set the change in trend as Positive */
         
         /* Return the optimal value of acceleration at trend change
          (in this case maximum of prev and prevToPrev acceleration values) */
         *v_OptimalAccelVal_s16r = (v_Prev1AccelVal_s16r < v_Prev2AccelVal_s16r)?
                                    v_Prev1AccelVal_s16r : v_Prev2AccelVal_s16r;
         }
      /* EasyCASE ) */
      }
   /* Update the second last composite value */
   v_Prev2AccelVal_s16r = v_Prev1AccelVal_s16r;
   /* Update the last composite values */
   v_Prev1AccelVal_s16r = v_AccelVal_s16r;
   /* Return the Change in trend */
   /* EasyCASE ) */
   }
/* EasyCASE ) */
/* EasyCASE ( 101612
   PEDO_Get_SingleAxisAccelFeatures */
/* EasyCASE F */
/*******************************************************************************
 * Description: *//**\brief This function should be called to get the acceleration pattern 
 * features for a single axis like Trend Type(Positive/Negetive), Value at Trends,
 * Stride Interval, Stride Cadence, Axis Stability (By checking the uniformity of
 * stride pattern) etc.  
 *
 *  
 *
 *
 *
 *  \param
 *   S16 v_AccelValFilt_s16r : Acceleration Value (in LSBs)
 *   ts_SingleAxis *p_AccelerationPatternFeatures_xxr : Structure variable to hold
 *   the single axis pattern features.
 *
 *
 *  \return 
 *   NONE
 *            
 *
 ******************************************************************************/
/* Scheduling: 
 *  Call this function every 40 msec for each individual axis to get the acceletation pattern features.
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
void PEDO_Get_SingleAxisAccelFeatures( S16 v_AccelValFilt_s16r, ts_SingleAxis *p_AccelerationPatternFeatures_xxr)
   {
   /* EasyCASE ( 101636
      Local Variables */
   U8  v_Index_u8r=0;   
   U8  v_TrendChangeFoundFlag_u8r=0;
   U8  v_StrideInterval_u8r=0;
   U8  v_InstStrideUniformity_u8r=0;
   U8 v_UniformityTestPassedFlag_u8r=0;
   U8 v_StrideIntervalTestPassedFlag_u8r=0;
   U8 v_TestCount_u8r=0;
   
   S16 v_InstTrendValue_s16r=0;
   S16 v_StrideMinVal_s16r=0;
   S16 v_StrideMaxVal_s16r=0;
   U16 v_StridePeakToPeakVal_u16r=0;
   U16 v_Temp_u16r=0;
   /* EasyCASE ) */
   /* EasyCASE ( 101662
      Increment V_StrideIntervalCount_U8X counter */
   if (p_AccelerationPatternFeatures_xxr->V_SuccessiveTrendTimeValidityFlag_U8X==1)
      {
      p_AccelerationPatternFeatures_xxr->V_SuccessiveTrendTimeCount_U8X++; /* Increment the count every 40msec */
      }
   else
      {
      /* If there is no trend detected for 2.4 seconds disable V_SuccessiveTrendTimeValidityFlag_U8X flag */
      /* Enable V_SuccessiveTrendTimeValidityFlag_U8X flag when any trend is detected */
      if (p_AccelerationPatternFeatures_xxr->V_SuccessiveTrendTimeCount_U8X >= C_STRIDE_INTERVAL_COUNT_MAXLIMIT_U8R)
         {
         p_AccelerationPatternFeatures_xxr->V_SuccessiveTrendTimeValidityFlag_U8X=0; /* Reset V_StrideIntervalValidityFlag_U8X flag */
         p_AccelerationPatternFeatures_xxr->V_SuccessiveTrendTimeCount_U8X=C_STRIDE_INTERVAL_COUNT_MAXLIMIT_U8R; /* Clear V_StrideIntervalCount_U8X  count */
         }
      }
   /* EasyCASE ) */
   /* EasyCASE ( 101661
      Update the acceleration value FIFO */
   if ((p_AccelerationPatternFeatures_xxr->V_AccelBufDataCount_U8X < C_SINGLE_AXIS_ACCEL_VALUE_BUFSIZE_U8R))
      {
      p_AccelerationPatternFeatures_xxr->A_AccelBuf_S16X[p_AccelerationPatternFeatures_xxr->V_AccelBufDataCount_U8X] = v_AccelValFilt_s16r;
      p_AccelerationPatternFeatures_xxr->V_AccelBufDataCount_U8X ++;
      }
   else
      {
      for (v_Index_u8r=0; v_Index_u8r<(C_SINGLE_AXIS_ACCEL_VALUE_BUFSIZE_U8R-1); v_Index_u8r++)
         {
         p_AccelerationPatternFeatures_xxr->A_AccelBuf_S16X[C_SINGLE_AXIS_ACCEL_VALUE_BUFSIZE_U8R-1-v_Index_u8r]=p_AccelerationPatternFeatures_xxr->A_AccelBuf_S16X[C_SINGLE_AXIS_ACCEL_VALUE_BUFSIZE_U8R-2-v_Index_u8r];
         }
      p_AccelerationPatternFeatures_xxr->A_AccelBuf_S16X[0]=v_AccelValFilt_s16r;
      }
   /* EasyCASE ) */
   /* EasyCASE ( 101660
      Check for trends ( Positive or Negetive Trend ) ?? */
   if (/* Check for Negetive Trend if Positive Trend got detected ?? */
       ( p_AccelerationPatternFeatures_xxr->A_AccelBuf_S16X[0] < p_AccelerationPatternFeatures_xxr->A_AccelBuf_S16X[1] ) &&
       ( p_AccelerationPatternFeatures_xxr->A_AccelBuf_S16X[0] < p_AccelerationPatternFeatures_xxr->A_AccelBuf_S16X[2] ) &&
       (p_AccelerationPatternFeatures_xxr->V_InstTrendType_S8X == 1))
      {
      /* Set the current trend as Negative trend ??  */
      p_AccelerationPatternFeatures_xxr->V_InstTrendType_S8X = -1;
                  
      /* Find the optimal value for the Negetive Trend */
      v_InstTrendValue_s16r = (p_AccelerationPatternFeatures_xxr->A_AccelBuf_S16X[1] > p_AccelerationPatternFeatures_xxr->A_AccelBuf_S16X[2])?
      p_AccelerationPatternFeatures_xxr->A_AccelBuf_S16X[1] : p_AccelerationPatternFeatures_xxr->A_AccelBuf_S16X[2];
      
      /* Set the trend change found flag */                    
      v_TrendChangeFoundFlag_u8r = 1;
      }
   else
      {
      if (/* Check for Positive Trend if Negetive Trend got detected ?? */
          ( p_AccelerationPatternFeatures_xxr->A_AccelBuf_S16X[0] > p_AccelerationPatternFeatures_xxr->A_AccelBuf_S16X[1] ) &&
          ( p_AccelerationPatternFeatures_xxr->A_AccelBuf_S16X[0] > p_AccelerationPatternFeatures_xxr->A_AccelBuf_S16X[2] )&&
          (p_AccelerationPatternFeatures_xxr->V_InstTrendType_S8X == -1))
         {
         /* Set the current trend as Positive trend */
         p_AccelerationPatternFeatures_xxr->V_InstTrendType_S8X = 1;
                           
         /* Find the optimal value for the Positive Trend */
         v_InstTrendValue_s16r =( p_AccelerationPatternFeatures_xxr->A_AccelBuf_S16X[1] < p_AccelerationPatternFeatures_xxr->A_AccelBuf_S16X[2])?
         p_AccelerationPatternFeatures_xxr->A_AccelBuf_S16X[1] : p_AccelerationPatternFeatures_xxr->A_AccelBuf_S16X[2];
         
         /* Set the trend change found flag */                            
         v_TrendChangeFoundFlag_u8r = 1;
         }
      }
   /* EasyCASE ) */
   /* EasyCASE ( 101777
      IF valid trend is found, (1)Check for Valid Stride Patterns; (2)Update the Stride Parameters
      ( Stride Peak-To-Peak Value, Stride Interval, Cadence, Axis_Stability_Flag etc.). */
   /* Check whether a valid trend (Positive or negetive) detected  ?? */
   if (v_TrendChangeFoundFlag_u8r == 1 /* a valid trend got detected */)
      {
      /* EasyCASE ( 101650
         Update the trend values like TYPE, VALUE, SUCCESSIVE_TREND_TIME_COUNT into FIFO */
      if (p_AccelerationPatternFeatures_xxr->V_AccelTrendBufDataCount_U8X < C_SINGLE_AXIS_TREND_BUFSIZE_U8R)
         {
         p_AccelerationPatternFeatures_xxr->A_AccelTrendBuf_S8X[p_AccelerationPatternFeatures_xxr->V_AccelTrendBufDataCount_U8X]       = p_AccelerationPatternFeatures_xxr->V_InstTrendType_S8X;
         p_AccelerationPatternFeatures_xxr->A_AccelTrendValueBuf_S16X[p_AccelerationPatternFeatures_xxr->V_AccelTrendBufDataCount_U8X] = v_InstTrendValue_s16r;
         p_AccelerationPatternFeatures_xxr->A_AccelSuccecciveTrendTimeBuf_U8X[p_AccelerationPatternFeatures_xxr->V_AccelTrendBufDataCount_U8X] = p_AccelerationPatternFeatures_xxr->V_SuccessiveTrendTimeCount_U8X;
         p_AccelerationPatternFeatures_xxr->V_AccelTrendBufDataCount_U8X++;
         }
      else
         {
         for (v_Index_u8r=0; v_Index_u8r<(C_SINGLE_AXIS_TREND_BUFSIZE_U8R-1); v_Index_u8r++)
            {
            p_AccelerationPatternFeatures_xxr->A_AccelTrendBuf_S8X[C_SINGLE_AXIS_TREND_BUFSIZE_U8R-1-v_Index_u8r]       = p_AccelerationPatternFeatures_xxr->A_AccelTrendBuf_S8X[C_SINGLE_AXIS_TREND_BUFSIZE_U8R-2-v_Index_u8r];
             p_AccelerationPatternFeatures_xxr->A_AccelTrendValueBuf_S16X[C_SINGLE_AXIS_TREND_BUFSIZE_U8R-1-v_Index_u8r] = p_AccelerationPatternFeatures_xxr->A_AccelTrendValueBuf_S16X[C_SINGLE_AXIS_TREND_BUFSIZE_U8R-2-v_Index_u8r];
            p_AccelerationPatternFeatures_xxr->A_AccelSuccecciveTrendTimeBuf_U8X[C_SINGLE_AXIS_TREND_BUFSIZE_U8R-1-v_Index_u8r] = p_AccelerationPatternFeatures_xxr->A_AccelSuccecciveTrendTimeBuf_U8X[C_SINGLE_AXIS_TREND_BUFSIZE_U8R-2-v_Index_u8r];
            }
         p_AccelerationPatternFeatures_xxr->A_AccelTrendBuf_S8X[0]       = p_AccelerationPatternFeatures_xxr->V_InstTrendType_S8X;
         p_AccelerationPatternFeatures_xxr->A_AccelTrendValueBuf_S16X[0] = v_InstTrendValue_s16r;
         p_AccelerationPatternFeatures_xxr->A_AccelSuccecciveTrendTimeBuf_U8X[0] = p_AccelerationPatternFeatures_xxr->V_SuccessiveTrendTimeCount_U8X;
         }
      v_TrendChangeFoundFlag_u8r = 0; /* clear v_TrendChangeFoundFlag_u8r flag*/
      p_AccelerationPatternFeatures_xxr->V_SuccessiveTrendTimeCount_U8X = 0; /* Clear Successive Trend Time Counter */
      p_AccelerationPatternFeatures_xxr->V_SuccessiveTrendTimeValidityFlag_U8X=1; /* Enable V_SuccessiveTrendTimeValidityFlag_U8X flag whenever u fing the trend */
      /* EasyCASE ) */
      /* Check for Valid Stride Pattern */
      if (/* Check for Stride Pattern whether it is proper or not i.e. {-1, +1,-1, +1 } or alternate positive and negetive trends ?? */
          (p_AccelerationPatternFeatures_xxr->A_AccelTrendBuf_S8X[0] == -1)&& (p_AccelerationPatternFeatures_xxr->A_AccelTrendBuf_S8X[1] == 1)&&
          (p_AccelerationPatternFeatures_xxr->A_AccelTrendBuf_S8X[2] == -1)&& (p_AccelerationPatternFeatures_xxr->A_AccelTrendBuf_S8X[3] == 1))
         {
         /* EasyCASE ( 101651
            Get the Stride Interval by adding the successive trends time counts */
         for (v_Index_u8r=0; v_Index_u8r<C_SINGLE_AXIS_TREND_BUFSIZE_U8R; v_Index_u8r++)
            {
            v_StrideInterval_u8r += p_AccelerationPatternFeatures_xxr->A_AccelSuccecciveTrendTimeBuf_U8X[v_Index_u8r];
            }
         /* EasyCASE ) */
         /*Check whether any discontinuity in the stride patterns by cheking Stride Interval value??*/
         if (( v_StrideInterval_u8r > 0 ) && ( v_StrideInterval_u8r < C_STRIDE_INTERVAL_COUNT_MAXLIMIT_U8R ))
            {
            /* EasyCASE ( 101659
               Set V_ValidStridePatternOccuredFlag_U8X flag and Clear A_AccelTrendBuf_S8X buffer */
            /* Set V_ValidStridePatternOccuredFlag_U8X If Stride Pattern don't have any discontinuity */
             p_AccelerationPatternFeatures_xxr->V_ValidStridePatternOccuredFlag_U8X=1;
            for (v_Index_u8r=1; v_Index_u8r<C_SINGLE_AXIS_TREND_BUFSIZE_U8R; v_Index_u8r++)
               {
               p_AccelerationPatternFeatures_xxr->A_AccelTrendBuf_S8X[v_Index_u8r] = 0;
               }
            /* EasyCASE ) */
            /* Check the Trend Peaks whether we are having uniform peaks or not?? */
            /* EasyCASE ( 101652
               Find the Stride Min, Max and PeakToPeak Values */
            v_StrideMinVal_s16r = p_AccelerationPatternFeatures_xxr->A_AccelTrendValueBuf_S16X[0];
            v_StrideMaxVal_s16r = p_AccelerationPatternFeatures_xxr->A_AccelTrendValueBuf_S16X[0];
            for (v_Index_u8r=1; v_Index_u8r < C_SINGLE_AXIS_TREND_BUFSIZE_U8R; v_Index_u8r++ /* Find Min-Max */)
               {
               if (p_AccelerationPatternFeatures_xxr->A_AccelTrendValueBuf_S16X[v_Index_u8r]<v_StrideMinVal_s16r)
                  {
                  v_StrideMinVal_s16r = p_AccelerationPatternFeatures_xxr->A_AccelTrendValueBuf_S16X[v_Index_u8r];
                  }
               else
                  {
                  if (p_AccelerationPatternFeatures_xxr->A_AccelTrendValueBuf_S16X[v_Index_u8r]>v_StrideMaxVal_s16r)
                     {
                     v_StrideMaxVal_s16r = p_AccelerationPatternFeatures_xxr->A_AccelTrendValueBuf_S16X[v_Index_u8r];
                     }
                  }
               }
            v_StridePeakToPeakVal_u16r = (v_StrideMaxVal_s16r-v_StrideMinVal_s16r);
            /* EasyCASE ) */
            /* EasyCASE ( 101658
               Check the instantaneous uniformity of strides by checking the ratio of three short peaks with peak-to-peak value of stride */
            if (((p_AccelerationPatternFeatures_xxr->A_AccelTrendValueBuf_S16X[0]-p_AccelerationPatternFeatures_xxr->A_AccelTrendValueBuf_S16X[1])> (C_STRIDE_SHORTPEAK_TO_PP_RATIO_CUTOFF_F32R*v_StridePeakToPeakVal_u16r))&&
                ((p_AccelerationPatternFeatures_xxr->A_AccelTrendValueBuf_S16X[2]-p_AccelerationPatternFeatures_xxr->A_AccelTrendValueBuf_S16X[1]) > (C_STRIDE_SHORTPEAK_TO_PP_RATIO_CUTOFF_F32R*v_StridePeakToPeakVal_u16r))&&
                ((p_AccelerationPatternFeatures_xxr->A_AccelTrendValueBuf_S16X[2]-p_AccelerationPatternFeatures_xxr->A_AccelTrendValueBuf_S16X[3]) > (C_STRIDE_SHORTPEAK_TO_PP_RATIO_CUTOFF_F32R*v_StridePeakToPeakVal_u16r)))
               {
               /*All three short stride peaks pass the minimum ratio test then this stride is uniform */
               v_InstStrideUniformity_u8r = 1;
               }
            else
               {
               v_InstStrideUniformity_u8r = 0;
               }
            /* EasyCASE ) */
            /* EasyCASE ( 101653
               Update the Stride Peak-To-Peak Value, Stride Interval, Stride Interval Diff, Stride Uniformity Test Result FIFOs */
            if (p_AccelerationPatternFeatures_xxr->V_StrideIntervalBufDataCount_U8X < C_STRIDE_INTERVAL_BUFSIZE_U8R)
               {
               p_AccelerationPatternFeatures_xxr->A_StridePeakToPeakValBuf_U16X[p_AccelerationPatternFeatures_xxr->V_StrideIntervalBufDataCount_U8X] = v_StridePeakToPeakVal_u16r;
               p_AccelerationPatternFeatures_xxr->A_StrideIntervalBuf_U8X[p_AccelerationPatternFeatures_xxr->V_StrideIntervalBufDataCount_U8X] = v_StrideInterval_u8r;
               p_AccelerationPatternFeatures_xxr->A_UniformityTestBuf_U8X[p_AccelerationPatternFeatures_xxr->V_StrideIntervalBufDataCount_U8X] = v_InstStrideUniformity_u8r;
               p_AccelerationPatternFeatures_xxr->V_StrideIntervalBufDataCount_U8X++;
               if (p_AccelerationPatternFeatures_xxr->V_StrideIntervalBufDataCount_U8X >1)
                  {
                  p_AccelerationPatternFeatures_xxr->A_StrideIntervalDiffBuf_U8X[p_AccelerationPatternFeatures_xxr->V_StrideIntervalBufDataCount_U8X-2] =                                  
                  (p_AccelerationPatternFeatures_xxr->A_StrideIntervalBuf_U8X[p_AccelerationPatternFeatures_xxr->V_StrideIntervalBufDataCount_U8X-1]
                  -p_AccelerationPatternFeatures_xxr->A_StrideIntervalBuf_U8X[p_AccelerationPatternFeatures_xxr->V_StrideIntervalBufDataCount_U8X-2]);
                  }
               }
            else
               {
               for (v_Index_u8r=0; v_Index_u8r<(C_STRIDE_INTERVAL_BUFSIZE_U8R-1); v_Index_u8r++)
                  {
                  p_AccelerationPatternFeatures_xxr->A_StridePeakToPeakValBuf_U16X[C_STRIDE_INTERVAL_BUFSIZE_U8R-1-v_Index_u8r] = p_AccelerationPatternFeatures_xxr->A_StridePeakToPeakValBuf_U16X[C_STRIDE_INTERVAL_BUFSIZE_U8R-2-v_Index_u8r];
                  p_AccelerationPatternFeatures_xxr->A_StrideIntervalBuf_U8X[C_STRIDE_INTERVAL_BUFSIZE_U8R-1-v_Index_u8r]       = p_AccelerationPatternFeatures_xxr->A_StrideIntervalBuf_U8X[C_STRIDE_INTERVAL_BUFSIZE_U8R-2-v_Index_u8r];
                  p_AccelerationPatternFeatures_xxr->A_StrideIntervalDiffBuf_U8X[C_STRIDE_INTERVAL_BUFSIZE_U8R-1-v_Index_u8r]   = p_AccelerationPatternFeatures_xxr->A_StrideIntervalDiffBuf_U8X[C_STRIDE_INTERVAL_BUFSIZE_U8R-2-v_Index_u8r];
                  p_AccelerationPatternFeatures_xxr->A_UniformityTestBuf_U8X[C_STRIDE_INTERVAL_BUFSIZE_U8R-1-v_Index_u8r]       = p_AccelerationPatternFeatures_xxr->A_UniformityTestBuf_U8X[C_STRIDE_INTERVAL_BUFSIZE_U8R-2-v_Index_u8r];
                  }
               p_AccelerationPatternFeatures_xxr->A_StridePeakToPeakValBuf_U16X[0] = v_StridePeakToPeakVal_u16r;
               p_AccelerationPatternFeatures_xxr->A_StrideIntervalBuf_U8X[0]       = v_StrideInterval_u8r;
               p_AccelerationPatternFeatures_xxr->A_StrideIntervalDiffBuf_U8X[0]   = (v_StrideInterval_u8r-p_AccelerationPatternFeatures_xxr->A_StrideIntervalBuf_U8X[1]);
               p_AccelerationPatternFeatures_xxr->A_UniformityTestBuf_U8X[0]       = v_InstStrideUniformity_u8r;
               }
            /* EasyCASE ) */
            /* EasyCASE ( 101654
               Update the Stable Cadence for this particular axis */
            for (v_Index_u8r=0; v_Index_u8r <(p_AccelerationPatternFeatures_xxr->V_StrideIntervalBufDataCount_U8X); v_Index_u8r++)
               {
               v_Temp_u16r += p_AccelerationPatternFeatures_xxr->A_StrideIntervalBuf_U8X[v_Index_u8r];
               }
            /* CadenceX100 = 100*(2*Strides/sec) = 100*(2*(1/StrideInterval))= 100*(2*(1/(0.040*AvgStrideIntervalCount)) = (5000/AvgStrideInterval) */
            p_AccelerationPatternFeatures_xxr->V_CadenceX100_U16X = (U16)((5000*(p_AccelerationPatternFeatures_xxr->V_StrideIntervalBufDataCount_U8X))/(F32)v_Temp_u16r);
            /* EasyCASE ) */
            /* AXIS STABILITY ??-->To decide this check the history of Uniformity Test and Stride Interval Diff Test*/
            /* EasyCASE ( 101655
               Check the History of Uniformity Test whether it is consistant?? */
            for (v_Index_u8r=0; v_Index_u8r<C_STRIDE_INTERVAL_BUFSIZE_U8R; v_Index_u8r++)
               {
               if (p_AccelerationPatternFeatures_xxr->A_UniformityTestBuf_U8X[v_Index_u8r]==1)
                  {
                  v_TestCount_u8r++;
                  }
               }
            if (v_TestCount_u8r>=C_AXIS_UNIFORMITY_PASSED_NO_CUTOFF_U8R /* Atleast 4 out of 5 tests should be passed */)
               {
               v_UniformityTestPassedFlag_u8r=1;
               }
            else
               {
               v_UniformityTestPassedFlag_u8r=0;
               }
            /* EasyCASE ) */
            /* EasyCASE ( 101656
               Check the History of Stride Interval Difference whether abrupt jumps in StrideInterval or not ?? */
            v_TestCount_u8r=0;
            for (v_Index_u8r=0; v_Index_u8r<C_STRIDE_INTERVAL_BUFSIZE_U8R; v_Index_u8r++)
               {
               if ((p_AccelerationPatternFeatures_xxr->A_StrideIntervalDiffBuf_U8X[v_Index_u8r])<C_STRIDE_INTERVAL_DIFF_MAX_LIMIT_U8R)
                  {
                  v_TestCount_u8r++;
                  }
               }
            if (v_TestCount_u8r>=C_AXIS_STRIDE_INTERVAL_PASSED_NO_CUTOFF_U8R /* Only two fluctuations are allowed in the Stride Interval Difference */)
               {
               v_StrideIntervalTestPassedFlag_u8r=1;
               }
            else
               {
               v_StrideIntervalTestPassedFlag_u8r=0;
               }
            /* EasyCASE ) */
            /* EasyCASE ( 101657
               Check whether Uniformity and Stride Interval both tests got passed ?? */
            if ((v_UniformityTestPassedFlag_u8r==1)&&(v_StrideIntervalTestPassedFlag_u8r==1))
               {
               p_AccelerationPatternFeatures_xxr->V_AxisIsStableFlag_U8X = 1;
               }
            else
               {
               p_AccelerationPatternFeatures_xxr->V_AxisIsStableFlag_U8X = 0;
               }
            /* EasyCASE ) */
            }
         }
      else
         {
         /* else of stride pattern check */
         }
      }
   else
      {
      /* else of IF (v_TrendChangeFoundFlag_u8r == 1) */
      /* Clear V_ValidStridePatternOccuredFlag_U8X flag, if valid stride is not detected */
      p_AccelerationPatternFeatures_xxr->V_ValidStridePatternOccuredFlag_U8X = 0;
      }
   /* EasyCASE ) */
   }
/* EasyCASE ) */
/* EasyCASE ( 101614
   PEDO_Reset_SingleAxisAccelFeatures */
/* EasyCASE F */
/*******************************************************************************
 * Description: *//**\brief This function resets the accel pattern parameters for single axis. 
 *  
 *
 *
 *
 *  \param
 *   ts_SingleAxis *p_AccelerationPatternFeatures_xxr --> Structure variable to hold
 *   the parameters of the single axis acceleration features.
 *               
 *    
 *              
 *  \return 
 *     
 *
 ******************************************************************************/
/* Scheduling:
 *   Call this function once to Initialize the single axis parameters,
 *   and every time when activity is stationary to reset the variables.
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/

void PEDO_Reset_SingleAxisAccelFeatures(ts_SingleAxis *p_AccelerationPatternFeatures_xxr)
   {
   /* EasyCASE ( 101670
      Local Variable */
   U8 v_Index_u8r=0;
   /* EasyCASE ) */
   /* EasyCASE ( 101671
      Reset single axis acceleration features Buffers */
   for (v_Index_u8r=0; v_Index_u8r<C_SINGLE_AXIS_ACCEL_VALUE_BUFSIZE_U8R; v_Index_u8r++)
      {
      p_AccelerationPatternFeatures_xxr->A_AccelBuf_S16X[v_Index_u8r]=0;
      }
   for (v_Index_u8r=0; v_Index_u8r<C_SINGLE_AXIS_TREND_BUFSIZE_U8R; v_Index_u8r++)
      {
      p_AccelerationPatternFeatures_xxr->A_AccelTrendBuf_S8X[v_Index_u8r]=0;
      p_AccelerationPatternFeatures_xxr->A_AccelTrendValueBuf_S16X[v_Index_u8r]=0; 
      p_AccelerationPatternFeatures_xxr->A_AccelSuccecciveTrendTimeBuf_U8X[v_Index_u8r]=0;
      }
   for (v_Index_u8r=0; v_Index_u8r<C_STRIDE_INTERVAL_BUFSIZE_U8R; v_Index_u8r++)
      {
      p_AccelerationPatternFeatures_xxr->A_StridePeakToPeakValBuf_U16X[v_Index_u8r]=0;
      p_AccelerationPatternFeatures_xxr->A_StrideIntervalBuf_U8X[v_Index_u8r]=0;
      p_AccelerationPatternFeatures_xxr->A_StrideIntervalDiffBuf_U8X[v_Index_u8r]=0;
      p_AccelerationPatternFeatures_xxr->A_UniformityTestBuf_U8X[v_Index_u8r]=0;
      }
   /* EasyCASE ) */
   /* EasyCASE ( 101672
      Reset single axis acceleration features variables */
   p_AccelerationPatternFeatures_xxr->V_AccelBufDataCount_U8X=0;
   p_AccelerationPatternFeatures_xxr->V_AccelTrendBufDataCount_U8X=0;
   p_AccelerationPatternFeatures_xxr->V_StrideIntervalBufDataCount_U8X=0;
   p_AccelerationPatternFeatures_xxr->V_SuccessiveTrendTimeCount_U8X=0;
          
   p_AccelerationPatternFeatures_xxr->V_InstTrendType_S8X = -1; /* Set default trend as Negetive Trend */
   p_AccelerationPatternFeatures_xxr->V_ValidStridePatternOccuredFlag_U8X=0;
   p_AccelerationPatternFeatures_xxr->V_SuccessiveTrendTimeValidityFlag_U8X=0;
   p_AccelerationPatternFeatures_xxr->V_AxisIsStableFlag_U8X=0;
   p_AccelerationPatternFeatures_xxr->V_CadenceX100_U16X=0;
   /* EasyCASE ) */
   }
/* EasyCASE ) */
/* EasyCASE ( 101745
   PEDO_FindDominantAxisOrder */
/* EasyCASE F */
/*******************************************************************************
 * Description: *//**\brief This function will be called to get the order of 
 * dominant axis (Ist, IInd and IIIrd) for X, Y and Z-axis.  
 * 
 *
 *
 *
 *  \param
 *  ts_SingleAxis *p_Xaxis_xxr : Structure pointer to hold X-axis Acceleration features
 *  ts_SingleAxis *p_Yaxis_xxr : Structure pointer to hold Y-axis Acceleration features
 *  ts_SingleAxis *p_Zaxis_xxr : Structure pointer to hold Z-axis Acceleration features
 *  U8 v_Activity_u8r          : User activity type
 *  U8 *p_IstDomAxis_u8r       : Ist dominating axis
 *  U8 *p_IIndDomAxis_u8r      : IInd dominating axis
 *  U8 *p_IIIrdDomAxis_u8r     : IIIrd dominating axis
 *
 *
 *
 *  \return 
 *  NONE
 *            
 *
 ******************************************************************************/
/* Scheduling: 
 *  Call this function after updating the features of single axis acceleration by calling
 *  function "PEDO_Get_SingleAxisAccelFeatures". 
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
void PEDO_FindDominantAxisOrder( ts_SingleAxis *p_Xaxis_xxr, ts_SingleAxis *p_Yaxis_xxr, ts_SingleAxis *p_Zaxis_xxr,
                                 U8 v_Activity_u8r, U8 *p_IstDomAxis_u8r, U8 *p_IIndDomAxis_u8r, U8 *p_IIIrdDomAxis_u8r )
   {
   /* EasyCASE ( 101757
      Local Variables */
   /* EasyCASE < */
   U8  v_Index_u8r=0, v_Index2_u8r=0;
   U8  v_AxisBufLength_u8r=3;
   U8  v_TempU8_u8r=0;
   U8  v_XaxisDominantCount_u8r=0;
   U8  v_YaxisDominantCount_u8r=0;
   U8  v_ZaxisDominantCount_u8r=0;
   U8  v_IstDomAxisDecidedFlag_u8r=0;
   
   U8  a_AxisBuf_u8r[3];
   U16 a_AxisValBuf_u16r[3];
   U16 v_Temp_u16r=0;
   
   /* Reset the variable Again when ACTIVITY is STATIONARY */
   static U8 v_DomAxisBufPacketCount_u8r=0;   
   static U8 v_IstDomAxisBuf_u8r[C_DOMINATING_AXIS_BUFSIZE_U8R]  ={0}; 
   static U8 v_IIndDomAxisBuf_u8r[C_DOMINATING_AXIS_BUFSIZE_U8R] ={0};
   /* EasyCASE > */
   /* EasyCASE ) */
   /* EasyCASE ( 101761
      Clear the v_DomAxisBufPacketCount_u8r when ACTIVITY == STATIONARY */
   if (v_Activity_u8r == STATIONARY)
      {
      v_DomAxisBufPacketCount_u8r=0;
      }
   /* EasyCASE ) */
   /* EasyCASE ( 101762
      Assign values to AxisBuf ( 0=X_Axis; 1=Y_Axis; 2=Z_Axis ) and AxisValueBuf ( Stride Peak-to-Peak Value ) */
   a_AxisBuf_u8r[0] = C_Xaxis_U8X; /* X-Axis */
   a_AxisBuf_u8r[1] = C_Yaxis_U8X; /* Y-Axis */
   a_AxisBuf_u8r[2] = C_Zaxis_U8X; /* Z-Axis */
   
   a_AxisValBuf_u16r[0] = p_Xaxis_xxr->A_StridePeakToPeakValBuf_U16X[0]; /* X-Axis Stride Peak-to-Peak Value */
   a_AxisValBuf_u16r[1] = p_Yaxis_xxr->A_StridePeakToPeakValBuf_U16X[0]; /* Y-Axis Stride Peak-to-Peak Value */
   a_AxisValBuf_u16r[2] = p_Zaxis_xxr->A_StridePeakToPeakValBuf_U16X[0]; /* Z-Axis Stride Peak-to-Peak Value */
   /* EasyCASE ) */
   /* EasyCASE ( 101763
      Sort the Stride Peak-To-Peak values in Descending Order */
   for (v_Index_u8r = 0; v_Index_u8r < (v_AxisBufLength_u8r-1); v_Index_u8r++)
      {
      for (v_Index2_u8r = (v_Index_u8r+1); v_Index2_u8r < v_AxisBufLength_u8r; v_Index2_u8r++)
         {
         if (a_AxisValBuf_u16r[v_Index_u8r] < a_AxisValBuf_u16r[v_Index2_u8r])
            {
            /* Swap the Axis */
            v_TempU8_u8r = a_AxisBuf_u8r[v_Index_u8r];
            a_AxisBuf_u8r[v_Index_u8r] = a_AxisBuf_u8r[v_Index2_u8r];
            a_AxisBuf_u8r[v_Index2_u8r] = v_TempU8_u8r; 
            
            /* Swap the Axis Values */
            v_Temp_u16r = a_AxisValBuf_u16r[v_Index_u8r];
            a_AxisValBuf_u16r[v_Index_u8r] = a_AxisValBuf_u16r[v_Index2_u8r];
            a_AxisValBuf_u16r[v_Index2_u8r] = v_Temp_u16r;
            }
         else
            {
            if (a_AxisValBuf_u16r[v_Index_u8r] == a_AxisValBuf_u16r[v_Index2_u8r])
               {
               /* Axis with less index will be given more priority in the descending order */
               if (a_AxisBuf_u8r[v_Index_u8r] > a_AxisBuf_u8r[v_Index2_u8r])
                  {
                  /* Swap the Axis */
                  v_TempU8_u8r = a_AxisBuf_u8r[v_Index_u8r];
                  a_AxisBuf_u8r[v_Index_u8r] = a_AxisBuf_u8r[v_Index2_u8r];
                  a_AxisBuf_u8r[v_Index2_u8r] = v_TempU8_u8r; 
                  
                  /* Swap the Axis Values */
                  v_Temp_u16r = a_AxisValBuf_u16r[v_Index_u8r];
                  a_AxisValBuf_u16r[v_Index_u8r] = a_AxisValBuf_u16r[v_Index2_u8r];
                  a_AxisValBuf_u16r[v_Index2_u8r] = v_Temp_u16r;
                  }
               }
            }
         }
      }
   /* EasyCASE ) */
   if (v_DomAxisBufPacketCount_u8r < C_DOMINATING_AXIS_BUFSIZE_U8R)
      {
      /* EasyCASE ( 101775
         Keep default dominating axis as COMPOSITE AXIS for first 5 Strides,
         untill Ist and IInd dominating axis FIFOs are filled. */
      v_IstDomAxisBuf_u8r[v_DomAxisBufPacketCount_u8r]  = a_AxisBuf_u8r[0];
      v_IIndDomAxisBuf_u8r[v_DomAxisBufPacketCount_u8r] = a_AxisBuf_u8r[1];
      v_DomAxisBufPacketCount_u8r++;
      
      /* Keep default dominating axis as composite axis for Ist 5 Strides untill Ist and IInd dominating axis FIFOs are filled*/ 
      *p_IstDomAxis_u8r = C_CompositeAxis_U8X;
      *p_IIndDomAxis_u8r = C_CompositeAxis_U8X;
      *p_IIIrdDomAxis_u8r = C_CompositeAxis_U8X;
      /* EasyCASE ) */
      }
   else
      {
      /* EasyCASE ( 101776
         Find dominating axis order */
      /* EasyCASE ( 101766
         Update the Ist and IInd dominant axis FIFOs */
      for (v_Index_u8r=0; v_Index_u8r<(C_DOMINATING_AXIS_BUFSIZE_U8R-1); v_Index_u8r++)
         {
         v_IstDomAxisBuf_u8r[C_DOMINATING_AXIS_BUFSIZE_U8R-1-v_Index_u8r]   = v_IstDomAxisBuf_u8r[C_DOMINATING_AXIS_BUFSIZE_U8R-2-v_Index_u8r];
         v_IIndDomAxisBuf_u8r[C_DOMINATING_AXIS_BUFSIZE_U8R-1-v_Index_u8r]  = v_IIndDomAxisBuf_u8r[C_DOMINATING_AXIS_BUFSIZE_U8R-2-v_Index_u8r];
         }
      v_IstDomAxisBuf_u8r[0]  = a_AxisBuf_u8r[0];
      v_IIndDomAxisBuf_u8r[0] = a_AxisBuf_u8r[1];
      /* EasyCASE ) */
      /* Find The Stable Ist, IInd and IIIrd dominating axis ?? */
      /* EasyCASE ( 101767
         Count the no. of Ist dominant occurances for X, Y and Z-axis out of 5?? */
      for (v_Index_u8r = 0; v_Index_u8r < C_DOMINATING_AXIS_BUFSIZE_U8R; v_Index_u8r++)
         {
         if (v_IstDomAxisBuf_u8r[v_Index_u8r]==C_Xaxis_U8X)
            {
            v_XaxisDominantCount_u8r++;
            }
         else
            {
            if (v_IstDomAxisBuf_u8r[v_Index_u8r]==C_Yaxis_U8X)
               {
               v_YaxisDominantCount_u8r++;
               }
            else
               {
               if (v_IstDomAxisBuf_u8r[v_Index_u8r]==C_Zaxis_U8X)
                  {
                  v_ZaxisDominantCount_u8r++;
                  }
               }
            }
         }
      /* EasyCASE ) */
      /* EasyCASE ( 101769
         Decide Ist Dominating Axis ?? */
      if (v_XaxisDominantCount_u8r>=C_NO_DOMINANT_AXIS_OCC_MIN_LIMIT)
         {
         *p_IstDomAxis_u8r = C_Xaxis_U8X; 
          v_IstDomAxisDecidedFlag_u8r=1;
         }
      else
         {
         if (v_YaxisDominantCount_u8r>=C_NO_DOMINANT_AXIS_OCC_MIN_LIMIT)
            {
            *p_IstDomAxis_u8r = C_Yaxis_U8X; 
             v_IstDomAxisDecidedFlag_u8r=1;
            }
         else
            {
            if (v_ZaxisDominantCount_u8r>=C_NO_DOMINANT_AXIS_OCC_MIN_LIMIT)
               {
               *p_IstDomAxis_u8r = C_Zaxis_U8X; 
                v_IstDomAxisDecidedFlag_u8r=1;
               }
            }
         }
      /* EasyCASE ) */
      if (v_IstDomAxisDecidedFlag_u8r ==1)
         {
         /* EasyCASE ( 101771
            Count the no of IInd dominant occurances for X, Y and Z-axis out of 5?? */
         for (v_Index_u8r = 0; v_Index_u8r < C_DOMINATING_AXIS_BUFSIZE_U8R; v_Index_u8r++)
            {
            if (v_IIndDomAxisBuf_u8r[v_Index_u8r]==C_Xaxis_U8X)
               {
               v_XaxisDominantCount_u8r++;
               }
            else
               {
               if (v_IIndDomAxisBuf_u8r[v_Index_u8r]==C_Yaxis_U8X)
                  {
                  v_YaxisDominantCount_u8r++;
                  }
               else
                  {
                  if (v_IIndDomAxisBuf_u8r[v_Index_u8r]==C_Zaxis_U8X)
                     {
                     v_ZaxisDominantCount_u8r++;
                     }
                  }
               }
            }
         /* EasyCASE ) */
         /* Decide IInd and IIrd Dominating Axis ?? */
         if (*p_IstDomAxis_u8r == C_Xaxis_U8X)
            {
            /* EasyCASE ( 101772
               Decide IInd and IIIrd dominating axis among Y and Z axis ? */
            if (v_YaxisDominantCount_u8r >= v_ZaxisDominantCount_u8r)
               {
               *p_IIndDomAxis_u8r  = C_Yaxis_U8X;
               *p_IIIrdDomAxis_u8r = C_Zaxis_U8X;
               }
            else
               {
               *p_IIndDomAxis_u8r  = C_Zaxis_U8X;
               *p_IIIrdDomAxis_u8r = C_Yaxis_U8X;
               }
            /* EasyCASE ) */
            }
         else
            {
            if (*p_IstDomAxis_u8r == C_Yaxis_U8X)
               {
               /* EasyCASE ( 101773
                  Decide IInd and IIIrd dominating axis among X and Y axis ? */
               if (v_XaxisDominantCount_u8r >= v_ZaxisDominantCount_u8r)
                  {
                  *p_IIndDomAxis_u8r  = C_Xaxis_U8X;
                  *p_IIIrdDomAxis_u8r = C_Zaxis_U8X;
                  }
               else
                  {
                  *p_IIndDomAxis_u8r  = C_Zaxis_U8X;
                  *p_IIIrdDomAxis_u8r = C_Xaxis_U8X;
                  }
               /* EasyCASE ) */
               }
            else
               {
               if (*p_IstDomAxis_u8r == C_Zaxis_U8X)
                  {
                  /* EasyCASE ( 101774
                     Decide IInd and IIIrd dominating axis among X and Y axis ? */
                  if (v_XaxisDominantCount_u8r >= v_YaxisDominantCount_u8r)
                     {
                     *p_IIndDomAxis_u8r  = C_Xaxis_U8X;
                     *p_IIIrdDomAxis_u8r = C_Yaxis_U8X;
                     }
                  else
                     {
                     *p_IIndDomAxis_u8r  = C_Yaxis_U8X;
                     *p_IIIrdDomAxis_u8r = C_Xaxis_U8X;
                     }
                  /* EasyCASE ) */
                  }
               }
            }
         }
      /* EasyCASE ) */
      }
   }
/* EasyCASE ) */
/* EasyCASE ( 101747
   PEDO_Find_StableSingleAxis_StableCadenceX100 */
/* EasyCASE F */
/*******************************************************************************
 * Description: *//**\brief This function will be called to find the single stable axis
 *                          and stable cadence of that axis. 
 *  
 *
 *
 *
 *  \param
 *   ts_SingleAxis *p_Xaxis_xxr    : Structure pointer to hold X-axis Acceleration features
 *   ts_SingleAxis *p_Yaxis_xxr    : Structure pointer to hold Y-axis Acceleration features
 *   ts_SingleAxis *p_Zaxis_xxr    : Structure pointer to hold Z-axis Acceleration features
 *   ts_SingleAxis *p_Caxis_xxr    : Structure pointer to hold Composite-axis Acceleration
 *                                   features
 *   U8 v_IstDomAxis_u8r           : Ist dominant axis
 *   U8 v_IIndDomAxis_u8r          : IInd dominant axis
 *   U8 v_Activity_u8r             : User activity type
 *   U8  *p_StableAxis_u8r         : Stable axis ( X, Y, Z or Composite axis )
 *   U16 *p_StableCadenceX100_u16r : Stable Cadence of stable axis
 *              
 *     
 *               
 *  \return 
 *   NONE
 *  
 *
 ******************************************************************************/
/* Scheduling:
 * Call the function every 40 msec cycle
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/

 void PEDO_Find_StableSingleAxis_StableCadenceX100( ts_SingleAxis *p_Xaxis_xxr, ts_SingleAxis *p_Yaxis_xxr, ts_SingleAxis *p_Zaxis_xxr, ts_SingleAxis *p_Caxis_xxr,
                                                    U8 v_IstDomAxis_u8r, U8 v_IIndDomAxis_u8r, U8 v_Activity_u8r, U8 *p_StableAxis_u8r, U16 *p_StableCadenceX100_u16r )
   {
   /* EasyCASE ( 101748
      Local Variables */
   //   U8 v_Index_u8r=0;
      static U8 v_LocalStableAxis_u8r = C_CompositeAxis_U8X;
      static U16 v_LocalStableCadenceX100_u16r=0;
   //   U16 v_Temp_u16r=0;
   /* EasyCASE ) */
   /* EasyCASE ( 101749
      Check whether activity is STATIONARY ? 
      IF-->YES, (1) Set Stable axis to Composite axis as default
                (2) Clear stable axis cadence */
   if (v_Activity_u8r==STATIONARY)
      {
      v_LocalStableAxis_u8r = C_CompositeAxis_U8X;
      v_LocalStableCadenceX100_u16r = 0;
      }
   /* EasyCASE ) */
   /* EasyCASE ( 101750
      Find Stable Axis */
   /* CASE1 : CURRENT STABLE AXIS = COMPOSITE AXIS && STATUS == UNSTABLE */
   if ((v_LocalStableAxis_u8r == C_CompositeAxis_U8X)&&(p_Caxis_xxr->V_AxisIsStableFlag_U8X == UNSTABLE))
      {
      /* EasyCASE ( 101751
         Check whether COMPOSITE_AXIS == UNSTABLE ? */
      /* Check Ist dominating axis stability */
      if ((v_IstDomAxis_u8r == C_Xaxis_U8X)&&(p_Xaxis_xxr->V_AxisIsStableFlag_U8X == STABLE))
         {
         v_LocalStableAxis_u8r = C_Xaxis_U8X;
         }
      else
         {
         if ((v_IstDomAxis_u8r == C_Yaxis_U8X)&&(p_Yaxis_xxr->V_AxisIsStableFlag_U8X == STABLE))
            {
            v_LocalStableAxis_u8r = C_Yaxis_U8X;
            }
         else
            {
            if ((v_IstDomAxis_u8r == C_Zaxis_U8X)&&(p_Zaxis_xxr->V_AxisIsStableFlag_U8X == STABLE))
               {
               v_LocalStableAxis_u8r = C_Zaxis_U8X;
               }
            else
               {
               /* Check IInd dominating axis stability */
               if ((v_IIndDomAxis_u8r == C_Xaxis_U8X)&&(p_Xaxis_xxr->V_AxisIsStableFlag_U8X == STABLE))
                  {
                  v_LocalStableAxis_u8r = C_Xaxis_U8X;
                  }
               else
                  {
                  if ((v_IIndDomAxis_u8r == C_Yaxis_U8X)&&(p_Yaxis_xxr->V_AxisIsStableFlag_U8X == STABLE))
                     {
                     v_LocalStableAxis_u8r = C_Yaxis_U8X;
                     }
                  else
                     {
                     if ((v_IIndDomAxis_u8r == C_Zaxis_U8X)&&(p_Zaxis_xxr->V_AxisIsStableFlag_U8X == STABLE))
                        {
                        v_LocalStableAxis_u8r = C_Zaxis_U8X;
                        }
                     else
                        {
                        /* Keep it Composite axis only */
                        /* EasyCASE - */
                        v_LocalStableAxis_u8r = C_CompositeAxis_U8X;
                        }
                     }
                  }
               }
            }
         }
      /* EasyCASE ) */
      }
   else
      {
      /* CASE2 : CURRENT STABLE AXIS = X-AXIS && STATUS == UNSTABLE */
      if ((v_LocalStableAxis_u8r == C_Xaxis_U8X)&&(p_Xaxis_xxr->V_AxisIsStableFlag_U8X == UNSTABLE))
         {
         /* EasyCASE ( 101752
            Check whether X_AXIS == UNSTABLE ? */
         /* Check Composite Axis stability */
         if (p_Caxis_xxr->V_AxisIsStableFlag_U8X == STABLE)
            {
            v_LocalStableAxis_u8r = C_CompositeAxis_U8X;
            }
         else
            {
            /* Check Ist dominant axis stability  */
            if ((v_IstDomAxis_u8r == C_Yaxis_U8X)&&(p_Yaxis_xxr->V_AxisIsStableFlag_U8X == STABLE))
               {
               v_LocalStableAxis_u8r = C_Yaxis_U8X;
               }
            else
               {
               if ((v_IstDomAxis_u8r == C_Zaxis_U8X)&&(p_Zaxis_xxr->V_AxisIsStableFlag_U8X == STABLE))
                  {
                  v_LocalStableAxis_u8r = C_Zaxis_U8X;
                  }
               else
                  {
                  /* Check IInd dominating axis stability */
                  if ((v_IIndDomAxis_u8r == C_Yaxis_U8X)&&(p_Yaxis_xxr->V_AxisIsStableFlag_U8X == STABLE))
                     {
                     v_LocalStableAxis_u8r = C_Yaxis_U8X;
                     }
                  else
                     {
                     if ((v_IIndDomAxis_u8r == C_Zaxis_U8X)&&(p_Zaxis_xxr->V_AxisIsStableFlag_U8X == STABLE))
                        {
                        v_LocalStableAxis_u8r = C_Zaxis_U8X;
                        }
                     else
                        {
                        /* Keep it Composite axis only */
                        /* EasyCASE - */
                        v_LocalStableAxis_u8r = C_CompositeAxis_U8X;
                        }
                     }
                  }
               }
            }
         /* EasyCASE ) */
         }
      else
         {
         /* CASE3 : CURRENT STABLE AXIS = Y-AXIS && STATUS == UNSTABLE */
         if ((v_LocalStableAxis_u8r == C_Yaxis_U8X)&&(p_Yaxis_xxr->V_AxisIsStableFlag_U8X == UNSTABLE))
            {
            /* EasyCASE ( 101753
               Check whether Y_AXIS == UNSTABLE ? */
            /* Check Composite Axis stability */
            if (p_Caxis_xxr->V_AxisIsStableFlag_U8X == STABLE)
               {
               v_LocalStableAxis_u8r = C_CompositeAxis_U8X;
               }
            else
               {
               /* Check Ist dominant axis stability  */
               if ((v_IstDomAxis_u8r == C_Xaxis_U8X)&&(p_Xaxis_xxr->V_AxisIsStableFlag_U8X == STABLE))
                  {
                  v_LocalStableAxis_u8r = C_Xaxis_U8X;
                  }
               else
                  {
                  if ((v_IstDomAxis_u8r == C_Zaxis_U8X)&&(p_Zaxis_xxr->V_AxisIsStableFlag_U8X == STABLE))
                     {
                     v_LocalStableAxis_u8r = C_Zaxis_U8X;
                     }
                  else
                     {
                     /* Check IInd dominating axis stability */
                     if ((v_IIndDomAxis_u8r == C_Xaxis_U8X)&&(p_Xaxis_xxr->V_AxisIsStableFlag_U8X == STABLE))
                        {
                        v_LocalStableAxis_u8r = C_Xaxis_U8X;
                        }
                     else
                        {
                        if ((v_IIndDomAxis_u8r == C_Zaxis_U8X)&&(p_Zaxis_xxr->V_AxisIsStableFlag_U8X == STABLE))
                           {
                           v_LocalStableAxis_u8r = C_Zaxis_U8X;
                           }
                        else
                           {
                           /* Keep it Composite axis only */
                           /* EasyCASE - */
                           v_LocalStableAxis_u8r = C_CompositeAxis_U8X;
                           }
                        }
                     }
                  }
               }
            /* EasyCASE ) */
            }
         else
            {
            /* CASE4 : CURRENT STABLE AXIS = Z-AXIS && STATUS == UNSTABLE */
            if ((v_LocalStableAxis_u8r == C_Zaxis_U8X)&&(p_Zaxis_xxr->V_AxisIsStableFlag_U8X == UNSTABLE))
               {
               /* EasyCASE ( 101754
                  Check whether Z_AXIS == UNSTABLE ? */
               /* Check Composite Axis stability */
               if (p_Caxis_xxr->V_AxisIsStableFlag_U8X == STABLE)
                  {
                  v_LocalStableAxis_u8r = C_CompositeAxis_U8X;
                  }
               else
                  {
                  /* Check Ist dominant axis stability  */
                  if ((v_IstDomAxis_u8r == C_Xaxis_U8X)&&(p_Xaxis_xxr->V_AxisIsStableFlag_U8X == STABLE))
                     {
                     v_LocalStableAxis_u8r = C_Xaxis_U8X;
                     }
                  else
                     {
                     if ((v_IstDomAxis_u8r == C_Yaxis_U8X)&&(p_Yaxis_xxr->V_AxisIsStableFlag_U8X == STABLE))
                        {
                        v_LocalStableAxis_u8r = C_Yaxis_U8X;
                        }
                     else
                        {
                        /* Check IInd dominating axis stability */
                        if ((v_IIndDomAxis_u8r == C_Xaxis_U8X)&&(p_Xaxis_xxr->V_AxisIsStableFlag_U8X == STABLE))
                           {
                           v_LocalStableAxis_u8r = C_Xaxis_U8X;
                           }
                        else
                           {
                           if ((v_IIndDomAxis_u8r == C_Yaxis_U8X)&&(p_Yaxis_xxr->V_AxisIsStableFlag_U8X == STABLE))
                              {
                              v_LocalStableAxis_u8r = C_Yaxis_U8X;
                              }
                           else
                              {
                              /* Keep it Composite axis only */
                              /* EasyCASE - */
                              v_LocalStableAxis_u8r = C_CompositeAxis_U8X;
                              }
                           }
                        }
                     }
                  }
               /* EasyCASE ) */
               }
            }
         }
      }
   /* Copy the local stable axis */
      *p_StableAxis_u8r = v_LocalStableAxis_u8r;
   /* EasyCASE ) */
   /* EasyCASE ( 101755
      Find the Stable Cadence */
   switch (v_LocalStableAxis_u8r)
      {
      case C_Xaxis_U8X:
         *p_StableCadenceX100_u16r = p_Xaxis_xxr->V_CadenceX100_U16X;
         break;
      case C_Yaxis_U8X:
         *p_StableCadenceX100_u16r = p_Yaxis_xxr->V_CadenceX100_U16X;
         break;
      case C_Zaxis_U8X:
         *p_StableCadenceX100_u16r = p_Zaxis_xxr->V_CadenceX100_U16X;
         break;
      default:
         *p_StableCadenceX100_u16r = p_Caxis_xxr->V_CadenceX100_U16X;
         break;
      }
   /* EasyCASE ) */
   }
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ) */
