#pragma once
#include<iostream>
#include<cstdio>
#include<algorithm>
#include<string>
#include<cstdlib>
#include<map>
#include"minisql.h"
#include"api.h"

bool con_satisfy(rec r, int con_size, cond* con_list);
data_c select_from_table(const string table_name, const select_c* data);
bool insert_into_table(const string table_name, const data_c* data);
bool delete_from_table(const string table_name, const where_c* data);
bool create_index(const string table_name, const string attr_name, const string index_name);
bool drop_index(const string table_name, const string index_name);
bool create_table(table_c data);
bool drop_table(string table_name);