/*
*	used to create snNum which is based on MAC address 
*	license is created by sn num and get from service of handsome jjf
*	
*  @ add by 6K 2015/12/10
*/
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/ioctl.h>
#include<net/if.h>
#include <netinet/in.h>
#include <linux/if.h>   
#include<TXSDKCommonDef.h>
#include<fcntl.h>
#include <arpa/inet.h>
#include "cjson/cJSON.h"
#include "skyAuth.h"




#define PORT 8080
#define MAXBUFSIZE 2048 
//#define IPADDRESS "nj.videochat.skysrt.com"
#define IPADDRESS "42.120.20.31"
#define REGISTE_PATH "/skyqq/eeipc/license/sign"


// get sn num based on mac address
bool getSnNum(unsigned char* snNum)
{ 	
	struct ifreq ifreq;
    int sock;
    //unsigned char szMac[20];
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror ("socket");
        return false;        
    }
    //only use eth0 
    strcpy (ifreq.ifr_name, "eth0");   

    if (ioctl (sock, SIOCGIFHWADDR, &ifreq) < 0)
    {
        perror ("ioctl");
        return false;
    }    
    sprintf( snNum, "%02X%02X%02X%02X%02X%02X0000", (unsigned char) ifreq.ifr_hwaddr.sa_data[0], (unsigned char) ifreq.ifr_hwaddr.sa_data[1], (unsigned char) ifreq.ifr_hwaddr.sa_data[2], (unsigned char) ifreq.ifr_hwaddr.sa_data[3], (unsigned char) ifreq.ifr_hwaddr.sa_data[4], (unsigned char) ifreq.ifr_hwaddr.sa_data[0]);
	
	for(int i = 0 ; i < 6 ; ++i)
	{
		printf("%d is %02X\t" ,i,(unsigned char) ifreq.ifr_hwaddr.sa_data[i]);
	}
	
    printf("6K-DEBUG, get SNNUM is %s\n", snNum);
    
    return true ;
	
}


/*!
* create json String 
* param1  unsigned char* sn  snNum 
* return  JSON String to post 
*/
unsigned char * makePostJSON(unsigned char* sn)
{	
	cJSON *pJSONRoot = NULL ;
	pJSONRoot = cJSON_CreateObject();	
	if(NULL == pJSONRoot)
	{
		printf("JSON Create failed\n");
		return NULL;
	}
	cJSON_AddStringToObject(pJSONRoot, "guid", sn);

	char * p = cJSON_Print(pJSONRoot);

	if(p == NULL)
	{
		printf("JSON Create failed\n"); 
		cJSON_Delete(pJSONRoot);
		return NULL ;
	}
	else
	{
		return p;
	}

}


/*!
* get license from json string 
* param 1 json string which recv from server
* param 2 address of license , will return the license 
* return  false or true 
*/

bool parseJSONString(char * jsonStr , char* license)
{
	if(jsonStr == NULL)
	{
		printf("JSON get NULL jsonStr\n");
		return false ;
	}
	
	cJSON * pJson = cJSON_Parse(jsonStr);
	if(pJson == NULL)
	{
		printf("JSON get NULL pJson\n");
		return false ;
	}
	
	cJSON * pSub = cJSON_GetObjectItem(pJson, "code");
	if(pSub == NULL )
	{
		printf("JSON get NULL pSub\n");
		//cJSON_Delete(pJson);
		return false ;
	}
	else 
	{
		int retCode = pSub->valueint ; 
		// code == 0 is  return success 
		if(retCode != 0)
		{
			cJSON_Delete(pSub);
			//cJSON_Delete(pJson);			
			return false ;
		}
	}

	
	pSub = cJSON_GetObjectItem(pJson, "license");
	if(pSub == NULL )
	{
		printf("JSON get NULL pSub\n");
		cJSON_Delete(pJson);
		return false ;
	}
	else 
	{
		strcpy(license,pSub->valuestring);		
		printf("JSON get license is %s\n", license);
	}

	cJSON_Delete(pSub);
	//cJSON_Delete(pJson);
	return true ;
	
	
}



