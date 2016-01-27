/*!
*  init devices 
*  first, you should get the guid and license.
*  then, add callback funcs for devices,
*  last, set the SDK path , and call tx_init_device func 
*  @ add by 6K 2015/12/15
*/

#include<stdio.h>
#include<string.h>

#include"TXSDKCommonDef.h"
#include"TXDeviceSDK.h"
#include"TXOTA.h"
#include"TXFileTransfer.h"
#include"TXIPCAM.h"
#include"skyAuth.h"
#include"ability/abilityMaster.h"
#include"ability/audioVideoAbility.h"
#include"skyLog.h"


#define PLAT_FORM  "linux"
#define DEVICE_NAME "Demo1"
#define PRODUCT_ID 1700002286 
#define PRODUCT_VERSION 1
#define SERVER_PUBLIC_KEY "04D3E455A6FF683959933DCB14786ABE0231B790D65B1803768A0D64398B48B1A190DD96B1A31159AD4905D93284B0B335"
#define TAG_SKYDEVICE  "skyDevice.c"

static bool g_start_av_service = false ;

/*!
*	log func ,set to TX_log , print TX log
*/
void log_func(int level , const char* module, int line, const char* message)
{
	printf("%s\n", message);
}

// init  tx_av_callback  funcs  
bool init_av_callback_notify(tx_av_callback *avCallBack)
{
	if(avCallBack)
	{
		avCallBack->on_start_camera = sky_start_camera ; 
		avCallBack->on_stop_camera =  sky_stop_camera ; 
		avCallBack->on_start_mic = sky_start_mic ; 
		avCallBack->on_stop_mic = sky_stop_mic ;
		avCallBack->on_recv_audiodata = sky_recv_audiodata ; 
		avCallBack->on_set_bitrate = sky_set_bitrate ;
		return true ; 
	}
	else 
	{
		return false ;
	}
}


// notify online status change, 11 means online , 21 means offline
// when online , set tx_av_callback and start av_service 
void  on_online_status(int old, int new) 
{
	if(11 == new && ! g_start_av_service)
	{
		tx_av_callback avCallBack = {0} ; 
		if(init_av_callback_notify(&avCallBack))
		{
			int ret = tx_start_av_service(&avCallBack) ; 
			if(err_null ==  ret )
			{
				SKY_LOG(3,(TAG_SKYDEVICE,"tx_start_av_service successed!"));				
			}
			else
			{
				SKY_LOG(1,(TAG_SKYDEVICE,"tx_start_av_service failed for [%d] \n", ret));				
			}
		}	
		g_start_av_service = true ; 
	}
}


// login callback  errcode 0 means login success ! 
void on_login_complete(int errcode)
{
	SKY_LOG(3,(TAG_SKYDEVICE,"on_login_complete | code[%d]\n", errcode));	
}

// bindlist change notify callback ,notify when the bindlist has change 

void on_binder_list_change(int err_code, tx_binder_info * pBinderList , int nCount)
{
	if (err_null != err_code)
	{
		SKY_LOG(1,(TAG_SKYDEVICE,"on_binder_list_change failed, errcode:%d\n", err_code));		
		return;
	}
   
    int i = 0;
    for (i = 0; i < nCount; ++i )
    {
        //SKY_LOG(3,("binder uin[%ld], nick_name[%s]\n", pBinderList[i].uin, pBinderList[i].nick_name));
    }
}

/*!
*	init tx_device_notify callback funcs,  tx_device_notify will use when device init 
*	when login result return from the service , call  on_login_complete  
*  	when online status change ,eg,offline >>> online , will call  on_online_status and start av service of device 
*	whne binder list change,  call on_binder_list_change
*/ 
void  init_device_notify(tx_device_notify *notify)
{
	if(notify)
	{
		notify->on_login_complete = on_login_complete ; 
		notify->on_online_status = on_online_status ;
		notify->on_binder_list_change = on_binder_list_change; 
		return ;
	}
}

