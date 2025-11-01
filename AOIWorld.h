#pragma once
#include<list>
#include<vector>
using namespace std;
/*拥有横纵坐标的玩家对象*/
class Player
{
public:
	virtual int GetX() = 0;
	virtual int GetY() = 0;
};


//储存玩家的网格类
class Grid
{
public:
	list<Player*> m_players;

};

/*游戏世界矩形*/
class AOIWorld
{
	int x_begin = 0;
	int x_end = 0;
	int y_begin = 0;
	int y_end = 0;
	int x_count = 0;
	int y_count = 0;
	int x_width = 0;
	int y_width = 0;

public:


	vector<Grid> m_grids;

	//获取周围玩家
	list<Player*> GetSrdPlayers(Player* _Player);


	AOIWorld();

	//添加玩家到AOI网格
	bool AddPlayer(Player* _player);
	//删除玩家
	void DelPlayer(Player* _player);



	AOIWorld(int x_begin, int x_end, int y_begin, int y_end, int x_count, int y_count);

	virtual ~AOIWorld();
};

