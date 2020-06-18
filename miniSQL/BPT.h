#ifndef _BPT_H
#define _BPT_H
#include <iostream>
#include <string>
#include <vector>

using namespace std;

/*--------------Node--------------*/
template<typename T>
class Node {
public:
	template<typename T>
	friend class BPT;							//声明友元类
	Node(int rank, bool isleaf);				//构造函数：阶+是否为叶节点
	~Node();									//析构函数
	bool isroot();								//返回是否为根节点
	Node<T>* split(T& up_key);					//节点分裂，up_key返回需要上移到父节点的key
	unsigned int insert_key(T k);				//向内节点插入
	unsigned int insert_key(T k, int block_num);//向叶节点插入
	bool delete_key(unsigned int index);		//删除
	bool search_key(T k, unsigned int &index);	//返回是否搜索到k，index：大于等于k的最小元素所在下标
	void print();								//debug print
public:
	unsigned int rank;		 //阶
	bool isleaf;			 //是否为叶节点
	unsigned int key_num;	 //key的数量
	unsigned int min_key;	 //节点中最少key数,leaf与branch有所不同
	vector<T> key;			 //存放key的容器
	vector<Node<T>*> pointer;//孩子指针
	vector<int> value;		 //block_num
	Node<T>* father;		 //父亲节点
	Node<T>* last_leaf;		 //上一个叶节点
	Node<T>* next_leaf;		 //下一个叶节点
};


/*----------------BPT---------------*/
template<typename T>
class BPT {
public:
	BPT(int Rank);
	~BPT();
	void Delete_Tree(Node<T>* node);  //被析构函数调用，删除B+树
	Node<T>* Search_Node(T k);		  //返回k所在或应当插入到的叶节点，便于插入操作
	bool Insert_Adjust(Node<T>* node);//插入之后B+树的调整
	bool Delete_Adjust(Node<T>* node);//删除之后B+树的调整
	void Print_Node(Node<T>* node);   //辅助Print，debug
	void Print();					  //debug print
public:
	bool Insert_Key(T k, int block_num);				  //插入
	bool Delete_Key(T k);								  //删除
	void Delete_All();									  //删除树中的所有key
	bool Search_Key(T k, int& block_num);				  //等值查询
	bool Search_Key(T min, T max, vector<int>& block_num);//范围查询
public:
	int Rank;		      //阶
	int Node_num;	      //节点数
	int Leaf_num;		  //叶节点数
	Node<T>* Root;		  //根节点
	Node<T>* MostLeftLeaf;//左端叶节点
};

/*------------------Node--------------------*/
template <typename T>
Node<T>::Node(int rank, bool isleaf) :
	rank(rank),
	isleaf(isleaf),
	key_num(0),
	father(NULL),
	last_leaf(NULL),
	next_leaf(NULL)
{
	//三个vector： key, value容量为rank, pointer的容量为rank + 1
	for (int i = 0; i < rank; i++) {
		key.push_back(T());		 //key: rank-1
		pointer.push_back(NULL); //pointer: rank
		if (isleaf) {
			value.push_back(int());//value: rank-1
		}
	}
	pointer.push_back(NULL);
	if (isleaf) {
		//如果阶是偶数
		if (rank % 2 == 0) {
			min_key = rank / 2;
		}
		//如果阶是奇数
		else {
			min_key = (rank - 1) / 2;
		}
	}
	else {
		//如果阶是偶数
		if (rank % 2 == 0) {
			min_key = rank / 2 - 1;
		}
		//如果阶是奇数
		else {
			min_key = (rank + 1) / 2 - 1;
		}
	}
}

template <typename T>
Node<T>::~Node() {
	//debug:
	//cout << "In ~Node()" << endl;
}

template <typename T>
bool Node<T>::isroot() {
	return (this->father == NULL);
}

