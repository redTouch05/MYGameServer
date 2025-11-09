#pragma once
#include<vector>
#include<string>
#include<random>
#include<fstream>
using namespace std; 
class FirstName
{
public:
	string m_first;
	//´æ´¢Ãû
	vector<string> m_last_list;
};

class RandomName
{
	//ÐÕÃû³Ø
	vector<FirstName*> m_pool;
public:
	RandomName();
	string GetName();
	void ReleaseName(std::string _name);
	void LoadFile();

	virtual ~RandomName();
};

