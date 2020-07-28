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
void inserttable(table_c& data);//��data������д�������ļ���ĩβ
bool load_one_table(string table_name, table_c* & data);//��table����data��,û���򷵻�0
bool deletetable(string table_name);//ɾ����Ϊtablename��table�����ڹ����ļ��У�
void insertindex(string index_name, string tablename, int tree_index);//��index������д�������ļ���ĩβ
bool load_one_index(string index_name, string& tablename, int& tree_index);//��index��������������Ϣ��,û���򷵻�0
bool deleteindex(string index_name);//ɾ����Ϊindex_name��index�����ڹ����ļ��У�

bool createBTfile(BTree T);//��Ӧtable�Ķ�ӦB+��������
bool deleteBTfile(string tablename, int tree_index, int BT_No);//ɾ����Ӧtable�Ķ�ӦB+���Ľ��BT_No
bool deleteBTfile(BTree& T);
BTree getBTfile(string tablename, int tree_index, int BT_No);//û���򷵻�NULL
BTree getRootfile(string tablename, int tree_index);//�ҵ����ڵ㲢���أ�û���򷵻�NULL
bool updateBTfile(BTree T);//��Ӧtable�Ķ�ӦB+�����½��
int getnewNo(string tablename, int tree_index);//����һ�����õı��
bool upd(string table_name, int tree_index, int new_root);