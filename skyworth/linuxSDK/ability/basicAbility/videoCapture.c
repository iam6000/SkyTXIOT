/*!
*	videocapture for linux ,capture with v4l2 , and encoder with x264 
*	@add by 6000 at 2015/12/24
*/
#include<asm/types.h>
#include<fcntl.h> // for low level i-o
#include<unistd.h>
#include<errno.h>
#include<malloc.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/time.h>
//#include<time.h>
#include<sys/mman.h>
#include<sys/ioctl.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<assert.h>
#include"videoCapture.h"
#include"h264Encoder.h"
#include"../../skyLog.h"

#define TAG_VIDEOCAPTURE "Video_Capture"

#define VIDEO_DEVICE "/dev/video0"

#define CLEAR(x) memset(&(x),0, sizeof(x))

typedef unsigned char uint8_t ;  

unsigned int n_buffers = 0 ;
uint8_t *h264_buf;
FILE * yuv_fp  ;
FILE * h264_fp ;


void errno_exit(const char * s)
{
	SKY_LOG(1,(s,"error %d, %s",errno,strerror(errno)));
	exit(EXIT_FAILURE);
}

int xioctl(int fd,int request,void * arg)
{
	int r = 0 ; 
	do{
		// do ioctl  
		r = ioctl(fd,request,arg);		
	} while( -1 == r && EINTR == errno );

	return r ;
}

bool open_camera(Camera * cam)
{
	SKY_LOG(3,(TAG_VIDEOCAPTURE,"begin openCamera"));
	struct stat st ; 
	//first check video file 
	if(-1 == stat(cam->device_name, &st))
	{
		SKY_LOG(1,(TAG_VIDEOCAPTURE,"Could not identify vide device%s",cam->device_name));
		return false ;
	}
	//then check the type of file
	if(!S_ISCHR(st.st_mode))
	{
		SKY_LOG(1,(TAG_VIDEOCAPTURE,"%s is not device",cam->device_name));
		return false ;
	}
	// open camera device
	cam->fd = open(cam->device_name,O_RDWR,0);
	if(-1 == cam->fd)
	{
		SKY_LOG(1,(TAG_VIDEOCAPTURE,"Can not open %s",cam->device_name));
		return false ;
	}
	
	return true ;
	
}


void close_camera(Camera * cam)
{
	if(-1 == close(cam->fd))
	{
		errno_exit("close");
	}
	cam->fd = -1 ; 
}


bool init_camera(Camera * cam)
{	
	SKY_LOG(3,(TAG_VIDEOCAPTURE,"begin init camera"));
	yuv_fp = fopen("capture.yuv","wa+");
	if(!yuv_fp)
	{
		SKY_LOG(1,(TAG_VIDEOCAPTURE, "Could not open yuv file"));
	}
	h264_fp = fopen("encode.h264","wa+");
	if(!h264_fp)
	{
		SKY_LOG(1,(TAG_VIDEOCAPTURE, "Could not open 264 file"));
	}
	struct v4l2_capability *cap = &(cam->v4l2_cap);
	struct v4l2_cropcap *cropcap = &(cam->v4l2_cropcp);
	struct v4l2_crop *crop = &(cam->crop);
	struct v4l2_format *fmt = &(cam->v4l2_fmt);
	unsigned int min ; 

	// do VIDIOC_QUERYCAP for camera device 
	if(-1 == xioctl(cam->fd,VIDIOC_QUERYCAP,cap))
	{
		if(EINVAL == errno)
		{
			SKY_LOG(1,(TAG_VIDEOCAPTURE,"%s is not v4l2 device",cam->device_name));
			return false  ;
		}
		else 
		{
			SKY_LOG(1,(TAG_VIDEOCAPTURE,"VIDIOC_QUERYCAP"));
			return false ; 
		}
	}
	
	// check Video Capture ability 
	if (!(cap->capabilities & V4L2_CAP_VIDEO_CAPTURE))
	{
		SKY_LOG(1,(TAG_VIDEOCAPTURE,"%s is not video capture device",cam->device_name));
		return false ;
	}
	// check Streaming support
	if (!(cap->capabilities & V4L2_CAP_STREAMING))
	{
		SKY_LOG(1,(TAG_VIDEOCAPTURE,"%s dose not support Streaming I/O",cam->device_name));
		return false ;
	}
	/*  
	//Segmentation fault here 
	SKY_LOG(3,(TAG_VIDEOCAPTURE,"init camera [name: %s]\t[driver : %s][card : %s]\t[bus info: %s]\t[version :%s]",
						cam->device_name,
						cap->driver,
						cap->card,
						cap->bus_info,
						cap->version ));
	*/
 
	// clear cropcap and set crop values
	CLEAR(*cropcap);
	cropcap->type = V4L2_BUF_TYPE_VIDEO_CAPTURE; 
	crop->c.width = cam->width; 
	crop->c.height = cam->height;
	crop->c.left = 0;
	crop->c.top = 0;
	crop->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	// clear fmt and set video fmt 
	CLEAR(*fmt);
	fmt->type = V4L2_BUF_TYPE_VIDEO_CAPTURE; 
	fmt->fmt.pix.width = cam->width; 
	fmt->fmt.pix.height = cam->height; 
	//fmt->fmt.pix.pixelformat = X264_CSP_YV12 ; 
	//fmt->fmt.pix.pixelformat = V4L2_PIX_FMT_NV21 ; 
	fmt->fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV ;
	fmt->fmt.pix.field = V4L2_FIELD_INTERLACED ;

	if (-1 == xioctl(cam->fd, VIDIOC_S_FMT, fmt))
	{
		SKY_LOG(1,(TAG_VIDEOCAPTURE,"VIDIOC_S_FMT error"));
		return false ;
	}

	// e....... paranoia 
	/*
	min = fmt->fmt.pix.width * 2;
	if (fmt->fmt.pix.bytesperline < min)
	{
		fmt->fmt.pix.bytesperline = min;
	}	
	min = fmt->fmt.pix.bytesperline * fmt->fmt.pix.height;	
	if (fmt->fmt.pix.sizeimage < min)
	{
		fmt->fmt.pix.sizeimage = min;
	}
	*/

	if(init_mmap(cam))
	{
		return false ;
	}	
	
	return true ;	
}


