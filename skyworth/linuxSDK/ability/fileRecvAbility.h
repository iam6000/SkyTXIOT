/*!
*	recv audio file from mobile QQ  
*	this call back will set to tx_init_file_transfer 
*	@add by 6K at 2015/12/16
*/

void cb_on_transfer_progress(unsigned long long transfer_cookie, unsigned long long transfer_progress, unsigned long long max_transfer_progress) ; 


void cb_on_recv_file(unsigned long long transfer_cookie, const tx_ccmsg_inst_info * inst_info, const tx_file_transfer_info * tran_info) ; 


/*!
*	report that audio file is ready , device could get the audio file from tran_info and play the file
*/
void cb_on_transfer_complete(unsigned long long transfer_cookie, int err_code, tx_file_transfer_info* tran_info);



