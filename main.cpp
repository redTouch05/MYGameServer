#include "GameChannel.h"
#include "GameMsg.h"
#include "msg.pb.h"
#include "AOIWorld.h"
using namespace std;
extern RandomName random_name;
//class myPlayer :public Player
//{
//public:
//	myPlayer(int _x, int _y, std::string _name) :x(_x), y(_y), name(_name) {}
//	int x;
//	int y;
//	std::string name;//easy to Debug
//	//继承Player中的get函数
//	//@Override
//	virtual int GetX() override
//	{
//		return x;
//	}
//
//	virtual int GetY() override
//	{
//		return y;
//	}
//
//};



int main()
{
	//pb::SyncPid* pmsg = new pb::SyncPid();
	//pmsg->set_pid(1);
	//pmsg->set_username("test");

	//GameMsg gm(GameMsg::MSG_TYPE_LOGIN_ID_NAME, pmsg);
	//auto output = gm.serialize();

	//for (auto byte : output)
	//{
	//	printf("%02X ", byte);
	//}
	//puts("");

	//char buff[] = { 0x08, 0x01, 0x12, 0x04 ,0x74, 0x65, 0x73, 0x74 };
	//std::string input(buff, sizeof(buff));

	//auto ingm = GameMsg(GameMsg::MSG_TYPE_LOGIN_ID_NAME, input);
	//std::cout << dynamic_cast<pb::SyncPid*> (ingm.pMsg)->pid() << std::endl;
	//std::cout << dynamic_cast<pb::SyncPid*> (ingm.pMsg)->username() << std::endl;


	/*AOIWorld w(20, 200, 50, 230, 6, 6);
	myPlayer p1(60, 107, "1");
	myPlayer p2(91, 118, "2");
	myPlayer p3(147, 133, "3");
	w.AddPlayer(&p1);
	w.AddPlayer(&p2);
	w.AddPlayer(&p3);

	auto srd_list = w.GetSrdPlayers(&p1);
	for (auto single : srd_list)
	{
		cout << dynamic_cast<myPlayer*>(single)->name << endl;
	}*/
	random_name.LoadFile();
	ZinxKernel::ZinxKernelInit();
	/*添加监听通道*/
	ZinxKernel::Zinx_Add_Channel(*(new ZinxTCPListen(9999, new GameConnFact())));
	ZinxKernel::Zinx_Add_Channel(*(new ZinxTimerChannel()));
	ZinxKernel::Zinx_Run();

	ZinxKernel::ZinxKernelFini();
}