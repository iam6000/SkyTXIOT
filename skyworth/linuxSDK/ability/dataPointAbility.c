/*!
*  method for DataPoint msg handler, will regiest a call back func to deal with DataPoint MSG 
*  which is receive from mobie QQ 
*  about DataPoint see TXDataPoint.h 
*  callback func will set to tx_data_point_notify and call tx_data_point_notify to add to Device 
*  @add by 6000 at 2015/12/15
*/
#include "dataPointAbility.h"  
#include <TXDataPoint.h>
#include <string.h>
#include "cjson/cJSON.h"

#define LOG_TAG "DataPoint>>>>Handler"

#define RecvMediaUrl 100001749
#define CloseDevice  100001822
#define someThing  100
#define anotherThing 200

void  cb_on_report_data_point_ret(unsigned int cookie,int err_code)
{
	//printf("%s, Report Data Point return [%d,%d]\n", LOG_TAG ,cookie ,err_code );
}

void cb_on_ack_data_point_ret(unsigned int cookie,unsigned long long from_client,int err_code)
{
	//printf("%s, Ack Data Point return [%d,%ll,%d]\n",cookie,from_client,err_code);
}


// MSG handler funcs , use to deal with all kinds of DataPoint MSG

// deal with mediaStr 
void handler_media_msg(char * JsonStr)
{
	char Url[256] = {0}; 
	if(!JsonStr)
	{
		//printf("%s, handler Null JSON String\n", LOG_TAG);
		return ;
	}
	// parser Json String 
	cJSON *pJson = cJSON_Parse(JsonStr);
	if( pJson == NULL )
	{
		//printf("%s, JSON get NULL String\n", LOG_TAG);
		return ;
	}
	cJSON * pSub = cJSON_GetObjectItem(pJson, "url");	
	if(pSub == NULL)
	{
		//printf("%s, JSON get NULL Sub String\n", LOG_TAG);
		return ;
	}	
	strcpy(Url,pSub->valuestring);
	cJSON_Delete(pSub);
	//printf("%s, Get Media Url is %s\n", LOG_TAG , Url);	
	//TODO  deal with url		
}


// handler  for  close Device msg  msg id is 100001822
void handler_close_msg(char* JsonStr)
{
	if(!JsonStr)
	{
		//printf("%s, handler Null JSON String\n", LOG_TAG);
		return ;
	}
	cJSON *pJson = cJSON_Parse(JsonStr);
	if( pJson == NULL )
	{
		//printf("%s, JSON get NULL String\n", LOG_TAG);
		return ;
	}
	cJSON * pSub = cJSON_GetObjectItem(pJson, "remoteClose");	
	if(pSub == NULL)
	{
		//printf("%s, JSON get NULL Sub String\n", LOG_TAG);
		return ;
	}	
	int closeValue = pSub->valueint ;
	//printf("%s, JSON get remoteClose value is %d\n", LOG_TAG,closeValue);
	// handler close value	
	
}


void cb_on_receive_data_point(unsigned long long from_client, tx_data_point * data_points, int data_points_count)
{
	//printf("%s, Receive Data Point Msg data point is [%d]\n",LOG_TAG , data_points_count);
	if( NULL == data_points )
	{
		//printf("%s Receive NULL DataPoint MSG\n", LOG_TAG);
		return ;
	}
	// get each dataPoint msg and  send it to different Modules 
	for(int i = 0 ; i < data_points_count ; ++i)
	{
		if( (data_points+i) != NULL)
		{
			switch((data_points + i)->id )
			{
				case RecvMediaUrl : 	
					//printf("%s Receive MediaURL msg :%s\n", LOG_TAG,(data_points + i)->value );
					handler_media_msg((data_points + i)->value);
					break ; 
				case CloseDevice : 
					//call another handler func
					//printf("%s Receive CloseDevice msg :%s\n", LOG_TAG,(data_points + i)->value );
					handler_close_msg((data_points + i)->value);
					break ; 
				default :
					break ; 
			}		
		}
		
	}
	
}


