/*!
*	master of all device abilities 
*	manage and setup each ability to device
*	func should set after TXDevice init 
* 	@add by 6K at 2015/12/16
*/


/*!
* 	load ability  to deal with DataPoint MSG, will load after Device init 
*/

void load_datapoint_handler_ability() ; 

/*!
*	load ability to control rotate for ipcamera 
*/
void load_control_rotate_ability();

/*!
*	load ability to Handle file recv and  send 
*/

void load_file_transfer_ability(); 

/*!
*	load ability to support OTA 
*/

void load_OTA_ability();

/*!
*	load TV video Push ability 
*/

void load_native_video_push_ability();


/*!
*	load all choosed ability to device 
*/
void load_abilities();


/*!
*	start the ability to do voice link 
*/
void start_voice_link_ability();


