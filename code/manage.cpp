#include<iostream>
#include<cstdio>
#include<algorithm>
#include<string>
#include<cstdlib>
#include<map>
#include"minisql.h"
#include"api.h"
#include"manage.h"
#include"b+tree.h"
#include"buffer.h"

bool con_satisfy(rec r, int con_size, cond* con_list) {
	bool state = 1;
	int i, j;
	string* a = r.get_attr_list();
	for (i = 0; i < con_size; i++) {//��ÿ������
		for (j = 0; j < r.get_attr_size(); j++) {//��ÿ����¼������
			if (a[j] == con_list[i].get_attr()) {//�����Ҫ�Ƚ�
				int op = con_list[i].get_op();//���бȽϣ��������򷵻�0
				if (op == 0) {
					if (r.at(j) != con_list[i].get_para()) {
						state = 0;
						return 0;
					}
				}
				else if (op == -1) {
					if (r.at(j) > con_list[i].get_para()) {
						state = 0;
						return 0;
					}
				}
				else if (op == 1) {
					if (r.at(j) < con_list[i].get_para()) {
						state = 0;
						return 0;
					}
				}
			}
		}
	}
	return state;//û�в����ϵģ�����1
}
//extern Vector<table_c *>* tables = NULL;//��������������ڵ�ȫ�ֱ�����������
//extern Vector<BTree> *table_indexs = NULL;//�Ժ���������뽨����BTree��Ϊ��ʡ�£�
//Index Manager ��Ҫʵ�ֵĺ���ԭ��
data_c select_from_table(const string table_name, const select_c* data) {
	int i, j, k;
	data_c result;
	table_c* tb;
	if (load_one_table(table_name, tb) == 1) {//��������table
		BTree poi = getRootfile(table_name, tb->get_primkey());//��������
		while (poi->isleaf == 0) {
			poi = getBTfile(poi->tablename, poi->tree_index, poi->child[0]);
		}
		while (poi != NULL) {
			for (i = 0; i < poi->num; i++) {//��ÿ��vector
				for (j = 0; j < poi->recs[i]->size(); j++) {//��vector���ÿ��rec
					rec r = poi->recs[i]->at(j);
					if (con_satisfy(r, data->get_conlist_size(), data->get_con_list())) {//����where
						string* sel = data->get_attr_list();
						string* recsel = r.get_attr_list();
						rec pop;
						int kk;
						for (k = 0; k < r.get_attr_size(); k++) {//��r���ÿ������
							for (kk = 0; kk < data->get_attr_size(); kk++) {//��select���ÿ������
								if (recsel[k] == sel[kk]) {//���ƥ��
									pop.ins_rec(sel[kk], r.at(k));//ѡ��
								}
							}
						}
						result.ins_data(pop);//���ؽ�data
					}
				}
			}
			poi = getBTfile(poi->tablename, poi->tree_index, poi->child[ORDER - 1]);
		}
	}
	else throw "Error: Table Not Exists!";
	return result;
}
bool insert_into_table(const string table_name, const data_c* data) {
	int i, j, k;
	table_c* tb;
	if (load_one_table(table_name, tb) == 1) {
		attr_c* tb_attr = tb->get_attr_list();
		rec* recs = data->get_rec_list();
		string* dt_attr = recs[0].get_attr_list();
		bool state = tb->get_attr_size()==recs->get_attr_size();
		/*for (i = 0; i < tb->get_attr_size(); i++) {
			if (tb_attr[i].get_attr_name() != dt_attr[i]) {
				state = 0; break;
			}
		}*/
		if (state == 0) return 0;//�������ݵ����������������������
		//�����Ը��ƽ�ȥ������insert�ǵ���ָ��������ôд�������Ժ���չ��
		for (int j = 0; j < data->get_rec_size(); j++)
			for (int i = 0; i < tb->get_attr_size(); i++)
				recs[j].set_attr_list(i, tb_attr[i].get_attr_name());

		BTree* T = new BTree[tb->get_attr_size()];
		for (j = 0; j < data->get_rec_size(); j++) {
			for (k = 0; k < tb->get_attr_size(); k++) {
				T[k] = getRootfile(table_name, k);
				if (T[k] != NULL) {
					if (k == tb->get_primkey()) Insert(recs[j].at(k), &T[k], 0, recs[j]);//���������
					else Insert(recs[j].at(k), &T[k], 1, recs[j]);
					delete T[k];//�ͷ��ڴ�
				}
			}
		}
		delete[] T;
		return 1;
	}
	else return 0;
}//�ɹ�����1�����򷵻�0����ͬ
bool delete_from_table(const string table_name, const where_c* data) {
	int i, j, k;
	table_c* tb;
	if (load_one_table(table_name, tb) == 1) {//��������table
		BTree* T = new BTree[tb->get_attr_size()];
		for (k = 0; k < tb->get_attr_size(); k++) {//��ÿ��index
			T[k] = getRootfile(table_name, k);
			if (T[k] != NULL) {
				BTree poi = T[k];
				while (poi->isleaf == 0) {
					poi = getBTfile(poi->tablename, poi->tree_index, poi->child[0]);
				}
				while (poi != NULL) {
					for (i = 0; i < poi->num; i++) {//��ÿ��vector
						for (j = 0; j < poi->recs[i]->size(); j++) {//��vector���ÿ��rec
							rec r = poi->recs[i]->at(j);
							if (con_satisfy(r, data->get_conlist_size(), data->get_con_list())) {//����where
								string key = poi->val[i];
								poi->recs[i]->deletei(j);
								if (poi->recs[i]->size() == 0) {
									Delete(key, &T[k]);//ɾ���������
								}
								else {
									updateBTfile(poi);//ɾ������е�һ����¼��ֻ��Ҫ���½��
								}
							}
						}
					}
					poi = getBTfile(poi->tablename, poi->tree_index, poi->child[ORDER - 1]);
				}
				delete T[k];//�ͷ��ڴ�
			}
		}
		delete[] T;
		return 1;
	}
	else return 0;
}