void uninit_camera(Camera * cam)
{
	// close  file  first 
	if(yuv_fp)
	{
		close(yuv_fp);
	}
	if(h264_fp)
	{
		close(h264_fp);
	}
	
	unsigned int i;
	// do munmap for each buffers
	for (i = 0; i < n_buffers; ++i)
	{
		if (-1 == munmap(cam->buffers[i].start, cam->buffers[i].length))
		{
			errno_exit("munmap");
		}	
	}
	free(cam->buffers);
}


bool init_mmap(Camera * cam)
{
	// use v4l2_requestbuffers to set mmap 
	struct v4l2_requestbuffers req ; 
	CLEAR(req); 
	req.count = 4 ; 
	req.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE ; 
	req.memory = V4L2_MEMORY_MMAP ;

	// do mapping check  and request 4 requestbuffers 
	if(-1 == xioctl(cam->fd ,VIDIOC_REQBUFS,&req))
	{
		// memory mapping  check failed
		if(EINVAL == errno)
		{
			SKY_LOG(1,(TAG_VIDEOCAPTURE,"%s, do not support memory mapping", cam->device_name));
			return false ; 
		}
		else 
		{
			SKY_LOG(1,(TAG_VIDEOCAPTURE,"VIDIOC_REQBUFS error"));
			return false ; 	
		}
	}

	if(req.count < 2)
	{
		SKY_LOG(1,(TAG_VIDEOCAPTURE,"Insufficient buffer memory on %s",cam->device_name));
		return false ;
	}

	cam->buffers = calloc(req.count, sizeof(*(cam->buffers)));
	if(!cam->buffers)
	{
		SKY_LOG(1,(TAG_VIDEOCAPTURE,"calloc failed ,out of memory"));
		return false ;
	}

	// loop to do mmapping!!!! get each v4l2buffers and mmap these buffers to cam->buffers
	for( n_buffers = 0; n_buffers < req.count; ++n_buffers)
	{
		struct v4l2_buffer buf ; 
		CLEAR(buf);
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE ; 
		buf.memory = V4L2_MEMORY_MMAP ; 
		buf.index = n_buffers ;

		if (-1 == xioctl(cam->fd, VIDIOC_QUERYBUF, &buf))
		{
			SKY_LOG(1,(TAG_VIDEOCAPTURE,"VIDIOC_QUERYBUF failed"));
			goto  failed_free;
		}

		// 
		cam->buffers[n_buffers].length = buf.length;		
		cam->buffers[n_buffers].start = mmap(NULL /* start anywhere */,
            	buf.length, PROT_READ | PROT_WRITE /* required */,
               	MAP_SHARED /* recommended */, cam->fd, buf.m.offset);

		 if (MAP_FAILED == cam->buffers[n_buffers].start)
		 {
		 		SKY_LOG(1,(TAG_VIDEOCAPTURE,"mmap failed"));
				goto  failed_free;
		 }
		
	}


	return true ;
	
failed_free :
	free(cam->buffers);
	return false ;
	
}

