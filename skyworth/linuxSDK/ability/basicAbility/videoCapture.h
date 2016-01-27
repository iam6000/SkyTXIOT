/*!
*	videoCapture module for capture video stream from camera 
*	video capture use v4l2   in videodev2
*	@add by 6000 at 2015/12/24
*/
#ifndef _VIDEOCAPTURE_H
#define _VIDEOCAPTURE_H

#include<linux/videodev2.h>
#include "TXSDKCommonDef.h"  // just for bool 
#include"h264Encoder.h"

// buffer for video frame
struct buffer{
	void *start ; 
	size_t length; 
};

typedef struct {
	char* device_name ; 
	int fd ; 
	int width ; 
	int height ;
	int display_depth ; 
	int image_size ; 
	int frame_numbers; 
	struct v4l2_capability v4l2_cap ; 
	struct v4l2_cropcap v4l2_cropcp ; 
	struct v4l2_format v4l2_fmt ; 
	struct v4l2_crop crop ; 
	struct buffer *buffers; 	
} Camera;

/*!
*	param s  LOG_TAG
*	return errono msg  EXIT_FAILURE
*/
void errno_exit(const char *s);

/*!
* brief  basic encapsulate the ioctl funcs 
*/
int xioctl(int fd, int request ,void *arg);

/*!
*	brief use struct camera to init camera funcs 
*/
bool open_camera(Camera *cam);

void close_camera(Camera *cam);

bool init_camera(Camera*cam);

void uninit_camera(Camera *cam); 

bool init_mmap(Camera *cam);

bool start_video_capturing(Camera*cam);

void stop_video_capturing(Camera *cam);

bool v4l2_init(Camera *cam);

void v4l2_close(Camera *cam);

void save_yuv_frame(unsigned char *yuv_frame, size_t yuv_length); 

/*!
* return h264 length 
*/

int encode_and_save(Encoder *enc,unsigned char * yuv_frame,size_t yuv_length,unsigned char* h264_buf);

bool read_yuv_frame(Camera *cam);

// add encoder 
bool read_and_encode_frame(Camera * cam, Encoder *enc ,unsigned char *h264_buf, int *length);


#endif 