/**
  ******************************************************************************
  * File Name          : simpl_protocol.h 
  * Description        : This file contains all the functions prototypes for 
  *                      the simple protocol ver 1.0
  * Autor              : Melnik Taras melnik.taras.6699@gmail.com   
  ******************************************************************************
  * @attention SN_PROTOCOL
  *
  * <h2><center>&copy; Copyright (c) 2021 KKS5.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by KKS5 under MIT license of code.
  *
  ******************************************************************************
*/
/*
// Protocol description ver 1.0
// data[0] HEADER  0xAF    8-bit unsigned integer (unsigned char)
// data[1] LENG    0x03    8-bit unsigned integer (unsigned char) min = 3
// data[2] COMMAND 0x00    8-bit unsigned integer (unsigned char)
// data[3] DATA    0x00    8-bit unsigned integer (unsigned char)
// data[4] CRC     0x00    8-bit unsigned integer (unsigned char) 
// CRC - data[4] XOR data[0...3]
// data transmit interval 10-250 ms
*/

/** 
 * @brief FOR ESP32 
//!!!! ACHTUNG !!!!

 * uint8_t m[28];
		m[0] = _buttons;
		m[1] = (_buttons >> 8);
		m[2] = (_buttons >> 16);
		m[3] = (_buttons >> 24);
		m[4] = (_buttons >> 32);
		m[5] = (_buttons >> 40);
		m[6] = (_buttons >> 48);
		m[7] = (_buttons >> 56);
		m[8] = _x;
		m[9] = (_x >> 8);
		m[10] = _y;
		m[11] = (_y >> 8);
		m[12] = _z;
		m[13] = (_z >> 8);
		m[14] = _rZ;
		m[15] = (_rZ >> 8);
		m[16] = _rX;
		m[17] = (_rX >> 8);
		m[18] = _rY;
		m[19] = (_rY >> 8);
		m[20] = _slider1;
		m[21] = (_slider1 >> 8);
		m[22] = _slider2;
		m[23] = (_slider2 >> 8);
		m[24] = _hat4;
		m[25] = _hat3;
		m[26] = _hat2;
		m[27] = _hat1;
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SN_PROTOCOL_H
#define __SN_PROTOCOL_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

#define SN_PROTOCOL_VERSION 1.001f
//typedef  unsigned char uint8_t

#define SN_VAL_HEADER       (uint8_t)0xAF
#define SN_VAL_CONST_LENG   (uint8_t)0x03 // for const data length

/// GATT maximum attribute length
#define ESP_GATT_MAX_LEN   256+4 //600 //as same as GATT_MAX_ATTR_LEN create for bluetooth transfer

#ifdef __cplusplus
 extern "C" {
#endif

/** 
 * @brief enum for constant protocol LEN
//!!!! ACHTUNG !!!!
**/
typedef enum 
{
  BYTE_NUM_HEADER = 0,
  BYTE_NUM_LENG = 1,
  BYTE_NUM_CMD  = 2,
  BYTE_NUM_DATA = 3,
  BYTE_NUM_CRC  = 4,
}byte_num_t_def;

typedef enum
{
  SN_NONE = 0x00,
  SN_OK = 1,
  SN_ERROR
} SN_Status;

typedef enum
{
  SN_PROTC_NONE,
  SN_PROTC_INIT,
  SN_PROTC_ERROR,
  SN_PROTC_NO_INIT_STATE
} SN_Prot_Stat_t_def;

typedef enum {
  NONE,
  JOYSTICK_STATE = 1
} _CMD;

typedef struct s_sn_protocol {  
  uint8_t data[ESP_GATT_MAX_LEN];       // YOU can set as piont *data
  uint8_t cmd;         //
  uint16_t len;
  uint8_t crc; 
  unsigned int error_crc;
  byte_num_t_def status;
  bool sn_connected;
} sn_protocol_t;

uint8_t* sn_pack_const(uint8_t cmd, uint8_t data); // const LEN, return pointer to pack array
uint16_t sn_pack_ext(uint8_t cmd, uint8_t *data_in, uint8_t* data_out, uint16_t data_in_size); // return data pack len
SN_Status sn_pars_char(uint8_t c, sn_protocol_t *msg);
SN_Status sn_unpack(sn_protocol_t *msg, uint8_t *in_data); // pointer to input data array
SN_Status sn_crc_check(sn_protocol_t *msg, uint8_t* buf);
sn_protocol_t* sn_get(void);

void sn_test (void);

extern sn_protocol_t sn;

#ifdef __cplusplus
}
#endif
#endif /*__SN_PROTOCOL_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT Melnik Taras *****END OF FILE****/
