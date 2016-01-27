/*!
*	h264 Encoder module 
*	use x264 as encoder 
*	@add by 6000 at 2015/12/25
*/

#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include"h264Encoder.h"
#include"../../skyLog.h"
#define TAG_H264ENCODER "H264Encoder"


int yuyv_to_yv12_2(unsigned char *yuyv, unsigned char *yv12, unsigned int width, unsigned int height)
{
    unsigned char *py;
    unsigned char *pu;
    unsigned char *pv;
    unsigned int linesize = width * 2;//yuyv格式每行字节数为width*2
    unsigned int  uvlinesize = width / 2;//yv12格式uv部分每行字节数为width/2
    unsigned int offset=0;
    unsigned int offset1=0;
    unsigned int offsety=0;
    unsigned int offsety1=0;
    unsigned int offsetuv=0;

    py = yv12;//指向保存y数据部分
    pv = py+(width*height);//指向保存v数据部分
    pu = pv+((width*height)/4);

    unsigned int h=0;
    unsigned int w=0;

    unsigned int wy=0;//一行y数据中第n个数据
    unsigned int huv=0;//uv数据部分的第n行
    unsigned int wuv=0;//一行uv数据中第n个数据

    for(h=0;h<height;h+=2)
    {
        wy=0;
        wuv=0;
        offset = h * linesize;//yuyv中y分量第一行的偏移
        offset1 = (h + 1) * linesize;//yuyv中y分量第二行的偏移
        offsety = h * width;//yv12中y分量第一行偏移
        offsety1 = (h + 1) * width;//yv12中y分量第二行偏移
        offsetuv = huv * uvlinesize;//yv12中uv分量的偏移

        for(w=0;w<linesize;w+=4)
        {
            /*y00*/
            py[wy + offsety] = yuyv[w + offset];
            /*y01*/
            py[(wy + 1) + offsety] = yuyv[(w + 2) + offset];
            /*y10*/
            py[wy + offsety1] = yuyv[w + offset1];
            /*y11*/
            py[(wy + 1) + offsety1] = yuyv[(w + 2) + offset1];
            /*v0*/
            pv[wuv + offsetuv] = yuyv[(w + 3) + offset];
            /*u0*/
            pu[wuv + offsetuv] = yuyv[(w + 1) + offset];

            wuv++;
            wy+=2;
        }
        huv++;
    }
    return 0;
}



//  for some reason, get YUYV  , so change  yuyv to yv12 do  264 encode
// YUYVYUYVYUYV... >>> YYYYYUV
int yuyv_to_yv12(unsigned char *yuyv, unsigned char *yv12, unsigned short width, unsigned short height)
{
    unsigned char *py;
    unsigned char *pu;
    unsigned char *pv;
    int y_count = 0 ; 
    int u_count = 0 ; 
    int v_count = 0 ;
    int yuv422_length = 2*(width*height); 
    bool is_U = true ; 
    bool is_Y = false ;
    bool is_drop_U = true;
    bool is_drop_Y = true ;
    unsigned char *pyuyv = yuyv ;
    

    py = yv12;
    pv = py+(width*height);//指向保存v数据部分
    pu = pv+((width*height)/4);   
	
    for(int i = 0 ; i < yuv422_length ; ++i)
	{
		//SKY_LOG(5,(TAG_H264ENCODER, "i is %d!!!!",i));		
		// cp the first y 	
		
		if( 0 == i)
		{	
			*py++ = *(yuyv + i);			
			y_count ++ ; 
		}
		else 
		{
			// except  i = 0 , if i is Even numbers ,cp to Y planes
			if(0 == i%2)
			{				
				*py++ = *(yuyv + i) ;
				y_count ++ ; 
				//SKY_LOG(5,(TAG_H264ENCODER, "i is %d!!!!,Y count is %d",i,y_count));
			}
			else 
			{
				// then use is_U as NOT gate , do intermittent copy 
				if(is_U)
				{	
					if(!is_drop_U)
					{
						*pv++ = *(yuyv + i) ; 
						u_count ++ ; 
						is_drop_U = true;
					}
					else
					{
						is_drop_U = false ; 
					}
					is_U = false ;					
					//SKY_LOG(5,(TAG_H264ENCODER, "i is %d!!!! U count is %d",i,u_count));
				}
				else 
				{	
					if(!is_drop_Y)
					{
						*pu++ = *(yuyv + i); // Segmentation fault here  change cp  method 
						v_count ++ ;
						is_drop_Y = true ;
					}
					else
					{
						is_drop_Y = false ;
					}
					is_U = true ;
					
					//SKY_LOG(5,(TAG_H264ENCODER, "i is %d!!!! V count is %d",i,v_count));
				}
			}			
		}
	}	

    return 0;
}



