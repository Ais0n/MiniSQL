1、工程文件下要有bin文件夹才能正常运行。
2、语句格式：（以下为示例）
select Attr_A, Attr_B from Tab_name where A = B and C between D and E
insert into Tab_name values(A,B,C,D)
delete from Tab_name where cond1 and cond2
create index index_name on Tab_name(column_name)
drop index index_name on Tab_name
create table Tab_name(attr_1 int, attr_2 float, primary key(attr_1)) //不加primary key会报错

3、文件格式
3.1 Index文件（存放BNode）
table_name tree_index BT_No num isleaf isroot
val_0 val_1 ... val_order-1
child_0 ... child_order
rec_size
attr_size
attr_0 attr_1 ...
rec_0_val_0 rec_0_val_1 ...
rec_1_val_0 rec_1_val_1 ...

每个文件文件名Ind_(table_name)_(tree_index)_(BT_No).txt

说明：（BNode已有的属性不再解释）
rec_size：记录条目数
attr_size：属性个数
attr_0, attr_1, ...：表格属性名
rec_0_val_0：第0条记录的第0个属性的属性值，其他类推

3.2 Catalog_table文件（存放table_c）
table_name element_size rec_per_page
attr_size primary_key_no
attr_name1 attr_type attr_len isunique
attr_name2 attr_type attr_len isunique
...

每个文件文件名Cat_(table_name).txt

说明：
element_size：每条记录的长度
rec_per_page：每页最多存放的记录个数

3.3 Catalog_index文件（即索引管理文件） 每一行是一个索引的信息，每行的格式如下
index_name table_name tree_index index_size index_root_no

文件名CatIndex.txt

说明：
index_size：索引的BNode节点个数（当前最大编号）
index_root_no：索引的根节点编号