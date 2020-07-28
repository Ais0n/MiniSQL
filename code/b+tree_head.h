#pragma once
#include<string>
#include"minisql.h"
#include"Vector.h"
using namespace std;
const int MAX = 50;//最高高度
const int ORDER = 4;
const int CAP = 3;//记录条数容量
typedef string valtype;
struct BNode {
	string tablename;
	int tree_index;
	int BT_No;
	valtype val[ORDER];//结点的键值，按递增的顺序排
	int child[ORDER];//指向孩子的指针
	int num;//键值的数量
	bool isleaf;//是否为叶结点
	bool isroot;//是否为根结点
	Vector<rec>* recs[ORDER];//记录
	BNode() {
		int i;
		for (i = 0; i < ORDER - 1; i++) recs[i] = new Vector<rec>(CAP);
	}
	BNode(string tn, int ti, int bt, valtype* v, int n, bool isl, bool isr, Vector<rec>** r = NULL, int* chi = NULL) {
		tablename = tn;
		tree_index = ti;
		BT_No = bt;
		num = n;
		isleaf = isl;
		isroot = isr;
		int i;
		for (i = 0; i < n; i++) val[i] = v[i];
		if (chi != NULL) {
			for (i = 0; i <= n; i++) child[i] = chi[i];
		}
		if (r != NULL) {
			for (i = 0; i < n; i++) recs[i] = r[i];
		}
	}
};
typedef struct BNode* BTree;