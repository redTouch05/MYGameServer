#include "GameRole.h"
#include "GameMsg.h"
#include"msg.pb.h"
#include"GameProtocol.h"
#include"GameChannel.h"
#include<list>
#include <random>  // 包含 default_random_engine 和分布类型
#include <ctime>
//创建全局随机姓名对象
RandomName random_name;
//创建游戏世界全局对象
static AOIWorld world(0, 400, 0, 400, 20, 20);

class ExitTimer :public TimerOutProc
{
	//通过TimerOutProc 继承
	virtual void Proc() override
	{
		ZinxKernel::Zinx_Exit();
	}
	virtual int GetTimeSec() override
	{
		return 20;
	}
};


static ExitTimer g_exit_timer;


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

GameMsg* GameRole::CreateTalkBroadCast(std::string _content)
{
	pb::BroadCast* pmsg = new pb::BroadCast();
	pmsg->set_pid(iPid);
	pmsg->set_username(szName);
	pmsg->set_tp(1);//表示聊天信息
	pmsg->set_content(_content);
	GameMsg* pRet = new GameMsg(GameMsg::MSG_TYPE_BROADCAST, pmsg);
	return pRet;
}

void GameRole::ProcTalkMsg(std::string _content)
{
	//发给所有人
	auto roleList = ZinxKernel::Zinx_GetAllRole();
	for (auto pRole : roleList)
	{
		auto pGameRole = dynamic_cast<GameRole*>(pRole);
		auto pMsg = CreateTalkBroadCast(_content);
		ZinxKernel::Zinx_SendOut(*pMsg, *pGameRole->m_protocol);
	}
}

//GameMsg* GameRole::CreateNewPositionBroadCast(pb::Position* NewPos)
//{
//	//组建待发送的报文
//	pb::BroadCast* pMsg = new pb::BroadCast();
//	auto pPos = pMsg->mutable_p();
//	pPos->set_x(NewPos->x());
//	pPos->set_y(NewPos->y());
//	pPos->set_z(NewPos->z());
//	pPos->set_v(NewPos->v());
//	pMsg->set_pid(iPid);
//	pMsg->set_tp(4);
//	pMsg->set_username(szName);
//	GameMsg* msg = new GameMsg(GameMsg::MSG_TYPE_BROADCAST, pMsg);
//	return msg;
//}

void GameRole::ProcMoveMsg(float _x, float _y, float _z, float _v)
{
	//1.跨网格处理
	//获取原来的邻居
	auto s1 = world.GetSrdPlayers(this);
	//摘除旧格子 更新坐标 添加新格子 获取新邻居s2
	world.DelPlayer(this);
	x = _x;
	y = _y;
	z = _z;
	v = _v;
	world.AddPlayer(this);
	auto s2 = world.GetSrdPlayers(this);
	//遍历s2 若元素不属于s1 视野出现
	for (auto single_player : s2)
	{
		//说明一下c++11 find函数用法 find.(范围1,范围2,查找对象) 找到范围非空迭代器(!= s.end())
		//找不到返回空迭代器(==s.end()) 
		if (find(s1.begin(), s1.end(), single_player) == s1.end())
		{
			//视野出现
			ViewAppear(dynamic_cast<GameRole*>(single_player));
			
		}
	}
	//遍历s1 若元素不属于s2 视野消失
	for (auto single_player : s1)
	{
		if (find(s2.begin(), s2.end(), single_player) == s2.end())
		{
			//视野消失
			ViewLost(dynamic_cast<GameRole*>(single_player));
		}
	}

	//2.广播新位置给周围玩家
	auto srd_list = world.GetSrdPlayers(this);
	for (auto single : srd_list)
	{
		//组建待发送的报文
		pb::BroadCast* pMsg = new pb::BroadCast();
		auto pPos = pMsg->mutable_p();
		pPos->set_x(_x);
		pPos->set_y(_y);
		pPos->set_z(_z);
		pPos->set_v(_v);
		pMsg->set_pid(iPid);
		pMsg->set_tp(4);
		pMsg->set_username(szName);
		GameMsg* msg = new GameMsg(GameMsg::MSG_TYPE_BROADCAST, pMsg);
		auto pRole = dynamic_cast<GameRole*>(single);
		ZinxKernel::Zinx_SendOut(*msg, *(pRole->m_protocol));
	}
}

void GameRole::ViewAppear(GameRole* _pRole)
{
	//向自己发送参数的200消息
	auto pmsg = _pRole->CreateSelfPosition();
	ZinxKernel::Zinx_SendOut(*pmsg, *m_protocol);
	//向参数玩家发送自己的200消息
	pmsg = this->CreateSelfPosition();
	ZinxKernel::Zinx_SendOut(*pmsg, *(_pRole->m_protocol));

}

void GameRole::ViewLost(GameRole* _pRole)
{
	//向自己发送参数的201消息
	auto pmsg = _pRole->CreateIDNameLogoff();
	ZinxKernel::Zinx_SendOut(*pmsg, *m_protocol);

	//向参数玩家发送自己的201消息
	pmsg = CreateIDNameLogoff();
	ZinxKernel::Zinx_SendOut(*pmsg, *(_pRole->m_protocol));
}


static default_random_engine gen(time(nullptr));
static uniform_int_distribution<int> dis_int(0, 50);

GameRole::GameRole()
{
	//初始化玩家姓名
	szName = random_name.GetName();
	//设置玩家的初始坐标
	x = 100 + dis_int(gen);
	z = 100 + dis_int(gen);
	y = 0;
	v = 0;
}


bool GameRole::Init()
{
	if (ZinxKernel::Zinx_GetAllRole().size() <= 0)
	{
		TimerOutMng::GetInstance().DelTask(&g_exit_timer);
	}

	//添加自己到游戏世界
	bool bRet = false;
	//设置玩家ID为当前连接的fd
	iPid = m_protocol->m_channel->GetFd();
	
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

		//取出新位置*/
		auto NewPos = dynamic_cast<pb::Position*>(single->pMsg);

		switch (single->enMsgType)
		{
			//判断消息类型是不是聊天类型消息
		case GameMsg::MSG_TYPE_CHAT_CONTENT:
			ProcTalkMsg(dynamic_cast<pb::Talk*>(single->pMsg)->content());
			break;
		//判断消息类型是不是玩家新位置类型
		case GameMsg::MSG_TYPE_NEW_POSTION:
			ProcMoveMsg(NewPos->x(), NewPos->y(), NewPos->z(), NewPos->v());
			break;
		default:
			break;
		}
	}

		// --- 新增的内存释放逻辑 ---
// 1. 遍历并释放MultiMsg中包含的所有GameMsg对象
	for (auto pMsg : input.m_Msgs)
	{
		delete pMsg;
	}
	// 2. 清空列表（虽然对象已删除，但清空列表是个好习惯）
	input.m_Msgs.clear();
	// 3. 释放MultiMsg对象本身(严重错误 因为multimsg由框架分配 最后也由框架释放 手动释放可能会导致双重delete问题)
	/*delete& input;*/

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
	//判断是不是最后一个玩家
	auto list = ZinxKernel::Zinx_GetAllRole();
	if (list.empty())
	{
		//启动 退出定时器
		TimerOutMng::GetInstance().AddTask(&g_exit_timer);
	}
}

int GameRole::GetX()
{
	return (int)x;
}

int GameRole::GetY()
{
	return (int)y;
}


GameRole::~GameRole()
{
	random_name.ReleaseName(szName);
}

