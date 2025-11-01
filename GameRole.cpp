#include "GameRole.h"
#include "GameMsg.h"
#include"msg.pb.h"
#include"GameProtocol.h"
#include"GameChannel.h"
//创建游戏世界全局对象
static AOIWorld world(0, 400, 0, 400, 20, 20);

GameMsg* GameRole::CreateIDNameLogin()
{
	pb::SyncPid* pmsg = new pb::SyncPid();
	pmsg->set_pid(iPid);
	pmsg->set_username(szName);
	GameMsg* pRet = new GameMsg(GameMsg::MSG_TYPE_LOGIN_ID_NAME, pmsg);
	return pRet;
}

GameRole::GameRole()
{

}

bool GameRole::Init()
{
	//添加自己到游戏世界
	bool bRet = false;
	//设置玩家ID为当前连接的fd
	iPid = m_protocol->m_channel->GetFd();
	szName = "Tom";
	bRet = world.AddPlayer(this);
	if (true == bRet)
	{
		auto pmsg = CreateIDNameLogin();
		ZinxKernel::Zinx_SendOut(*pmsg, *m_protocol);

	}

	return bRet;
}

UserData* GameRole::ProcMsg(UserData& _poUserData)
{
	GET_REF2DATA(MultiMsg, input, _poUserData);
	for (auto single : input.m_Msgs)
	{
		std::cout << "type is " << single->enMsgType << std::endl;
		std::cout << single->pMsg->Utf8DebugString() << std::endl;

	}
	return nullptr;
}

void GameRole::Fini()
{
	world.DelPlayer(this);
}

int GameRole::GetX()
{
	return (int)x;
}

int GameRole::GetY()
{
	return (int)y;
}




