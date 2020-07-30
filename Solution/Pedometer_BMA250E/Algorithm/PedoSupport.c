/** \mainpage Pedometer support API
* 
*       \section intro_sec Introduction:  
*		Support for Pedometer algorithm to find the step count
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
#include "PedoSupport.h"
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ( 63
   Local Declarations and Definitions */
/* EasyCASE ( 322
   Protected Data Interfaces (PDIs) */
/* EasyCASE ) */
/* EasyCASE ( 251
   #Define Constants */
/* EasyCASE ) */
/* EasyCASE ( 222
   ENUM Definitions */
/* EasyCASE ) */
/* EasyCASE ( 217
   Variable Definitions */
U8 A_ActivityInternalBuffer_U8R[INTERNAL_FIFO_MAX_SIZE];         /**< Internal Buffer to hold Instantaneous Activity */
U8 A_Peak2PeakCountInternalBuffer_U8R[INTERNAL_FIFO_MAX_SIZE];   /**< Internal Buffer to hold Peak-To-Peak Time Counts */
U8 A_StepToStepCountInternalBuffer_U8R[INTERNAL_FIFO_MAX_SIZE];  /**< Internal Buffer to hold Step-To-Step Time Counts */

U8 A_ActivityExternalBuffer_U8R[EXTERNAL_FIFO_MAX_SIZE];         /**< External Buffer to hold Instantaneous Activity */
U8 A_Peak2PeakCountExternalBuffer_U8R[EXTERNAL_FIFO_MAX_SIZE];   /**< External Buffer to hold Peak-To-Peak Time Counts */
U8 A_StepToStepCountExternalBuffer_U8R[EXTERNAL_FIFO_MAX_SIZE];  /**< External Buffer to hold Step-To-Step Time Counts */

U8 V_InternalFIFOSize_U8R = 0;  /**<  FIFO size for Internal Buffer */
U8 V_ExternalFIFOSize_U8R = 0;  /**<  FIFO size for External Buffer */
/* EasyCASE ) */
/* EasyCASE ( 223
   Constant- and Table-Definitions in ROM/EEPROM */
/* EasyCASE ) */
/* EasyCASE ( 252
   Functions Prototype Declaration */
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ( 70
   API-Interface functions */
/* EasyCASE ( 100324
   ShiftFIFO_U8 */
/* EasyCASE F */
/*******************************************************************************
 * Description: *//**\brief This API should be called to shift the FIFO values
 * by desired no of shift counts.
 *                          
 *
 *
 *
 * \param
 *  U8 *p_InputBuffer_u8r ---> Input Buffer on which shifting has to be done.
 *  U8  v_FifoSize_u8r --> Length of the Input FIFO buffer.
 *  U8  v_ShiftCount_u8r --> Shift count by which data into input buffer has to be shifted.
 *
 * \return 
 *  NA  
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
void ShiftFIFO_U8(U8 *p_InputBuffer_u8r, U8 v_FifoSize_u8r, U8 v_ShiftCount_u8r)
   {
   /* EasyCASE ( 101013
      Local Variables */
   U8 v_Index_u8r = 0;
   /* EasyCASE ) */
   /* EasyCASE ( 101014
      Shift FIFO values */
   for (v_Index_u8r = 0; v_Index_u8r < (v_FifoSize_u8r-v_ShiftCount_u8r); v_Index_u8r++)
      {
      p_InputBuffer_u8r[v_FifoSize_u8r-v_Index_u8r-1] = p_InputBuffer_u8r[v_FifoSize_u8r-v_Index_u8r-1-v_ShiftCount_u8r];
      }
   /* EasyCASE ) */
   }
/* EasyCASE ) */
/* EasyCASE ( 100415
   SetInternalBuffer */