template<typename T>
Node<T>* Node<T>::split(T& up_key) {
	/*
		up_key:
		1、叶节点：key[left_key_num]
		2、中间节点：key[left_pointer_num - 1]
	*/
	Node* nnode = new Node(rank, isleaf);
	//叶节点
	if (isleaf) {
		unsigned int left_key_num; //分裂后左节点key的数量
		unsigned int right_key_num;//分裂后右节点key的数量
		//根据rank计算左节点key的数量
		if (rank % 2 == 1) {
			left_key_num = (rank + 1) / 2;
		}
		else {
			left_key_num = rank / 2;
		}
		right_key_num = rank - left_key_num;
		up_key = key[left_key_num];	//新节点的第一个元素需要上移
		//key和value转移
		for (unsigned int i = left_key_num; i < rank; i++) {
			nnode->key[i - left_key_num] = key[i];
			nnode->value[i - left_key_num] = value[i];
			key[i] = T();
			value[i] = int();
		}
		//调整指针
		nnode->last_leaf = this;
		nnode->next_leaf = this->next_leaf;
		if (this->next_leaf != NULL) {
			this->next_leaf->last_leaf = nnode;
		}
		this->next_leaf = nnode;
		nnode->father = this->father;
		//调整key的个数
		nnode->key_num = right_key_num;
		key_num = left_key_num;
	}
	//中间节点
	else {
		unsigned int right_pointer_num;//分裂后右节点key的数量
		unsigned int left_pointer_num; //分裂后左节点key的数量
		if (rank % 2 == 0) {
			right_pointer_num = rank / 2 + 1;
		}
		else {
			right_pointer_num = (rank + 1) / 2;
		}
		left_pointer_num = rank + 1 - right_pointer_num;
		up_key = key[left_pointer_num - 1];//夹在两部分指针的中间的key需要上移
		key[left_pointer_num - 1] = T();   //夹在两部分之间的key归0
		//指针转移
		for (unsigned int i = left_pointer_num; i <= rank; i++) {
			nnode->pointer[i - left_pointer_num] = pointer[i];
			nnode->pointer[i - left_pointer_num]->father = nnode;
			pointer[i] = NULL;
		}
		//key转移
		for (unsigned int i = left_pointer_num; i < rank; i++) {
			nnode->key[i - left_pointer_num] = key[i];
			key[i] = T();
		}
		nnode->father = this->father;   //连接父节点
		key[left_pointer_num - 1] = T();//把需要提上去的key的位置置空
		//修改前后两个节点的key_num
		nnode->key_num = right_pointer_num - 1;
		key_num = left_pointer_num - 1;
	}
	return nnode;
}
//向飞叶节点中插入元素
template<typename T>
unsigned int Node<T>::insert_key(T k) {
	if (key_num == 0) {
		key[0] = k;
		key_num++;
		return 0;
	}
	else {
		unsigned int index = 0;
		bool exist = search_key(k, index);//查找大于或等于k的最小值的index
		if (exist) {
			cout << "In insert_key(T k), k is already exist in position: " << index << endl;
			return index;
		}
		else {
			for (unsigned int i = key_num; i > index; i--) {
				key[i] = key[i - 1];
			}
			key[index] = k;
			for (unsigned int i = key_num + 1; i > index + 1; i--) {
				pointer[i] = pointer[i - 1];
			}
			pointer[index + 1] = NULL;
			key_num++;
			return index;//返回插入的位置
		}
	}
}
//向叶节点中插入元素
template<typename T>
unsigned int Node<T>::insert_key(T k, int block_num) {
	if (!isleaf) {
		return -1;
	}
	if (key_num == 0) {
		key[0] = k;
		value[0] = block_num;
		key_num++;
		return 0;
	}
	else {
		unsigned int index = 0;
		search_key(k, index);
		for (unsigned int i = key_num; i > index; i--) {
			key[i] = key[i - 1];
			value[i] = value[i - 1];
		}
		key[index] = k;
		value[index] = block_num;
		key_num++;
		return index;
	}
}

