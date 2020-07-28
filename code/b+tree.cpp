#include <iostream>
#include <fstream>
#include <string>
#include "vector.h"
#include "minisql.h"
#include "b+tree.h"
#include "buffer.h"
BTree CreateBTree(string tn, int ti)
{
	BTree T = new struct BNode;
	T->tablename = tn;
	T->tree_index = ti;
	T->num = 0;
	T->isleaf = 1;
	T->isroot = 1;
	T->child[0] = -1;
	T->BT_No = getnewNo(tn, ti);
	createBTfile(T);
	upd(tn, ti, T->BT_No);
	return T;
}

bool Insert(valtype key, BTree* T, bool dupl, rec& data) {//dupl:是否允许重复,默认T不等于NULL
	string tn;
	int ti;
	tn = (*T)->tablename; ti = (*T)->tree_index;
	if ((*T)->num == 0) {
		(*T)->val[0] = key;
		(*T)->isleaf = 1;
		(*T)->num++;
		(*T)->recs[0] = new Vector<rec>(CAP);
		(*T)->recs[0]->push_back(data);
		updateBTfile(*T);
		return 1;
	}
	BTree poi = (*T);
	int toppoi = 0;
	BTree path[MAX];

	while (poi->isleaf == 0) {
		path[toppoi++] = poi;//记录搜索路径
		int i;//位置
		for (i = 0; i < poi->num; i++) {
			if (dupl == 0) {
				if (key == poi->val[i]) {
					//	cout << "already have this value" << endl;
					return 0;
				}//不允许重复
			}
			if (key < poi->val[i]) break;
		}
		if (i < poi->num) poi = getBTfile(tn, ti, poi->child[i]);//此时i位置是比key大的最小值，向左找小的
		else poi = getBTfile(tn, ti, poi->child[i]);//式子一样，意义不同，因为i已经加过1了
	}

	int i;

	for (i = 0; i < poi->num; i++) {
		if (key == poi->val[i]) {
			if (dupl == 0) {//不允许重复
				//cout << "already have this value" << endl;
				return 0;
			}
			else {
				poi->recs[i]->push_back(data);
				updateBTfile(poi);
				return 1;
			}
		}
	}

	if (poi->num < ORDER - 1) {//没满
		for (i = poi->num; i > 0 && key < poi->val[i - 1]; i--) {//向右挪位置
			poi->val[i] = poi->val[i - 1];
			poi->recs[i] = poi->recs[i - 1];
		}
		poi->val[i] = key;
		poi->num++;
		poi->recs[i] = new Vector<rec>(CAP);
		poi->recs[i]->push_back(data);
		updateBTfile(poi);
		return 1;
	}
	else {
		valtype a[ORDER];
		Vector<rec>* ar[ORDER];
		for (i = 0; i < ORDER - 1; i++) {
			a[i] = poi->val[i];
			ar[i] = poi->recs[i];
		}
		for (i = ORDER - 1; i > 0 && key < a[i - 1]; i--) {
			a[i] = a[i - 1];//向右挪位置
			ar[i] = ar[i - 1];
		}
		a[i] = key;//插入
		ar[i] = new Vector<rec>(CAP);
		ar[i]->push_back(data);

		BTree new1 = new struct BNode;
		BTree new2 = new struct BNode;
		for (i = 0; i < (ORDER + 1) / 2; i++) {// order/2向上取整
			new1->val[i] = a[i];
			new1->recs[i] = ar[i];
		}
		for (i = (ORDER + 1) / 2; i < ORDER; i++) {// order/2向上取整
			new2->val[i - (ORDER + 1) / 2] = a[i];
			new1->recs[i - (ORDER + 1) / 2] = ar[i];
		}
		new1->isleaf = 1;
		new2->isleaf = 1;
		new1->isroot = 0;
		new2->isroot = 0;
		new1->num = (ORDER + 1) / 2;
		new2->num = ORDER - (ORDER + 1) / 2;
		new1->child[ORDER - 1] = new2->BT_No;//连接起来
		new2->child[ORDER - 1] = -1;
		new1->tablename = tn;
		new1->tree_index = ti;
		new2->tablename = tn;
		new2->tree_index = ti;
		new1->BT_No = getnewNo(tn, ti);
		createBTfile(new1);
		new2->BT_No = getnewNo(tn, ti);
		createBTfile(new2);

		BTree father;
		while (1) {
			if (toppoi > 0) father = path[--toppoi];//回到上一层
			else father = NULL;
			if (father == NULL) {//需要开新的根
				BTree root;
				root = new struct BNode;
				root->tablename = tn;
				root->tree_index = ti;
				root->child[0] = new1->BT_No;
				root->child[1] = new2->BT_No;
				root->val[0] = FindLeft(new2);
				root->isleaf = 0;
				root->isroot = 1;
				root->num = 1;
				root->BT_No = getnewNo(tn, ti);
				createBTfile(root);
				upd(tn, ti, root->BT_No);
				(*T)->isroot = 0;
				updateBTfile(*T);
				*T = root;
				break;
			}
			else if (father->num < ORDER - 1) {
				father->child[father->num + 1] = father->child[father->num];
				for (i = father->num; i > 0 && key < father->val[i - 1]; i--) {
					father->val[i] = father->val[i - 1];//向右挪位置
					father->child[i] = father->child[i - 1];//注意要移孩子
				}
				//	BTree release = father->child[i + 1];
				deleteBTfile(tn, ti, father->child[i]);
				father->child[i] = new1->BT_No;
				father->child[i + 1] = new2->BT_No;//把原来的替代掉,记得free
				if (new1->isleaf) {
					if (i > 0) {
						BTree tmp = getBTfile(tn, ti, father->child[i - 1]);
						tmp->child[ORDER - 1] = new1->BT_No;
						updateBTfile(tmp);
					}
					if (i < father->num) {
						new2->child[ORDER - 1] = father->child[i + 2];//连接
						updateBTfile(new2);
					}
				}
				father->val[i] = FindLeft(new2);//右下移一层，向左找
				father->num++;
				updateBTfile(father);
				//	free(release);//释放原来的
				break;
			}
			else if (father->num == ORDER - 1) {
				BTree new3 = new struct BNode;
				BTree new4 = new struct BNode;
				new3->tablename = tn;
				new3->tree_index = ti;
				new4->tablename = tn;
				new4->tree_index = ti;
				new3->isleaf = 0;
				new4->isleaf = 0;
				new3->isroot = 0;
				new4->isroot = 0;
				new3->num = (ORDER + 1) / 2;
				new4->num = ORDER - 1 - (ORDER + 1) / 2;//不一样的分裂方式，要丢掉连接处的值

				valtype a[ORDER];
				int b[ORDER + 1];
				for (i = 0; i < ORDER - 1; i++) {
					a[i] = father->val[i];
					b[i] = father->child[i];
				}
				b[ORDER - 1] = father->child[ORDER - 1];
				b[ORDER] = b[ORDER - 1];
				for (i = ORDER - 1; i > 0 && key < a[i - 1]; i--) {
					a[i] = a[i - 1];//向右挪位置
					b[i] = b[i - 1];
				}
				//	BTree release = b[i + 1];
				deleteBTfile(tn, ti, b[i]);
				b[i] = new1->BT_No;
				b[i + 1] = new2->BT_No;//替换原来的
				a[i] = FindLeft(new2);//插入
				if (new1->isleaf) {
					if (i > 0) {
						BTree tmp = getBTfile(tn, ti, b[i - 1]);
						tmp->child[ORDER - 1] = new1->BT_No;
						updateBTfile(tmp);
					}
					if (i < ORDER - 1) {
						new2->child[ORDER - 1] = b[i + 2];//连接
						updateBTfile(new2);
					}
				}

				for (i = 0; i < (ORDER + 1) / 2; i++) {// order/2向上取整
					new3->val[i] = a[i];
					new3->child[i] = b[i];
				}
				new3->child[i] = b[i];//已经+1过了

				for (i = (ORDER + 1) / 2 + 1; i < ORDER; i++) {// order/2向上取整,丢掉第一个值，因为孩子已经给了new3
					new4->val[i - (ORDER + 1) / 2 - 1] = a[i];
					new4->child[i - (ORDER + 1) / 2 - 1] = b[i];
				}
				new4->child[i - (ORDER + 1) / 2 - 1] = b[i];
				new3->BT_No = getnewNo(tn, ti);
				createBTfile(new3);
				new4->BT_No = getnewNo(tn, ti);
				createBTfile(new4);

				new1 = new3;
				new2 = new4;//继续往上找
			//	free(release);//释放原来的
			}
		}
	}

	return 1;
}

