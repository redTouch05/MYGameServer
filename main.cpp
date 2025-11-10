#include "GameChannel.h"
#include "GameMsg.h"
#include "msg.pb.h"
#include "AOIWorld.h"
using namespace std;
extern RandomName random_name;

//守护进程函数
void safeguard()
{
	int ipid = fork();
	if (ipid < 0)
	{
		exit(-1);
	}
	//父进程退出
	if (ipid > 0)
	{
		exit(0);
	}
	//子进程设置会话 脱离当前会话组
	setsid();
	//重定向守护进程的标准输入 输出 错误 到黑洞设备 使守护进程无法与终端进行IO交互
	int nullfd = open("/dev/null", O_RDWR);
	if (nullfd > 0)
	{
		dup2(nullfd, 0);
		dup2(nullfd, 1);
		dup2(nullfd, 2);
		close(nullfd);
	}
}


int main()
{
	safeguard();
	random_name.LoadFile();
	ZinxKernel::ZinxKernelInit();
	/*添加监听通道*/
	ZinxKernel::Zinx_Add_Channel(*(new ZinxTCPListen(9999, new GameConnFact())));
	ZinxKernel::Zinx_Add_Channel(*(new ZinxTimerChannel()));
	ZinxKernel::Zinx_Run();

	ZinxKernel::ZinxKernelFini();
}