//删除之后调整节点内元素的位置
template<typename T>
bool Node<T>::delete_key(unsigned int index) {
	if (index > key_num - 1 || index < 0) {
		return false;
	}
	else {
		//leaf node
		if (isleaf) {
			for (unsigned int i = index; i < key_num - 1; i++) {
				key[i] = key[i + 1];
				value[i] = value[i + 1];
			}
			key[key_num - 1] = T();
			value[key_num - 1] = int();
			key_num--;
		}
		//branch node，被删除key左边的指针是保留的，右边的指针被删除
		else {
			for (unsigned int i = index; i < key_num - 1; i++) {
				key[i] = key[i + 1];
			}
			for (unsigned int i = index + 1; i < key_num; i++) {
				pointer[i] = pointer[i + 1];
			}
			key[key_num - 1] = T();
			pointer[key_num] = NULL;
			key_num--;
		}
		return true;
	}
}

template<typename T>
bool Node<T>::search_key(T k, unsigned int &index) {
	if (key_num == 0) {
		index = 0;
		return false;
	}
	else {
		//比该节点最小的值还要小
		if (k < key[0]) {
			index = 0;
			return false;
		}
		//比该节点最大的值还要大
		else if (k > key[key_num - 1]) {
			index = key_num;
			return false;
		}
		else {
			//k介于该节点最大最小key之间，二分法找到大于等于k的最小index
			unsigned int left = 0;
			unsigned int right = key_num - 1;
			unsigned int mid;
			while (left < right) {
				mid = (left + right) / 2;
				if (key[mid] >= k) {
					right = mid;
				}
				else {
					left = mid + 1;
				}
			}
			index = right;
			return (key[index] == k);
		}
	}
}

template<typename T>
void Node<T>::print() {
	for (unsigned int i = 0; i < key_num; i++) {
		cout << key[i];
		if (this->isleaf) {
			cout << "(" << value[i] << ")";
		}
		cout << "-";
	}
	cout << endl;
}

/*-----------------------BPT-----------------------*/
template <typename T>
BPT<T>::BPT(int Rank) :
	Rank(Rank),
	Node_num(1),
	Leaf_num(1)
{
	Root = new Node<T>(Rank, true);
	MostLeftLeaf = Root;
}

template <typename T>
BPT<T>::~BPT() {
	Delete_Tree(Root);
	MostLeftLeaf = NULL;
	Root = NULL;
}

template<typename T>
void BPT<T>::Delete_Tree(Node<T>* node) {
	if (node == NULL) {
		return;
	}
	else if (!node->isleaf) {
		for (unsigned int i = 0; i <= node->key_num; i++) {
			Delete_Tree(node->pointer[i]);
			node->pointer[i] = NULL;
		}
	}
	delete node;
}

/*-----Insertion: Search_Node + Insert_Key + Insert_Adjust-----*/
template <typename T>
Node<T>* BPT<T>::Search_Node(T k)
{
	Node<T>* node = Root;
	while (!node->isleaf) {
		for (unsigned int i = 0; i <= node->key_num; i++) {
			if (i < node->key_num && k == node->key[i]) {
				node = node->pointer[i + 1];
				break;
			}
			if (i == node->key_num || k < node->key[i]) {
				node = node->pointer[i];
				break;
			}
		}
	}
	return node;
}

template <typename T>
bool BPT<T>::Insert_Key(T k, int block_num) {
	if (!Root) {
		Root = new Node<T>(Rank, true);
		Node_num = 1;
		Leaf_num = 1;
		MostLeftLeaf = Root;
		Root->insert_key(k, block_num);
		return true;
	}
	Node<T>* node = Search_Node(k);
	unsigned int index = 0;
	bool exist = node->search_key(k, index);
	//存在
	if (exist) {
		return false;
	}
	//不存在，则执行插入操作
	else {
		node->insert_key(k, block_num);
		if (node->key_num == Rank) {
			Insert_Adjust(node);//插入之后的节点调整
		}
		return true;
	}
}

