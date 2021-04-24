/**
  ******************************************************************************
  * File Name          : simpl_protocol.c
  * Description        : This file contains simpl protocol of KKS5
  * Autor              : Melnik Taras melnik.taras.6699@gmail.com
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 KKS5.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by KKS5 under private license of code.
  *
  ******************************************************************************
*/

#include "simpl_protocol.h"
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

sn_protocol_t sn = {
    .cmd = SN_PROTC_NONE,
    .len = 0,
    .status = BYTE_NUM_HEADER,
    .error_crc = 0,
    .sn_connected = false,
    .crc = 0
};

// HardwareSerial* _serial; 

static uint16_t len;
static uint8_t buf[ESP_GATT_MAX_LEN];

sn_protocol_t *sn_get(void)
{
    return &sn;
}

uint8_t get_cmd(void)
{
    return sn.cmd;
}

/**
 * @brief Pack a const len protocol
 * @param cmd  ID command
 * @return length of the message in bytes (excluding serial stream start sign)
 */
uint8_t *sn_pack_const(uint8_t cmd, uint8_t data)
{ // const len = 3
    static uint8_t arr[5];
    memset(arr, 0, sizeof(arr));

    arr[BYTE_NUM_HEADER] = SN_VAL_HEADER;
    arr[BYTE_NUM_LENG] = SN_VAL_CONST_LENG;
    arr[BYTE_NUM_CMD] = cmd;
    arr[BYTE_NUM_DATA] = data;

    for (uint8_t i = 0; i < SN_VAL_CONST_LENG + 1; i++)
    {
        arr[BYTE_NUM_CRC] ^= arr[i];
    }

    return arr;
}

/**
 * @brief Pack a extended len protocol
 * @param cmd  ID command
 * @return SIZE of the message in bytes (excluding serial stream start sign)
 */

uint16_t sn_pack_ext(uint8_t cmd, uint8_t *data_in, uint8_t* data_out, uint8_t data_in_size){ //
    uint16_t byte_num = 0;
    //memset(data_out,0, data_in_size);

    if(data_in_size > 255-2) {
        return 0;
    }

    data_out[byte_num++] = SN_VAL_HEADER;
    data_out[byte_num++] = data_in_size+2;
    data_out[byte_num++] = cmd;

    memcpy(&data_out[byte_num], data_in, data_in_size);
    byte_num += data_in_size;

    data_out[byte_num] = 0; // start crc must = 0
    unsigned short i;
    for( i = 0; i < byte_num;i++){
        data_out[byte_num] ^= data_out[i];
    }

    byte_num += 1;
    return byte_num;
}

SN_Status sn_crc_check(sn_protocol_t *msg, uint8_t* buf)
{
    SN_Status status = SN_CRC_ERROR;
    uint8_t crc = 0;
    
    uint8_t i = 0;
    for (i=0; i < msg->len+1; i++)  {
        crc ^= buf[i];
    }

    if(crc == msg->crc){
        status = SN_OK;
    }

    return status;
}

SN_Status sn_unpack(sn_protocol_t *msg, uint8_t *in_data)
{
    SN_Status status = SN_ERROR;
    uint8_t crc = 0;

    for (int i = 0; i < in_data[BYTE_NUM_LENG] + 1; i++)
    {
        crc ^= in_data[i];
    }

    if (crc == in_data[BYTE_NUM_CRC])
    {
        status = SN_OK;

        switch (in_data[(uint8_t)BYTE_NUM_CMD])
        {

        }
    } else{

        status = SN_ERROR;

    }

    return status;
}

/**
 * This is a varient of sn_pars_char() but with caller supplied
 * parsing buffers. It is useful when you want to create a SN
 * parser in a library that doesn't use any global variables
 *
 * @param rxmsg    parsing message buffer | status - parsing starus buffer
 * @param c        The char to parse
 *
 * @param returnMsg NULL if no message could be decoded, the message data else
 * @param returnStats if a message was decoded, this is filled with the channel's stats
 * @return 0 if no message could be decoded, 1 on good message and CRC, 2 on bad CRC
 *
 * A typical use scenario of this function call is:
 *
 * @code
 * #include <simpl_protocol.h>
 *
 * sn_protocol_t msg;
 * int chan = 0;
 *
 *
 * while(serial.bytesAvailable > 0)
 * {
 *   uint8_t byte = serial.getNextByte();
 *   if sn_pars_char(byte, &msg) == SN_OK)
 *     {
 *      printf("Received message with CMD %d, len: %d , msg.data[BYTE_NUM_CMD], msg.data[BYTE_NUM_LEN]);
 *     }
 * }
 *
 *
 * @endcode
 */

