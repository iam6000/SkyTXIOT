/*!
*  for OTA ability 
*  allow device has the ability to do TOA update 
*  OTA update is initiate from mobile QQ 
*  see http://iot.open.qq.com/wiki/index.html#!FUNC/OTA.md for details
@add by 6K at 2015/12/16 
*/

/*!
*	if there is any useable firmware version , mobile QQ  will send the info of firmware to device 
* 	param:pkg_size  the sie of the new firmware, size : Byte 
*	param£ºtitle + desc	the desc of this update 
*	param: target_version	version of the target firmware
*	return: if return 0 , sdk will begin the firmware download
*/
void cb_on_new_pkg_come(unsigned long long from, unsigned long long pkg_size, const char * title, const char * desc, unsigned int target_version) ; 


/*!
*	download the firmware and show the progress to mobile QQ
*	param£ºdownload_size 
*	param: total_size  
*/
void cb_on_download_progress(unsigned long long download_size, unsigned long long total_size);


/*!
*	rebport download result 
*	param:ret_code 
	* 0   success 
	* 2   unkown error
	* 3  401
	* 4  write file error
	* 5   net error
	* 7   404
	* 8   503
	* 9   stop by mobile qq
	* 10  url error
	* 11  md5 error
*/
void cb_on_download_complete(int ret_code); 


/*!
*	update firmware should begin after receive this confim  
*/
void cb_on_update_confirm();


