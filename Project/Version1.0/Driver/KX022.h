/**
 * @File    KX022.h
 * @Date    Tuesday, Dec. 25, 2018 at 12:04:53 PM BJT
 * @Author  Washington Ruan
 * @Email   washingtonxr@live.com
 *
 * This file contains the implementation of the gravity sensor devicer
 * module's head file.
 *
 * @bug No known bugs.
 **/
#ifndef KX022_H
#define KX022_H

#include "Peripherals.h"
#include "GAlgorithmCom.h"
#include "debug.h"

/* Defination for KX022. */
#define KX022_ADDR          0X1E
#define KX022_dreg          0x3F
#define KX022_dclc          0x3e

#define KX022_axis          3
#define KX022_blen          0x3C
#define KX022_drate         100000
#define KX022_creg          0x0C
#define KX022_ddepth        42
#define KX022_OK            0x00
#define KX022_ERR           0x01
#define KX022_CHE           0x02
#define KX022_RESOLUTION    16384
#define KX022_NG            9.8
void KX022_Init(axis_dblock_t *GDB);
int KX022_get_data(axis_dblock_t *GDB);

#endif
