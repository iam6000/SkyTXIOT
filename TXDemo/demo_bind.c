/**
 * 这段代码的目的是用来演示：如何初始化SDK和设备登陆
 *
 * 主要调用了tx_init_device函数来初始化设备信息以及相关回调，这些信息用于服务器在设备登陆时做验证使用。
 * 在SDK初始化成功后，您应该使用我们最新版的手Q，确保手Q和设备连接上了同一个路由器，并且能够访问互联网，
 * 然后在手Q好友面板”我的设备“中点击”搜索新设备“，将扫描到待绑定的设备，绑定成功后将触发设备的登陆逻辑，
 * 这个登陆过程将在SDK内部完成后通过设置的回调函数向上层通知。
 *
 * 期望的结果：
 * 下面的两个函数 on_online_status，on_login_complete 回调的结果是：errcode为0，new 为 11
 */

#include <stdio.h>
#include <string.h>

#include "TXSDKCommonDef.h"
#include "TXDeviceSDK.h"

/**
 * 登录完成的通知，errcode为0表示登录成功，其余请参考全局的错误码表
 */
void on_login_complete(int errcode) {
    printf("on_login_complete | code[%d]\n", errcode);
}

/**
 * 在线状态变化通知， 状态（status）取值为 11 表示 在线， 取值为 21 表示  离线
 * old是前一个状态，new是变化后的状态（当前）
 */
void on_online_status(int old, int new) {
	printf("online status: %s\n", 11 == new ? "true" : "false"); 
}

/**
 * 辅助函数: 从文件读取buffer
 * 这里用于读取 license 和 guid
 * 这样做的好处是不用频繁修改代码就可以更新license和guid
 */
bool readBufferFromFile(char *pPath, unsigned char *pBuffer, int nInSize, int *pSizeUsed) {
	if (!pPath || !pBuffer) {
		return false;
	}

	int uLen = 0;
	FILE * file = fopen(pPath, "rb");
	if (!file) {
	    return false;
	}

	fseek(file, 0L, SEEK_END);
	uLen = ftell(file);
	fseek(file, 0L, SEEK_SET);

	if (0 == uLen || nInSize < uLen) {
		printf("invalide file or buffer size is too small...\n");
		return false;
	}

	*pSizeUsed = fread(pBuffer, 1, uLen, file);
	// bugfix: 0x0a is a lineend char, no use.
	if (pBuffer[uLen-1] == 0x0a)
	{
		*pSizeUsed = uLen - 1;
		pBuffer[uLen-1] = '\0';
	}

	printf("len:%d, ulen:%d\n",uLen, *pSizeUsed);
	fclose(file);
	return true;
}

/**
 * 辅助函数：SDK的log输出回调
 * SDK内部调用改log输出函数，有助于开发者调试程序
 */
void log_func(int level, const char* module, int line, const char* message)
{
	printf("%s\n", message);
}

/**
 * SDK初始化
 * 例如：
 * （1）填写设备基本信息
 * （2）打算监听哪些事件，事件监听的原理实际上就是设置各类消息的回调函数，
 * 	例如设置CC消息通知回调：
 * 	开发者应该定义如下的 my_on_receive_ccmsg 函数，将其赋值tx_msg_notify对象中对应的函数指针，并初始化：
 *
 * 			tx_msg_notify msgNotify = {0};
 * 			msgNotify.on_receive_ccmsg = my_on_receive_ccmsg;
 * 			tx_init_msg(&msgNotify);
 *
 * 	那么当SDK内部的一个线程收到对方发过来的CC消息后（通过服务器转发），将同步调用 msgNotify.on_receive_ccmsg 
 */
