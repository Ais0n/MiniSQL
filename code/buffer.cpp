#include<iostream>
#include<fstream>
#include<cstdio>
#include<algorithm>
#include<string>
#include<cstdlib>
#include<map>
#include<queue>
#include"minisql.h"
#include"api.h"
#include"buffer.h"
#include"b+tree_head.h"
using namespace std;
map<string, void*>buf_mp;
queue<string>buf_queue;
string buf_string[10];
int buf_int[10];
float buf_float[10];
void inserttable(table_c& data)//将data各属性写到管理文件的末尾
{
	string file_name = (string)"bin/Cat_" + data.get_table_name().c_str() + (string)".txt";
	FILE* fp;
	fp = fopen(file_name.c_str(), "w");
	attr_c* tmp_attr = data.get_attr_list();
	fprintf(fp,"%s %d %d\n%d %d\n", data.get_table_name().c_str(), data.get_element_size(),data.get_rec_per_page(), data.get_attr_size(), data.get_primkey());
	for (int i = 0; i < data.get_attr_size(); i++)
	{
		fprintf(fp,"%s %d %d %d\n", tmp_attr[i].get_attr_name().c_str(), tmp_attr[i].get_type(), tmp_attr[i].get_length(), tmp_attr[i].get_isunique());
	}
	fclose(fp);
}
bool load_one_table(string table_name, table_c* & data)//将table读到data里,没有则返回0
{
	string file_name = (string)"bin/Cat_" + table_name + (string)".txt";
	//FILE* fp = fopen(file_name.c_str(), "r");
	if (buf_mp[file_name] != NULL) data = (table_c*)buf_mp[file_name];
	else
	{
		ifstream fp(file_name.c_str());
		if (!fp.good())
		{
			fp.close();
			return 0;
		}
		if ((int)buf_queue.size() == max_page)
		{
			free(buf_mp[buf_queue.front()]);
			buf_queue.pop();
		}
		data = new table_c();
		buf_mp[file_name] = data;
		buf_queue.push(file_name);
		fp >> buf_string[0];
		data->set_table_name(buf_string[0]);
		fp >> buf_int[0];
		data->set_element_size(buf_int[0]);
		fp >> buf_int[0];
		data->set_rec_per_page(buf_int[0]);
		int tmp; fp >> tmp;
		fp >> buf_int[0];
		data->set_primkey(buf_int[0]);
		attr_c tmp_attr;
		for (int i = 0; i < tmp; i++)
		{
			fp >> buf_string[0] >> buf_int[0] >> buf_int[1] >> buf_int[2];
			tmp_attr.set_attr_c(buf_string[0], buf_int[0], buf_int[1], buf_int[2]);
			data->ins_attr(tmp_attr);
		}
		fp.close();
	}
	return 1;
}
bool deletetable(string table_name)//删除名为tablename的table（仅在管理文件中）
{
	string file_name = (string)"bin/Cat_" + table_name.c_str() + (string)".txt";
	return remove(file_name.c_str());
}
void insertindex(string index_name, string tablename, int tree_index)//将index各属性写到管理文件的末尾
{
	FILE* fp = fopen("bin/CatIndex.txt","w");
	fseek(fp, 0, SEEK_END);
	fprintf(fp, "%s %s %d 0 0\n", index_name.c_str(), tablename.c_str(), tree_index);
	fclose(fp);
}
bool load_one_index(string index_name, string& tablename, int& tree_index)//将index读到后面两个信息里,没有则返回0
{
	ifstream fp("bin/CatIndex.txt");
	if (!fp.good())return 0;
	int flag = 0;
	while (fp >> buf_string[0] >> buf_string[1] >> buf_int[0] >> buf_int[1] >> buf_int[2])
	{
		if (buf_string[0] == index_name)
		{
			tablename = buf_string[1];
			tree_index = buf_int[0];
			flag = 1;
			break;
		}
	}
	fp.close();
	return flag;
}
bool deleteindex(string index_name)//删除名为index_name的index（仅在管理文件中）
{
	ifstream fp; 
	fp.open("bin/CatIndex.txt", ios::out | ios::trunc); if (!fp.is_open())return 0;
	FILE* fp2 = fopen("bin/Tmp_CatIndex.txt", "w");
	while (fp>>buf_string[0]>>buf_string[1]>>buf_int[0]>>buf_int[1]>>buf_int[2])
	{
		if (buf_string[0] != index_name)
		{
			fprintf(fp2, "%s %s %d %d %d\n", buf_string[0].c_str(), buf_string[1].c_str(), buf_int[0], buf_int[1], buf_int[2]);
		}
	}
	fp.close();
	fclose(fp2);
	remove("bin/CatIndex.txt");
	rename("bin/Tmp_CatIndex.txt", "bin/CatIndex.txt");
	return 1;
}