/*!
*  param1 
*  param2 
*  return  success or failed 
*/
bool getLicenseFromService(char * license , unsigned char* snNum)
{	
	//such as  method in  RK2818/external/lollipop_wifi/common/Common_function.c   @ jinxiaoqiong@172.22.197.184
	char *sendRequestBuf ;

	char *requestJSON = makePostJSON(snNum);
	if(!requestJSON)
	{
		printf("JSON makePostJSON failed\n" );
		return false ;
	}

	printf("JSON makePostJSON is %s\n", requestJSON);
	
	sendRequestBuf = (char*)malloc(strlen(requestJSON)+256);
	if(!sendRequestBuf)
	{
		printf("malloc failed!\n");
		return false ;
	}
	
	memset(sendRequestBuf , 0 ,strlen(requestJSON) + 256);

	int sockfd, sendCount , recvCount ;
	int requestJSONLength = strlen(requestJSON);
	
	struct sockaddr_in dataServerAddr ; 
	char recvMsgBuf[MAXBUFSIZE] = {0};

	// create socket and connect to server
	if( (sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0 )
	{
		printf("create socket failed!\n");
		goto failed; 
	}
	bzero(&dataServerAddr, sizeof(dataServerAddr));
	dataServerAddr.sin_family = AF_INET ; 
	dataServerAddr.sin_port = htons(PORT) ;

	if((inet_pton(AF_INET, IPADDRESS,&dataServerAddr.sin_addr)) < 0 )
	{
		printf("inet_pton failed\n");
		goto failedClose ;		
	}

	if(connect(sockfd,(struct sockaddr*)&dataServerAddr, sizeof(dataServerAddr)) < 0 )
	{
		printf("connect to server failed\n");
		goto failedClose ;
	}

	printf("Connect successed !!!!\n");

	// create POST msg and send to server
	sprintf(sendRequestBuf,\
	"POST %s HTTP/1.0\r\n"\
	"User-Agent: Apache-HttpClient/4.2.6 (java 1.5)\r\n"\
    "Host: %s\r\n"\
    "Connection: Keep-Alive\r\n"\
    "Content-Length: %d\r\n"\
    "Content-Type: application/json\r\n\r\n%s",\
    REGISTE_PATH, IPADDRESS, requestJSONLength, requestJSON);	

    printf("Send_request_buf:[%d, %s]\n",requestJSONLength , sendRequestBuf);

    sendCount = send(sockfd, sendRequestBuf , strlen(sendRequestBuf) , 0 );
	if( sendCount < 0 )
	{
		printf("Send request msg to server failed !\n");
		goto failedClose ;
	}
	else 
	{
		printf("Send request msg to server success!!\n");
	}

	// recever return msg from server 
	memset(recvMsgBuf , 0 ,MAXBUFSIZE);
	recvCount = recv(sockfd , recvMsgBuf , MAXBUFSIZE - 1, 0 );
	recvMsgBuf[MAXBUFSIZE] = '\0'; // set end str
	printf("recv from server ,return[%d,%s]\n",recvCount, recvMsgBuf);

	// get return Json str and parser json str
	if( strlen(recvMsgBuf) > 155 )
	{
		// Pointer is   MAGIC POWER 	!!!
		printf("Get JSON is %s\n" , recvMsgBuf+155);
		parseJSONString(recvMsgBuf+155, license);
	}	

	close(sockfd);    	
		
	free(sendRequestBuf); 
	return true ;
	
failedClose : 
	close(sockfd);	
	
failed : 
	free(sendRequestBuf); 
	return false ;
	
}


/*
// unit test demo
int main()
{
	//unsigned char snNum[20] = {0} ; 
	//bool ret = getSnNum(snNum); 
	//if(ret)
	//{
	//	 printf("6K-DEBUG, get SNNUM is %s\n", snNum);	
	//}	
	//char * jsonStr = makePostJSON(snNum); 
	//if(jsonStr != NULL)
	//{
	//	printf("JSON makePostJSON is %s\n", jsonStr);
	//}

	char license[256] = {0};		
	unsigned char snNum[20] = {0} ; 	
	if( !getSnNum(snNum) )
	{
		printf("6K-DEBUG , get snNum failed\n");
		return false ;
	}
	bool ret = getLicenseFromService(license ,snNum);
	{
		if(ret)
		{
			printf("getLicense DONE!!\n");
		}
	}	

	return 0 ;	
}
*/

