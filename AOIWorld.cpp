#include "AOIWorld.h"

list<Player*> AOIWorld::GetSrdPlayers(Player* _Player)
{
	list<Player*>ret;
	//计算所属编号
	int grid_id = (_Player->GetX() - this->x_begin) / this->x_width + (_Player->GetY() - this->y_begin) / y_width * x_count;
   


	//计算当前网格横轴数和纵轴数
	int x_index = grid_id % x_count;
	int y_index = grid_id / y_count;

	//判断具体情况 取出周围网格的玩家们
	//左上方
	if (x_index > 0 && y_index > 0)
	{
		list<Player*>& cur_list = m_grids[grid_id - 1 - x_count].m_players;
		ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	}
	//正上方
	if (y_index > 0)
	{
		list<Player*>& cur_list = m_grids[grid_id - x_count].m_players;
		ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	}
	//右上方
	if (x_index < x_count - 1 && y_index>0)
	{
		list<Player*>& cur_list = m_grids[grid_id + 1 - x_count].m_players;
		ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	}
	//左方
	if (x_index>0)
	{
		list<Player*>& cur_list = m_grids[grid_id - 1].m_players;
		ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	}
	//自己
	list<Player*>& cur_list = m_grids[grid_id].m_players;
	ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	//右方
	if (x_index < x_count - 1)
	{
		list<Player*>& cur_list = m_grids[grid_id + 1].m_players;
		ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	}
	//左下方
	if (x_index > 0 && y_index < y_count - 1)
	{
		list<Player*>& cur_list = m_grids[grid_id - 1 + x_count].m_players;
		ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	}
	//下方
	if (y_index < y_count - 1)
	{
		list<Player*>& cur_list = m_grids[grid_id + 1].m_players;
		ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	}

	//右下方
	if (x_index < x_count - 1 && y_index < y_count - 1)
	{
		list<Player*>& cur_list = m_grids[grid_id + 1 + x_count].m_players;
		ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	}

	return ret;
}

bool AOIWorld::AddPlayer(Player* _player)
{
	//x轴网格宽度
	//网格编号=(x-x.begin)/x.width + (y-.begin)/y.width*x.count

	int grid_id = (_player->GetX() - x_begin) / x_width + (_player->GetY() - y_begin) / y_width * x_count;

	//添加到网格中
	m_grids[grid_id].m_players.push_back(_player);

	return true;
}

void AOIWorld::DelPlayer(Player* _player)
{
	int grid_id = (_player->GetX() - x_begin) / x_width + (_player->GetY() - y_begin) / y_width * x_count;
	m_grids[grid_id].m_players.remove(_player);
}

AOIWorld::AOIWorld(int x_begin, int x_end, int y_begin, int y_end, int x_count, int y_count) :
	x_begin(x_begin), x_end(x_end), y_begin(y_begin), y_end(y_end), x_count(x_count), y_count(y_count)
{
	//x轴网格宽度 = (x.end - x.begin)/x轴网格数量 y轴相同
	x_width = (x_end - x_begin) / x_count;
	y_width = (y_end - y_begin) / y_count;

	//创建世界格子
	for (int i = 0; i < x_count * y_count; i++)
	{
		Grid temp;
		m_grids.push_back(temp);
	}





}

AOIWorld::~AOIWorld()
{
}
