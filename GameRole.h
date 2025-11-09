#pragma once
#include<zinx.h>
#include"AOIWorld.h"
#include"msg.pb.h"
#include"ZinxTimer.h"
#include"RandomName.h"
class GameProtocol;
class GameMsg;
class GameRole :
    public Irole,public Player
{
    float x = 0;
    float y = 0;
    float z = 0;
    float v = 0;

    int iPid = 0;
    std::string szName;

    GameMsg* CreateIDNameLogin();
    GameMsg* CreateSrdPlayers();
    GameMsg* CreateSelfPosition();
    GameMsg* CreateIDNameLogoff();
    GameMsg* CreateTalkBroadCast(std::string _content);
    GameMsg* CreateNewPositionBroadCast(pb::Position* NewPos);
    //处理聊天文本信息
    void ProcTalkMsg(std::string _content);

    //处理玩家移动的信息
    void ProcMoveMsg(float _x, float _y, float _z, float _v);
    //视野出现
    void ViewAppear(GameRole* _pRole);
    //视野消失
    void ViewLost(GameRole* _pRole);
public:
    GameRole();
    ~GameRole();

    GameProtocol* m_protocol = NULL;
    // 通过 Irole 继承
    bool Init() override;
    UserData* ProcMsg(UserData& _poUserData) override;
    void Fini() override;

    // 通过 Player 继承
    int GetX() override;

    int GetY() override;

 

};

