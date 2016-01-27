# replace with your target cross-compile toolchains.
# default for host compiler.
#

# running:
#     shell>export LD_LIBRARY_PATH=../../lib
#	  shell>./ipcamera_demo

# you will got this message "[error]get license from file failed..."
# fix this:
#      step 1: access http://iot.open.qq.com/wiki/index.html#CASE/IP_Camera.md 
#      
#      step 2: replace "pid" which is a product id number in main.c 
#              copy sn file, licence file and server public key file (file name like "pid".pem) to ./
#
#	   step 3: > make

CC:=gcc

C_FLAGS:=-O0 -g3  -m32 -std=c99 -d 

C_FILES:=\
	ability/dataPointAbility.c \
	ability/dataPointAbility.c \
	ability/videoPushAbility.c \
	ability/OTAAbility.c \
	ability/alarmAbility.c \
	ability/rotateAbility.c \
	ability/fileRecvAbility.c \
	ability/audioVideoAbility.c \
	ability/voiceLinkAbility.c \
	ability/abilityMaster.c \
	ability/basicAbility/unitTest.c\
	ability/basicAbility/videoCapture.c\
	ability/basicAbility/h264Encoder.c\
	third_party/cjson/cJSON.c \
	skyAuth.c \
	skyLog.c \
	skyDevice.c 


all: 
	$(CC) $(C_FLAGS) $(C_FILES) -o ipcamera_demo -I../../include -I./third_party  -L../../lib -lx264 -ltxdevicesdk -lpthread -ldl -lm -ltinyalsa

clean:
	rm ipcamera_demo
