/*!
 *	for Alarm event 
 * 	1, call back for send process 
 *	2, alarm event funcs for device ,include pic , msg and notify alarm 
 * 	@add  by 6K at 2015/12/13
 */
 
#include <stdio.h>
#include <string.h>

#include "TXSDKCommonDef.h"
#include "TXDeviceSDK.h"
#include "TXMsg.h"

// call back  funcs for check if msg is send to QQ Server 
void sky_on_send_alarm_file_progress(const unsigned int cookie, unsigned long long transfer_progress, unsigned long long max_transfer_progress)
{
    printf("on_send_alarm_file_progress, cookie[%u]\n", cookie);
}

void sky_on_send_alarm_msg_ret(const unsigned int cookie, int err_code)
{
    printf("on_send_alarm_msg_ret, cookie[%u] ret[%d]\n", cookie, err_code);
}

void sky_on_send_notify_msg_ret(unsigned int cookie , int err_code )
{
	printf("<6K-DEBUG> on_send_alarm_msg_ret, cookie[%u] ret[%d]\n",cookie,err_code);
}

// send pic alarm  msg_id is 1 ,which is configure in the QQ Server Web 
void sky_send_pic_alarm()
{   
    structuring_msg msg = {0};
    msg.msg_id = 1;  
    msg.file_path = "./alarm.png";
    msg.thumb_path = "./thumb.png";
    msg.title = "发现异常";
    msg.digest = "客厅发现异常";
    msg.guide_words = "点击查看";
    
    tx_send_msg_notify notify = {0};
    notify.on_file_transfer_progress = sky_on_send_alarm_file_progress;
    notify.on_send_structuring_msg_ret = sky_on_send_alarm_msg_ret;
    tx_send_structuring_msg(&msg, &notify, 0);
}


// send audio alarm  msg_id is 2  
void sky_send_audio_alarm()
{
    
    structuring_msg msg = {0};
    msg.msg_id = 2;  
    msg.file_path = "./test.mp3";
    msg.title = "语音警报";
    msg.digest = "收到语音警报";
    msg.guide_words = "点击查看";
    
    tx_send_msg_notify notify = {0};
    notify.on_file_transfer_progress = sky_on_send_alarm_file_progress;
    notify.on_send_structuring_msg_ret = sky_on_send_alarm_msg_ret;
    tx_send_structuring_msg(&msg, &notify, 0);
}

//sen notify alarm  the msg_id is  3 ,
void sky_send_notify_alarm()
{
	int msg_id = 3; 
	char* msg  = " test alarm by6K";
	tx_send_notify_msg(msg_id,"Test by6K",sky_on_send_notify_msg_ret,0,0,0);
}




