/*!
*	used for voice Link 
*	get pcm data and send to  x_fill_audio 
* 	@add by 6K at 2015/12/18
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"../skyLog.h"
#include"voiceLinkAbility.h"
#define TAG_VOICELINK "VoiceLink"


static bool g_voice_link_finish = false;


//TODO will set wifi in this call back func , use pparam 
void sky_on_wifi_link_return(tx_voicelink_param * pparam)
{
	SKY_LOG(2,(TAG_VOICELINK ,"voiceLink return :\n"));
	SKY_LOG(2,(TAG_VOICELINK ,"get ssid is :%s",pparam->sz_ssid)) ;
	SKY_LOG(2,(TAG_VOICELINK ,"get password is :%s",pparam->sz_password)) ;	
	// set g_voice_link_finish to true to end wifi link	
	g_voice_link_finish = true ; 
}


/*------------------audio capture------------------------------*/
bool start_voide_link()
{
	// add file test ,for check pcm data  
	FILE *testFile ; 
	FILE *pcmFile ; 
	testFile = fopen("audioLink.pcm","wb");
	if(!testFile)
	{
		SKY_LOG(1,(TAG_VOICELINK, "Unable to create audioLink.pcm file\n"));
	}
	pcmFile = fopen("audioLink.pcm","rb"); 
	if(!pcmFile)
	{
		SKY_LOG(1,(TAG_VOICELINK, "Unable to create audioLink1.pcm file\n"));
	}
	
	
	sky_audio_formats  audioFormats ; 
	// set audio format 
	audioFormats.card = 1 ; 
	audioFormats.device = 0 ; 
	audioFormats.channels = 1 ; 
	audioFormats.format =  PCM_FORMAT_S16_LE ;
	audioFormats.rate = 16000 ; 
	audioFormats.period_size = 1024 ; 
	audioFormats.period_count = 4 ;

	struct pcm_config  config  ; 
	struct pcm *pcm ; 	
	unsigned int size ; 
	unsigned int bytes_read =  0; 
	char* buffer ;
	signed short* readbuffer ; 
	//char* readbuffer ;

	config.channels = audioFormats.channels ; 
	config.rate  = audioFormats.rate ; 
	config.period_size = audioFormats.period_size ; 
	config.period_count = audioFormats.period_count  ;
	config.format = audioFormats.format ;  
	config.start_threshold = 0 ; 
	config.stop_threshold = 0 ; 
	config.silence_threshold = 0 ; 

	pcm = pcm_open(audioFormats.card,audioFormats.device,PCM_IN,&config);
	if(!pcm || !pcm_is_ready(pcm))
	{
		SKY_LOG(1,(TAG_VOICELINK,"Unable to open PCM device (%s)\n", pcm_get_error(pcm)));
		return false ;
	}

	// get buffer size and malloc pcm buffer to perpare receive pcm data
	size = pcm_frames_to_bytes(pcm,pcm_get_buffer_size(pcm));
	buffer = malloc(size);
	//readbuffer = malloc(160*sizeof(signed short));

	SKY_LOG(3,(TAG_VOICELINK, "pcm_frames_to_bytes return %d",size));

	
	if(!buffer)
	{
		SKY_LOG(1,(TAG_VOICELINK, "Unable to allocate %d bytes",size));
		free(buffer); 
		pcm_close(pcm);
		return false ;
	}	

	// use pcm_read(struct pcm * pcm,void * data,unsigned int count) to read pcm data to buffer  
	//begin init tx decoder , send pcm buffer to txdevice  
	tx_init_decoder(sky_on_wifi_link_return, audioFormats.rate );
	
	while(!g_voice_link_finish )
	{				
		if(!pcm_read(pcm,buffer,size))		
		{
			
			SKY_LOG(3,(TAG_VOICELINK, "pcm_get_buffer_size return %d",pcm_get_buffer_size(pcm)));
			// every cycle read 320 Bytes , 160 signed short 
			signed short* tempPoint = (signed short*)buffer ; 
			for( int i = 0; i < size - 640 ; i += 640 )
			{	
				SKY_LOG(3,(TAG_VOICELINK, "tx_fill_audio now i is %d",i));				
				tx_fill_audio(tempPoint,320);
				// point tempPoint to the next 160 samples buffer
				tempPoint += 320 ; 			
			}				
			if(testFile)
			{			
				fwrite(buffer, 1, size, testFile);
			}
		}
		else 
		{
			//read pcm failed ,end voice link  stop loop 
			SKY_LOG(1,(TAG_VOICELINK, "pcm_read failed,stop vioce link"));
			g_voice_link_finish = true; 
			break ;
		}		
	}
	
	free(buffer) ; 
	pcm_close(pcm);

	// after voice link ,uninit decoder
	tx_uninit_decoder();	
	return true  ;
	
}




