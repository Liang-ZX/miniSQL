#ifndef _BUFFER_MANAGER
#define _BUFFER_MANAGER

#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <cstring>
using namespace std;

#define BLOCK_SIZE 4096
#define MAX_BLOCK_NUM 40

struct sqlFile;
struct sqlBlock
{
	int blockid;
	int timestamp = 0;
	char* blockdata = nullptr;
	bool pin = false;
	bool dirty = false;
	int offsetnum = -1; //offset in the block list of the file, from 0
	sqlBlock* nextblock = nullptr;
	sqlFile* sfile = nullptr;
	int block_size=0;
};

struct sqlFile
{
	int filetype; //0 data file, 1 index file, 2 catalog file
	int count_of_blockinbuffer = 0;
	string location = "Data_files\\";
	string filename;
	bool pin = false;
	sqlBlock* blockhead = nullptr;	//no pseudo-head
	sqlFile* nextfile = nullptr;
	bool is_writing = false;
};

class BufferManager 
{
private:
	string db_name = "";
	int totalblock;
	int totalfile;
	sqlBlock* BlockPool[MAX_BLOCK_NUM];
	sqlFile* filehead;	//pseudo-head
	sqlFile* getFileInfo(const string file_name, int file_type);
	void removeFileInfo(sqlFile* fileInfo); //auxiliary function, don't care
	sqlBlock* getBlockInBuffer(sqlFile* fileInfo, int block_num);
	sqlBlock* readBlocktoBuffer(const string db_name, sqlFile* fileInfo, int offset); //ensure it's not in buffer
	sqlBlock* readFiletoBuffer(const string db_name, const string file_name, int file_type, int block_num, bool setPin=false, bool is_writing=false);
	void writeBlocktoDisk(const string db_name, sqlBlock* block);
	void writeBlockAlltoDisk(const string db_name); // call when close DataBase
	void closeFile(const string db_name, sqlFile* fileInfo);
	void updatePool(int blockid); //update LRU timestamp
	sqlBlock* getUsableBlock(const string db_name, sqlFile* fileInfo); //LRU here
	void remove_from_block_list(sqlBlock * block, sqlFile* newfileInfo);

public:
	BufferManager(const string db_name) :totalblock(0), totalfile(0)
	{
		this->db_name = db_name;
		filehead = new sqlFile;
		filehead->nextfile = nullptr;
		memset(BlockPool, (int)nullptr, MAX_BLOCK_NUM * sizeof(sqlBlock*));
	}
	~BufferManager()
	{
		writeBlockAll();
		delete filehead;
	}
	void setBlockPin(const string file_name, int file_type, int block_num); //块必须已在缓存区，否则调用readFile即可
	void unsetBlockPin(const string file_name, int file_type, int block_num);
	string readFile(const string file_name, int file_type, int block_num, bool setPin=false, bool first_create=false); //按块则输入块号，否则合并成一个string返回
	void writeFile(const string data, const string file_name, int file_type, int block_num, bool setPin=false); //写空串块仍在内存中
	void clearBlock(const string file_name, int file_type, int block_num); //会写回并释放空间
	void writeBlockAll(); // call when close DataBase
	void resetDB_name(const string db_name);
	const string GetDB_name(); // Get the name of database
	void deleteFile(const string file_name, int file_type);
};

#endif