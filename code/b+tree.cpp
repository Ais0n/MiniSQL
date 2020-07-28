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

bool Insert(valtype key, BTree* T, bool dupl, rec& data) {//dupl:�Ƿ������ظ�,Ĭ��T������NULL
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
		path[toppoi++] = poi;//��¼����·��
		int i;//λ��
		for (i = 0; i < poi->num; i++) {
			if (dupl == 0) {
				if (key == poi->val[i]) {
					//	cout << "already have this value" << endl;
					return 0;
				}//�������ظ�
			}
			if (key < poi->val[i]) break;
		}
		if (i < poi->num) poi = getBTfile(tn, ti, poi->child[i]);//��ʱiλ���Ǳ�key�����Сֵ��������С��
		else poi = getBTfile(tn, ti, poi->child[i]);//ʽ��һ�������岻ͬ����Ϊi�Ѿ��ӹ�1��
	}

	int i;

	for (i = 0; i < poi->num; i++) {
		if (key == poi->val[i]) {
			if (dupl == 0) {//�������ظ�
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

	if (poi->num < ORDER - 1) {//û��
		for (i = poi->num; i > 0 && key < poi->val[i - 1]; i--) {//����Ųλ��
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
			a[i] = a[i - 1];//����Ųλ��
			ar[i] = ar[i - 1];
		}
		a[i] = key;//����
		ar[i] = new Vector<rec>(CAP);
		ar[i]->push_back(data);

		BTree new1 = new struct BNode;
		BTree new2 = new struct BNode;
		for (i = 0; i < (ORDER + 1) / 2; i++) {// order/2����ȡ��
			new1->val[i] = a[i];
			new1->recs[i] = ar[i];
		}
		for (i = (ORDER + 1) / 2; i < ORDER; i++) {// order/2����ȡ��
			new2->val[i - (ORDER + 1) / 2] = a[i];
			new1->recs[i - (ORDER + 1) / 2] = ar[i];
		}
		new1->isleaf = 1;
		new2->isleaf = 1;
		new1->isroot = 0;
		new2->isroot = 0;
		new1->num = (ORDER + 1) / 2;
		new2->num = ORDER - (ORDER + 1) / 2;
		new1->child[ORDER - 1] = new2->BT_No;//��������
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
			if (toppoi > 0) father = path[--toppoi];//�ص���һ��
			else father = NULL;
			if (father == NULL) {//��Ҫ���µĸ�
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
					father->val[i] = father->val[i - 1];//����Ųλ��
					father->child[i] = father->child[i - 1];//ע��Ҫ�ƺ���
				}
				//	BTree release = father->child[i + 1];
				deleteBTfile(tn, ti, father->child[i]);
				father->child[i] = new1->BT_No;
				father->child[i + 1] = new2->BT_No;//��ԭ���������,�ǵ�free
				if (new1->isleaf) {
					if (i > 0) {
						BTree tmp = getBTfile(tn, ti, father->child[i - 1]);
						tmp->child[ORDER - 1] = new1->BT_No;
						updateBTfile(tmp);
					}
					if (i < father->num) {
						new2->child[ORDER - 1] = father->child[i + 2];//����
						updateBTfile(new2);
					}
				}
				father->val[i] = FindLeft(new2);//������һ�㣬������
				father->num++;
				updateBTfile(father);
				//	free(release);//�ͷ�ԭ����
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
				new4->num = ORDER - 1 - (ORDER + 1) / 2;//��һ���ķ��ѷ�ʽ��Ҫ�������Ӵ���ֵ

				valtype a[ORDER];
				int b[ORDER + 1];
				for (i = 0; i < ORDER - 1; i++) {
					a[i] = father->val[i];
					b[i] = father->child[i];
				}
				b[ORDER - 1] = father->child[ORDER - 1];
				b[ORDER] = b[ORDER - 1];
				for (i = ORDER - 1; i > 0 && key < a[i - 1]; i--) {
					a[i] = a[i - 1];//����Ųλ��
					b[i] = b[i - 1];
				}
				//	BTree release = b[i + 1];
				deleteBTfile(tn, ti, b[i]);
				b[i] = new1->BT_No;
				b[i + 1] = new2->BT_No;//�滻ԭ����
				a[i] = FindLeft(new2);//����
				if (new1->isleaf) {
					if (i > 0) {
						BTree tmp = getBTfile(tn, ti, b[i - 1]);
						tmp->child[ORDER - 1] = new1->BT_No;
						updateBTfile(tmp);
					}
					if (i < ORDER - 1) {
						new2->child[ORDER - 1] = b[i + 2];//����
						updateBTfile(new2);
					}
				}

				for (i = 0; i < (ORDER + 1) / 2; i++) {// order/2����ȡ��
					new3->val[i] = a[i];
					new3->child[i] = b[i];
				}
				new3->child[i] = b[i];//�Ѿ�+1����

				for (i = (ORDER + 1) / 2 + 1; i < ORDER; i++) {// order/2����ȡ��,������һ��ֵ����Ϊ�����Ѿ�����new3
					new4->val[i - (ORDER + 1) / 2 - 1] = a[i];
					new4->child[i - (ORDER + 1) / 2 - 1] = b[i];
				}
				new4->child[i - (ORDER + 1) / 2 - 1] = b[i];
				new3->BT_No = getnewNo(tn, ti);
				createBTfile(new3);
				new4->BT_No = getnewNo(tn, ti);
				createBTfile(new4);

				new1 = new3;
				new2 = new4;//����������
			//	free(release);//�ͷ�ԭ����
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
		return 0;//����û��
	}
	BTree poi = (*T);
	int toppoi = 0;
	BTree path[MAX];
	int pathpos[MAX];
	int prepos;

	while (poi->isleaf == 0) {
		path[toppoi++] = poi;//��¼����·��
		int i;//λ��
		for (i = 0; i < poi->num; i++) {
			if (key < poi->val[i]) break;
		}
		if (i < poi->num) getBTfile(tn, ti, poi->child[i]);//��ʱiλ���Ǳ�key�����Сֵ��������С��
		else getBTfile(tn, ti, poi->child[i]);//ʽ��һ�������岻ͬ����Ϊi�Ѿ��ӹ�1��
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
	if (poi->num > (ORDER - 1 + 1) / 2 || toppoi == 0) {//�㹻���ֵ��Ϊ��������Ϊ��n-1��/2��ȡ��
	//	poi->recs[position]->clear();
		for (i = position; i < poi->num - 1; i++) {
			poi->val[i] = poi->val[i + 1];
			poi->recs[i] = poi->recs[i + 1];
		}//����Ųλ��
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
			sibpoi = getBTfile(tn, ti, pre->child[prepos - 1]);//�����
		}
		else sibpoi = getBTfile(tn, ti, pre->child[prepos + 1]);//���ұ�
		toppoi++;

		if (sibpoi->num > (ORDER - 1 + 1) / 2) {
			if (prepos == 0) {//�����ҵ�
				for (i = position; i < poi->num - 1; i++) {
					poi->val[i] = poi->val[i + 1];
					poi->recs[i] = poi->recs[i + 1];
				}//����Ųλ��
				poi->val[poi->num - 1] = sibpoi->val[0];
				poi->recs[poi->num - 1] = sibpoi->recs[0];
				for (i = 0; i < sibpoi->num - 1; i++) {
					sibpoi->val[i] = sibpoi->val[i + 1];
					sibpoi->recs[i] = sibpoi->recs[i + 1];
				}
				sibpoi->num--;//ǰ��Ľ����-1

				pre->val[prepos] = sibpoi->val[0];//��һ���ֵҪ��
			}
			else {//�����ҵ�
				for (i = position; i > 0; i--) {
					poi->val[i] = poi->val[i - 1];
					poi->recs[i] = poi->recs[i - 1];
				}//����Ųλ��
				poi->val[0] = sibpoi->val[sibpoi->num - 1];
				poi->recs[0] = sibpoi->recs[sibpoi->num - 1];
				sibpoi->num--;//ǰ��Ľ����-1

				pre->val[prepos - 1] = poi->val[0];
			}
			updateBTfile(poi);
			updateBTfile(sibpoi);
			updateBTfile(pre);
			return 1;
		}
		else {
			if (prepos > 0) {//�����ҵ�
				sibpoi->child[ORDER - 1] = poi->child[ORDER - 1];
				for (i = 0; i < poi->num; i++) {
					if (i != position) {
						sibpoi->val[sibpoi->num++] = poi->val[i];
						sibpoi->recs[sibpoi->num - 1] = poi->recs[i];
					}
				}//��ֵ�Ƶ���һ��leaf��
			}
			else {//�����ҵ�
				int tmp = toppoi;
				while (1) {
					if (toppoi > 0) {
						pre = path[--toppoi];
						prepos = pathpos[toppoi];
					}
					else {
						pre = NULL;//˵��poi�������
					}
					if (prepos > 0) {
						pre = getBTfile(tn, ti, pre->child[prepos - 1]);
						while (pre->isleaf == 0) {
							pre = getBTfile(tn, ti, pre->child[pre->num]);
						}//��ߵ������½�
						break;
					}
				}
				toppoi = tmp;
				if (pre != NULL) pre->child[ORDER - 1] = sibpoi->BT_No;//��������
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
				}//��ֵ�Ƶ���һ��leaf��
				pre = path[--toppoi];
				pre->val[0] = sibpoi->val[0];
				updateBTfile(pre);
				toppoi++;
			}
			updateBTfile(sibpoi);

			//�ݹ��ɾ��poi
			int poipos;
			while (1) {
				if (toppoi > 1) {
					poi = path[--toppoi];
					poipos = pathpos[toppoi];
					if (poipos > 0) position = poipos - 1;
					else position = poipos;//�ҵ�Ҫɾ���Ľ�㣬�ǵ�ɾchild[poipos]
				}
				else {
					poi = path[--toppoi];
					poipos = pathpos[toppoi];
					if (poipos > 0) position = poipos - 1;
					else position = poipos;//�ҵ�Ҫɾ���Ľ�㣬�ǵ�ɾchild[poipos]
					for (i = position; i < poi->num - 1; i++) {
						poi->val[i] = poi->val[i + 1];
					}
					deleteBTfile(tn, ti, poi->child[poipos]);
					for (i = poipos; i < poi->num; i++) {
						poi->child[i] = poi->child[i + 1];
					}//����Ųλ��
					poi->num--;
					if (poi->num == 0) {
						(*T) = getBTfile(tn, ti, poi->child[0]);//�ϲ�
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
				if (poi->num > (ORDER + 1) / 2 - 1) {//����ȡ����-1
					for (i = position; i < poi->num - 1; i++) {
						poi->val[i] = poi->val[i + 1];
					}
					deleteBTfile(tn, ti, poi->child[poipos]);
					for (i = poipos; i < poi->num; i++) {
						poi->child[i] = poi->child[i + 1];
					}//����Ųλ��
					poi->num--;
					updateBTfile(poi);
					return 1;
				}
				else {
					pre = path[--toppoi];
					prepos = pathpos[toppoi];
					if (prepos > 0) sibpoi = getBTfile(tn, ti, pre->child[prepos - 1]);//�����
					else sibpoi = getBTfile(tn, ti, pre->child[prepos + 1]);//���ұ�
					toppoi++;//�ҵ��ֵܽڵ�

					if (sibpoi->num > (ORDER + 1) / 2 - 1) {//����ȡ����-1
						if (prepos == 0) {//�����ҵ�
							for (i = position; i < poi->num - 1; i++) {
								poi->val[i] = poi->val[i + 1];
							}
							deleteBTfile(tn, ti, poi->child[poipos]);
							for (i = poipos; i < poi->num; i++) {
								poi->child[i] = poi->child[i + 1];
							}//����Ųλ��
							poi->val[poi->num - 1] = FindLeft(sibpoi);
							poi->child[poi->num] = sibpoi->child[0];
							for (i = 0; i < sibpoi->num - 1; i++) {
								sibpoi->val[i] = sibpoi->val[i + 1];
								sibpoi->child[i] = sibpoi->child[i + 1];
							}
							sibpoi->child[i] = sibpoi->child[i + 1];//����һ��child
							sibpoi->num--;//ǰ��Ľ����-1

							pre->val[prepos] = FindLeft(sibpoi);//��һ���ֵҪ��
						}
						else {//�����ҵ�
							for (i = position; i > 0; i--) {
								poi->val[i] = poi->val[i - 1];
							}
							deleteBTfile(tn, ti, poi->child[poipos]);
							for (i = poipos; i > 0; i--) {
								poi->child[i] = poi->child[i - 1];
							}
							//����Ųλ��

							poi->val[0] = FindLeft(getBTfile(tn, ti, poi->child[1]));
							poi->child[0] = sibpoi->child[sibpoi->num];
							sibpoi->num--;//ǰ��Ľ����-1

							pre->val[prepos - 1] = FindLeft(poi);
						}
						updateBTfile(poi);
						updateBTfile(sibpoi);
						updateBTfile(pre);
						return 1;
					}
					else {
						deleteBTfile(tn, ti, poi->child[poipos]);
						if (prepos > 0) {//��
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
						else {//��
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
						//��ֵ�Ƶ���һ��leaf��
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
	delete T;//�ͷ��ڴ�
	return 1;
}
/*
bool Find(valtype key, BTree *T) {
	BTree poi = (*T);
	while (poi->isleaf == 0) {
		int i;//λ��
		for (i = 0; i < poi->num; i++) {
			if (key < poi->val[i]) break;
		}
		if (i < poi->num) poi = poi->child[i];//��ʱiλ���Ǳ�key�����Сֵ��������С��
		else poi = poi->child[i];//ʽ��һ�������岻ͬ����Ϊi�Ѿ��ӹ�1��
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