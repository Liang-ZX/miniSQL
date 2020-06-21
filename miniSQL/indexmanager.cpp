#include "indexmanager.h"
// 表格：|123|324|244|3435|5351|$|3435|1245235|1223|4343|214|$**************|2424|242222|65576|33643|23623|$
// 索引：213|34$123|24$2424|23$

Index_Manager::Index_Manager(string Table_name) : Table_name(Table_name) {
	vector<Index> index; //存储表上所有的索引信息
	catalog_manager.getIndex(Table_name, index);
	for (int i = 0; i < index.size(); i++) {//把所有已经创建的index文件写入内存
		if (index[i].type == 0) 
			Read_Index(index[i].indexName, INT);
		else if (index[i].type == -1) 
			Read_Index(index[i].indexName, FLOAT);
		else 
			Read_Index(index[i].indexName, STRING);
	}
}

Index_Manager::~Index_Manager() {
	//将所有索引写回缓冲区
	map<string, BPT<int>*>::iterator i_int;
	map<string, BPT<float>*>::iterator i_float;
	map<string, BPT<string>*>::iterator i_string;
	for (i_int = BPT_Int.begin(); i_int != BPT_Int.end();) {
		Node<int>* Leaf = i_int->second->MostLeftLeaf;
		string File = i_int->first;
		for (int i = 0; Leaf != NULL; i++) {
			string data;
			buffer_manager.writeFile("", File, 1, i);//将块i写空
			for (int j = 0; j < Leaf->key_num; j++) {
				string key = to_string(Leaf->key[j]);	
				string block_num = to_string(Leaf->value[j]);
				data.append(key);
				data = data + '|';
				data.append(block_num);
				data = data + '$';
			}
			buffer_manager.writeFile(data, File, 1, i);//重写块i
			Leaf = Leaf->next_leaf;
		}
		delete i_int->second;
		BPT_Int.erase(i_int++);
	}
	for (i_float = BPT_Float.begin(); i_float != BPT_Float.end();) {
		Node<float>* Leaf = i_float->second->MostLeftLeaf;
		string File = i_float->first;			  
		for (int i = 0; Leaf != NULL; i++) {
			string data;
			buffer_manager.writeFile("", File, 1, i);//将块i写空
			for (int j = 0; j < Leaf->key_num; j++) {
				string key = to_string(Leaf->key[j]);
				string block_num = to_string(Leaf->value[j]);
				data.append(key);
				data = data + '|';
				data.append(block_num);
				data = data + '$';
			}
			buffer_manager.writeFile(data, File, 1, i);//重写块i
			Leaf = Leaf->next_leaf;
		}
		delete i_float->second;
		BPT_Float.erase(i_float++);
	}
	for (i_string = BPT_String.begin(); i_string != BPT_String.end();) {
		Node<string>* Leaf = i_string->second->MostLeftLeaf;
		string File = i_string->first;
		for (int i = 0; Leaf != NULL; i++) {
			string data;
			buffer_manager.writeFile("", File, 1, i);//将块i写空
			for (int j = 0; j < Leaf->key_num; j++) {
				string key = Leaf->key[j];
				string block_num = to_string(Leaf->value[j]);
				data.append(key);
				data = data + '|';
				data.append(block_num);
				data = data + '$';
			}
			buffer_manager.writeFile(data, File, 1, i);//重写块i
			Leaf = Leaf->next_leaf;
		}
		delete i_string->second;
		BPT_String.erase(i_string++);
	}
}