bool Delete(valtype key, BTree* T) {
	string tn;
	int ti;
	tn = (*T)->tablename; ti = (*T)->tree_index;
	if ((*T)->num == 0) {
		return 0;//本身没有
	}
	BTree poi = (*T);
	int toppoi = 0;
	BTree path[MAX];
	int pathpos[MAX];
	int prepos;

	while (poi->isleaf == 0) {
		path[toppoi++] = poi;//记录搜索路径
		int i;//位置
		for (i = 0; i < poi->num; i++) {
			if (key < poi->val[i]) break;
		}
		if (i < poi->num) getBTfile(tn, ti, poi->child[i]);//此时i位置是比key大的最小值，向左找小的
		else getBTfile(tn, ti, poi->child[i]);//式子一样，意义不同，因为i已经加过1了
		pathpos[toppoi - 1] = i;
		prepos = i;
	}

	int i, position;
	bool state = 0;
	for (i = 0; i < poi->num; i++) {
		if (key == poi->val[i]) {
			state = 1;
			position = i;
			break;
		}
	}
	if (state == 0) {
		return 0;
	}
	if (poi->num > (ORDER - 1 + 1) / 2 || toppoi == 0) {//足够多的值或为根，最少为（n-1）/2上取整
	//	poi->recs[position]->clear();
		for (i = position; i < poi->num - 1; i++) {
			poi->val[i] = poi->val[i + 1];
			poi->recs[i] = poi->recs[i + 1];
		}//向左挪位置
		poi->num--;
		updateBTfile(poi);
		return 1;
	}
	else {
		BTree sibpoi;
		BTree pre;

		pre = path[--toppoi];
		prepos = pathpos[toppoi];
		if (prepos > 0) {
			sibpoi = getBTfile(tn, ti, pre->child[prepos - 1]);//找左边
		}
		else sibpoi = getBTfile(tn, ti, pre->child[prepos + 1]);//找右边
		toppoi++;

		if (sibpoi->num > (ORDER - 1 + 1) / 2) {
			if (prepos == 0) {//向右找的
				for (i = position; i < poi->num - 1; i++) {
					poi->val[i] = poi->val[i + 1];
					poi->recs[i] = poi->recs[i + 1];
				}//向左挪位置
				poi->val[poi->num - 1] = sibpoi->val[0];
				poi->recs[poi->num - 1] = sibpoi->recs[0];
				for (i = 0; i < sibpoi->num - 1; i++) {
					sibpoi->val[i] = sibpoi->val[i + 1];
					sibpoi->recs[i] = sibpoi->recs[i + 1];
				}
				sibpoi->num--;//前面的结点数-1

				pre->val[prepos] = sibpoi->val[0];//上一层的值要变
			}
			else {//向左找的
				for (i = position; i > 0; i--) {
					poi->val[i] = poi->val[i - 1];
					poi->recs[i] = poi->recs[i - 1];
				}//向右挪位置
				poi->val[0] = sibpoi->val[sibpoi->num - 1];
				poi->recs[0] = sibpoi->recs[sibpoi->num - 1];
				sibpoi->num--;//前面的结点数-1

				pre->val[prepos - 1] = poi->val[0];
			}
			updateBTfile(poi);
			updateBTfile(sibpoi);
			updateBTfile(pre);
			return 1;
		}
		else {
			if (prepos > 0) {//向左找的
				sibpoi->child[ORDER - 1] = poi->child[ORDER - 1];
				for (i = 0; i < poi->num; i++) {
					if (i != position) {
						sibpoi->val[sibpoi->num++] = poi->val[i];
						sibpoi->recs[sibpoi->num - 1] = poi->recs[i];
					}
				}//把值移到另一个leaf里
			}
			else {//向右找的
				int tmp = toppoi;
				while (1) {
					if (toppoi > 0) {
						pre = path[--toppoi];
						prepos = pathpos[toppoi];
					}
					else {
						pre = NULL;//说明poi在最左边
					}
					if (prepos > 0) {
						pre = getBTfile(tn, ti, pre->child[prepos - 1]);
						while (pre->isleaf == 0) {
							pre = getBTfile(tn, ti, pre->child[pre->num]);
						}//左边的最右下角
						break;
					}
				}
				toppoi = tmp;
				if (pre != NULL) pre->child[ORDER - 1] = sibpoi->BT_No;//左右连接
				updateBTfile(pre);

				for (i = sibpoi->num + poi->num - 2; i >= poi->num - 1; i--) {
					sibpoi->val[i] = sibpoi->val[i - poi->num + 1];
					sibpoi->recs[i] = sibpoi->recs[i - poi->num + 1];
				}
				int count = 0;
				for (i = 0; i < poi->num; i++) {
					if (i != position) {
						sibpoi->val[count++] = poi->val[i];
						sibpoi->recs[count - 1] = poi->recs[i];
						sibpoi->num++;
					}
				}//把值移到另一个leaf里
				pre = path[--toppoi];
				pre->val[0] = sibpoi->val[0];
				updateBTfile(pre);
				toppoi++;
			}
			updateBTfile(sibpoi);

			//递归地删除poi
			int poipos;
			while (1) {
				if (toppoi > 1) {
					poi = path[--toppoi];
					poipos = pathpos[toppoi];
					if (poipos > 0) position = poipos - 1;
					else position = poipos;//找到要删除的结点，记得删child[poipos]
				}
				else {
					poi = path[--toppoi];
					poipos = pathpos[toppoi];
					if (poipos > 0) position = poipos - 1;
					else position = poipos;//找到要删除的结点，记得删child[poipos]
					for (i = position; i < poi->num - 1; i++) {
						poi->val[i] = poi->val[i + 1];
					}
					deleteBTfile(tn, ti, poi->child[poipos]);
					for (i = poipos; i < poi->num; i++) {
						poi->child[i] = poi->child[i + 1];
					}//向左挪位置
					poi->num--;
					if (poi->num == 0) {
						(*T) = getBTfile(tn, ti, poi->child[0]);//合并
						(*T)->isroot = 1;
						deleteBTfile(poi);
						upd(tn, ti, (*T)->BT_No);
						updateBTfile(*T);
					}
					else {
						updateBTfile(poi);
					}
					return 1;
				}
				if (poi->num > (ORDER + 1) / 2 - 1) {//向上取整并-1
					for (i = position; i < poi->num - 1; i++) {
						poi->val[i] = poi->val[i + 1];
					}
					deleteBTfile(tn, ti, poi->child[poipos]);
					for (i = poipos; i < poi->num; i++) {
						poi->child[i] = poi->child[i + 1];
					}//向左挪位置
					poi->num--;
					updateBTfile(poi);
					return 1;
				}
				else {
					pre = path[--toppoi];
					prepos = pathpos[toppoi];
					if (prepos > 0) sibpoi = getBTfile(tn, ti, pre->child[prepos - 1]);//找左边
					else sibpoi = getBTfile(tn, ti, pre->child[prepos + 1]);//找右边
					toppoi++;//找到兄弟节点

					if (sibpoi->num > (ORDER + 1) / 2 - 1) {//向上取整并-1
						if (prepos == 0) {//向右找的
							for (i = position; i < poi->num - 1; i++) {
								poi->val[i] = poi->val[i + 1];
							}
							deleteBTfile(tn, ti, poi->child[poipos]);
							for (i = poipos; i < poi->num; i++) {
								poi->child[i] = poi->child[i + 1];
							}//向左挪位置
							poi->val[poi->num - 1] = FindLeft(sibpoi);
							poi->child[poi->num] = sibpoi->child[0];
							for (i = 0; i < sibpoi->num - 1; i++) {
								sibpoi->val[i] = sibpoi->val[i + 1];
								sibpoi->child[i] = sibpoi->child[i + 1];
							}
							sibpoi->child[i] = sibpoi->child[i + 1];//多移一个child
							sibpoi->num--;//前面的结点数-1

							pre->val[prepos] = FindLeft(sibpoi);//上一层的值要变
						}
						else {//向左找的
							for (i = position; i > 0; i--) {
								poi->val[i] = poi->val[i - 1];
							}
							deleteBTfile(tn, ti, poi->child[poipos]);
							for (i = poipos; i > 0; i--) {
								poi->child[i] = poi->child[i - 1];
							}
							//向右挪位置

							poi->val[0] = FindLeft(getBTfile(tn, ti, poi->child[1]));
							poi->child[0] = sibpoi->child[sibpoi->num];
							sibpoi->num--;//前面的结点数-1

							pre->val[prepos - 1] = FindLeft(poi);
						}
						updateBTfile(poi);
						updateBTfile(sibpoi);
						updateBTfile(pre);
						return 1;
					}
					else {
						deleteBTfile(tn, ti, poi->child[poipos]);
						if (prepos > 0) {//左
							int oldnum = sibpoi->num;
							if (poipos > 0) sibpoi->val[sibpoi->num++] = FindLeft(getBTfile(tn, ti, poi->child[0]));
							else sibpoi->val[sibpoi->num++] = FindLeft(getBTfile(tn, ti, poi->child[1]));
							for (i = 0; i < poi->num; i++) {
								if (i != position) {
									sibpoi->val[sibpoi->num++] = poi->val[i];
								}
							}
							for (i = 0; i <= poi->num; i++) {
								if (i != poipos) {
									sibpoi->child[++oldnum] = poi->child[i];
								}
							}
						}
						else {//右
							sibpoi->child[sibpoi->num + poi->num] = sibpoi->child[sibpoi->num];
							for (i = sibpoi->num + poi->num - 1; i >= poi->num; i--) {
								sibpoi->val[i] = sibpoi->val[i - poi->num];
								sibpoi->child[i] = sibpoi->child[i - poi->num];
							}
							sibpoi->val[poi->num - 1] = FindLeft(sibpoi);
							sibpoi->num++;
							int count = 0;
							for (i = 0; i < poi->num; i++) {
								if (i != position) {
									sibpoi->val[count++] = poi->val[i];
									sibpoi->num++;
								}
							}
							count = 0;
							for (i = 0; i <= poi->num; i++) {
								if (i != poipos) {
									sibpoi->child[count++] = poi->child[i];
								}
							}
							pre = path[--toppoi];
							pre->val[0] = FindLeft(sibpoi);
							toppoi++;
							updateBTfile(pre);
						}
						updateBTfile(sibpoi);
						//把值移到另一个leaf里
					}
				}
			}
		}
	}
}