template<typename T>
bool BPT<T>::Insert_Adjust(Node<T>* node) {
	T up_key;
	Node<T>* nnode = node->split(up_key);
	Node_num++;
	//如果要分裂的节点是根节点
	if (node->isroot()) {
		Node<T>* Root = new Node<T>(Rank, false);//新的根节点，且该节点一定不是叶节点
		this->Root = Root;
		Node_num++;//节点数再增加1
		if (node->isleaf) {
			Leaf_num++;
		}
		node->father = Root;
		nnode->father = Root;
		Root->insert_key(up_key);
		Root->pointer[0] = node;
		Root->pointer[1] = nnode;
		return true;
	}
	//不是根节点
	else {
		if (node->isleaf) {
			Leaf_num++;
		}
		unsigned int index = node->father->insert_key(up_key);
		node->father->pointer[index + 1] = nnode;
		nnode->father = node->father;
		if (node->father->key_num == Rank) {
			Insert_Adjust(node->father);
		}
		return true;
	}
}

/*---------Deletion: Search_Node + Delete_Key + Delete_Adjust-------------*/
template<typename T>
bool BPT<T>::Delete_Key(T k) {
	if (!Root) {
		cout << "This is an empty BPtree!" << endl;
		return false;
	}
	else {
		Node<T>* leaf = Search_Node(k);//找到叶节点
		unsigned int index = 0;//T在叶节点中的index
		bool exist;//返回T是否存在于叶节点
		exist = leaf->search_key(k, index);
		if (exist) {
			leaf->delete_key(index);
			return Delete_Adjust(leaf);
		}
		else {
			cout << "Key " << k << " dosen't exist!" << endl;
			return false;
		}
	}
}

//删除树中的所有Key，变成一棵空树
template<typename T>
inline void BPT<T>::Delete_All() {
	Delete_Tree(Root);
	MostLeftLeaf = NULL;
	Root = NULL;
}


