#pragma once
#include<string>
#include"minisql.h"
#include"Vector.h"
using namespace std;
const int MAX = 50;//��߸߶�
const int ORDER = 4;
const int CAP = 3;//��¼��������
typedef string valtype;
struct BNode {
	string tablename;
	int tree_index;
	int BT_No;
	valtype val[ORDER];//���ļ�ֵ����������˳����
	int child[ORDER];//ָ���ӵ�ָ��
	int num;//��ֵ������
	bool isleaf;//�Ƿ�ΪҶ���
	bool isroot;//�Ƿ�Ϊ�����
	Vector<rec>* recs[ORDER];//��¼
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