/*!
*  do Device init ,device begin at this func , regiest to Tencent Service
*  set basic param to device
*  set listen callback funcs to device
*/
bool initDevice()
{	
	unsigned char snNum[20] = {0} ;
	unsigned char license[256] = {0};
	
	if(!getSnNum(snNum))
	{
		SKY_LOG(1,(TAG_SKYDEVICE,"getSnNum failed"));
		return false ;
	}
	
	if(!getLicenseFromService(license, snNum))
	{
		SKY_LOG(1,(TAG_SKYDEVICE,"get license failed"));
		return false ;
	}
	
	// basic info of device 
	tx_device_info info = {0} ; 
	//init param 	
	
	info.os_platform = PLAT_FORM ; 
	info.device_name = DEVICE_NAME ; 
	info.device_serial_number =  (char*)snNum ;
	info.device_license  = (char*)license; 
	info.product_version = PRODUCT_VERSION ; 
	info.product_id = PRODUCT_ID;

	info.network_type = network_type_wifi ;
	info.server_pub_key = SERVER_PUBLIC_KEY ;
	
	//register login , online_status and binder list  notify call back 
	tx_device_notify notify = {0} ; 
	init_device_notify(&notify); 
	
	// init sdk path 
	// SDK初始化目录，写入配置、Log输出等信息
	// 为了了解设备的运行状况，存在上传异常错误日志 到 服务器的必要
	// system_path：SDK会在该目录下写入保证正常运行必需的配置信息，请一定注意：system_path目录下的文件必须永久保存，除
	// 用户按了reset键外，其他情况下一律不能删除该目录下文件，包括重启设备，ota升级等，否则sdk将无法正常工作
	// system_path_capicity：是允许SDK在该目录下最多写入多少字节的数据（最小大小：10K，建议大小：100K）
	// app_path：用于保存运行中产生的log或者crash堆栈
	// app_path_capicity：同上，（最小大小：300K，建议大小：1M）
	// temp_path：可能会在该目录下写入临时文件
	// temp_path_capicity：这个参数实际没有用的，可以忽略
	
	tx_init_path init_path = {0} ;
	init_path.system_path = "./" ; 
	init_path.system_path_capicity = 100*1024 ;  
	init_path.app_path = "./" ; 
	init_path.app_path_capicity = 1024*1024 ; 
	init_path.temp_path = "./"; 
	init_path.temp_path_capicity = 10*1024 ;
	

	// set log func  for debug 
	tx_set_log_func(log_func);

	// begin init device to server 
	if(tx_init_device(&info,&notify,&init_path) != err_null)
	{
		SKY_LOG(1,(TAG_SKYDEVICE,"tx_init_device init failed"));		
		return false ;
	}	
	return true ; 
	
}





// Unit test  for  initDevice
#if 0 
int main(int argc , char* agrv[])
{

	// set log func  for debug 
	// open tx log func
	tx_set_log_func(log_func);
	
	char input[100];
	/*
	while(scanf("%s",input))
	{
		if(!strcmp(input,"quit"))
		{
			if(g_start_av_service)
			{
				tx_stop_av_service();
			}
			tx_exit_device();
			break ;
		}
		if(!strcmp(input,"link"))
		{
			start_voice_link_ability();
		}
		sleep(1);
	}
	*/
	if(!initDevice())
	{
		SKY_LOG(1,(TAG_SKYDEVICE,"initDevice failed"));			
		return false ; 
	}
	else 
	{
		SKY_LOG(3,(TAG_SKYDEVICE,"initDevice Success"));		
		load_abilities();	
	}

	
	char input[100];
	while(scanf("%s",input))
	{
		if(!strcmp(input,"quit"))
		{
			if(g_start_av_service)
			{
				tx_stop_av_service();
			}
			tx_exit_device();
			break ;
		}
		if(!strcmp(input,"link"))
		{
			start_voice_link_ability();
		}
		sleep(1);
	}
	
	return 0 ;
}
#endif 

