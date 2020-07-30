#include "__cf_sldemo_raccel_engine_idle_speed.h"
#include "rtw_capi.h"
#ifdef HOST_CAPI_BUILD
#include "sldemo_raccel_engine_idle_speed_capi_host.h"
#define sizeof(s) ((size_t)(0xFFFF))
#undef rt_offsetof
#define rt_offsetof(s,el) ((uint16_T)(0xFFFF))
#define TARGET_CONST
#define TARGET_STRING(s) (s)    
#else
#include "builtin_typeid_types.h"
#include "sldemo_raccel_engine_idle_speed.h"
#include "sldemo_raccel_engine_idle_speed_capi.h"
#include "sldemo_raccel_engine_idle_speed_private.h"
#ifdef LIGHT_WEIGHT_CAPI
#define TARGET_CONST                  
#define TARGET_STRING(s)               (NULL)                    
#else
#define TARGET_CONST                   const
#define TARGET_STRING(s)               (s)
#endif
#endif
static const rtwCAPI_Signals rtBlockSignals [ ] = { { 0 , 0 , TARGET_STRING (
"sldemo_raccel_engine_idle_speed/Gain" ) , TARGET_STRING ( "" ) , 0 , 0 , 0 ,
0 , 0 } , { 1 , 0 , TARGET_STRING ( "sldemo_raccel_engine_idle_speed/Gain1" )
, TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 0 } , { 2 , 0 , TARGET_STRING (
"sldemo_raccel_engine_idle_speed/Gain2" ) , TARGET_STRING ( "" ) , 0 , 0 , 0
, 0 , 0 } , { 3 , 0 , TARGET_STRING ( "sldemo_raccel_engine_idle_speed/Sum" )
, TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 0 } , { 0 , 0 , ( NULL ) , ( NULL )
, 0 , 0 , 0 , 0 , 0 } } ; static const rtwCAPI_BlockParameters
rtBlockParameters [ ] = { { 4 , TARGET_STRING (
"sldemo_raccel_engine_idle_speed/x^1" ) , TARGET_STRING ( "Coefs" ) , 0 , 1 ,
0 } , { 5 , TARGET_STRING ( "sldemo_raccel_engine_idle_speed/x^2" ) ,
TARGET_STRING ( "Coefs" ) , 0 , 2 , 0 } , { 6 , TARGET_STRING (
"sldemo_raccel_engine_idle_speed/x^3" ) , TARGET_STRING ( "Coefs" ) , 0 , 3 ,
0 } , { 7 , TARGET_STRING ( "sldemo_raccel_engine_idle_speed/Transfer Fcn" )
, TARGET_STRING ( "A" ) , 0 , 4 , 0 } , { 8 , TARGET_STRING (
"sldemo_raccel_engine_idle_speed/Transfer Fcn" ) , TARGET_STRING ( "C" ) , 0
, 1 , 0 } , { 9 , TARGET_STRING (
"sldemo_raccel_engine_idle_speed/Transfer Fcn1" ) , TARGET_STRING ( "A" ) , 0
, 4 , 0 } , { 10 , TARGET_STRING (
"sldemo_raccel_engine_idle_speed/Transfer Fcn1" ) , TARGET_STRING ( "C" ) , 0
, 1 , 0 } , { 11 , TARGET_STRING (
"sldemo_raccel_engine_idle_speed/Transfer Fcn2" ) , TARGET_STRING ( "A" ) , 0
, 4 , 0 } , { 12 , TARGET_STRING (
"sldemo_raccel_engine_idle_speed/Transfer Fcn2" ) , TARGET_STRING ( "C" ) , 0
, 1 , 0 } , { 13 , TARGET_STRING (
"sldemo_raccel_engine_idle_speed/Transport Delay" ) , TARGET_STRING (
"InitialOutput" ) , 0 , 0 , 0 } , { 0 , ( NULL ) , ( NULL ) , 0 , 0 , 0 } } ;
static const rtwCAPI_ModelParameters rtModelParameters [ ] = { { 14 ,
TARGET_STRING ( "delay" ) , 0 , 0 , 0 } , { 15 , TARGET_STRING ( "gain1" ) ,
0 , 0 , 0 } , { 16 , TARGET_STRING ( "gain2" ) , 0 , 0 , 0 } , { 17 ,
TARGET_STRING ( "gain3" ) , 0 , 0 , 0 } , { 18 , TARGET_STRING ( "meanSpeed"
) , 0 , 0 , 0 } , { 0 , ( NULL ) , 0 , 0 , 0 } } ;
#ifndef HOST_CAPI_BUILD
static void * rtDataAddrMap [ ] = { & rtB . o3mmlz1lnc , & rtB . hk0pd3xosq ,
& rtB . mnpqmrtgy1 , & rtB . c0ybjkfx0j , & rtP . x1_Coefs [ 0 ] , & rtP .
x2_Coefs [ 0 ] , & rtP . x3_Coefs [ 0 ] , & rtP . TransferFcn_A [ 0 ] , & rtP
. TransferFcn_C [ 0 ] , & rtP . TransferFcn1_A [ 0 ] , & rtP . TransferFcn1_C
[ 0 ] , & rtP . TransferFcn2_A [ 0 ] , & rtP . TransferFcn2_C [ 0 ] , & rtP .
TransportDelay_InitOutput , & rtP . delay , & rtP . gain1 , & rtP . gain2 , &
rtP . gain3 , & rtP . meanSpeed , } ; static int32_T * rtVarDimsAddrMap [ ] =
{ ( NULL ) } ;
#endif
static TARGET_CONST rtwCAPI_DataTypeMap rtDataTypeMap [ ] = { { "double" ,
"real_T" , 0 , 0 , sizeof ( real_T ) , SS_DOUBLE , 0 , 0 } } ;
#ifdef HOST_CAPI_BUILD
#undef sizeof
#endif
static TARGET_CONST rtwCAPI_ElementMap rtElementMap [ ] = { { ( NULL ) , 0 ,
0 , 0 , 0 } , } ; static const rtwCAPI_DimensionMap rtDimensionMap [ ] = { {
rtwCAPI_SCALAR , 0 , 2 , 0 } , { rtwCAPI_VECTOR , 2 , 2 , 0 } , {
rtwCAPI_VECTOR , 4 , 2 , 0 } , { rtwCAPI_VECTOR , 6 , 2 , 0 } , {
rtwCAPI_VECTOR , 8 , 2 , 0 } } ; static const uint_T rtDimensionArray [ ] = {
1 , 1 , 1 , 2 , 1 , 3 , 1 , 4 , 2 , 1 } ; static const real_T
rtcapiStoredFloats [ ] = { 0.0 } ; static const rtwCAPI_FixPtMap rtFixPtMap [
] = { { ( NULL ) , ( NULL ) , rtwCAPI_FIX_RESERVED , 0 , 0 , 0 } , } ; static
const rtwCAPI_SampleTimeMap rtSampleTimeMap [ ] = { { ( const void * ) &
rtcapiStoredFloats [ 0 ] , ( const void * ) & rtcapiStoredFloats [ 0 ] , 0 ,
0 } } ; static rtwCAPI_ModelMappingStaticInfo mmiStatic = { { rtBlockSignals
, 4 , ( NULL ) , 0 , ( NULL ) , 0 } , { rtBlockParameters , 10 ,
rtModelParameters , 5 } , { ( NULL ) , 0 } , { rtDataTypeMap , rtDimensionMap
, rtFixPtMap , rtElementMap , rtSampleTimeMap , rtDimensionArray } , "float"
, { 3318482466U , 1837455036U , 3832219310U , 3045271466U } , ( NULL ) , 0 ,
0 } ; const rtwCAPI_ModelMappingStaticInfo *
sldemo_raccel_engine_idle_speed_GetCAPIStaticMap ( void ) { return &
mmiStatic ; }
#ifndef HOST_CAPI_BUILD
void sldemo_raccel_engine_idle_speed_InitializeDataMapInfo ( void ) {
rtwCAPI_SetVersion ( ( * rt_dataMapInfoPtr ) . mmi , 1 ) ;
rtwCAPI_SetStaticMap ( ( * rt_dataMapInfoPtr ) . mmi , & mmiStatic ) ;
rtwCAPI_SetLoggingStaticMap ( ( * rt_dataMapInfoPtr ) . mmi , ( NULL ) ) ;
rtwCAPI_SetDataAddressMap ( ( * rt_dataMapInfoPtr ) . mmi , rtDataAddrMap ) ;
rtwCAPI_SetVarDimsAddressMap ( ( * rt_dataMapInfoPtr ) . mmi ,
rtVarDimsAddrMap ) ; rtwCAPI_SetInstanceLoggingInfo ( ( * rt_dataMapInfoPtr )
. mmi , ( NULL ) ) ; rtwCAPI_SetChildMMIArray ( ( * rt_dataMapInfoPtr ) . mmi
, ( NULL ) ) ; rtwCAPI_SetChildMMIArrayLen ( ( * rt_dataMapInfoPtr ) . mmi ,
0 ) ; }
#else
#ifdef __cplusplus
extern "C" {
#endif
void sldemo_raccel_engine_idle_speed_host_InitializeDataMapInfo (
sldemo_raccel_engine_idle_speed_host_DataMapInfo_T * dataMap , const char *
path ) { rtwCAPI_SetVersion ( dataMap -> mmi , 1 ) ; rtwCAPI_SetStaticMap (
dataMap -> mmi , & mmiStatic ) ; rtwCAPI_SetDataAddressMap ( dataMap -> mmi ,
NULL ) ; rtwCAPI_SetVarDimsAddressMap ( dataMap -> mmi , NULL ) ;
rtwCAPI_SetPath ( dataMap -> mmi , path ) ; rtwCAPI_SetFullPath ( dataMap ->
mmi , NULL ) ; rtwCAPI_SetChildMMIArray ( dataMap -> mmi , ( NULL ) ) ;
rtwCAPI_SetChildMMIArrayLen ( dataMap -> mmi , 0 ) ; }
#ifdef __cplusplus
}
#endif
#endif
