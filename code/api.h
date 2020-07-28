#pragma once
#include<iostream>
#include<cstdio>
#include<algorithm>
#include<string>
#include<cstdlib>
#include<map>
#include<vector>
#include"minisql.h"
#include"vector.h"
#include"manage.h"
#include"b+tree.h"
#include"buffer.h"
using namespace std;
void init();
void login(user& usr);
void exec(string o);
void getwhere(string o, int i, where_c* data);
void select_exec(string o, int i);
void insert_exec(string o, int i);
void delete_exec(string o, int i);
void create_exec(string o, int i);
void drop_exec(string o, int i);
void create_table_exec(string o, int i);
void create_index_exec(string o, int i);
void drop_table_exec(string o, int i);
void drop_index_exec(string o, int i);
void ssplit(const string& s, vector<string>& sv);