// start only send 4 buffers to QBUF
bool start_video_capturing(Camera * cam)
{
	unsigned int i ; 
	enum v4l2_buf_type  type  ;

	for(i = 0 ; i < n_buffers; ++i)
	{
		struct v4l2_buffer buf ; 

		CLEAR(buf) ;

		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE ; 
		buf.memory = V4L2_MEMORY_MMAP ; 
		buf.index = i ;
		
		if(-1 == xioctl(cam->fd,VIDIOC_QBUF,&buf))
		{
			SKY_LOG(1,(TAG_VIDEOCAPTURE,"VIDIOC_QBUF failed when start_video_capture"));
			return false ;
		}
	
	}
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE ;  
	if(-1 == xioctl(cam->fd,VIDIOC_STREAMON,&type))
	{
		SKY_LOG(1,(TAG_VIDEOCAPTURE,"VIDIOC_STREAMON failed when start_video_capture"));
		return false ;		
	}

	return true ;
	
}


void stop_video_capturing(Camera * cam)
{
	enum v4l2_buf_type  type ; 
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE ;
	if(-1 == xioctl(cam->fd,VIDIOC_STREAMOFF,&type))
	{
		 errno_exit("VIDIOC_STREAMOFF");
	}
}
// test func 
void save_yuv_frame(uint8_t * yuv_frame,size_t yuv_length)
{
	if(yuv_fp)
	{
		fwrite(yuv_frame ,yuv_length ,1, yuv_fp );
	}
}

//test func 
int encode_and_save(Encoder *enc,unsigned char * yuv_frame,size_t yuv_length,unsigned char* h264_buf )
{	
	int length = 0 ; 
	if(yuv_frame[0] == 0 )
	{
		//return 0;
	}
	
	length = encode_frame(enc,-1,yuv_frame,h264_buf) ;
	if(length > 0)
	{	
		if(h264_fp)
		{	
			fwrite(h264_buf, length, 1, h264_fp);
		}		
	}

	return length ;
}


//TODO add encoder 
bool read_and_encode_frame(Camera * cam, Encoder *enc ,unsigned char *h264_buf, int *length)
{
	struct v4l2_buffer buf ;  
	CLEAR(buf);
	buf.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE ; 
	buf.memory = V4L2_MEMORY_MMAP;
	if (-1 == xioctl(cam->fd, VIDIOC_DQBUF, &buf))
	{
		 switch (errno) {
		 case EAGAIN: 
		 	return false ; 
		 case EIO : // ?? 

		 default:
		 	errno_exit("VIDIOC_DQBUF");
		 }
	}

	//encode_frame(Encoder * enc,int type,uint8_t * yuv_in,uint8_t * h264_out)
	save_yuv_frame(cam->buffers[buf.index].start,buf.length);
	SKY_LOG(1,(TAG_VIDEOCAPTURE,"buf add is %x \t buf.length is %d",cam->buffers[buf.index].start,buf.length));
	*length = encode_and_save(enc,cam->buffers[buf.index].start,buf.length,h264_buf);

	if (-1 == xioctl(cam->fd, VIDIOC_QBUF, &buf))
	{
		 SKY_LOG(1,(TAG_VIDEOCAPTURE,"VIDIOC_QBUF failed when read_frame"));
		 return false ;
	}

	return true ;
	
}

// only read yuv frame from camera 
bool read_yuv_frame(Camera * cam)
{
	struct v4l2_buffer buf ;  
	CLEAR(buf);

	buf.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE ; 
	buf.memory = V4L2_MEMORY_MMAP;

	if (-1 == xioctl(cam->fd, VIDIOC_DQBUF, &buf))
	{
		 switch (errno) {
		 case EAGAIN: 
		 	return false ; 
		 case EIO : // ?? 

		 default:
		 	errno_exit("VIDIOC_DQBUF");
		 }
	}

	// save yuv frames 	
	save_yuv_frame(cam->buffers[buf.index].start,buf.length);
	
	if (-1 == xioctl(cam->fd, VIDIOC_QBUF, &buf))
	{
		 SKY_LOG(1,(TAG_VIDEOCAPTURE,"VIDIOC_QBUF failed when read_frame"));
		 return false ;
	}

	return true ;
}

bool v4l2_init(Camera* cam)
{
	open_camera(cam); 
	init_camera(cam);
	start_video_capturing(cam);	
	return true ;
}

void v4l2_close(Camera* cam)
{
	stop_video_capturing(cam) ; 
	uninit_camera(cam);
	close_camera(cam);
	free(cam);
}
