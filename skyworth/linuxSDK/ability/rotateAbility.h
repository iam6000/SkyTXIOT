/*!
*	set bitrate and  rotate by mobile QQ
*	this call back will set to tx_ipcamera_set_callback  
*	@add by 6K at 2015/12/16
*/

/*!
*	set bitrate to device 
*	param: definition   
		def_low     = 1  
		def_middle  = 2
		def_high    = 3
*	param: cur_definition   set to null 
*	param: cur_definition_length   set to null
*/
int cb_on_set_definition(int definition, char *cur_definition, int cur_definition_length);


/*!
*	let the ipc rotage 
*	param: rotate_direction   
		rotate_direction_left   = 1,    
		rotate_direction_right  = 2,    
		rotate_direction_up     = 3,    
		rotate_direction_down   = 4,    
*	param: rotate_degree 
		rotate_degree_h_min = 0,   // horizontal
		rotate_degree_h_max = 360,  //horizontal
		rotate_degree_v_min = 0,   //vertical
		rotate_degree_v_max = 180,//vertical
*/
int cb_on_control_rotate(int rotate_direction, int rotate_degree);


