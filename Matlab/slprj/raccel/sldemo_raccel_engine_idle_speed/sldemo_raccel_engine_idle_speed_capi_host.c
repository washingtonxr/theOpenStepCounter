#include "sldemo_raccel_engine_idle_speed_capi_host.h"
static sldemo_raccel_engine_idle_speed_host_DataMapInfo_T root;
static int initialized = 0;
__declspec( dllexport ) rtwCAPI_ModelMappingInfo *getRootMappingInfo()
{
    if (initialized == 0) {
        initialized = 1;
        sldemo_raccel_engine_idle_speed_host_InitializeDataMapInfo(&(root), "sldemo_raccel_engine_idle_speed");
    }
    return &root.mmi;
}

rtwCAPI_ModelMappingInfo *mexFunction() {return(getRootMappingInfo());}
