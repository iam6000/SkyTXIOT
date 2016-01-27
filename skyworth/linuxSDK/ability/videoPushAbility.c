/*!
*	native video Push ability , TXTV support, different from VideoPush use DataPoint MSG 
*	all call back  will set to  tx_tv_notify .
*	see details in TXTVSDK.h
* 	@add by 6K at 2015/12/16
*/

#include<string.h>
#include<stdio.h>
#include"TXSDKCommonDef.h"

#define LOG_TAG_PUSH  "Push Video Ability >>>>"

/*!
*	will be called after the first bind complete  
*/
void sky_on_bind_complete(unsigned long long ddwID, int error)
{
	printf("%s, sky On bind complete\n", LOG_TAG_PUSH);
}


/*!
*	will be called after receive video push from mobile QQ 
* 	but now , i have not found how to push Video to Device , except use DataPoint MSG as we want , but ,just try
*/
void sky_on_receive_video_push(char * pBufReply, int nLenReply)
{
	printf("%s, sky_on_receiver_video_push  receive %s\n" , LOG_TAG_PUSH , pBufReply);
}