bool create_index(const string table_name, const string attr_name, const string index_name) {
	int i, j, k; string a;
	if (load_one_index(index_name, a, i) == 0) {//���û���ظ���index
		table_c* tb;
		if (load_one_table(table_name, tb) == 1) {//���table����
			bool state = 0;
			attr_c* attrs = tb->get_attr_list();
			for (k = 0; k < tb->get_attr_size(); k++) {
				if (attrs[k].get_attr_name() == attr_name) {//�������������
					state = 1;
					break;
				}
			}
			if (state == 0) return 0;//������������
			BTree T = getRootfile(table_name, k);
			if (T != NULL) {//����Ѿ���index
				insertindex(index_name, table_name, k);
				upd(table_name, k, T->BT_No);
			}
			else {//�½�index
				insertindex(index_name, table_name, k);//��Ϣ�ӵ������
				T = CreateBTree(table_name, k);//�½�����
				BTree poi = getRootfile(table_name, tb->get_primkey());//��������
				while (poi->isleaf == 0) {
					poi = getBTfile(poi->tablename, poi->tree_index, poi->child[0]);
				}
				while (poi != NULL) {
					for (i = 0; i < poi->num; i++) {//��ÿ��vector
						for (j = 0; j < poi->recs[i]->size(); j++) {//��vector���ÿ��rec
							rec r = poi->recs[i]->at(j);
							Insert(r.at(k), &T, 1, r);//�ܴ��������Ŀ϶����������
						}
					}
					poi = getBTfile(poi->tablename, poi->tree_index, poi->child[ORDER - 1]);
				}
			}
			return 1;
		}
		else return 0;
	}
	else return 0;
}

//create index index_name on table_name(attr_name)
bool drop_index(const string table_name, const string index_name) {
	int i, j, k;
	string tb_name;
	if (load_one_index(index_name, tb_name, k) == 1) {//�ҵ�index�����Ϣ
		deleteindex(index_name);
		if (table_name != tb_name) return 0;
		table_c* tb;
		if (load_one_table(table_name, tb) == 1) {//���table����
			if (k == tb->get_primkey()) return 1;//��ɾ�����index
			BTree T = getRootfile(table_name, k);
			return DeleteATree(T);//ɾ��index
		}
		else return 0;
	}
	else return 0;
}
//drop index index_name on table_name
bool create_table(table_c data) {
	table_c* tb;
	if (load_one_table(data.get_table_name(), tb) == 1) return 0;
	else {
		inserttable(data);
		string index_name = data.get_table_name();
		insertindex(index_name, data.get_table_name(), data.get_primkey());
		BTree T = CreateBTree(data.get_table_name(), data.get_primkey());//���������
		return 1;
	}
}

bool drop_table(string table_name) {
	int i, k;
	table_c* tb;
	if (load_one_table(table_name, tb) == 1) {
		deletetable(table_name);
		//ɾ����������
		BTree* T = new BTree[tb->get_attr_size()];
		for (k = 0; k < tb->get_attr_size(); k++) {
			T[k] = getRootfile(table_name, k);
			DeleteATree(T[k]);
		}
		delete[] T;//�ͷ��ڴ�
		return 1;
	}
	else return 0;
}