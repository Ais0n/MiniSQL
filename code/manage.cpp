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
	for (i = 0; i < con_size; i++) {//对每个条件
		for (j = 0; j < r.get_attr_size(); j++) {//对每个记录的属性
			if (a[j] == con_list[i].get_attr()) {//如果需要比较
				int op = con_list[i].get_op();//进行比较，不符合则返回0
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
	return state;//没有不符合的，返回1
}
//extern Vector<table_c *>* tables = NULL;//添加在主函数所在的全局变量声明部分
//extern Vector<BTree> *table_indexs = NULL;//对函数表的主码建立的BTree（为了省事）
//Index Manager 需要实现的函数原型
data_c select_from_table(const string table_name, const select_c* data) {
	int i, j, k;
	data_c result;
	table_c* tb;
	if (load_one_table(table_name, tb) == 1) {//有这样的table
		BTree poi = getRootfile(table_name, tb->get_primkey());//读入数据
		while (poi->isleaf == 0) {
			poi = getBTfile(poi->tablename, poi->tree_index, poi->child[0]);
		}
		while (poi != NULL) {
			for (i = 0; i < poi->num; i++) {//对每个vector
				for (j = 0; j < poi->recs[i]->size(); j++) {//对vector里的每个rec
					rec r = poi->recs[i]->at(j);
					if (con_satisfy(r, data->get_conlist_size(), data->get_con_list())) {//满足where
						string* sel = data->get_attr_list();
						string* recsel = r.get_attr_list();
						rec pop;
						int kk;
						for (k = 0; k < r.get_attr_size(); k++) {//对r里的每个属性
							for (kk = 0; kk < data->get_attr_size(); kk++) {//对select里的每个属性
								if (recsel[k] == sel[kk]) {//如果匹配
									pop.ins_rec(sel[kk], r.at(k));//选出
								}
							}
						}
						result.ins_data(pop);//加载进data
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
		if (state == 0) return 0;//插入数据的属性数与表格的属性数不符
		//将属性复制进去（现在insert是单条指令所以这么写，这里以后扩展）
		for (int j = 0; j < data->get_rec_size(); j++)
			for (int i = 0; i < tb->get_attr_size(); i++)
				recs[j].set_attr_list(i, tb_attr[i].get_attr_name());

		BTree* T = new BTree[tb->get_attr_size()];
		for (j = 0; j < data->get_rec_size(); j++) {
			for (k = 0; k < tb->get_attr_size(); k++) {
				T[k] = getRootfile(table_name, k);
				if (T[k] != NULL) {
					if (k == tb->get_primkey()) Insert(recs[j].at(k), &T[k], 0, recs[j]);//主码的索引
					else Insert(recs[j].at(k), &T[k], 1, recs[j]);
					delete T[k];//释放内存
				}
			}
		}
		delete[] T;
		return 1;
	}
	else return 0;
}//成功返回1，否则返回0，下同
bool delete_from_table(const string table_name, const where_c* data) {
	int i, j, k;
	table_c* tb;
	if (load_one_table(table_name, tb) == 1) {//有这样的table
		BTree* T = new BTree[tb->get_attr_size()];
		for (k = 0; k < tb->get_attr_size(); k++) {//对每个index
			T[k] = getRootfile(table_name, k);
			if (T[k] != NULL) {
				BTree poi = T[k];
				while (poi->isleaf == 0) {
					poi = getBTfile(poi->tablename, poi->tree_index, poi->child[0]);
				}
				while (poi != NULL) {
					for (i = 0; i < poi->num; i++) {//对每个vector
						for (j = 0; j < poi->recs[i]->size(); j++) {//对vector里的每个rec
							rec r = poi->recs[i]->at(j);
							if (con_satisfy(r, data->get_conlist_size(), data->get_con_list())) {//满足where
								string key = poi->val[i];
								poi->recs[i]->deletei(j);
								if (poi->recs[i]->size() == 0) {
									Delete(key, &T[k]);//删除整个结点
								}
								else {
									updateBTfile(poi);//删除结点中的一个记录，只需要更新结点
								}
							}
						}
					}
					poi = getBTfile(poi->tablename, poi->tree_index, poi->child[ORDER - 1]);
				}
				delete T[k];//释放内存
			}
		}
		delete[] T;
		return 1;
	}
	else return 0;
}

bool create_index(const string table_name, const string attr_name, const string index_name) {
	int i, j, k; string a;
	if (load_one_index(index_name, a, i) == 0) {//如果没有重复的index
		table_c* tb;
		if (load_one_table(table_name, tb) == 1) {//如果table存在
			bool state = 0;
			attr_c* attrs = tb->get_attr_list();
			for (k = 0; k < tb->get_attr_size(); k++) {
				if (attrs[k].get_attr_name() == attr_name) {//如果属性名存在
					state = 1;
					break;
				}
			}
			if (state == 0) return 0;//属性名不存在
			BTree T = getRootfile(table_name, k);
			if (T != NULL) {//如果已经有index
				insertindex(index_name, table_name, k);
				upd(table_name, k, T->BT_No);
			}
			else {//新建index
				insertindex(index_name, table_name, k);//信息加到管理表
				T = CreateBTree(table_name, k);//新建索引
				BTree poi = getRootfile(table_name, tb->get_primkey());//读入数据
				while (poi->isleaf == 0) {
					poi = getBTfile(poi->tablename, poi->tree_index, poi->child[0]);
				}
				while (poi != NULL) {
					for (i = 0; i < poi->num; i++) {//对每个vector
						for (j = 0; j < poi->recs[i]->size(); j++) {//对vector里的每个rec
							rec r = poi->recs[i]->at(j);
							Insert(r.at(k), &T, 1, r);//能创建索引的肯定不是主码的
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
	if (load_one_index(index_name, tb_name, k) == 1) {//找到index相关信息
		deleteindex(index_name);
		if (table_name != tb_name) return 0;
		table_c* tb;
		if (load_one_table(table_name, tb) == 1) {//如果table存在
			if (k == tb->get_primkey()) return 1;//不删主码的index
			BTree T = getRootfile(table_name, k);
			return DeleteATree(T);//删除index
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
		BTree T = CreateBTree(data.get_table_name(), data.get_primkey());//主码的索引
		return 1;
	}
}

bool drop_table(string table_name) {
	int i, k;
	table_c* tb;
	if (load_one_table(table_name, tb) == 1) {
		deletetable(table_name);
		//删除所有索引
		BTree* T = new BTree[tb->get_attr_size()];
		for (k = 0; k < tb->get_attr_size(); k++) {
			T[k] = getRootfile(table_name, k);
			DeleteATree(T[k]);
		}
		delete[] T;//释放内存
		return 1;
	}
	else return 0;
}