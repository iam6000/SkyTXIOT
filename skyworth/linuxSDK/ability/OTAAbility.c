/*!
*  call back for OTA Module  
*  @ add by 6K 
*/
#include <stdio.h>
#include <string.h>

#include "TXSDKCommonDef.h"
#include "TXDeviceSDK.h"
#include "TXFileTransfer.h"



int cb_on_new_pkg_come(unsigned long long from, unsigned long long pkg_size, const char * title, const char * desc, unsigned int target_version)
{
    //todo
    return 0;
}


void cb_on_download_progress(unsigned long long download_size, unsigned long long total_size)
{
    //todo
}


void cb_on_download_complete(int ret_code)
{
    //todo
}


void cb_on_update_confirm()
{
    //todo 
}


