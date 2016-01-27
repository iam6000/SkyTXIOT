/*!
* header for DataPoint msg handler, will add to Device as one kind of  ability 
@add by 6k at 2015/12/15
*/

#include "TXDataPoint.h" 

/*!
*	call back for recevice data_point , will deal with the dataPoint msg  main func for dataPoint msg event 
*	will use to set to  tx_data_point_notify 
*	PARAM from_client   : client id , where the msg is from 
*	PARAM data_points :  MSG which is in the format of DataPoint 
*	PARAM data_points_count : counts 
*/
void cb_on_receive_data_point(unsigned long long from_client, tx_data_point * data_points, int data_points_count);


/*!
*	for get  ret from func  tx_report_data_point
* 	will used in  func tx_report_data_point
*/
void cb_on_report_data_point_ret(unsigned int cookie, int err_code);


/*!
*	for get ack ret  from func tx_ack_data_points
*	will used in tx_ack_data_points 
*/

void cb_on_ack_data_point_ret(unsigned int cookie, unsigned long long from_client, int err_code);


// unit test  null  