template<typename T>
bool BPT<T>::Delete_Adjust(Node<T>* node) {
	//根节点
	if (node->isroot()) {
		if (node->key_num > 0) {
			return true;
		}
		else {
			if (node->pointer[0]) {
				Root = node->pointer[0];
				Root->father = NULL;
				delete node;
				Node_num--;
				return true;
			}
			//变成空树
			else {
				Root = NULL;
				MostLeftLeaf = NULL;
				delete node;
				Node_num--;
				return true;
			}
		}
	}
	//叶节点
	else if (node->isleaf) {
		//不需要调整
		if (node->key_num >= node->min_key) {
			return true;
		}
		//调整
		else {
			//leaf左兄弟
			if (node->last_leaf != NULL && node->father == node->last_leaf->father) {
				Node<T>* last_leaf = node->last_leaf;
				Node<T>* father = node->father;
				//借key
				if (last_leaf->key_num > last_leaf->min_key) {
					//该节点的所有key和value往后移动一位
					for (unsigned int i = node->key_num; i > 0; i--) {
						node->key[i] = node->key[i - 1];
						node->value[i] = node->value[i - 1];
					}
					//左叶节点给一个key和value放到该节点的0号位
					node->key[0] = last_leaf->key[last_leaf->key_num - 1];
					node->value[0] = last_leaf->value[last_leaf->key_num - 1];
					node->key_num++;
					last_leaf->delete_key(last_leaf->key_num - 1);
					//替换父节点中两个指针之间的key
					unsigned int index = 0;
					bool exist;
					exist = father->search_key(last_leaf->key[0], index);
					if (exist) {
						index++;
					}
					father->key[index] = node->key[0];
					return true;
				}
				//两个节点合并到左叶节点
				else {
					for (unsigned int i = 0; i < node->key_num; i++) {
						last_leaf->key[last_leaf->key_num] = node->key[i];
						last_leaf->value[last_leaf->key_num] = node->value[i];
						last_leaf->key_num++;
					}
					//删除父节点中两个指针之间的key
					unsigned int index = 0;
					bool exist;
					exist = father->search_key(last_leaf->key[0], index);
					if (exist) {
						index++;
					}

					father->delete_key(index);

					last_leaf->next_leaf = node->next_leaf;
					if (last_leaf->next_leaf != NULL) {
						last_leaf->next_leaf->last_leaf = last_leaf;
					}
					delete node;
					Node_num--;
					Leaf_num--;
					return Delete_Adjust(father);
				}
			}
			//leaf右兄弟
			else {
				Node<T>* next_leaf = node->next_leaf;
				Node<T>* father = node->father;
				//借key
				if (next_leaf->key_num > next_leaf->min_key) {
					node->key[node->key_num] = next_leaf->key[0];
					node->value[node->key_num] = next_leaf->value[0];
					node->key_num++;

					//替换父节点中两个指针之间的key
					unsigned int index = 0;
					bool exist;
					exist = father->search_key(next_leaf->key[0], index);
					if (!exist) {
						index--;
					}
					next_leaf->delete_key(0);//next_leaf->key_num--
					father->key[index] = next_leaf->key[0];
					return true;
				}
				//合并节点到左节点
				else {
					//key和value转移
					for (unsigned int i = 0; i < next_leaf->key_num; i++) {
						node->key[node->key_num] = next_leaf->key[i];
						node->value[node->key_num] = next_leaf->value[i];
						node->key_num++;
					}
					//搜索父节点中两个节点之间的key的index
					unsigned int index = 0;
					bool exist;
					exist = father->search_key(next_leaf->key[0], index);
					if (!exist) {
						index--;
					}
					//在父节点中删除中间的key
					father->delete_key(index);
					//调整指针
					node->next_leaf = next_leaf->next_leaf;
					if (node->next_leaf != NULL) {
						node->next_leaf->last_leaf = node;
					}
					delete next_leaf;
					Node_num--;
					Leaf_num--;
					return Delete_Adjust(father);
				}
			}
		}
	}
	//枝干节点
	else {
		//不需要调整
		if (node->key_num >= node->min_key) {
			return true;
		}
		//调整
		else {
			//左兄弟
			if (node->father->pointer[0] != node) {
				Node<T>* father = node->father;
				Node<T>* left_node;
				unsigned int index = 0;//
				bool exist;
				exist = father->search_key(node->key[0], index);
				if (!exist) {
					index--;//如果不存在就--
				}
				left_node = father->pointer[index];
				//借key
				if (left_node->key_num > left_node->min_key) {
					for (unsigned int i = node->key_num; i > 0; i--) {
						node->key[i] = node->key[i - 1];
					}
					for (unsigned int i = node->key_num + 1; i > 0; i--) {
						node->pointer[i] = node->pointer[i - 1];
					}
					node->key_num++;
					node->key[0] = father->key[index];//父节点下移到node
					node->pointer[0] = left_node->pointer[left_node->key_num];//左指针转移到node
					left_node->pointer[left_node->key_num]->father = node;//子节点的父亲节点转移；
					father->key[index] = left_node->key[left_node->key_num - 1];//左节点上移到父节点
					//左节点更新key和指针数量
					left_node->key[left_node->key_num - 1] = T();
					left_node->pointer[left_node->key_num] = NULL;
					left_node->key_num--;
					return true;
				}
				//合并到左边
				else {
					left_node->key[left_node->key_num] = father->key[index];//父亲节点下移到左节点
					left_node->key_num++;//key_num++
					//转移key
					for (unsigned int i = 0; i < node->key_num; i++) {
						left_node->key[left_node->key_num + i] = node->key[i];
					}
					//转移指针
					for (unsigned int i = 0; i <= node->key_num; i++) {
						left_node->pointer[left_node->key_num + i] = node->pointer[i];
						node->pointer[i]->father = left_node;
					}
					left_node->key_num += node->key_num;
					delete node;
					Node_num--;
					father->delete_key(index);
					return Delete_Adjust(father);
				}
			}
			//右兄弟
			else {
				Node<T>* father = node->father;
				Node<T>* right_node;
				unsigned int index = 0;//父节点中两个指针之间的key的index
				bool exist;
				exist = father->search_key(node->key[0], index);
				if (exist) {
					index++;//如果存在就++
				}
				right_node = father->pointer[index + 1];
				//借key
				if (right_node->key_num > right_node->min_key) {
					node->key[node->key_num] = father->key[index];//父节点下移
					father->key[index] = right_node->key[0];//右节点上移
					node->pointer[node->key_num + 1] = right_node->pointer[0];//右指针左移
					right_node->pointer[0]->father = node;//右指针节点父亲节点转移到node

					for (unsigned int i = 0; i < right_node->key_num - 1; i++) {
						right_node->key[i] = right_node->key[i + 1];
					}
					for (unsigned int i = 0; i < right_node->key_num; i++) {
						right_node->pointer[i] = right_node->pointer[i + 1];
					}
					right_node->key[right_node->key_num - 1] = T();
					right_node->pointer[right_node->key_num] = NULL;
					right_node->key_num--;
					node->key_num++;
					return true;
				}//合并到左节点
				else {
					node->key[node->key_num] = father->key[index];
					node->key_num++;
					for (unsigned int i = 0; i < right_node->key_num; i++) {
						node->key[node->key_num + i] = right_node->key[i];
					}
					for (unsigned int i = 0; i < right_node->key_num + 1; i++) {
						node->pointer[node->key_num + i] = right_node->pointer[i];
						right_node->pointer[i]->father = node;
					}
					node->key_num += right_node->key_num;
					delete right_node;
					Node_num--;
					father->delete_key(index);
					return Delete_Adjust(father);
				}
			}
		}
	}
}

