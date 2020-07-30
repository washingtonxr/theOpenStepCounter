#include "__cf_sldemo_raccel_engine_idle_speed.h"
#ifndef RTW_HEADER_sldemo_raccel_engine_idle_speed_h_
#define RTW_HEADER_sldemo_raccel_engine_idle_speed_h_
#include <stddef.h>
#include <float.h>
#include <string.h>
#include "rtw_modelmap.h"
#ifndef sldemo_raccel_engine_idle_speed_COMMON_INCLUDES_
#define sldemo_raccel_engine_idle_speed_COMMON_INCLUDES_
#include <stdlib.h>
#include "rtwtypes.h"
#include "simtarget/slSimTgtSigstreamRTW.h"
#include "simtarget/slSimTgtSlioCoreRTW.h"
#include "simtarget/slSimTgtSlioClientsRTW.h"
#include "simtarget/slSimTgtSlioSdiRTW.h"
#include "sigstream_rtw.h"
#include "simstruc.h"
#include "fixedpoint.h"
#include "raccel.h"
#include "slsv_diagnostic_codegen_c_api.h"
#include "rt_logging.h"
#include "dt_info.h"
#include "ext_work.h"
#endif
#include "sldemo_raccel_engine_idle_speed_types.h"
#include "multiword_types.h"
#include "mwmathutil.h"
#include "rt_defines.h"
#include "rtGetInf.h"
#include "rt_nonfinite.h"
#define MODEL_NAME sldemo_raccel_engine_idle_speed
#define NSAMPLE_TIMES (2) 
#define NINPUTS (1)       
#define NOUTPUTS (1)     
#define NBLOCKIO (4) 
#define NUM_ZC_EVENTS (0) 
#ifndef NCSTATES
#define NCSTATES (6)   
#elif NCSTATES != 6
#error Invalid specification of NCSTATES defined in compiler command
#endif
#ifndef rtmGetDataMapInfo
#define rtmGetDataMapInfo(rtm) (*rt_dataMapInfoPtr)
#endif
#ifndef rtmSetDataMapInfo
#define rtmSetDataMapInfo(rtm, val) (rt_dataMapInfoPtr = &val)
#endif
#ifndef IN_RACCEL_MAIN
#endif
typedef struct { real_T c0ybjkfx0j ; real_T o3mmlz1lnc ; real_T hk0pd3xosq ;
real_T mnpqmrtgy1 ; } B ; typedef struct { struct { real_T modelTStart ; }
ljjmxwgrb1 ; struct { void * LoggedData ; } enqgjhbe1a ; struct { void *
LoggedData ; } fysfdp5o4k ; struct { void * LoggedData ; } dyt1r0sofp ;
struct { void * TUbufferPtrs [ 2 ] ; } hxqfcb0yu2 ; struct { int_T Tail ;
int_T Head ; int_T Last ; int_T CircularBufSize ; int_T MaxNewBufSize ; }
pd2qgdg4h5 ; } DW ; typedef struct { real_T nyx3j5uylc [ 2 ] ; real_T
fwk4zep044 [ 2 ] ; real_T hb3rhw3ymk [ 2 ] ; } X ; typedef struct { real_T
nyx3j5uylc [ 2 ] ; real_T fwk4zep044 [ 2 ] ; real_T hb3rhw3ymk [ 2 ] ; } XDot
; typedef struct { boolean_T nyx3j5uylc [ 2 ] ; boolean_T fwk4zep044 [ 2 ] ;
boolean_T hb3rhw3ymk [ 2 ] ; } XDis ; typedef struct { real_T nyx3j5uylc [ 2
] ; real_T fwk4zep044 [ 2 ] ; real_T hb3rhw3ymk [ 2 ] ; } CStateAbsTol ;
typedef struct { real_T m43pnoktrn ; } ExtU ; typedef struct { real_T
kcupnkf2mu ; } ExtY ; typedef struct { rtwCAPI_ModelMappingInfo mmi ; }
DataMapInfo ; struct P_ { real_T delay ; real_T gain1 ; real_T gain2 ; real_T
gain3 ; real_T meanSpeed ; real_T TransferFcn_A [ 2 ] ; real_T TransferFcn_C
[ 2 ] ; real_T TransferFcn1_A [ 2 ] ; real_T TransferFcn1_C [ 2 ] ; real_T
TransferFcn2_A [ 2 ] ; real_T TransferFcn2_C [ 2 ] ; real_T
TransportDelay_InitOutput ; real_T x1_Coefs [ 2 ] ; real_T x2_Coefs [ 3 ] ;
real_T x3_Coefs [ 4 ] ; } ; extern const char * RT_MEMORY_ALLOCATION_ERROR ;
extern B rtB ; extern X rtX ; extern DW rtDW ; extern ExtU rtU ; extern ExtY
rtY ; extern P rtP ; extern const rtwCAPI_ModelMappingStaticInfo *
sldemo_raccel_engine_idle_speed_GetCAPIStaticMap ( void ) ; extern SimStruct
* const rtS ; extern const int_T gblNumToFiles ; extern const int_T
gblNumFrFiles ; extern const int_T gblNumFrWksBlocks ; extern rtInportTUtable
* gblInportTUtables ; extern const char * gblInportFileName ; extern const
int_T gblNumRootInportBlks ; extern const int_T gblNumModelInputs ; extern
const int_T gblInportDataTypeIdx [ ] ; extern const int_T gblInportDims [ ] ;
extern const int_T gblInportComplex [ ] ; extern const int_T
gblInportInterpoFlag [ ] ; extern const int_T gblInportContinuous [ ] ;
extern const int_T gblParameterTuningTid ; extern DataMapInfo *
rt_dataMapInfoPtr ; extern rtwCAPI_ModelMappingInfo * rt_modelMapInfoPtr ;
void MdlOutputs ( int_T tid ) ; void MdlOutputsParameterSampleTime ( int_T
tid ) ; void MdlUpdate ( int_T tid ) ; void MdlTerminate ( void ) ; void
MdlInitializeSizes ( void ) ; void MdlInitializeSampleTimes ( void ) ;
SimStruct * raccel_register_model ( void ) ;
#endif