bool init_x264_encoder(Encoder * enc,int width,int height)
{
	enc->param = (x264_param_t*)malloc(sizeof(x264_param_t));
	enc->picture = (x264_picture_t*)malloc(sizeof(x264_picture_t));	
	enc->picture->i_pts = 0 ; 
	// set default  param  	
	// todo improvements later  
	x264_param_default(enc->param); 
	// set width and height 
	enc->param->i_width = width;
	enc->param->i_height = height ;

	enc->param->rc.i_lookahead = 0; 

	// set fps  
	enc->param->i_fps_num = 10 ; 
	enc->param->i_fps_den = 1 ;

	// set baseline  
	x264_param_apply_profile(enc->param, x264_profile_names[0]);

	 // open encoder 
	if( (enc->handle = x264_encoder_open(enc->param)) == 0 )
	{
	 	SKY_LOG(1,(TAG_H264ENCODER,"Could not Open x264_encoder"));
	 	// will free when encoder close  or now 
	 	//free(enc->param); 
	 	//free(enc->picture);
	 	return false ;
	}
	 
	 // create a new picture   malloc enc->picture here  X264_CSP_I422   X264_CSP_YV16  X264_CSP_NV16
	//x264_picture_alloc(enc->picture,X264_CSP_YV12,enc->param->i_width,enc->param->i_height);
	//enc->picture->img.i_csp = X264_CSP_YV12 ; 	
	//x264_picture_alloc(enc->picture,X264_CSP_NV12,enc->param->i_width,enc->param->i_height);	
	//enc->picture->img.i_csp = X264_CSP_NV12 ; 
	x264_picture_alloc(enc->picture,X264_CSP_YV12,enc->param->i_width,enc->param->i_height);	
	enc->picture->img.i_csp = X264_CSP_YV12 ; 
	enc->picture->img.i_plane = 3 ;   
	
	return true ; 
	 
}


