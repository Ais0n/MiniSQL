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
using namespace std;

valtype FindLeft(BTree T);

BTree CreateBTree(string tn, int ti);
bool Insert(valtype key, BTree* T, bool dupl, rec& data);
bool Delete(valtype key, BTree* T);
bool DeleteATree(BTree& T);