void Index_Manager::Read_Index(string File, Type type, int n) {//读取硬盘中的索引文件
	Index &index = catalog_manager.getIndex(File);
	if (type == INT) {
		int Rank = BLOCK_SIZE / (10 + sizeof('$') + sizeof('|') + 10);//10:block_num对应的int和key对应的int
		BPT<int>* bpt = new BPT<int>(Rank);
		for (int i = 0; i < index.blockNum; i++) {
			string data = buffer_manager.readFile(File, 1, i);
			for (int j = 0; j < data.length();) {
				string key;
				string block_num;
				while (data[j] != '|') { key = key + data[j++]; } j++;
				while (data[j] != '$') { block_num = block_num + data[j++]; } j++;
				bpt->Insert_Key(atoi(key.c_str()), atoi(block_num.c_str()));
			}
		}
		BPT_Int.insert({ File, bpt });
	}
	else if (type == FLOAT) {
		int Rank = BLOCK_SIZE / (9 + sizeof('$') + sizeof('|') + 10);
		BPT<float>* bpt = new BPT<float>(Rank);
		BPT_Float.insert({ File, bpt });
		for (int i = 0; i < index.blockNum; i++) {
			string data = buffer_manager.readFile(File, 1, i);
			for (int j = 0; j < data.length();) {
				string key;
				string block_num;
				while (data[j] != '|') { key = key + data[j++]; } j++;
				while (data[j] != '$') { block_num = block_num + data[j++]; } j++;
				bpt->Insert_Key(atof(key.c_str()), atoi(block_num.c_str()));
			}
		}
		BPT_Float.insert({ File, bpt });
	}
	else {
		int Rank = BLOCK_SIZE / (sizeof(char) * n + sizeof('$') + sizeof('|') + 10);
		BPT<string>* bpt = new BPT<string>(Rank);
		for (int i = 0; i < index.blockNum; i++) {
			string data = buffer_manager.readFile(File, 1, i);
			for (int j = 0; j < data.length();) {
				string key;
				string block_num;
				while (data[j] != '|') { key = key + data[j++]; } j++;
				while (data[j] != '$') { block_num = block_num + data[j++]; } j++;
				bpt->Insert_Key(key, atoi(block_num.c_str()));
			}
		}
		BPT_String.insert({ File, bpt });
	}
}

void Index_Manager::Create_Index(string File, int column, Type type, Index& index, int n) {
	Table &table = catalog_manager.getTable(Table_name);
	////维护hasindex
	//table.attributes[column].hasindex = true;
	//Index index;
	//index.indexName = File;
	//index.column = column;
	//index.tableName = Table_name;
	//index.type = type;
	if (type == INT) {//创建int类型的索引文件
		int Rank = BLOCK_SIZE / (10 + sizeof('$') + sizeof('|') + 10);//10:block_num对应的int和key对应的int
		int symbol_num = table.attriNum + 1;//每行数据中'|'字符的数目
		BPT<int>* bpt = new BPT<int>(Rank);
		for (int i = 0; i < table.blockNum; i++) {
			string data = buffer_manager.readFile(Table_name, 0, i);
			for (int j = 0, count = 0; j < data.length();) {
				string key;
				if (data[j] == '|') {
					count++;
					j++;
					if (count % symbol_num == column + 1) {//找到了对应列数据之前的'|'
						while (data[j] != '|') { key = key + data[j++]; }
						bpt->Insert_Key(atoi(key.c_str()), i);
					}
				}
				else j++;
			}
		}
		index.blockNum = bpt->Leaf_num;//更新blockNum
		BPT_Int.insert({ File, bpt });
	}
	else if (type == FLOAT) {//创建float类型的索引文件
		int Rank = BLOCK_SIZE / (9 + sizeof('$') + sizeof('|') + 10);//9：float占用的最大位数
		int symbol_num = table.attriNum + 1;//每行数据中'|'字符的数目
		BPT<float>* bpt = new BPT<float>(Rank);
		for (int i = 0; i < table.blockNum; i++) {
			string data = buffer_manager.readFile(Table_name, 0, i);
			for (int j = 0, count = 0; j < data.length();) {
				string key;
				if (data[j] == '|') {
					count++;
					j++;
					if (count % symbol_num == column + 1) {//找到了对应列数据之前的'|'
						while (data[j] != '|') { key = key + data[j++]; }
						bpt->Insert_Key(atof(key.c_str()), i);
					}
				}
				else j++;
			}
		}
		index.blockNum = bpt->Leaf_num;//更新blockNum
		BPT_Float.insert({ File, bpt });
	}
	else {//创建string类型的索引文件
		int Rank = BLOCK_SIZE / (sizeof(char) * n + sizeof('$') + sizeof('|') + 10);
		int symbol_num = table.attriNum + 1;//每行数据中'|'字符的数目
		BPT<string>* bpt = new BPT<string>(Rank);
		for (int i = 0; i < table.blockNum; i++) {
			string data = buffer_manager.readFile(Table_name, 0, i);
			for (int j = 0, count = 0; j < data.length();) {
				string key;
				if (data[j] == '|') {
					count++;
					j++;
					if (count % symbol_num == column + 1) {//找到了对应列数据之前的'|'
						while (data[j] != '|') { key = key + data[j++]; }
						bpt->Insert_Key(key, i);
					}
				}
				else j++;
			}
		}
		index.blockNum = bpt->Leaf_num;//更新blockNum
		BPT_String.insert({ File, bpt });
	}
	/*catalog_manager.createIndex(index);*/
}


