#include<iostream>
#include<cstdio>
#include<algorithm>
#include<string>
#include<cstdlib>
#include<map>
#include<iomanip>
#include<vector>
#include"api.h"
#include"minisql.h"
using namespace std;
map <string, string> usr_dtb;
void login(user& usr)
{
	if (usr_dtb[usr.get_account()] == usr.get_pwd())usr.login();
}
void ssplit(const string& s, vector<string>& sv) {
	sv.clear();
	string temp;
	int flag1 = 0,  flag2 = 0;
	for (int i = 0; i < s.length(); i++)
	{
		if (s[i] == '\'')flag1 = !flag1;
		if (s[i] == ' ' && !flag1)
		{
			if (flag2)
			{
				sv.push_back(temp);
				temp = "";
				flag2 = 0;
			}
		}
		else
		{
			temp = temp + s[i];
			flag2 = 1;
		}
	}
	if (temp != "")sv.push_back(temp);
}
int check_digit(string u)
{
	if (u.length() > 5)return -1;
	long long int res = 0;
	for (int i = 0; i < u.length(); i++)
	{
		if (!isdigit(u[i]))return -1;
		res = res * 10 + u[i] - '0';
	}
	return res;
}
bool cmp(string u,string v)
{
	transform(u.begin(), u.end(), u.begin(), tolower);
	transform(v.begin(), v.end(), v.begin(), tolower);
	return u == v;
}
void init()
{
	usr_dtb["root"] = "123456";
}
void exec(string o)
{
	int i = 0;
	string op = "";
	try {
		for (; i < o.length(); i++)if (o[i] != ' ')break;
		for (; i < o.length(); i++)
		{
			if (!isalpha(o[i]))break;
			op = op + o[i];
		}
		transform(op.begin(), op.end(), op.begin(), tolower);
		if (op == "select")
		{
			select_exec(o,i);
		}
		else if (op == "insert")
		{
			insert_exec(o,i);
		}
		else if (op == "delete")
		{
			delete_exec(o, i);
		}
		else if (op == "create")
		{
			create_exec(o, i);
		}
		else if (op == "drop")
		{
			drop_exec(o, i);
		}
		else
		{
			throw "Error : Unknown Operation Type!";
		}
	}
	catch (const char* e)
	{
		cout << e << endl;
		return;
	}
}
void getwhere(string o,int i, where_c* data)
{
	for (; i < o.length(); i++)if (o[i] != ' ' && o[i] != '\n')break;
	if (i == o.length())
	{
		return;
	}
	if (i + 4 >= o.length())throw "Error: grammatical error when finding 'where'";
	string tmp = o.substr(i, 5);
	transform(tmp.begin(), tmp.end(), tmp.begin(), tolower);
	if (tmp == "where" && (i + 5 >= o.length() || o[i + 5] == ' ' || o[i + 5] == '\n'))
	{
		i += 5;
	}
	else
	{
		throw "Error: grammatical error when finding 'where'";
	}
	int flag = 0;
	for (int j = i; j < o.length(); j++)
	{
		if (o[j] == '\'')
		{
			flag = !flag;
		}
		if (o[j] == '=' && !flag)
		{
			o = o.substr(0, j) + " = "+ o.substr(j + 1, o.length() - j - 1);
			j += 2;
		}
	}
	if (flag)throw "Error: Quotation Marks Not Paired!";
	cond ncond;
	vector<string> sv;
	ssplit(o.substr(i, o.length() - i), sv);
	for (int j = 0; j < sv.size();)
	{
		if (j + 2 >= sv.size())
		{
			throw "Error: Parameters Too Few";
		}
		else
		{
			string temp = sv[j + 1];
			transform(temp.begin(), temp.end(), temp.begin(), tolower);
			if (temp == "between")
			{
				if (j + 4 >= sv.size())throw "Error: Parameters Too Few";
				temp = sv[j + 3];
				transform(temp.begin(), temp.end(), temp.begin(), tolower);
				if (temp != "and")throw "Error: 'And' Not Found";
				ncond.set_attr(sv[j]);
				ncond.set_op(1);
				ncond.set_para(sv[j + 2]);
				data->ins_con(ncond);
				ncond.set_op(-1);
				ncond.set_para(sv[j + 4]);
				data->ins_con(ncond);
				j += 5;
			}
			else if (temp == "=")
			{
				ncond.set_attr(sv[j]);
				ncond.set_op(0);
				ncond.set_para(sv[j + 2]);
				data->ins_con(ncond);
				j += 3;
			}
			else throw "Error: Unidentified Operation!";
			if (j == sv.size())break;
			else
			{
				temp = sv[j];
				transform(temp.begin(), temp.end(), temp.begin(), tolower);
				if (temp != "and")
				{
					throw "Error: 'And' Not Found!";
				}
				j++;
				if (j == sv.size())
				{
					throw "Error: Expression Not Found After 'And'!";
				}
			}
		}
	}

}
void select_exec(string o,int i)
{
	int j, k;
	for (; i < o.length(); i++)if (o[i] != ' ')break;
	select_c* data = new select_c();
	for (j = i; j < o.length(); j++)
	{
		if (j + 3 < o.length() && o[j - 1] == ' ' && tolower(o[j]) == 'f' && tolower(o[j + 1]) == 'r' && tolower(o[j + 2]) == 'o' && tolower(o[j + 3]) == 'm' && (j + 4 >= o.length() || o[j + 4] == ' ' || o[j + 4] == '\n'))
		{
			break;
		}
	}
	if (j == o.length())
	{
		throw "Error: 'From' Not Found!";
	}
	while (i < j)
	{
		string attr_p = "";
		for (; i < j; i++)if (o[i] != ',' && o[i] != ' ')break;
		for (; i < j; i++)if (o[i] == ',' || o[i] == ' ')break; else attr_p = attr_p + o[i];
		if (attr_p == "")throw "Error: Attribute Not Found!";
		data->ins_attr(attr_p);
		if (o[i] == ',')
		{
			i++;
			continue;
		}
		else
		{
			for (++i; i < j; i++)
			{
				if (o[i] != ' ')
				{
					throw "Error: ',' omitted in attributes!";
				}
			}
		}
	}
	if (data->get_attr_size() == 0)
	{
		throw "Error: Attribute CANNOT be NULL!";
	}

	i = j + 4;
	for (; i < o.length(); i++)if (o[i] != ' ')break;
	string table_name = "";
	for (; i < o.length(); i++)if (o[i] != ' '&&o[i]!='\n')table_name = table_name + o[i]; else break;
	if (table_name == "")throw "Error: Table name CANNOT be NULL!";

	int flag = 1;
	for (int j = i; j < o.length(); j++)if (o[j] != ' ' && o[j] != '\n')flag = 0;
	if (!flag)
	{
		getwhere(o, i, data);
		if (data->get_conlist_size() == 0)
		{
			throw "Error: Expressions After 'WHERE' CANNOT be NULL!";
		}
	}
	data_c ans = select_from_table(table_name, data);
	if (ans.get_rec_size() == 0)
	{
		cout << "No Record Found." << endl;
		return;
	}
	rec* tmp = ans.get_rec_list();
	for (int j = 0; j < tmp[0].get_attr_size(); j++)
	{
		cout << std::left << setw(15) << tmp[0].get_attr_list(j) << " ";
	}
	cout << endl;
	for (int i = 0; i < ans.get_rec_size(); i++)
	{
		for (int j = 0; j < tmp[i].get_attr_size(); j++)
		{
			cout << std::left << setw(15) << tmp[i].get_val_list(j).c_str() << " ";
		}
		cout << endl;
	}
}
void insert_exec(string o, int i)
{
	int flag = 0;
	for (int j = i; j < o.length(); j++)
	{
		if (o[j] == '\'')flag = !flag;
		if (!flag&&(o[j] == '(' || o[j] == ')' || o[j] == ','))
		{
			o = o.substr(0, j) + ' ' + o[j] + ' ' + o.substr(j + 1, o.length() - j - 1);
			j += 2;
		}
	}
	vector<string> sv;
	ssplit(o.substr(i, o.length() - i), sv);
	if (sv.size() < 1 || !cmp(sv[0], "into"))throw "Error: 'Into' Not Found!";
	if (sv.size() < 2)throw "Error: Table Name CANNOT be NULL!";
	string tab_name = sv[1];
	if (sv.size() < 3 || !cmp(sv[2], "values"))throw "Error: 'Values' Not Found!";
	if (sv.size() < 4 || !cmp(sv[3], "("))throw "Error: Bracket(s) Not Found!";
	data_c data; rec single_rec;
	for (int j = 4; j < sv.size(); j+=2)
	{
		single_rec.ins_rec("", sv[j]);
		if (j + 1 == sv.size())
		{
			throw "Error: Bracket(s) Not Found!";
		}
		else if (!cmp(sv[j + 1], ",") && !cmp(sv[j + 1], ")"))
		{
			throw "Error: Grammatical Error When Processing Values!";
		}
	}
	if (!cmp(sv[sv.size() - 1], ")"))throw "Error: Bracket(s) Not Found!";
	if (single_rec.get_attr_size() == 0) throw "Error: Values CANNOT be NULL!";
	data.ins_data(single_rec);
	const data_c* tmp = &data;
	insert_into_table(tab_name, tmp);
}
void delete_exec(string o, int i)
{
	for (; i < o.length(); i++) if (o[i] != ' ')break;
	string temp = "";
	for (; i < o.length(); i++)if (o[i] != ' ')temp = temp + o[i]; else break;
	if (!cmp(temp, "from"))throw "Error: 'From' Not Found!";
	for (; i < o.length(); i++) if (o[i] != ' ')break;
	string tab_name;
	for (; i < o.length(); i++)if (o[i] != ' ')tab_name = tab_name + o[i]; else break;
	if (tab_name == "")throw "Error: Table Name CANNOT be NULL!";
	where_c* data=new where_c();
	getwhere(o, i, data);
	delete_from_table(tab_name, data);
}
void create_exec(string o, int i)
{
	for (; i < o.length(); i++) if (o[i] != ' ')break;
	string temp = "";
	for (; i < o.length(); i++)if (o[i] != ' ')temp = temp + o[i]; else break;
	if (cmp(temp, "table"))create_table_exec(o, i);
	else if (cmp(temp, "index"))create_index_exec(o, i);
	else throw "Error: Grammatical Error when processing 'Create'!";
}
void drop_exec(string o, int i)
{
	for (; i < o.length(); i++) if (o[i] != ' ')break;
	string temp = "";
	for (; i < o.length(); i++)if (o[i] != ' ')temp = temp + o[i]; else break;
	if (cmp(temp, "table"))drop_table_exec(o, i);
	else if (cmp(temp, "index"))drop_index_exec(o, i);
	else throw "Error: Grammatical Error when processing 'Create'!";
}
void create_table_exec(string o, int i)
{
	for (int j = i; j < o.length(); j++)
	{
		if (o[j] == '(' || o[j] == ')' || o[j] == ',')
		{
			o = o.substr(0, j) + ' ' + o[j] + ' ' + o.substr(j + 1, o.length() - j - 1);
			j += 2;
		}
	}
	vector<string>sv;
	ssplit(o.substr(i, o.length() - i), sv);
	table_c data;
	if (sv.size() < 1)throw "Error: Table Name CANNOT be NULL!";
	else data.set_table_name(sv[0]);
	if (sv.size() < 2 || sv[1] != "(")throw "Error: Bracket(s) Not Found!";
	if (sv.size() == 2)throw "Table CANNOT be NULL!";
	string attr_name = "", primkey = ""; int typ = 0, len = 0; bool isunique = 0;
	for (int j = 2; j < sv.size(); )
	{
		if (j + 1 >= sv.size())
		{
			throw "Error: Type Not Found!";
		}
		else if (j + 2 >= sv.size())
		{
			throw "Error: Bracket(s) Not Found!";
		}
		if (sv[j] == ")")throw "Error: Attribute Not Found!";
		attr_name = sv[j];
		if (j + 1 < sv.size() && cmp(sv[j], "primary") && cmp(sv[j + 1], "key"))
		{
			if (j + 2 >= sv.size() || !cmp(sv[j + 2], "(")) throw "Error: Bracket(s) Not Found!";
			if (j + 3 >= sv.size())throw "Error: Primary Key CANNOT be NULL!";
			primkey = sv[j + 3];
			if (j + 4 >= sv.size() || !cmp(sv[j + 4], ")")) throw "Error: Bracket(s) Not Found!";
			j += 5;
		}
		else
		if (cmp(sv[j + 1], "int") || cmp(sv[j + 1], "float"))
		{
			if (cmp(sv[j + 1], "int"))typ = 0; else typ = 1;
			isunique = len = 0;
			if (cmp(sv[j + 2], "unique"))
			{
				isunique = 1;
				j += 3;
			}
			else j += 2;
			data.ins_attr(attr_c(attr_name, typ, len, isunique));
		}
		else if (cmp(sv[j + 1], "char"))
		{
			attr_name = sv[j]; typ = 2; isunique = len = 0;
			if (sv[j + 2] != "(")throw "Error: Bracket(s) Not Found!";
			if (j + 3 >= sv.size() || check_digit(sv[j + 3]) == -1)throw"Error: Length of Char Not Found!";
			else len = check_digit(sv[j + 3]);
			if (len == 0)throw"Error: Length of type 'Char' should be positive!";
			if (j + 4 >= sv.size() || sv[j + 4] != ")")throw "Error: Bracket(s) Not Found!";
			if (j + 5 < sv.size() && cmp(sv[j + 5], "unique"))isunique = 1, j += 6;
			else j += 5;
			data.ins_attr(attr_c(attr_name, typ, len, isunique));
		}
		else throw "Error: Unidentified Attribute Type!";
		if (j >= sv.size() || (sv[j] != "," && sv[j] != ")"))throw "Error: ',' or ')' Expected!";
		else if (sv[j] == ",")
		{
			j++;
			if (j >= sv.size())
			{
				throw "Error: Attribute Not Found!";
			}
		}
		else
		{
			if (j != sv.size() - 1)throw"Grammatical Error when processing 'Create Table'!";
			break;
		}
	}
	attr_c* tmp = data.get_attr_list();
	for (int j = 0; j < data.get_attr_size(); j++)
	{
		if (tmp[j].get_attr_name() == primkey)
		{
			if (data.get_primkey() == -1)
			{
				data.set_primkey(j);
				tmp[j].set_isunique(1);
			}
			else throw "Error: Primary Key not Unique!";
		}
	}
	if (data.get_primkey() == -1)throw "Error: Primary Key Not Found!";
	create_table(data);
}
void create_index_exec(string o, int i)
{
	for (int j = i; j < o.length(); j++)
	{
		if (o[j] == '(' || o[j] == ')')
		{
			o = o.substr(0, j) + ' ' + o[j] + ' ' + o.substr(j + 1, o.length() - j - 1);
			j += 2;
		}
	}
	string index_name, table_name, column_name;
	vector<string>sv;
	ssplit(o.substr(i, o.length() - i), sv);
	if (sv.size() < 1)throw "Error: Index Name CANNOT be NULL!";
	else index_name = sv[0];
	if (sv.size() < 2 || !cmp(sv[1], "on")) throw "Error: 'On' Not Found!";
	if (sv.size() < 3)throw "Error: Table Name CANNOT be NULL!";
	else table_name = sv[2];
	if (sv.size() < 4 || sv[3] != "(") throw "Error: Bracket(s) Not Found!";
	if (sv.size() < 5)throw "Error: Column Name Not Found!";
	else column_name = sv[4];
	if (sv.size() < 6 || sv[5] != ")") throw "Error: Bracket(s) Not Found!";
	if (sv.size() != 6)throw "Error: Grammatical Error when processing 'Create Index'!";
	create_index(table_name, column_name, index_name);
}
void drop_table_exec(string o, int i)
{
	string table_name;
	vector<string>sv;
	ssplit(o.substr(i, o.length() - i), sv);
	if (sv.size() < 1)throw "Error: Table Name CANNOT be NULL!";
	table_name = sv[0];
	if (sv.size() != 1) throw "Error: Grammatical Error when processing 'Drop Table'!";
	drop_table(table_name);
}
void drop_index_exec(string o, int i)
{
	string index_name, table_name;
	vector<string>sv;
	ssplit(o.substr(i, o.length() - i), sv);
	if (sv.size() < 1)throw "Error: Index Name CANNOT be NULL!";
	else index_name = sv[0];
	if (sv.size() < 2 || !cmp(sv[1], "on")) throw "Error: 'On' Not Found!";
	if (sv.size() < 3)throw "Error: Table Name CANNOT be NULL!";
	else table_name = sv[2];
	if (sv.size() != 3)throw "Error: Grammatical Error when processing 'Drop Index'!";
	drop_index(table_name, index_name);
}