/* EasyCASE F */
/*******************************************************************************
 * Description: *//**\brief This API should be called to set the values into 
 * respective internal buffers.
 *  
 *
 *
 *
 *  \param
 *   U8 v_StepToStepCount_u8r --> Current Step-To-Step time counts.
 *   U8 v_Peak2PeakCount_u8r --> Current Peak-To-Peak time counts 
 *   U8 v_Activity_u8r --> Instantaneous activity.
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
void SetInternalBuffer(U8 v_StepToStepCount_u8r, U8 v_Peak2PeakCount_u8r, U8 v_Activity_u8r)
   {
   /* EasyCASE ( 101016
      Push the current Step-To-Step, Peak-To-Peak time counts and instantaneous activity values into internal FIFOs. */
   ShiftFIFO_U8( A_StepToStepCountInternalBuffer_U8R,INTERNAL_FIFO_MAX_SIZE,1 );
   ShiftFIFO_U8( A_Peak2PeakCountInternalBuffer_U8R,INTERNAL_FIFO_MAX_SIZE,1 );
   ShiftFIFO_U8( A_ActivityInternalBuffer_U8R,INTERNAL_FIFO_MAX_SIZE,1 );
   
   A_StepToStepCountInternalBuffer_U8R[FIFO_START_INDEX] = v_StepToStepCount_u8r;
   A_Peak2PeakCountInternalBuffer_U8R[FIFO_START_INDEX] = v_Peak2PeakCount_u8r;
   A_ActivityInternalBuffer_U8R[FIFO_START_INDEX] = v_Activity_u8r;
   /* EasyCASE ) */
   /* EasyCASE ( 101015
      Increment the Internal FIFO data size counter */
   if (V_InternalFIFOSize_U8R < INTERNAL_FIFO_MAX_SIZE)
      {
      V_InternalFIFOSize_U8R++;
      }
   /* EasyCASE ) */
   }
/* EasyCASE ) */
/* EasyCASE ( 100852
   GetInternalBuffer */
/* EasyCASE F */
/*******************************************************************************
 * Description: *//**\brief This API should be called to get the values from 
 * respective internal buffers.
 *  
 *
 *
 *
 *  \param
 *   U8 *p_StepToStepCount_u8r --> Pointer to Step-To-Step time count buffer.
 *   U8 *p_Peak2PeakCount_u8r --> Pointer to Peak-To-Peak time count buffer. 
 *   U8 *p_Activity_u8r --> Pointer to Instantaneous activity buffer.
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
void GetInternalBuffer(U8 *p_StepToStepCount_u8r, U8 *p_Peak2PeakCount_u8r, U8 *p_Activity_u8r)
   {
   /* EasyCASE ( 101017
      Local Variables */
   U8 v_Index_u8r;
   /* EasyCASE ) */
   /* EasyCASE ( 101018
      Copy the values from internal buffer to the external buffer */
   for (v_Index_u8r = 0; v_Index_u8r < V_InternalFIFOSize_U8R; v_Index_u8r++)
      {
      *(p_StepToStepCount_u8r + v_Index_u8r) = A_StepToStepCountInternalBuffer_U8R[v_Index_u8r];
      *(p_Peak2PeakCount_u8r + v_Index_u8r) = A_Peak2PeakCountInternalBuffer_U8R[v_Index_u8r];
      *(p_Activity_u8r + v_Index_u8r) = A_ActivityInternalBuffer_U8R[v_Index_u8r];
      }
   /* EasyCASE ) */
   /* EasyCASE ( 101019
      Reset the internal buffer data size counter variable */
   ResetInternalFIFO();
   /* EasyCASE ) */
   }
/* EasyCASE ) */
/* EasyCASE ( 100804
   ResetInternalFIFO */
/* EasyCASE F */
/*******************************************************************************
 * Description: *//**\brief This API should be called to reset the internal FIFO
 * data size counter.
 *                         
 *  
 *
 *
 *
 *  \param
 *   NONE   
 *
 *  \return 
 *   NONE  
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
 void ResetInternalFIFO(void)
   {
   /* EasyCASE ( 101020
      Clear the Internal FIFO data size counter */
   V_InternalFIFOSize_U8R = 0;
   /* EasyCASE ) */
   }
/* EasyCASE ) */
/* EasyCASE ( 100817
   GetInternalFIFOSize */
/* EasyCASE F */
/*******************************************************************************
 * Description: *//**\brief This API should be called to get the size of the
 * Internal FIFO buffer data.
 *                         
 *  
 *
 *
 *
 *  \param
 *   NONE
 *
 *  \return 
 *   U8 --> Data Size of the internal FIFO buffer.  
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
 U8 GetInternalFIFOSize(void)
   {
   /* EasyCASE ( 101021
      Return Internal FIFO size */
   return V_InternalFIFOSize_U8R;
   /* EasyCASE ) */
   }
/* EasyCASE ) */
/* EasyCASE ( 101003
   SetExternalBuffer */
