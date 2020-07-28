#include<iostream>
#include<cstdio>
#include<algorithm>
#include<string>
#include<cstdlib>
#include<map>
#include"minisql.h"
#include"api.h"
#include"vector.h"
#include"manage.h"
#include"b+tree.h"
#include"buffer.h"
using namespace std;
user usr;
int main()
{
	init();
	string nexec;
	while(1)
	{
		printf("»¶Ó­Ê¹ÓÃMiniSQL!\n");
		printf("ÇëÊäÈëÄúµÄÕËºÅ:\n");
		string bfr; cin >> bfr; usr.set_account(bfr);
		printf("ÇëÊäÈëÄúµÄÃÜÂë:\n");
		cin >> bfr; usr.set_pwd(bfr);
		usr.get_pwd();
		login(usr);
		if (usr.get_status())
		{
			printf("µÇÂ½³É¹¦£¡\n");
		}
		else
		{
			printf("ÕËºÅ»òÃÜÂë´íÎó£¬ÇëÖØÐÂµÇÂ¼\n");
			continue;
		}
		while (usr.get_status())
		{
			printf("(%s)MiniSQL > ",usr.get_account().c_str());
			string tmp="";
			while (1)
			{
				tmp = nexec = "";
				getline(cin, tmp);
				int flag = 0;
				for (int i = tmp.length() - 1; i >= 0; i--)
				{
					if (tmp[i] != ' ' && tmp[i] != '\n')
					{
						if (tmp[i] == '^')
						{
							nexec = nexec + tmp.substr(0, i);
							break;
						}
						else
						{
							flag = 1;
							nexec = nexec + tmp.substr(0, i + 1);
							break;
						}
					}
				}
				if (flag)break;
			}
			exec(nexec);
		}
	}
	
}