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

GameMsg* GameRole::CreateSrdPlayers()
{
	pb::SyncPlayers* pMsg = new pb::SyncPlayers();
	auto srd_list = world.GetSrdPlayers(this);
	for (auto single : srd_list)
	{
		auto pPlayer = pMsg->add_ps();
		auto pRole = dynamic_cast<GameRole*>(single);

		pPlayer->set_pid(pRole->iPid);
		pPlayer->set_username(pRole->szName);

		auto pPosition = pPlayer->mutable_p();
		pPosition->set_x(pRole->x);
		pPosition->set_y(pRole->y);
		pPosition->set_z(pRole->z);
		pPosition->set_v(pRole->v);
	}
	GameMsg* pret = new GameMsg(GameMsg::MSG_TYPE_SRD_POSTION, pMsg);

	return pret;
}

GameMsg* GameRole::CreateSelfPosition()
{
	pb::BroadCast* pMsg = new pb::BroadCast();
	pMsg->set_pid(this->iPid);
	pMsg->set_username(this->szName);
	/*tp设置为2 是由客户端规定的 表示此时的BroadCast信息包含初始位置*/
	pMsg->set_tp(2);
	auto pPosition = pMsg->mutable_p();
	pPosition->set_x(x);
	pPosition->set_y(y);
	pPosition->set_z(z);
	pPosition->set_v(v);

	GameMsg* pret = new GameMsg(GameMsg::MSG_TYPE_BROADCAST, pMsg);
	return pret;
}

GameMsg* GameRole::CreateIDNameLogoff()
{
	pb::SyncPid* pmsg = new pb::SyncPid();
	pmsg->set_pid(iPid);
	pmsg->set_username(szName);
	GameMsg* pRet = new GameMsg(GameMsg::MSG_TYPE_LOGOFF_ID_NAME, pmsg);
	return pRet;

}

GameRole::GameRole()
{
	//设置玩家的初始坐标
	x = 100;
	z = 100;
	y = 0;
	v = 0;
}


bool GameRole::Init()
{
	//添加自己到游戏世界
	bool bRet = false;
	//设置玩家ID为当前连接的fd
	iPid = m_protocol->m_channel->GetFd();
	//初始化玩家姓名
	szName = "tudou0921";
	bRet = world.AddPlayer(this);
	if (true == bRet)
	{
		//向自己发送ID和名称
		auto pmsg = CreateIDNameLogin();
		ZinxKernel::Zinx_SendOut(*pmsg, *m_protocol);
		//向自己发送周围玩家的位置
		pmsg = CreateSrdPlayers();
		ZinxKernel::Zinx_SendOut(*pmsg, *m_protocol);
		//向周围玩家发送自己的位置
		auto srd_list = world.GetSrdPlayers(this);
		for (auto single : srd_list)
		{
			auto pmsg = CreateSelfPosition();
			auto pRole = dynamic_cast<GameRole*>(single);
			ZinxKernel::Zinx_SendOut(*pmsg, *(pRole->m_protocol));

		}


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
	//向周围玩家发送下线信息
	auto srd_list = world.GetSrdPlayers(this);
	for (auto single : srd_list)
	{
		auto pMsg = CreateIDNameLogoff();
		auto pRole = dynamic_cast<GameRole*>(single);
		ZinxKernel::Zinx_SendOut(*pMsg, *(pRole->m_protocol));
	}
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