void Index_Manager::Drop_Index(string File, Type type) {
	buffer_manager.deleteFile(File, 1);
	////维护hasindex
	//Table table = catalog_manager.getTable(Table_name);
	//Index index = catalog_manager.getIndex(File);
	//int column = index.column;
	//table.attributes[column].hasindex = false;
	////删除catalog上的索引，清理内存
	//catalog_manager.dropIndex(File);
	if (type == INT) {
		delete BPT_Int.find(File)->second;
		BPT_Int.erase(File);
	}
	else if (type == FLOAT) {
		delete BPT_Float.find(File)->second;
		BPT_Float.erase(File);
	}
	else {
		delete BPT_String.find(File)->second;
		BPT_String.erase(File);
	}
}

//删除所有索引文件
void Index_Manager::Drop_All() {
	map<string, BPT<int>*>::iterator i_int;
	map<string, BPT<float>*>::iterator i_float;
	map<string, BPT<string>*>::iterator i_string;
	for (i_int = BPT_Int.begin(); i_int != BPT_Int.end();) {
		buffer_manager.deleteFile(i_int->first, 1);
		////维护hasindex
		//Index index = catalog_manager.getIndex(i_int->first);
		//Table table = catalog_manager.getTable(Table_name);
		//int column = index.column;
		//table.attributes[column].hasindex = false;
		////删除catalog上的索引，清理内存
		//catalog_manager.dropIndex(i_int->first);
		delete i_int->second;
		BPT_Int.erase(i_int++);
	}
	for (i_float = BPT_Float.begin(); i_float != BPT_Float.end();) {
		buffer_manager.deleteFile(i_float->first, 1);
		////维护hasindex
		//Index index = catalog_manager.getIndex(i_int->first);
		//Table table = catalog_manager.getTable(Table_name);
		//int column = index.column;
		//table.attributes[column].hasindex = false;
		////删除catalog上的索引，清理内存
		//catalog_manager.dropIndex(i_float->first);
		delete i_float->second;
		BPT_Float.erase(i_float++);
	}
	for (i_string = BPT_String.begin(); i_string != BPT_String.end();) {
		buffer_manager.deleteFile(i_string->first, 1);
		////维护hasindex
		//Index index = catalog_manager.getIndex(i_int->first);
		//Table table = catalog_manager.getTable(Table_name);
		//int column = index.column;
		//table.attributes[column].hasindex = false;
		////删除catalog上的索引，清理内存
		//catalog_manager.dropIndex(i_string->first);
		delete i_string->second;
		BPT_String.erase(i_string++);
	}
}

//清空所有索引文件
void Index_Manager::Clear_Index() {
	map<string, BPT<int>*>::iterator i_int;
	map<string, BPT<float>*>::iterator i_float;
	map<string, BPT<string>*>::iterator i_string;
	for (i_int = BPT_Int.begin(); i_int != BPT_Int.end(); i_int++) {//int
		i_int->second->Delete_All();
		Index &index = catalog_manager.getIndex(i_int->first);
		for (int i = 0; i < index.blockNum; i++) {
			buffer_manager.writeFile("", i_int->first, 1, i);
		}
		index.blockNum = 0;
	}
	for (i_float = BPT_Float.begin(); i_float != BPT_Float.end(); i_float++) {//float
		i_float->second->Delete_All();
		Index &index = catalog_manager.getIndex(i_float->first);
		for (int i = 0; i < index.blockNum; i++) {
			buffer_manager.writeFile("", i_int->first, 1, i);
		}
		index.blockNum = 0;
	}
	for (i_string = BPT_String.begin(); i_string != BPT_String.end(); i_string++) {//string
		i_string->second->Delete_All();
		Index &index = catalog_manager.getIndex(i_string->first);
		for (int i = 0; i < index.blockNum; i++) {
			buffer_manager.writeFile("", i_int->first, 1, i);
		}
		index.blockNum = 0;
	}
}

