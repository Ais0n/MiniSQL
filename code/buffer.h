#pragma once
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
#include"b+tree_head.h"
void inserttable(table_c& data);//将data各属性写到管理文件的末尾
bool load_one_table(string table_name, table_c* & data);//将table读到data里,没有则返回0
bool deletetable(string table_name);//删除名为tablename的table（仅在管理文件中）
void insertindex(string index_name, string tablename, int tree_index);//将index各属性写到管理文件的末尾
bool load_one_index(string index_name, string& tablename, int& tree_index);//将index读到后面两个信息里,没有则返回0
bool deleteindex(string index_name);//删除名为index_name的index（仅在管理文件中）

bool createBTfile(BTree T);//对应table的对应B+树创造结点
bool deleteBTfile(string tablename, int tree_index, int BT_No);//删除对应table的对应B+树的结点BT_No
bool deleteBTfile(BTree& T);
BTree getBTfile(string tablename, int tree_index, int BT_No);//没有则返回NULL
BTree getRootfile(string tablename, int tree_index);//找到根节点并返回，没有则返回NULL
bool updateBTfile(BTree T);//对应table的对应B+树更新结点
int getnewNo(string tablename, int tree_index);//分配一个可用的编号
bool upd(string table_name, int tree_index, int new_root);