bool initDevice() {
	// 读取 license
	unsigned char license[256] = {0};
	int nLicenseSize = 0;
	if (!readBufferFromFile("./licence.sign.file.txt", license, sizeof(license), &nLicenseSize)) {
		printf("[error]get license from file failed...\n");
		return false;
	}

	// 读取guid
	unsigned char guid[32] = {0};
	int nGUIDSize = 0;
	if(!readBufferFromFile("./GUID_file.txt", guid, sizeof(guid), &nGUIDSize)) {
		printf("[error]get guid from file failed...\n");
		return false;
	}

    char svrPubkey[256] = {0};
    int nPubkeySize = 0;
    if (!readBufferFromFile("./1000000004.pem", svrPubkey, sizeof(svrPubkey), &nPubkeySize))
    {
        printf("[error]get svrPubkey from file failed...\n");
        return NULL;
    }

    // 设备的基本信息
    tx_device_info info = {0};
    info.os_platform            = "Linux";

    info.device_name            = "demo1";
    info.device_serial_number   = guid;
    info.device_license         = license;
    info.product_version        = 1;
    info.network_type			= network_type_wifi;
    info.product_id             = 1000000004;
    info.server_pub_key         = svrPubkey;

	// 设备登录、在线状态、消息等相关的事件通知
	// 注意事项：
	// 如下的这些notify回调函数，都是来自硬件SDK内部的一个线程，所以在这些回调函数内部的代码一定要注意线程安全问题
	// 比如在on_login_complete操作某个全局变量时，一定要考虑是不是您自己的线程也有可能操作这个变量
    tx_device_notify notify      = {0};
    notify.on_login_complete     = on_login_complete;
    notify.on_online_status      = on_online_status;
    notify.on_binder_list_change = NULL;

    // SDK初始化目录，写入配置、Log输出等信息
    // 为了了解设备的运行状况，存在上传异常错误日志 到 服务器的必要
    // system_path：SDK会在该目录下写入保证正常运行必需的配置信息
    // system_path_capicity：是允许SDK在该目录下最多写入多少字节的数据（最小大小：10K，建议大小：100K）
    // app_path：用于保存运行中产生的log或者crash堆栈
    // app_path_capicity：同上，（最小大小：300K，建议大小：1M）
    // temp_path：可能会在该目录下写入临时文件
    // temp_path_capicity：这个参数实际没有用的，可以忽略
    tx_init_path init_path = {0};
    init_path.system_path = "./";
    init_path.system_path_capicity = 100 * 1024;
    init_path.app_path = "./";
    init_path.app_path_capicity = 1024 * 1024;
    init_path.temp_path = "./";
    init_path.temp_path_capicity = 10 * 1024;

    // 设置log输出函数，如果不想打印log，则无需设置。
    // 建议开发在开发调试阶段开启log，在产品发布的时候禁用log。
    tx_set_log_func(log_func);

    // 初始化SDK，若初始化成功，则内部会启动一个线程去执行相关逻辑，该线程会持续运行，直到收到 exit 调用
	int ret = tx_init_device(&info, &notify, &init_path);
	if (err_null == ret) {
		printf(" >>> tx_init_device success\n");
	}
	else {
		printf(" >>> tx_init_device failed [%d]\n", ret);
		return false;
	}

	return true;
}

/****************************************************************
*  测试代码：
*
*  （1）while循环的作用仅仅是使 Demo进程不会退出，实际使用SDK时一般不需要
*
*  （2） 输入 "quit" 将会退出当前进程，这段逻辑存在的原因在于：
*     					在某些芯片上，直接用Ctrl+C 退出易产生僵尸进程
*
*  （3）while循环里面的sleep(1)在这里是必须的，因为如果Demo进程后台运行，scanf没有阻塞作用，会导致当前线程跑满CPU
*
*****************************************************************/
int main(int argc, char* argv[]) {
	if ( !initDevice() ) {
		return -1;
	}
	
	// 你可以在做其他相关的事情
	// ...

	char input[100];
	while (scanf("%s", input)) {
		if ( !strcmp(input, "quit") ) {
			tx_exit_device();
			break;
		}
		sleep(1);
	}
	
	return 0;
}