// do encode  
int encode_frame(Encoder * enc,int type,uint8_t * yuv_in,uint8_t * h264_out)
{
	x264_picture_t pic_out ; 
	int nNal = -1 ; 	
	long i = 0 ; 
	int h264_length = 0 ;
	uint8_t *p_out = h264_out;
	// set y/u/v plane 
	char *y = enc->picture->img.plane[0] ; 
	char *u = enc->picture->img.plane[1] ; 
	char *v = enc->picture->img.plane[2] ;	
	
	long yuv420_length = (3.0/2.0)*enc->param->i_width * enc->param->i_height; 
	SKY_LOG(1,(TAG_H264ENCODER, "yuv420_length is  %d",yuv420_length));

	unsigned char* yv12 = (unsigned char*)malloc(yuv420_length*sizeof(unsigned char));

	if( 0 == yv12)
	{
		SKY_LOG(1,(TAG_H264ENCODER, "YV12 malloc failed "));
	}

	yuyv_to_yv12_2(yuv_in,yv12,enc->param->i_width, enc->param->i_height);
	
	int y_length  =  enc->param->i_width * enc->param->i_height; 
	int yu_length = y_length + ( yuv420_length - y_length ) / 2 ; 
	
	for(i = 0 ; i < yuv420_length ; ++i)
	{
		// read y data to y plane plane[0]
		if(i < y_length)
		{
			// cp y value 
			*y++ = *(yv12 + i)  ; 
		}
		else if(i < yu_length)
		{
			*u++ = *(yv12 + i) ; 
		}
		else 
		{
			*v++ = *(yv12 + i);
		}
	}		

	/*	
	// deal with yuv422
	long yuv422_length =  2*enc->param->i_width * enc->param->i_height;
	bool is_U = true ;
	bool is_drop_U = false ; 
	bool is_drop_Y = false; 
	// now use YUY2  belongs to yuv422sp   YUV data store as  [YUYVYUYVYUYV...]   
	int y_count = 0 ; 
	int u_count = 0 ;
	int v_count = 0 ;
	for(i = 0 ; i < yuv422_length ; ++i)
	{
		//SKY_LOG(5,(TAG_H264ENCODER, "i is %d!!!!",i));		
		// cp the first y 	
		
		if( 0 == i)
		{	
			*y++ = *(yuv_in + i); 
			y_count ++ ; 
		}
		else 
		{
			// except  i = 0 , if i is Even numbers ,cp to Y planes
			if(0 == i%2)
			{				
				*y++ = *(yuv_in + i) ;
				y_count ++ ; 
				//SKY_LOG(5,(TAG_H264ENCODER, "i is %d!!!!,Y count is %d",i,y_count));
			}
			else 
			{
				// then use is_U as NOT gate , do intermittent copy 
				if(is_U)
				{	
					if(!is_drop_U)
					{
						*u++ = *(yuv_in + i) ; 
						u_count ++ ; 
						is_drop_U = true;
					}
					else 
					{
						is_drop_U = false ; 
					}
					is_U = false ;
					
					//SKY_LOG(5,(TAG_H264ENCODER, "i is %d!!!! U count is %d",i,u_count));
				}
				else 
				{	
					if(!is_drop_Y)
					{
						//*v++ = *(yuv_in + i); // Segmentation fault here  change cp  method 
						v_count ++ ;
						is_drop_Y = true ;
					}
					else
					{
						is_drop_Y = false ;
					}
					is_U = true ;
					
					//SKY_LOG(5,(TAG_H264ENCODER, "i is %d!!!! V count is %d",i,v_count));
				}
			}			
		}
	}	
	*/	


	// set Frame type  
	switch(type){	
	case 0 : 
		enc->picture->i_type = X264_TYPE_P ; 
		break ;
	case 1 :
		enc->picture->i_type = X264_TYPE_IDR  ; 
		break ; 
	case 2: 
		enc->picture->i_type = X264_TYPE_I ; 
		break ; 
	default : 
		 enc->picture->i_type = X264_TYPE_AUTO;
		break ;
	}

	//SKY_LOG(5,(TAG_H264ENCODER, "Do encode_frame!!!!!"));
	// send frame to x264 and do encode
	//NOTICE  264 stream is in enc->nal , pic_out  is only return encode picture  info such as  i_type , quality ... 
	int ret = x264_encoder_encode(enc->handle , &(enc->nal),&nNal,enc->picture,&pic_out);
	if(ret < 0 )
	{
		SKY_LOG(1,(TAG_H264ENCODER, "x264_encoder_encode failed error is %d",ret));
		return 0 ;
	}
	enc->picture->i_pts++ ;

	// get 264 payload ,return h264 NALS
	for(i = 0 ; i < nNal ; ++i)
	{
		memcpy(p_out, enc->nal[i].p_payload,enc->nal[i].i_payload);
		p_out += enc->nal[i].i_payload ;
		h264_length += enc->nal[i].i_payload;
	}

	return h264_length ;
	
}

void close_x264_encoder(Encoder * enc)
{
	// free picture and set to null
	if(enc->picture)
	{
		x264_picture_clean(enc->picture); 
		free(enc->picture);
		enc->picture = 0 ;
	}

	// free param  
	if(enc->param)
	{
		free(enc->param);
		enc->param = 0 ; 
	}

	if(enc->handle)
	{
		x264_encoder_close(enc->handle); 
	}
	// should i free the handle ?
	free(enc->handle);	
}


