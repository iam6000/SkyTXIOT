/**
 *  This file get the callback for Audio and Video ,these funcs will register to tx_av_callback 
 *  1,sky_start_camera   start video capture 
 *  2,sky_stop_camera   stop video caputre 
 *  3,sky_set_birtate	 set bitrate not used  now 
 *  4,sky_restart_gop	 I Frame request 
 *  5,sky_start_mic      start Audio capture 
 *  6,sky_stop_mic		stop Audio capture 
 *  7,sky_recv_audio	recevice audio msg 
 *	audio/video ability now use as a basic ability which is add in device
 *  @ add by 6K  2015/12/15
 */

 bool sky_start_camera()  ; 

 bool sky_stop_camera() ; 

 bool sky_set_bitrate(int bit_rate) ; 

 bool sky_restart_gop() ; 

 bool sky_start_mic()  ; 

 bool sky_stop_mic() ; 

 void sky_recv_audiodata(tx_audio_encode_param *param, unsigned char *pcEncData, int nEncDataLen);

