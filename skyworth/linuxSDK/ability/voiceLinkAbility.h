/*!
*	used for voice Link 
*	get pcm data and send to  x_fill_audio 
* 	@add by 6K at 2015/12/18
*/

#include"TXVoiceLink.h"
#include"tinyalsa/include/tinyalsa/asoundlib.h"   // will change later 

//define struct to set audio format
typedef struct {
	unsigned int card ; 
	unsigned int device ; 
	unsigned int channels ; 
	unsigned int rate ; 
	enum pcm_format format;
	unsigned int period_size ; 
	unsigned int period_count ; 
} sky_audio_formats;

/*!
	call back func to get the audio decode Result 
	this call back 	
*/
void sky_on_wifi_link_return(tx_voicelink_param *pparam);



/*!
* main func for voice Link 
* start audio capture and send pcm data to TXDevice to do voice Analysis 
*/
bool start_voide_link() ; 



/*-----------------audio capture below , will move to basic ability  soon after ----------------------*/ 

/*!
*	begin audio capture , must called after sky_audio_device_init
*/
//bool sky_audio_capture(sky_audio_formats formats );

//TODO  func to check audio formats




