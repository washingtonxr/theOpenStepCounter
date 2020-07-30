#ifndef __PEDO_SUPPORT_H__
#define __PEDO_SUPPORT_H__
/** \mainpage Interface support
* 
*       \section intro_sec Introduction
*		Interface support for pedometer algorithm
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


/* Module includes */
#include "define.h"
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ( 75
   #Define Constants */
#define INTERNAL_FIFO_MAX_SIZE  20 /**< Internal FIFO buffer size */
#define EXTERNAL_FIFO_MAX_SIZE  20 /**< External FIFO buffer size */
#define FIFO_START_INDEX        0  /**< FIFO start Index */
/* EasyCASE ) */
/* EasyCASE ( 76
   ENUM and struct Definitions */
/* LOW PASS FILTER */
/* EasyCASE - */
/* EasyCASE < */
typedef struct LOW_PASS_FILTER
{
     F32 V_FilterValue_F32R; /**< Filtered value */
     U16 V_CurSample_U16R;   /**< Current Sample */
     F32 V_ALPHA_F32R;       /**< Value of ALPHA for LPF */ 

}ts_LPF;
/* EasyCASE > */
/* EasyCASE - */
/* SMOOTH FILTER */
/* EasyCASE - */
/* EasyCASE < */
typedef struct SMOOTH_FILTER
{
        U16 V_FilteredValue_U16R; /**< Filtered Value */
        U16 V_NewSample_U16R;     /**< New Sample */
        U8  V_FilterLength_U8R;   /**< Smooth Buffer Size */
        U8  V_FilterIndex_U8R;    /**< Smooth Buffer Index */
        U16 A_SmoothBuffer_U16R[20]; /**< Smooth Buffer array */         
}ts_SmoothFilter;
/* EasyCASE > */
/* EasyCASE ) */
/* EasyCASE ( 79
   Public API Declarations */
/* EasyCASE ( 1018
   ShiftFIFO_U8 */
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
void ShiftFIFO_U8( U8*, U8, U8 );
/* EasyCASE ) */
/* EasyCASE ( 1019
   SetInternalBuffer */
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
void SetInternalBuffer( U8, U8, U8 );
/* EasyCASE ) */
/* EasyCASE ( 1020
   GetInternalBuffer */
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
void GetInternalBuffer( U8*, U8*, U8* );
/* EasyCASE ) */
/* EasyCASE ( 1021
   ResetInternalFIFO */
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
 void ResetInternalFIFO(void);
/* EasyCASE ) */
/* EasyCASE ( 1022
   GetInternalFIFOSize */
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
 U8 GetInternalFIFOSize(void);
/* EasyCASE ) */
/* EasyCASE ( 1023
   SetExternalBuffer */
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
void SetExternalBuffer(U8*, U8*, U8*, U8 );
/* EasyCASE ) */
/* EasyCASE ( 1024
   GetExternalBuffer */
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
void GetExternalBuffer( U8*, U8*, U8* );
/* EasyCASE ) */
/* EasyCASE ( 1025
   ResetExternalFIFO */
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
 void ResetExternalFIFO(void);
/* EasyCASE ) */
/* EasyCASE ( 1026
   GetExternalFIFOSize */
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
 U8 GetExternalFIFOSize(void);
/* EasyCASE ) */
/* EasyCASE ( 1027
   ProcessLPF */
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
 void ProcessLPF(ts_LPF *p_ObjLPF);
/* EasyCASE ) */
/* EasyCASE ( 1028
   GetSmoothValue */
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
 void GetSmoothValue(ts_SmoothFilter *p_SmoothFilter_xxr);
/* EasyCASE ) */
/* EasyCASE ) */
#endif
/* EasyCASE ) */