//查询操作
bool Index_Manager::Search(string File, int k, int& block_num) {
	BPT<int>* bpt = BPT_Int[File];
	if (bpt->Search_Key(k, block_num)) return true;
	return false;
}

bool Index_Manager::Search(string File, float k, int& block_num) {
	BPT<float>* bpt = BPT_Float[File];
	if (bpt->Search_Key(k, block_num)) return true;
	return false;
}

bool Index_Manager::Search(string File, string k, int& block_num) {
	BPT<string>* bpt = BPT_String[File];
	if (bpt->Search_Key(k, block_num)) return true;
	return false;
}

bool Index_Manager::Search(string File, int min, int max, set<int>& block_num) {
	BPT<int>* bpt = BPT_Int[File];
	if (bpt->Search_Key(min, max, block_num)) return true;
	return false;
}

bool Index_Manager::Search(string File, float min, float max, set<int>& block_num) {
	BPT<float>* bpt = BPT_Float[File];
	if (bpt->Search_Key(min, max, block_num)) return true;
	return false;
}

//插入操作
void Index_Manager::Insert(string File, int k, int block_num) {
	Index &index = catalog_manager.getIndex(File);
	BPT<int>* bpt = BPT_Int.find(File)->second;
	bpt->Insert_Key(k, block_num);
	index.blockNum = bpt->Leaf_num;//更新block_num
}

void Index_Manager::Insert(string File, float k, int block_num) {
	Index &index = catalog_manager.getIndex(File);
	BPT<float>* bpt = BPT_Float.find(File)->second;
	bpt->Insert_Key(k, block_num);
	index.blockNum = bpt->Leaf_num;//更新block_num
}

void Index_Manager::Insert(string File, string k, int block_num) {
	Index &index = catalog_manager.getIndex(File);
	BPT<string>* bpt = BPT_String.find(File)->second;
	bpt->Insert_Key(k, block_num);
	index.blockNum = bpt->Leaf_num;//更新block_num
}

//删除操作
void Index_Manager::Delete(string File, int k) {
	Index &index = catalog_manager.getIndex(File);
	BPT<int>* bpt = BPT_Int.find(File)->second;
	bpt->Delete_Key(k);
	if (index.blockNum > bpt->Leaf_num)
		buffer_manager.writeFile("", File, 1, index.blockNum - 1);
	index.blockNum = bpt->Leaf_num;//更新block_num
}

void Index_Manager::Delete(string File, float k) {
	Index &index = catalog_manager.getIndex(File);
	BPT<float>* bpt = BPT_Float.find(File)->second;
	bpt->Delete_Key(k);
	if (index.blockNum > bpt->Leaf_num)
		buffer_manager.writeFile("", File, 1, index.blockNum - 1);
	index.blockNum = bpt->Leaf_num;//更新block_num
}

void Index_Manager::Delete(string File, string k) {
	Index &index = catalog_manager.getIndex(File);
	BPT<string>* bpt = BPT_String.find(File)->second;
	bpt->Delete_Key(k);
	if (index.blockNum > bpt->Leaf_num)
		buffer_manager.writeFile("", File, 1, index.blockNum - 1);
	index.blockNum = bpt->Leaf_num;//更新block_num
}

void Index_Manager::Debug_Print(string File, Type type) {
	if (type == INT)
		BPT_Int.find(File)->second->Print();
	else if (type == FLOAT)
		BPT_Float.find(File)->second->Print();
	else
		BPT_String.find(File)->second->Print();
}
