/*!
*	set bitrate and  rotate by mobile QQ
*	this call back will set to tx_ipcamera_set_callback  
*	@add by 6K at 2015/12/16
*/
#include <stdio.h>
#include <string.h>


int cb_on_set_definition(int definition, char *cur_definition, int cur_definition_length)
{
    printf("==============cb_on_set_definition, definition:%d\n", definition);
    return 0;
}

int cb_on_control_rotate(int rotate_direction, int rotate_degree)
{
    printf("===============cb_on_control_rotate, rotate_direction:%d, rotate_degree:%d\n", rotate_direction, rotate_degree);
    return 0;
}