SN_Status sn_pars_char(uint8_t c, sn_protocol_t *msg){
    SN_Status state = SN_NONE;


    switch (msg->status)
    {
    case BYTE_NUM_HEADER:
        if (c == SN_VAL_HEADER)
        {
            len = 0;
            buf[len] = c;
            len += 1;
            msg->status = BYTE_NUM_LENG;
        }
        break;
    case BYTE_NUM_LENG:
    	buf[len] = c;
        len += 1;
        msg->status = BYTE_NUM_CMD;
        msg->len = c;
        break;
    case BYTE_NUM_CMD:
    	buf[len] = c;
        msg->cmd = c;
        len += 1;
        msg->status = BYTE_NUM_DATA;
        break;
    case BYTE_NUM_DATA:
         if(len >= msg->len){
        	buf[len] = c;
            msg->data[len-3] = c;
            msg->status = BYTE_NUM_CRC;
        }
        else
        {
        	buf[len] = c;
            msg->data[len-3] = c;
            len += 1;
            msg->status = BYTE_NUM_DATA;
        }
        break;
    case BYTE_NUM_CRC:
        msg->status = BYTE_NUM_HEADER;
        msg->crc = c;

        state = sn_crc_check(msg, buf);

        if(state == SN_OK) {
            msg->sn_connected = true;
            // for (uint8_t i = 0; i <= len-3; i++)  {
            //     msg->data[i] = buf[i+3]; 
            // }
            // __disable_irq();
        	// memcpy(msg->data, &buf[0], len+1);
            // __enable_irq();
            // msg->cmd = buf[BYTE_NUM_CMD];
            // msg->len = buf[BYTE_NUM_LENG];
        } else if(state == SN_CRC_ERROR) {
            msg->sn_connected = false;
            msg->error_crc += 1;
        }

        // clear after parsing
        memset(buf, 0, sizeof(buf));
        break;

    default:
        msg->status = BYTE_NUM_HEADER;
        break;
    }

    return state;
}

//void main(void)
void sn_test(uint8_t* m, uint16_t size)
{
    // uint64_t _buttons = 123456789012345678;
    // uint16_t _x = 1111, _y = 2222, _z = 3333, _rX = 4444, _rY = 5555, _rZ = 6666, _slider1 = 7777, _slider2 = 8888;
    // uint8_t _hat4 = 55, _hat3 = 44, _hat2 = 33, _hat1 = 22;
    // static uint8_t m[28];
    // memset(m, 0, sizeof(m));
    static uint8_t out[100];
    memset(out, 0, sizeof(out));

    // m[0] = _buttons;
    // m[1] = (_buttons >> 8);
    // m[2] = (_buttons >> 16);
    // m[3] = (_buttons >> 24);
    // m[4] = (_buttons >> 32);
    // m[5] = (_buttons >> 40);
    // m[6] = (_buttons >> 48);
    // m[7] = (_buttons >> 56);
    // m[8] = _x;
    // m[9] = (_x >> 8);
    // m[10] = _y;
    // m[11] = (_y >> 8);
    // m[12] = _z;
    // m[13] = (_z >> 8);
    // m[14] = _rZ;
    // m[15] = (_rZ >> 8);
    // m[16] = _rX;
    // m[17] = (_rX >> 8);
    // m[18] = _rY;
    // m[19] = (_rY >> 8);
    // m[20] = _slider1;
    // m[21] = (_slider1 >> 8);
    // m[22] = _slider2;
    // m[23] = (_slider2 >> 8);
    // m[24] = _hat4;
    // m[25] = _hat3;
    // m[26] = _hat2;
    // m[27] = _hat1;

    uint16_t len = sn_pack_ext(JOYSTICK_STATE, m, out, size);//sizeof(m));
//    printf("SN pack done len = %d\n\r", len);
//    for (uint8_t i = 0; i <= len+5; i++)  {
//      printf("%d ", out[i]);
//
//    }
//    printf("\n\r");
//
//    printf(" ");
    static SN_Status st;
    for (uint8_t i = 0; i < len+1; i++)  {
//        printf("i= %d ", i);
//        printf("%d",out[i]);
        st = sn_pars_char(out[i], &sn);
//        printf(" st = %d", st);
//        printf(" msgSTat = %d \n\r", sn.status);
        if(st == SN_OK){
            asm("nop");
            // printf("SN pars char = DANE, CRC - OK");
            break;
        } else {
          if(i == len){
            asm("nop");
            // printf("SN pars ERROR");
          }
        }

        // if(i > sizeof(m)+4){
        //     // printf("SN pars char = ERROR LEN,");
        //     break;
        // }
        // delay(5);
    }
//   printf("END SN_test1()");

}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT KKS5 *****END OF FILE****/