/*-----------------Search_Key-----------------------*/
//等值查询
template<typename T>
bool BPT<T>::Search_Key(T k, int& block_num) {
	unsigned int index = 0;			//返回k在节点中的下标
	Node<T>* node = Search_Node(k); //叶节点
	bool exist = node->search_key(k, index);
	if (exist) {
		block_num = node->value[index];
	}
	return exist;
}

//范围查询, min < k < max
template<typename T>
bool BPT<T>::Search_Key(T min, T max, vector<int>& block_num) {
	unsigned int index1 = 0;//min
	unsigned int index2 = 0;//max
	Node<T>* node1 = Search_Node(min);//min
	Node<T>* node2 = Search_Node(max);//max
	bool exist1 = node1->search_key(min, index1);//min
	bool exist2 = node2->search_key(max, index2);//max
	if (exist1) {
		index1++;
	}
	//min,max在同一节点内
	if (node1 == node2) {
		if (index1 == index2) {
			return false;
		}
		else {
			for (unsigned int i = index1; i < index2; i++) {
				block_num.push_back(node1->value[i]);
			}
		}
	}
	//min，max不在同一节点内
	else {
		while (node1 != node2) {
			for (unsigned int i = index1; i < node1->key_num; i++) {
				block_num.push_back(node1->value[i]);
			}
			index1 = 0;
			node1 = node1->next_leaf;
		}
		for (unsigned int i = 0; i < index2; i++) {
			block_num.push_back(node2->value[i]);
		}
	}
	return true;
}

/*-------------------debug print----------------------*/
template<typename T>
void BPT<T>::Print_Node(Node<T>* node) {
	node->print();
	if (!node->isleaf) {
		for (unsigned int i = 0; i <= node->key_num; i++) {
			Print_Node(node->pointer[i]);
		}
	}
}

template<typename T>
void BPT<T>::Print() {
	if (!Root) {
		cout << "This is an empty tree!" << endl;
		return;
	}
	else {
		Print_Node(Root);//打印所有节点
		Node<T>* node = MostLeftLeaf;
		for (int i = 0; ; i++) {//打印叶节点
			if (node != NULL) {
				cout << "leaf" << "[" << i << "]: ";
				node->print();
				node = node->next_leaf;
			}
			else {
				break;
			}
		}
		cout << "Node_num: " << Node_num << endl;
		cout << "Leaf_num: " << Leaf_num << endl;
		return;
	}
}

#endif