bool createBTfile(BTree T) //对应table的对应B+树创造结点
{
	string file_name = (string)"bin/Ind_" + T->tablename.c_str() + (string)"_" + to_string(T->tree_index) + (string)"_" + to_string(T->BT_No) + (string)".txt";
	FILE* fp = fopen(file_name.c_str(), "w");
	if (fp == NULL)return 0;
	fprintf(fp, "%s %d %d %d %d %d\n", T->tablename.c_str(), T->tree_index, T->BT_No, T->num, T->isleaf, T->isroot);
	for (int i = 0; i < T->num; i++)
	{
		fprintf(fp, "%s ", T->val[i].c_str());
	}
	fprintf(fp, "\n");
	for (int i = 0; i <= T->num; i++)
	{
		fprintf(fp, "%d ", T->child[i]);
	}
	fprintf(fp, "\n");
	int flag = 0;
	for (int i = 0; i < T->num; i++)
	{
		fprintf(fp, "%d\n", T->recs[i]->size());
		for (int j = 0; j < T->recs[i]->size(); j++)
		{
			rec tmp= (*(T->recs[i]))[j];
			if (!flag)
			{
				fprintf(fp, "%d\n", tmp.get_attr_size()), flag = 1;
				for (int k = 0; k < tmp.get_attr_size(); k++)
					fprintf(fp, "%s ", (*(tmp.get_attr_list() + k)).c_str());
				fprintf(fp, "\n");
			}
			for (int k = 0; k < tmp.get_attr_size(); k++)
			{
				string tmp2 = *(tmp.get_val_list() + k);
				if (tmp2 == "")fprintf(fp, "$NULL$ ");
				fprintf(fp, "%s ", tmp2.c_str());
			}
			fprintf(fp, "\n");
		}
	}
	fclose(fp);
	return 1;
}
bool deleteBTfile(string tablename, int tree_index, int BT_No) //删除对应table的对应B+树的结点BT_No
{
	string file_name = (string)"bin/Ind_" + tablename.c_str() + (string)"_" + to_string(tree_index) + (string)"_" + to_string(BT_No) + (string)".txt";
	return remove(file_name.c_str());
}
bool deleteBTfile(BTree& T) {
	return deleteBTfile(T->tablename, T->tree_index, T->BT_No);
}
BTree getBTfile(string tablename, int tree_index, int BT_No)//没有则返回NULL
{
	string file_name = (string)"bin/Ind_" + tablename.c_str() + (string)"_" + to_string(tree_index) + (string)"_" + to_string(BT_No) + (string)".txt";
	BTree data = new BNode();
	data->tablename = tablename;
	data->tree_index = tree_index;
	data->BT_No = BT_No;
	if (buf_mp[file_name] != NULL) data = (BTree)buf_mp[file_name];
	else
	{
		ifstream fp(file_name.c_str());
		if (!fp.good())
		{
			fp.close(); return NULL;
		}
		if ((int)buf_queue.size() == max_page)
		{
			free(buf_mp[buf_queue.front()]);
			buf_queue.pop();
		}
		fp >> buf_string[0] >> buf_int[0] >> buf_int[1] >> buf_int[2] >> buf_int[3] >> buf_int[4];
		data->num = buf_int[2];
		data->isleaf = buf_int[3];
		data->isroot = buf_int[4];
		for (int i = 0; i < data->num; i++)
		{
			fp >> buf_string[0];
			data->val[i] = buf_string[0];
		}
		for (int i = 0; i <= data->num; i++)
		{
			fp >> buf_int[0];
			data->child[i] = buf_int[0];
		}
		for (int i = 0; i <= data->num; i++)
		{
			int rec_size,attr_size;
			string* attr_name = new string[32];
			fp >> rec_size >> attr_size;
			for (int k = 0; k < attr_size; k++)fp >> attr_name[k];
			for (int j = 0; j < rec_size; j++)
			{
				rec tmp;
				for (int k = 0; k < attr_size; k++)
				{
					fp >> buf_string[0];
					tmp.ins_rec(attr_name[k], buf_string[0]);
				}
				(*data->recs[i]).push_back(tmp);
			}
		}
		buf_mp[file_name] = data;
		buf_queue.push(file_name);
		fp.close();
	}
	return data;
}
BTree getRootfile(string tablename, int tree_index) //找到根节点并返回，没有则返回NULL
{
	ifstream fp("bin/CatIndex.txt");
	if (!fp.good())
	{
		fp.close();
		return NULL;
	}
	int now_root=-1;
	while (fp >> buf_string[0] >> buf_string[1] >> buf_int[0] >> buf_int[1] >> buf_int[2])
	{
		if (buf_string[1] == tablename && buf_int[0] == tree_index)
		{
			now_root = buf_int[2];
			break;
		}
	}
	if (now_root == -1)	return NULL;
	fp.close();
	return getBTfile(tablename, tree_index, now_root);
}
bool updateBTfile(BTree T) //对应table的对应B+树更新结点
{
	return createBTfile(T);
}
int getnewNo(string tablename, int tree_index) //分配一个可用的编号
{
	ifstream fp("bin/CatIndex.txt");
	if (!fp.good())return 0;
	FILE* fp2 = fopen("bin/CatIndex2.txt", "w");
	int flag = 0;
	while (fp >> buf_string[0] >> buf_string[1] >> buf_int[0] >> buf_int[1] >> buf_int[2])
	{
		if (buf_string[1] == tablename && buf_int[0] == tree_index)
		{
			flag = ++ buf_int[1];
		}
		fprintf(fp2, "%s %s %d %d %d\n", buf_string[0].c_str(), buf_string[1].c_str(), buf_int[0], buf_int[1], buf_int[2]);
	}
	fp.close();
	fclose(fp2);
	remove("bin/CatIndex.txt");
	rename("bin/CatIndex2.txt", "bin/CatIndex.txt");
	return flag;
}
bool upd(string table_name, int tree_index, int new_root)
{
	ifstream fp("bin/CatIndex.txt");
	if (!fp.good())return 0;
	FILE* fp2 = fopen("bin/CatIndex2.txt", "w");
	while (fp >> buf_string[0] >> buf_string[1] >> buf_int[0] >> buf_int[1] >> buf_int[2])
	{
		if (buf_string[1] == table_name && buf_int[0] == tree_index)
		{
			buf_int[2] = new_root;
		}
		fprintf(fp2, "%s %s %d %d %d\n", buf_string[0].c_str(), buf_string[1].c_str(), buf_int[0], buf_int[1], buf_int[2]);
	}
	fp.close();
	fclose(fp2);
	remove("bin/CatIndex.txt");
	rename("bin/CatIndex2.txt", "bin/CatIndex.txt");
	return 1;
}