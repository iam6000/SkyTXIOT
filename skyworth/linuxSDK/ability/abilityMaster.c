/*!
*	master of all device abilities 
*	manage and setup each ability to device
*	func should set after device init
*	
* 	@add by 6K at 2015/12/16
*/
#include<stdio.h>
#include<string.h>
#include"TXDataPoint.h"
#include"TXDeviceSDK.h"
#include"TXIPCAM.h"
#include"TXFileTransfer.h"
#include"TXOTA.h"
#include"TXTVSDK.h"

// include ability headers  
#include"dataPointAbility.h"
#include"fileRecvAbility.h"
#include"OTAAbility.h"
#include"rotateAbility.h"
#include"audioVideoAbility.h"
#include"videoPushAbility.h"
#include"barrageAbility.h"
#include"voiceLinkAbility.h"

// sky log
#include"../skyLog.h"


#define USE_AUDIOVIDEO 1
#define USE_DATAPOINT 1
#define USE_ROTATE_CONTROL 0 
#define USE_OTA 1 
#define USE_FILE_RECV 1 
#define USE_NATIVE_VIDEO_PUSH 0  
#define USE_BARRAGE 0 


#define TAG_MASTER  "AbilityMaster"



void load_datapoint_handler_ability()
{
	tx_data_point_notify notify = {0} ; 
	notify.on_receive_data_point =  cb_on_receive_data_point ; 
	tx_init_data_point(&notify);
}

void load_control_rotate_ability()
{
	tx_ipcamera_notify ipcamera_notify = {0};
	ipcamera_notify.on_control_rotate = cb_on_control_rotate;
	ipcamera_notify.on_set_definition = cb_on_set_definition;
	tx_ipcamera_set_callback(&ipcamera_notify);
}


void load_file_transfer_ability()
{
	tx_file_transfer_notify fileTransferNotify = {0};
	fileTransferNotify.on_transfer_complete = cb_on_transfer_complete;
	fileTransferNotify.on_transfer_progress = cb_on_transfer_progress;
	fileTransferNotify.on_file_in_come      = cb_on_recv_file;
	tx_init_file_transfer(fileTransferNotify, "/tmp/ramdisk/");
}

void load_OTA_ability()
{
	tx_ota_notify ota_notify = {0};
	ota_notify.on_new_pkg_come		= cb_on_new_pkg_come;
	ota_notify.on_download_progress = cb_on_download_progress;
	ota_notify.on_download_complete = cb_on_download_complete;
	ota_notify.on_update_confirm	= cb_on_update_confirm;
	tx_init_ota(&ota_notify, 10*60, "/tmp/update_pkg.tar");
}

void load_audioVideo_ability()
{
	tx_av_callback avCallBack = {0} ;
	avCallBack.on_start_camera = sky_start_camera ; 
	avCallBack.on_stop_camera =  sky_stop_camera ; 
	avCallBack.on_start_mic = sky_start_mic ; 
	avCallBack.on_stop_mic = sky_stop_mic ;
	avCallBack.on_recv_audiodata = sky_recv_audiodata ; 
	avCallBack.on_set_bitrate = sky_set_bitrate ;	
	// do not init now ,This ability init when online status changed 
}


void load_native_video_push_ability()
{
	tx_tv_notify tvNotify = {0} ; 
	tvNotify.on_bind_complete = sky_on_bind_complete; 
	tvNotify.on_receive_video_push = sky_on_receive_video_push ; 
	//  undefined reference to `tx_set_tv_notify'   may  not found this func in TX.so
	// tx_set_tv_notify(&tvNotify);    
}

void load_barrage_ability()
{
	tx_barrage_notify barrageNotify = {0};
	barrageNotify.on_receive_barrage_msg = NULL ;
	//tx_set_barrage_notify(&barrageNotify); // removed since sdk1.3
}


void  start_voice_link_ability()
{
	if(start_voide_link())
	{
		SKY_LOG(3,(TAG_MASTER,"voice link success"));
	}
	else 
	{
		SKY_LOG(3,(TAG_MASTER,"voice link failed"));
	}
	
}




void load_abilities()
{	
	printf("***************************************************************************************\n");
	SKY_LOG(2,(TAG_MASTER,"\nload device abilities as blow:\n [USE VideoAudio : %d]\n [USE DataPoint MSG : %d] \n "
								"[USE RotateControl : %d]\n [USE OTA update : %d]\n "
								"\[USE FileRecv : %d]\n [USE_NATIVE_VIDEO_PUSH : %d ] \n"
								"\ [USE_BARRAGE : %d] \n",									
									USE_AUDIOVIDEO ,
									USE_DATAPOINT,
									USE_ROTATE_CONTROL,
									USE_OTA,
									USE_FILE_RECV,
									USE_NATIVE_VIDEO_PUSH,
									USE_BARRAGE));	
	printf("***************************************************************************************\n");
	if(USE_AUDIOVIDEO)
	{
		load_audioVideo_ability();
	}
	if(USE_DATAPOINT)
	{
		load_datapoint_handler_ability(); 
	}
	if(USE_ROTATE_CONTROL)
	{
		load_control_rotate_ability();
	}
	if(USE_OTA)
	{
		load_OTA_ability();
	}
	if(USE_FILE_RECV)
	{
		load_file_transfer_ability();
	}
	if(USE_NATIVE_VIDEO_PUSH)
	{
		load_native_video_push_ability();
	}
	if(USE_BARRAGE)
	{
		load_barrage_ability();
	}
}


