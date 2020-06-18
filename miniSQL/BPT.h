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
	friend class BPT;							//������Ԫ��
	Node(int rank, bool isleaf);				//���캯������+�Ƿ�ΪҶ�ڵ�
	~Node();									//��������
	bool isroot();								//�����Ƿ�Ϊ���ڵ�
	Node<T>* split(T& up_key);					//�ڵ���ѣ�up_key������Ҫ���Ƶ����ڵ��key
	unsigned int insert_key(T k);				//���ڽڵ����
	unsigned int insert_key(T k, int block_num);//��Ҷ�ڵ����
	bool delete_key(unsigned int index);		//ɾ��
	bool search_key(T k, unsigned int &index);	//�����Ƿ�������k��index�����ڵ���k����СԪ�������±�
	void print();								//debug print
public:
	unsigned int rank;		 //��
	bool isleaf;			 //�Ƿ�ΪҶ�ڵ�
	unsigned int key_num;	 //key������
	unsigned int min_key;	 //�ڵ�������key��,leaf��branch������ͬ
	vector<T> key;			 //���key������
	vector<Node<T>*> pointer;//����ָ��
	vector<int> value;		 //block_num
	Node<T>* father;		 //���׽ڵ�
	Node<T>* last_leaf;		 //��һ��Ҷ�ڵ�
	Node<T>* next_leaf;		 //��һ��Ҷ�ڵ�
};


/*----------------BPT---------------*/
template<typename T>
class BPT {
public:
	BPT(int Rank);
	~BPT();
	void Delete_Tree(Node<T>* node);  //�������������ã�ɾ��B+��
	Node<T>* Search_Node(T k);		  //����k���ڻ�Ӧ�����뵽��Ҷ�ڵ㣬���ڲ������
	bool Insert_Adjust(Node<T>* node);//����֮��B+���ĵ���
	bool Delete_Adjust(Node<T>* node);//ɾ��֮��B+���ĵ���
	void Print_Node(Node<T>* node);   //����Print��debug
	void Print();					  //debug print
public:
	bool Insert_Key(T k, int block_num);				  //����
	bool Delete_Key(T k);								  //ɾ��
	void Delete_All();									  //ɾ�����е�����key
	bool Search_Key(T k, int& block_num);				  //��ֵ��ѯ
	bool Search_Key(T min, T max, vector<int>& block_num);//��Χ��ѯ
public:
	int Rank;		      //��
	int Node_num;	      //�ڵ���
	int Leaf_num;		  //Ҷ�ڵ���
	Node<T>* Root;		  //���ڵ�
	Node<T>* MostLeftLeaf;//���Ҷ�ڵ�
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
	//����vector�� key, value����Ϊrank, pointer������Ϊrank + 1
	for (int i = 0; i < rank; i++) {
		key.push_back(T());		 //key: rank-1
		pointer.push_back(NULL); //pointer: rank
		if (isleaf) {
			value.push_back(int());//value: rank-1
		}
	}
	pointer.push_back(NULL);
	if (isleaf) {
		//�������ż��
		if (rank % 2 == 0) {
			min_key = rank / 2;
		}
		//�����������
		else {
			min_key = (rank - 1) / 2;
		}
	}
	else {
		//�������ż��
		if (rank % 2 == 0) {
			min_key = rank / 2 - 1;
		}
		//�����������
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
		1��Ҷ�ڵ㣺key[left_key_num]
		2���м�ڵ㣺key[left_pointer_num - 1]
	*/
	Node* nnode = new Node(rank, isleaf);
	//Ҷ�ڵ�
	if (isleaf) {
		unsigned int left_key_num; //���Ѻ���ڵ�key������
		unsigned int right_key_num;//���Ѻ��ҽڵ�key������
		//����rank������ڵ�key������
		if (rank % 2 == 1) {
			left_key_num = (rank + 1) / 2;
		}
		else {
			left_key_num = rank / 2;
		}
		right_key_num = rank - left_key_num;
		up_key = key[left_key_num];	//�½ڵ�ĵ�һ��Ԫ����Ҫ����
		//key��valueת��
		for (unsigned int i = left_key_num; i < rank; i++) {
			nnode->key[i - left_key_num] = key[i];
			nnode->value[i - left_key_num] = value[i];
			key[i] = T();
			value[i] = int();
		}
		//����ָ��
		nnode->last_leaf = this;
		nnode->next_leaf = this->next_leaf;
		if (this->next_leaf != NULL) {
			this->next_leaf->last_leaf = nnode;
		}
		this->next_leaf = nnode;
		nnode->father = this->father;
		//����key�ĸ���
		nnode->key_num = right_key_num;
		key_num = left_key_num;
	}
	//�м�ڵ�
	else {
		unsigned int right_pointer_num;//���Ѻ��ҽڵ�key������
		unsigned int left_pointer_num; //���Ѻ���ڵ�key������
		if (rank % 2 == 0) {
			right_pointer_num = rank / 2 + 1;
		}
		else {
			right_pointer_num = (rank + 1) / 2;
		}
		left_pointer_num = rank + 1 - right_pointer_num;
		up_key = key[left_pointer_num - 1];//����������ָ����м��key��Ҫ����
		key[left_pointer_num - 1] = T();   //����������֮���key��0
		//ָ��ת��
		for (unsigned int i = left_pointer_num; i <= rank; i++) {
			nnode->pointer[i - left_pointer_num] = pointer[i];
			nnode->pointer[i - left_pointer_num]->father = nnode;
			pointer[i] = NULL;
		}
		//keyת��
		for (unsigned int i = left_pointer_num; i < rank; i++) {
			nnode->key[i - left_pointer_num] = key[i];
			key[i] = T();
		}
		nnode->father = this->father;   //���Ӹ��ڵ�
		key[left_pointer_num - 1] = T();//����Ҫ����ȥ��key��λ���ÿ�
		//�޸�ǰ�������ڵ��key_num
		nnode->key_num = right_pointer_num - 1;
		key_num = left_pointer_num - 1;
	}
	return nnode;
}
//���Ҷ�ڵ��в���Ԫ��
template<typename T>
unsigned int Node<T>::insert_key(T k) {
	if (key_num == 0) {
		key[0] = k;
		key_num++;
		return 0;
	}
	else {
		unsigned int index = 0;
		bool exist = search_key(k, index);//���Ҵ��ڻ����k����Сֵ��index
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
			return index;//���ز����λ��
		}
	}
}
//��Ҷ�ڵ��в���Ԫ��
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