/* EasyCASE F */
/*******************************************************************************
 * Description: *//**\brief This API should be called to set the values into 
 * respective external buffers.
 *  
 *
 *
 *  \param
 *   U8 *p_StepToStepCount_u8r --> Current Step-To-Step time counts.
 *   U8 *p_Peak2PeakCount_u8r --> Current Peak-To-Peak time counts 
 *   U8 *p_Activity_u8r --> Instantaneous activity.
 *   U8  v_BufferLength_u8r --> No of data which has to be pushed into external buffer.
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
void SetExternalBuffer(U8 *p_StepToStepCount_u8r, U8 *p_Peak2PeakCount_u8r, U8 *p_Activity_u8r, U8 v_BufferLength_u8r)
   {
   /* EasyCASE ( 101022
      Local Variables */
   U8 v_Index_u8r = 0;
   /* EasyCASE ) */
   /* EasyCASE ( 101023
      Shift the Step-To-Step, Peak-To-Peak time counts and instantaneous activity values FIFOs */
   ShiftFIFO_U8(A_StepToStepCountExternalBuffer_U8R,EXTERNAL_FIFO_MAX_SIZE,v_BufferLength_u8r );
   ShiftFIFO_U8(A_Peak2PeakCountExternalBuffer_U8R,EXTERNAL_FIFO_MAX_SIZE,v_BufferLength_u8r );
   ShiftFIFO_U8(A_ActivityExternalBuffer_U8R,EXTERNAL_FIFO_MAX_SIZE,v_BufferLength_u8r );
   /* EasyCASE ) */
   /* EasyCASE ( 101024
      Copy the new values into External FIFOs */
   for (v_Index_u8r = 0; v_Index_u8r < v_BufferLength_u8r; v_Index_u8r++)
      {
      A_StepToStepCountExternalBuffer_U8R[v_Index_u8r] = *(p_StepToStepCount_u8r+v_Index_u8r);
      A_Peak2PeakCountExternalBuffer_U8R[v_Index_u8r] = *(p_Peak2PeakCount_u8r+v_Index_u8r);
      A_ActivityExternalBuffer_U8R[v_Index_u8r] = *(p_Activity_u8r+v_Index_u8r);
      /* EasyCASE - */
      /* Increment the External FIFO data size counter  */
      if (V_ExternalFIFOSize_U8R < EXTERNAL_FIFO_MAX_SIZE)
         {
         V_ExternalFIFOSize_U8R++;
         }
      }
   /* EasyCASE ) */
   }
/* EasyCASE ) */
/* EasyCASE ( 101004
   GetExternalBuffer */
/* EasyCASE F */
/*******************************************************************************
 * Description: *//**\brief This API should be called to get the values from 
 * respective external buffers.
 *  
 *
 *
 *
 *  \param
 *   U8 *p_StepToStepCount_u8r --> Pointer to Step-To-Step time count buffer.
 *   U8 *p_Peak2PeakCount_u8r --> Pointer to Peak-To-Peak time count buffer. 
 *   U8 *p_Activity_u8r --> Pointer to Instantaneous activity buffer.
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
 void GetExternalBuffer(U8 *p_StepToStepCount_u8r, U8 *p_Peak2PeakCount_u8r, U8 *p_Activity_u8r)
   {
   /* EasyCASE ( 101025
      Local variables */
   U8 v_Index_u8r;
   /* EasyCASE ) */
   /* EasyCASE ( 101026
      Copy the values from external buffer */
   for (v_Index_u8r = 0; v_Index_u8r < V_ExternalFIFOSize_U8R; v_Index_u8r++)
      {
      *(p_StepToStepCount_u8r + v_Index_u8r) = A_StepToStepCountExternalBuffer_U8R[v_Index_u8r];
      *(p_Activity_u8r + v_Index_u8r) = A_ActivityExternalBuffer_U8R[v_Index_u8r];          
      *(p_Peak2PeakCount_u8r + v_Index_u8r) = A_Peak2PeakCountExternalBuffer_U8R[v_Index_u8r];
      }
   /* EasyCASE ) */
   /* EasyCASE ( 101027
      Reset the external buffer data size counter variable */
   ResetExternalFIFO();
   /* EasyCASE ) */
   }
/* EasyCASE ) */
/* EasyCASE ( 101005
   ResetExternalFIFO */
