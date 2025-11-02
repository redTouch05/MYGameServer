#pragma once
#include<zinx.h>
#include"AOIWorld.h"
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



public:
    GameRole();
   

    GameProtocol* m_protocol = NULL;
    // 通过 Irole 继承
    bool Init() override;
    UserData* ProcMsg(UserData& _poUserData) override;
    void Fini() override;

    


    // 通过 Player 继承
    int GetX() override;

    int GetY() override;

};

