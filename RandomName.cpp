#pragma once
#include "RandomName.h"

static default_random_engine rand_engine(time(NULL));
RandomName::RandomName()
{
}

string RandomName::GetName()
{
	if (m_pool.empty()) {
		return "default name";
	}
	auto num = rand_engine() % m_pool.size();
	//取姓
	auto first = m_pool[num]->m_first;
	//取名
	auto m_last_list_length = m_pool[num]->m_last_list.size();
	auto last_name_index = rand_engine() % m_last_list_length;
	auto last = m_pool[num]->m_last_list[last_name_index];
	//取出来后把这个名字删除
	m_pool[num]->m_last_list.erase(m_pool[num]->m_last_list.begin() + last_name_index);
	//如果该姓中的所有名都被取完 删除这个姓
	if (m_pool[num]->m_last_list.size() <= 0)
	{
		delete m_pool[num];
		m_pool.erase(m_pool.begin() + num);
	}

	std::string result = first + " " + last;

	std::cout << "--- GetName Debug ---" << std::endl;
	std::cout << "First Name: [" << first << "], Length: " << first.size() << std::endl;
	std::cout << "Last Name:  [" << last << "], Length: " << last.size() << std::endl;
	std::cout << "Result:     [" << result << "], Length: " << result.size() << std::endl;
	std::cout << "---------------------" << std::endl;

	return result;
}

void RandomName::ReleaseName(std::string _name)
{
	//分割名字 得到姓和名
	auto space_pos = _name.find(" ", 0);
	auto first = _name.substr(0, space_pos);
	auto last = _name.substr(space_pos + 1, _name.size() - space_pos - 1);
	bool found = false;

	for (auto first_name : m_pool)
	{
		if (first == first_name->m_first)
		{
			found = true;                                    
			first_name->m_last_list.push_back(last);
			break;
		}
	}

	if (found == false)
	{
		auto first_name = new FirstName();
		first_name->m_first = first;
		first_name->m_last_list.push_back(last);
		m_pool.push_back(first_name);
	}

}

//在读取文件后，清除掉字符串末尾的 \r 字符
void remove_cr(std::string& s) {
	// 检查字符串最后一个字符是否为 '\r'
	if (!s.empty() && s.back() == '\r') {
		s.pop_back(); // 移除它
	}
}


void RandomName::LoadFile()
{
	ifstream first("./random_first.txt");
	ifstream last("./random_last.txt");

	//读取所有名字组成一个线性表
	string last_name;
	vector<string> temp;
	while (getline(last, last_name))
	{
		remove_cr(last_name);
		temp.push_back(last_name);
	}
	//读取所有姓 创建姓名池节点 拷贝名字组成的线性表
	string first_name;
	while (getline(first, first_name))
	{
		remove_cr(first_name);
		auto first_name_list = new FirstName();
		first_name_list->m_first = first_name;
		first_name_list->m_last_list = temp;
		m_pool.push_back(first_name_list);
		std::cout << "LoadFile:" << std::endl;
		std::cout << first_name<< endl;
	}
}

RandomName::~RandomName()
{

}
