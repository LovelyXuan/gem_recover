#pragma once
#include <iostream>
#include <corecrt_io.h>
#include <string>
#include <iosfwd>
#include <fstream>
#include <sstream>
#include <map>
//#include <winnls.h>
#include <windows.h>

using namespace std;


class Gem
{
public:

	void SetEvent(int id) { m_event_id = id; }

	void GetFiles(const string& src_path, const string& dst_path)
	{
		intptr_t   hFile = 0;//intptr_t和uintptr_t是什么类型:typedef long int/ typedef unsigned long int
		struct _finddata_t fileinfo;
		string p;
		string q;
		if ((hFile = _findfirst(p.assign(src_path).append("/*").c_str(), &fileinfo)) != -1)//assign方法可以理解为先将原字符串清空，然后赋予新的值作替换。
		{
			do
			{
				if ((fileinfo.attrib & _A_SUBDIR))//是否为文件夹
				{
					if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					{
						GetFiles(p.assign(src_path).append("/").append(fileinfo.name), dst_path);//子文件夹下递归访问
					}
				}
				else//非文件夹
				{
					ReadFile(p.assign(src_path).append("/").append(fileinfo.name), q.assign(dst_path).append("/").append(fileinfo.name));
				}
			} while (_findnext(hFile, &fileinfo) == 0);
			_findclose(hFile);
		}

	}


	void ReadFile(const string& src_path, const string& dst_path)
	{
		ifstream infile(src_path.c_str());
		
		if (infile)
		{

			string line;
			stringstream ss;

			string uid;
			wchar_t  name[32];
			memset(name, 0, 32 * sizeof(wchar_t));

			string temp;
			int n_temp;

			while (getline(infile, line))
			{
				ss << line;

				ss >> uid;
				ss >> temp;

				::MultiByteToWideChar(CP_UTF8, 0, temp.c_str(), -1, name, 32);

				ss >> n_temp;
				if (n_temp == m_event_id)
				{
					ss >> n_temp;
					ss >> n_temp;

					PlayerInfo& player = m_playerInfo[uid];
					if (player.name.empty())
					{
						player.name = name;
					}

					player.value += n_temp;
				}

				// 去掉无用的
				while (!ss.eof())
				{
					ss >> n_temp;
				}
				
				ss.clear();
			}

			OutputFile(dst_path);
		}

	}

	void OutputFile(const string& dst_path)
	{
		ofstream outfile(dst_path.c_str());

		if (outfile)
		{
			char name[64];
			memset(name, 0, 64);

			for (MapPlayerInfo::iterator it = m_playerInfo.begin(); it != m_playerInfo.end(); ++it)
			{
				::WideCharToMultiByte(CP_UTF8, 0, it->second.name.c_str(), -1, name, 64, 0, false);
				outfile << it->first.c_str() << "\t" << name << "\t" << it->second.value << endl;
			}
		}

		outfile.close();
	}

protected:
private:

	struct PlayerInfo
	{
		PlayerInfo() : value(0) {}

		int64_t value;
		wstring name;
	};

	int m_event_id;

	using MapPlayerInfo = map<string, PlayerInfo>;
	MapPlayerInfo m_playerInfo;

};