//ɾ��֮������ڵ���Ԫ�ص�λ��
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
		//branch node����ɾ��key��ߵ�ָ���Ǳ����ģ��ұߵ�ָ�뱻ɾ��
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
		//�ȸýڵ���С��ֵ��ҪС
		if (k < key[0]) {
			index = 0;
			return false;
		}
		//�ȸýڵ�����ֵ��Ҫ��
		else if (k > key[key_num - 1]) {
			index = key_num;
			return false;
		}
		else {
			//k���ڸýڵ������Сkey֮�䣬���ַ��ҵ����ڵ���k����Сindex
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
	//����
	if (exist) {
		return false;
	}
	//�����ڣ���ִ�в������
	else {
		node->insert_key(k, block_num);
		if (node->key_num == Rank) {
			Insert_Adjust(node);//����֮��Ľڵ����
		}
		return true;
	}
}

template<typename T>
bool BPT<T>::Insert_Adjust(Node<T>* node) {
	T up_key;
	Node<T>* nnode = node->split(up_key);
	Node_num++;
	//���Ҫ���ѵĽڵ��Ǹ��ڵ�
	if (node->isroot()) {
		Node<T>* Root = new Node<T>(Rank, false);//�µĸ��ڵ㣬�Ҹýڵ�һ������Ҷ�ڵ�
		this->Root = Root;
		Node_num++;//�ڵ���������1
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
	//���Ǹ��ڵ�
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
		Node<T>* leaf = Search_Node(k);//�ҵ�Ҷ�ڵ�
		unsigned int index = 0;//T��Ҷ�ڵ��е�index
		bool exist;//����T�Ƿ������Ҷ�ڵ�
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

//ɾ�����е�����Key�����һ�ÿ���
template<typename T>
inline void BPT<T>::Delete_All() {
	Delete_Tree(Root);
	MostLeftLeaf = NULL;
	Root = NULL;
}


template<typename T>
bool BPT<T>::Delete_Adjust(Node<T>* node) {
	//���ڵ�
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
			//��ɿ���
			else {
				Root = NULL;
				MostLeftLeaf = NULL;
				delete node;
				Node_num--;
				return true;
			}
		}
	}
	//Ҷ�ڵ�
	else if (node->isleaf) {
		//����Ҫ����
		if (node->key_num >= node->min_key) {
			return true;
		}
		//����
		else {
			//leaf���ֵ�
			if (node->last_leaf != NULL && node->father == node->last_leaf->father) {
				Node<T>* last_leaf = node->last_leaf;
				Node<T>* father = node->father;
				//��key
				if (last_leaf->key_num > last_leaf->min_key) {
					//�ýڵ������key��value�����ƶ�һλ
					for (unsigned int i = node->key_num; i > 0; i--) {
						node->key[i] = node->key[i - 1];
						node->value[i] = node->value[i - 1];
					}
					//��Ҷ�ڵ��һ��key��value�ŵ��ýڵ��0��λ
					node->key[0] = last_leaf->key[last_leaf->key_num - 1];
					node->value[0] = last_leaf->value[last_leaf->key_num - 1];
					node->key_num++;
					last_leaf->delete_key(last_leaf->key_num - 1);
					//�滻���ڵ�������ָ��֮���key
					unsigned int index = 0;
					bool exist;
					exist = father->search_key(last_leaf->key[0], index);
					if (exist) {
						index++;
					}
					father->key[index] = node->key[0];
					return true;
				}
				//�����ڵ�ϲ�����Ҷ�ڵ�
				else {
					for (unsigned int i = 0; i < node->key_num; i++) {
						last_leaf->key[last_leaf->key_num] = node->key[i];
						last_leaf->value[last_leaf->key_num] = node->value[i];
						last_leaf->key_num++;
					}
					//ɾ�����ڵ�������ָ��֮���key
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
			//leaf���ֵ�
			else {
				Node<T>* next_leaf = node->next_leaf;
				Node<T>* father = node->father;
				//��key
				if (next_leaf->key_num > next_leaf->min_key) {
					node->key[node->key_num] = next_leaf->key[0];
					node->value[node->key_num] = next_leaf->value[0];
					node->key_num++;

					//�滻���ڵ�������ָ��֮���key
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
				//�ϲ��ڵ㵽��ڵ�
				else {
					//key��valueת��
					for (unsigned int i = 0; i < next_leaf->key_num; i++) {
						node->key[node->key_num] = next_leaf->key[i];
						node->value[node->key_num] = next_leaf->value[i];
						node->key_num++;
					}
					//�������ڵ��������ڵ�֮���key��index
					unsigned int index = 0;
					bool exist;
					exist = father->search_key(next_leaf->key[0], index);
					if (!exist) {
						index--;
					}
					//�ڸ��ڵ���ɾ���м��key
					father->delete_key(index);
					//����ָ��
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
	//֦�ɽڵ�
	else {
		//����Ҫ����
		if (node->key_num >= node->min_key) {
			return true;
		}
		//����
		else {
			//���ֵ�
			if (node->father->pointer[0] != node) {
				Node<T>* father = node->father;
				Node<T>* left_node;
				unsigned int index = 0;//
				bool exist;
				exist = father->search_key(node->key[0], index);
				if (!exist) {
					index--;//��������ھ�--
				}
				left_node = father->pointer[index];
				//��key
				if (left_node->key_num > left_node->min_key) {
					for (unsigned int i = node->key_num; i > 0; i--) {
						node->key[i] = node->key[i - 1];
					}
					for (unsigned int i = node->key_num + 1; i > 0; i--) {
						node->pointer[i] = node->pointer[i - 1];
					}
					node->key_num++;
					node->key[0] = father->key[index];//���ڵ����Ƶ�node
					node->pointer[0] = left_node->pointer[left_node->key_num];//��ָ��ת�Ƶ�node
					left_node->pointer[left_node->key_num]->father = node;//�ӽڵ�ĸ��׽ڵ�ת�ƣ�
					father->key[index] = left_node->key[left_node->key_num - 1];//��ڵ����Ƶ����ڵ�
					//��ڵ����key��ָ������
					left_node->key[left_node->key_num - 1] = T();
					left_node->pointer[left_node->key_num] = NULL;
					left_node->key_num--;
					return true;
				}
				//�ϲ������
				else {
					left_node->key[left_node->key_num] = father->key[index];//���׽ڵ����Ƶ���ڵ�
					left_node->key_num++;//key_num++
					//ת��key
					for (unsigned int i = 0; i < node->key_num; i++) {
						left_node->key[left_node->key_num + i] = node->key[i];
					}
					//ת��ָ��
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
			//���ֵ�
			else {
				Node<T>* father = node->father;
				Node<T>* right_node;
				unsigned int index = 0;//���ڵ�������ָ��֮���key��index
				bool exist;
				exist = father->search_key(node->key[0], index);
				if (exist) {
					index++;//������ھ�++
				}
				right_node = father->pointer[index + 1];
				//��key
				if (right_node->key_num > right_node->min_key) {
					node->key[node->key_num] = father->key[index];//���ڵ�����
					father->key[index] = right_node->key[0];//�ҽڵ�����
					node->pointer[node->key_num + 1] = right_node->pointer[0];//��ָ������
					right_node->pointer[0]->father = node;//��ָ��ڵ㸸�׽ڵ�ת�Ƶ�node

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
				}//�ϲ�����ڵ�
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
//��ֵ��ѯ
template<typename T>
bool BPT<T>::Search_Key(T k, int& block_num) {
	unsigned int index = 0;			//����k�ڽڵ��е��±�
	Node<T>* node = Search_Node(k); //Ҷ�ڵ�
	bool exist = node->search_key(k, index);
	if (exist) {
		block_num = node->value[index];
	}
	return exist;
}

//��Χ��ѯ, min < k < max
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
	//min,max��ͬһ�ڵ���
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
	//min��max����ͬһ�ڵ���
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
		Print_Node(Root);//��ӡ���нڵ�
		Node<T>* node = MostLeftLeaf;
		for (int i = 0; ; i++) {//��ӡҶ�ڵ�
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
