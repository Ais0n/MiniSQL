1�������ļ���Ҫ��bin�ļ��в����������С�
2������ʽ��������Ϊʾ����
select Attr_A, Attr_B from Tab_name where A = B and C between D and E
insert into Tab_name values(A,B,C,D)
delete from Tab_name where cond1 and cond2
create index index_name on Tab_name(column_name)
drop index index_name on Tab_name
create table Tab_name(attr_1 int, attr_2 float, primary key(attr_1)) //����primary key�ᱨ��

3���ļ���ʽ
3.1 Index�ļ������BNode��
table_name tree_index BT_No num isleaf isroot
val_0 val_1 ... val_order-1
child_0 ... child_order
rec_size
attr_size
attr_0 attr_1 ...
rec_0_val_0 rec_0_val_1 ...
rec_1_val_0 rec_1_val_1 ...

ÿ���ļ��ļ���Ind_(table_name)_(tree_index)_(BT_No).txt

˵������BNode���е����Բ��ٽ��ͣ�
rec_size����¼��Ŀ��
attr_size�����Ը���
attr_0, attr_1, ...�����������
rec_0_val_0����0����¼�ĵ�0�����Ե�����ֵ����������

3.2 Catalog_table�ļ������table_c��
table_name element_size rec_per_page
attr_size primary_key_no
attr_name1 attr_type attr_len isunique
attr_name2 attr_type attr_len isunique
...

ÿ���ļ��ļ���Cat_(table_name).txt

˵����
element_size��ÿ����¼�ĳ���
rec_per_page��ÿҳ����ŵļ�¼����

3.3 Catalog_index�ļ��������������ļ��� ÿһ����һ����������Ϣ��ÿ�еĸ�ʽ����
index_name table_name tree_index index_size index_root_no

�ļ���CatIndex.txt

˵����
index_size��������BNode�ڵ��������ǰ����ţ�
index_root_no�������ĸ��ڵ���