valtype FindLeft(BTree T) {
	string tn;
	int ti;
	tn = T->tablename;
	ti = T->tree_index;
	while (T->isleaf == 0)  T = getBTfile(tn, ti, T->child[0]);
	return T->val[0];
}

bool DeleteATree(BTree& T) {
	if (T == NULL) return 0;
	int i, j;
	if (T->isleaf == 0) {
		for (i = 0; i <= T->num; i++) {
			string tn;
			int ti;
			tn = T->tablename;
			ti = T->tree_index;
			BTree tmp = getBTfile(tn, ti, T->child[i]);
			DeleteATree(tmp);
		}
		deleteBTfile(T);
	}
	else {
		deleteBTfile(T);
	}
	delete T;//释放内存
	return 1;
}
/*
bool Find(valtype key, BTree *T) {
	BTree poi = (*T);
	while (poi->isleaf == 0) {
		int i;//位置
		for (i = 0; i < poi->num; i++) {
			if (key < poi->val[i]) break;
		}
		if (i < poi->num) poi = poi->child[i];//此时i位置是比key大的最小值，向左找小的
		else poi = poi->child[i];//式子一样，意义不同，因为i已经加过1了
	}
	int i;
	for (i = 0; i < poi->num; i++) {
		if (key == poi->val[i]) {
			cout << key << endl;
			return 1;
		}
	}
	return 0;
}

bool FindDupl(valtype key, BTree *T) {
	bool state = 0;
	BTree poi = (*T);
	while (poi->isleaf == 0)  poi = poi->child[0];
	while (1) {
		int i;
		for (i = 0; i < poi->num; i++) {
			if (key == poi->val[i]) {
				cout << poi->val[i] << ", ";
				state = 1;
			}
		}
		if (poi->child[ORDER - 1] == NULL)break;
		else poi = poi->child[ORDER - 1];
	}
	return state;
}

void output(BTree *T) {
	BTree poi = (*T);
	while (poi->isleaf == 0)  poi = poi->child[0];
	while (1) {
		int i;
		for (i = 0; i < poi->num; i++) {
			cout << poi->val[i] << ", ";
		}
		if (poi->child[ORDER - 1] == NULL)break;
		else poi = poi->child[ORDER - 1];
	}
}
*/