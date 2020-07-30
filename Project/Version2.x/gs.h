/**
 * @File    gs.h
 * @Date    Tuesday, Dec. 25, 2018 at 14:17:09 PM BJT
 * @Author  Washington Ruan
 * @Email   washingtonxr@live.com
 *
 * This file contains the implementation of the gravity sensor devicer
 * module's main file.
 *
 * @bug No known bugs.
 **/

#ifndef _GS_H
#define _GS_H

#define AK09918_NOTEST 0
#define LATCH_NORMAL_PERIOD   500
//#define NO_COMPASS  1   /* Close the function of Compass. */
typedef enum
{
  GS_NULL,
  GS_ENABLE,
  GS_DISABLE,
} gs_cmd_t;

typedef struct
{
    gs_cmd_t cmdCode;
}   gs_data_t;

void SendGsCmd(gs_cmd_t cmdCode);

void task_gs(void* pvParameters);

#endif

