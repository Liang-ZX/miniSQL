#include "indexmanager.h"


//构造函数和析构函数
Index_Manager::Index_Manager() {

}
Index_Manager::~Index_Manager() {
	map<string, BPT<int>*>::iterator i_int;
	map<string, BPT<float>*>::iterator i_float;
	map<string, BPT<string>*>::iterator i_string;
	for (i_int = BPT_Int.begin(); i_int != BPT_Int.end(); i_int++) {
		delete i_int->second;
		BPT_Int.erase(i_int);
	}
	for (i_float = BPT_Float.begin(); i_float != BPT_Float.end(); i_float++) {
		delete i_float->second;
		BPT_Float.erase(i_float);
	}
	for (i_string = BPT_String.begin(); i_string != BPT_String.end(); i_string++) {
		delete i_string->second;
		BPT_String.erase(i_string);
	}
}
//创建索引，Type: 1:int, 3:float, 3:char
void Index_Manager::Create_Index(string File, Type type) {
	int Rank = 5;//根据block的大小计算阶的大小,暂时有点问题，先设为5
	if (type == INT) {
		//Rank = (4096 - 76 - 4) / (sizeof(int) * 2 + 4);
		BPT<int>* bpt = new BPT<int>(File, Rank);
		BPT_Int.insert({ File, bpt });
	}
	else if (type == FLOAT) {
		//Rank = (4096 - 76 - 4) / (sizeof(int) + sizeof(float) + 4);
		BPT<float>* bpt = new BPT<float>(File, Rank);
		BPT_Float.insert({ File, bpt });
	}
	else {
		//Rank = (4096 - 76 - 4) / (sizeof(int) + sizeof(char) + 4);
		BPT<string>* bpt = new BPT<string>(File, Rank);
		BPT_String.insert({ File,bpt });
	}
}

//删除索引
void Index_Manager::Drop_Index(string File, Type type) {
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
//查询操作
//等值查询
bool Index_Manager::Search(string File, int k, int& block_num) {
	if (BPT_Int.find(File)->second->Search_Key(k, block_num)) {
		return true;
	}
	return false;
}
bool Index_Manager::Search(string File, float k, int& block_num) {
	if (BPT_Float.find(File)->second->Search_Key(k, block_num)) {
		return true;
	}
	return false;
}
bool Index_Manager::Search(string File, string k, int& block_num) {
	if (BPT_String.find(File)->second->Search_Key(k, block_num)) {
		return true;
	}
	return false;
}
//区间查询
bool Index_Manager::Search(string File, int min, int max, vector<int>& block_num) {
	if (BPT_Int.find(File)->second->Search_Key(min, max, block_num)) {
		return true;
	}
	return false;
}
bool Index_Manager::Search(string File, float min, float max, vector<int>& block_num) {
	if (BPT_Float.find(File)->second->Search_Key(min, max, block_num)) {
		return true;
	}
	return false;
}

//插入
void Index_Manager::Insert(string File, int k, int block_num) {
	BPT_Int.find(File)->second->Insert_Key(k, block_num);
}
void Index_Manager::Insert(string File, float k, int block_num) {
	BPT_Float.find(File)->second->Insert_Key(k, block_num);
}
void Index_Manager::Insert(string File, string k, int block_num) {
	BPT_String.find(File)->second->Insert_Key(k, block_num);
}
//删除
void Index_Manager::Delete(string File, int k) {
	BPT_Int.find(File)->second->Delete_Key(k);
}
void Index_Manager::Delete(string File, float k) {
	BPT_Float.find(File)->second->Delete_Key(k);
}
void Index_Manager::Delete(string File, string k) {
	BPT_String.find(File)->second->Delete_Key(k);
}

void Index_Manager::Debug_Print(string File, Type type){
	if (type == INT) {
		BPT_Int.find(File)->second->Print();
	}
	else if (type == FLOAT) {
		BPT_Float.find(File)->second->Print();
	}
	else {
		BPT_String.find(File)->second->Print();
	}
}
