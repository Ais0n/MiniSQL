#pragma once
#include<iostream>
#include<cstdio>
#include<algorithm>
#include<string>
#include<cstdlib>
#include<map>
using namespace std;
const int max_page = 10;
const int page_size = 4000;
class user {
	string account;
	string pwd;
	bool status;
public:
	user()
	{
		account = pwd = "";
		status = 0;
	}
	void login() {
		status = 1;
	}
	void logout() {
		status = 0;
	}
	string get_account()
	{
		return account;
	}
	string get_pwd()
	{
		return pwd;
	}
	void set_account(string account)
	{
		this->account = account;
		status = 0;
	}
	void set_pwd(string pwd)
	{
		this->pwd = pwd;
		status = 0;
	}
	bool get_status()
	{
		return status;
	}
};
class cond { //用于表达式的判断，如 a = 5
protected:
	string attr; //被比较的属性名
	int op; //比较符号（0：等于 -1：小于等于 1：大于等于）
	string para; //被比较的参数
public:
	cond()
	{
		attr = para = "";
		op = 0;
	}
	void set_attr(string attr)
	{
		this->attr = attr;
	}
	void set_para(string para)
	{
		this->para = para;
	}
	void set_op(int op)
	{
		this->op = op;
	}
	string get_attr()
	{
		return attr;
	}
	int get_op()
	{
		return op;
	}
	string get_para()
	{
		return para;
	}
};
class where_c {  //用于where约束
protected:
	int conlist_size; //where约束数量（0表示没有）
	cond* con_list;
public:
	where_c()
	{
		conlist_size = 0;
		con_list = new cond[32];
	}
	where_c(const where_c& u)
	{
		conlist_size = u.conlist_size;
		con_list = new cond[32];
		for (int i = 0; i < conlist_size; i++)con_list[i] = u.con_list[i];
	}
	where_c& operator = (const where_c& u)
	{
		conlist_size = u.conlist_size;
		for (int i = 0; i < conlist_size; i++)con_list[i] = u.con_list[i];
		return *this;
	}
	virtual ~where_c()
	{
		delete[] con_list;
	}
	void ins_con(cond u)
	{
		con_list[conlist_size++] = u;
	}
	int get_conlist_size() const
	{
		return conlist_size;
	}
	cond* get_con_list() const
	{
		return con_list;
	}
	cond con_at(int index)const { return con_list[index]; }
};
class select_c :public where_c {
protected:
	int attr_size; //被选择的属性数量（0表示全部）
	string* attr_list;
public:
	select_c() :where_c()
	{
		attr_size = 0;
		attr_list = new string[32];
	}
	select_c(const select_c& u) : where_c(u)
	{
		attr_size = u.attr_size;
		attr_list = new string[32];
		for (int i = 0; i < attr_size; i++)attr_list[i] = u.attr_list[i];
	}
	select_c& operator = (const select_c& u)
	{
		attr_size = u.attr_size;
		for (int i = 0; i < attr_size; i++)attr_list[i] = u.attr_list[i];
		conlist_size = u.conlist_size;
		for (int i = 0; i < conlist_size; i++)con_list[i] = u.con_list[i];
		return *this;
	}
	virtual ~select_c()
	{
		delete[] attr_list;
	}
	void ins_attr(string u)
	{
		attr_list[attr_size++] = u;
	}
	int get_attr_size() const
	{
		return attr_size;
	}
	string* get_attr_list() const
	{
		return attr_list;
	}
	string attr_at(int index) const {
		return attr_list[index];
	}
};
class rec {  //单条记录
protected:
	int attr_size;  //属性个数
	string* attr_list;  //属性名
	string* val_list;   //属性值
public:
	rec()
	{
		attr_size = 0;
		attr_list = new string[32];
		val_list = new string[32];
	}
	rec(const rec& u)
	{
		attr_size = u.attr_size;
		attr_list = new string[32];
		val_list = new string[32];
		for (int i = 0; i < attr_size; i++)
		{
			this->attr_list[i] = u.attr_list[i];
			this->val_list[i] = u.val_list[i];
		}
	}
	rec& operator = (const rec& u)
	{
		attr_size = u.attr_size;
		for (int i = 0; i < attr_size; i++)
		{
			this->attr_list[i] = u.attr_list[i];
			this->val_list[i] = u.val_list[i];
		}
		return *this;
	}
	~rec()
	{
		delete[] attr_list;
		delete[] val_list;
	}
	void ins_rec(string uu, string vv)
	{
		attr_list[attr_size] = uu;
		val_list[attr_size++] = vv;
	}
	int get_attr_size()
	{
		return attr_size;
	}
	string* get_attr_list()
	{
		return attr_list;
	}
	string* get_val_list()
	{
		return val_list;
	}
	string get_attr_list(int i)
	{
		return attr_list[i];
	}
	string get_val_list(int i)
	{
		return val_list[i];
	}
	string at(int index) { return val_list[index]; }
	void set_attr_list(int i,string u)
	{
		this->attr_list[i] = u;
	}
};
class data_c {  //记录表，用于insert操作的传参和select操作的返回
protected:
	int rec_size;
	rec* rec_list;
public:
	data_c()
	{
		rec_size = 0;
		rec_list = new rec[1000];
	}
	data_c(const data_c& u)
	{
		rec_size = u.rec_size;
		rec_list = new rec[1000];
		for (int i = 0; i < rec_size; i++)rec_list[i] = u.rec_list[i];
	}
	data_c& operator =(const data_c& u)
	{
		rec_size = u.rec_size;
		for (int i = 0; i < rec_size; i++)rec_list[i] = u.rec_list[i];
		return *this;
	}
	~data_c()
	{
		delete[] rec_list;
	}
	void ins_data(rec u)
	{
		rec_list[rec_size++] = u;
	}
	int get_rec_size() const
	{
		return rec_size;
	}
	rec* get_rec_list() const
	{
		return rec_list;
	}
};
class attr_c {
	string attr_name;
	int type;//0 int 1 float 2 char
	int length;//0 for int&float, length for char
	bool isunique;//1 if unique
public:
	attr_c(string attr_name = "", int type = 0, int length = 0, bool isunique = 0)
	{
		this->attr_name = attr_name;
		this->type = type;
		this->length = length;
		this->isunique = isunique;
	}
	void set_attr_c(string attr_name, int type, int length, bool isunique)
	{
		this->attr_name = attr_name;
		this->type = type;
		this->length = length;
		this->isunique = isunique;
	}
	string get_attr_name()
	{
		return attr_name;
	}
	int get_type()
	{
		return type;
	}
	int get_length()
	{
		return length;
	}
	bool get_isunique()
	{
		return isunique;
	}
	void set_isunique(bool inunique)
	{
		this->isunique = isunique;
	}
	bool operator==(attr_c& u) {
		return attr_name == u.attr_name;
	}
};
class table_c {
private:
	string table_name;//表名
	int attr_size;//属性个数
	attr_c* attr_list;//属性名
	int primkey;//主码的index，只有一个
	int element_size;//记录大小
	int rec_per_page;//每页记录数
public:
	table_c()
	{
		table_name = "";
		attr_list = new attr_c[32];
		attr_size = 0;
		primkey = -1;
		element_size = 0;
		rec_per_page = 0;
	}
	table_c(const table_c& u)
	{
		table_name = u.table_name;
		attr_size = u.attr_size;
		attr_list = new attr_c[32];
		for (int i = 0; i < u.attr_size; i++)
		{
			this->attr_list[i] = u.attr_list[i];
		}
		primkey = u.primkey;
		element_size = u.element_size;
		rec_per_page = u.rec_per_page;
	}
	table_c& operator =(const table_c& u)
	{
		table_name = u.table_name;
		attr_size = u.attr_size;
		for (int i = 0; i < u.attr_size; i++)
		{
			this->attr_list[i] = u.attr_list[i];
		}
		primkey = u.primkey;
		element_size = u.element_size;
		rec_per_page = u.rec_per_page;
		return *this;
	}
	~table_c()
	{
		delete[] attr_list;
	}
	void set_table_name(string u)
	{
		this->table_name = u;
	}
	void set_element_size(int u)
	{
		this->element_size = u;
	}
	void set_rec_per_page(int u)
	{
		this->rec_per_page = u;
	}
	void set_primkey(int u)
	{
		this->primkey = u;
	}
	void ins_attr(attr_c u)
	{
		attr_list[attr_size++] = u;
		//if (u.get_isunique()) primkey = attr_size - 1;
		if (u.get_type() != 2)element_size += 4;
		else element_size += u.get_length();
		rec_per_page = page_size / element_size;
	}
	string get_table_name()
	{
		return table_name;
	}
	int get_attr_size()
	{
		return attr_size;
	}
	attr_c* get_attr_list()
	{
		return attr_list;
	}
	int get_primkey() { 
		return primkey; 
	}
	int get_element_size()
	{
		return element_size;
	}
	int get_rec_per_page()
	{
		return rec_per_page;
	}
};