/* EasyCASE F */
/*******************************************************************************
 * Description: *//**\brief This API should be called to reset the external FIFO
 * data size counter.
 *                         
 *  
 *
 *
 *
 *  \param
 *   NONE   
 *
 *  \return 
 *   NONE  
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
 void ResetExternalFIFO(void)
   {
   /* EasyCASE ( 101028
      Clear the External FIFO data size counter */
   V_ExternalFIFOSize_U8R = 0;
   /* EasyCASE ) */
   }
/* EasyCASE ) */
/* EasyCASE ( 101006
   GetExternalFIFOSize */
/* EasyCASE F */
/*******************************************************************************
 * Description: *//**\brief This API should be called to get the size of the
 * External FIFO buffer data.
 *                         
 *  
 *
 *
 *
 *  \param
 *   NONE
 *
 *  \return 
 *   U8 --> Data Size of the external FIFO buffer.  
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
 U8 GetExternalFIFOSize(void)
   {
   /* EasyCASE ( 101029
      Return external buffer size */
   return V_ExternalFIFOSize_U8R;
   /* EasyCASE ) */
   }
/* EasyCASE ) */
/* EasyCASE ( 100991
   ProcessLPF */
/* EasyCASE F */
/*******************************************************************************
 * Description: *//**\brief This API should be called to process the LOW PASS FILTER Data.
 *                         
 *  
 *
 *
 *
 *  \param
 *   ts_LPF *p_ObjLPF --> Structure pointer to low pass filter object.
 *   
 *
 *  \return 
 *   NONE 
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
 void ProcessLPF(ts_LPF *p_ObjLPF)
   {
   /* EasyCASE ( 101030
      Calculate LPF value by using current sample value */
   p_ObjLPF->V_FilterValue_F32R =  (float)p_ObjLPF->V_CurSample_U16R * p_ObjLPF->V_ALPHA_F32R + (float)p_ObjLPF->V_FilterValue_F32R * (1 - p_ObjLPF->V_ALPHA_F32R);
   /* EasyCASE ) */
   }
/* EasyCASE ) */
/* EasyCASE ( 100996
   GetSmoothValue */
/* EasyCASE F */
/*******************************************************************************
 * Description: *//**\brief This API should be called to smoothen the data of
 * the input buffer by using Moving Average Method.
 *                         
 *  
 *
 *
 *
 *  \param
 *   ts_SmoothFilter *p_SmoothFilter_xxr --> Smooth Filter Variable
 *   
 *
 *  \return 
 *   NONE  
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
 void GetSmoothValue(ts_SmoothFilter *p_SmoothFilter_xxr)
   {
   /* EasyCASE ( 101031
      Local Variables */
   /* Local Variable */
      U8 v_Index_u8r = 0;
      U32 v_TempSum_u32r = 0;
   /* EasyCASE ) */
   /* EasyCASE ( 101032
      Copy the new sample values into Smooth Buffer */
   p_SmoothFilter_xxr->A_SmoothBuffer_U16R[p_SmoothFilter_xxr->V_FilterIndex_U8R] = p_SmoothFilter_xxr->V_NewSample_U16R;
   /* EasyCASE ) */
   /* EasyCASE ( 101033
      Check whether Filter Index has crossed the Filter Length or not ?
      IF-->YES, reset the Filter Index. */
   if (p_SmoothFilter_xxr->V_FilterIndex_U8R >= (p_SmoothFilter_xxr->V_FilterLength_U8R-1))
      {
      p_SmoothFilter_xxr->V_FilterIndex_U8R = 0;
      }
   else
      {
      p_SmoothFilter_xxr->V_FilterIndex_U8R ++;
      }
   /* EasyCASE ) */
   /* EasyCASE ( 101034
      Take the moving average of the values of the Filter Buffer. */
   for (v_Index_u8r = 0; v_Index_u8r < p_SmoothFilter_xxr->V_FilterLength_U8R; v_Index_u8r++)
      {
      v_TempSum_u32r += p_SmoothFilter_xxr->A_SmoothBuffer_U16R[v_Index_u8r];
      }
   p_SmoothFilter_xxr->V_FilteredValue_U16R = (U16)(v_TempSum_u32r/p_SmoothFilter_xxr->V_FilterLength_U8R);
   /* EasyCASE ) */
   }
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ( 100504
   Local functions */
/* EasyCASE ) */
/* EasyCASE ) */
