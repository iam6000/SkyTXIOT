/*!
*	h264 Encoder module 
*	use x264 as encoder 
*	@add by 6000 at 2015/12/25
*/

#ifndef _H264ENCODER_H
#define _H264ENCODER_H

#include<stdint.h>
#include<stdio.h>
#include "TXSDKCommonDef.h"  // just for bool 
#include"../../third_party/x264/include/x264.h"  // will change the dir later  

typedef unsigned char uint8_t ;

typedef struct {
	x264_param_t *param ; 
	x264_t *handle ; 
	x264_picture_t *picture ; 
	x264_nal_t *nal ; 
}Encoder;

/*!
*	init x264 encoder 
*	param *enc  : pointer to Encoder which will do init 
*	param width :  video width 
*	param hegith: video height
*	return true or false
*/
bool init_x264_encoder(Encoder *enc , int width, int height);


/*!
*	encoder func to encode one yuv frame  
*	param *enc : x264 Encoder 
*	param type : frame types  I/P/B
*	param *yuv_in  : yuv frame to be encode
*	param *h264_out : 264 frame return
*	return h264 length 
*/
int encode_frame(Encoder *enc , int type , uint8_t *yuv_in, uint8_t * h264_out);


void close_x264_encoder(Encoder* enc);

#endif 