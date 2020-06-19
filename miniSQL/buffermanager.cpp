#include "buffermanager.h"

sqlFile * BufferManager::getFileInfo(const string file_name, int file_type) //find or create sqlFile*
{
	sqlFile* filetmp = filehead->nextfile;
	while (filetmp != nullptr)
	{
		if (filetmp->filename == file_name && filetmp->filetype == file_type)
		{
			break;
		}
		filetmp = filetmp->nextfile;
	}
	if (filetmp == nullptr)
	{
		filetmp = new sqlFile;
		filetmp->filename = file_name;
		filetmp->filetype = file_type;
		filetmp->nextfile = filehead->nextfile;
		filehead->nextfile = filetmp;
		totalfile++;
	}
	return filetmp;
}

void BufferManager::removeFileInfo(sqlFile * fileInfo)
{
	sqlFile* filetmp = filehead;
	while (filetmp->nextfile != nullptr)
	{
		if (filetmp->nextfile->filename == fileInfo->filename && filetmp->nextfile->filetype == fileInfo->filetype)
		{
			break;
		}
		filetmp = filetmp->nextfile;
	}
	filetmp->nextfile = fileInfo->nextfile;
	delete fileInfo;
}

sqlBlock* BufferManager::getBlockInBuffer(sqlFile * fileInfo, int block_num)
{
	sqlBlock* blocktmp = fileInfo->blockhead;
	while (blocktmp != nullptr)
	{
		if (blocktmp->offsetnum == block_num) {
			break;
		}
		blocktmp = blocktmp->nextblock;
	}
	return blocktmp;
}

void BufferManager::writeBlocktoDisk(const string db_name, sqlBlock * blockInfo)
{
	if (!blockInfo->dirty) 
	{
		return;
	}
	else 
	{
		sqlFile* fileInfo = blockInfo->sfile;
		string fileaddr = "";
		if (fileInfo->filetype == 0)
		{
			fileaddr = fileInfo->location + db_name + "\\table\\" + fileInfo->filename + ".txt";
		}
		else if(fileInfo->filetype == 1)
		{
			fileaddr = fileInfo->location + db_name + "\\index\\" + fileInfo->filename + ".txt";
		}
		else
		{
			fileaddr = fileInfo->location + db_name + "\\catalog\\" + fileInfo->filename + ".txt";
		}
		fstream file(fileaddr, ios::out | ios::in);
		if (!file.is_open())
		{
			cout << "ERROR(buffer manager): write-back cannot open file " + fileaddr << endl;
		}
		int offset_byte = (blockInfo->offsetnum) * BLOCK_SIZE;
		file.seekp(offset_byte, ios::beg);
		char * data = blockInfo->blockdata;
		file.write(data, BLOCK_SIZE);
		file.close();
		blockInfo->dirty = 0;
		return;
	}
}

void BufferManager::closeFile(const string db_name, sqlFile * fileInfo)
{
	if (fileInfo == nullptr) {
		//cout << "ERROR(buffer manager): closeFile cannot find file." << endl;
		return;
	}
	sqlBlock* blocktmp = fileInfo->blockhead;
	sqlBlock* blockdelete;
	while (blocktmp != NULL)
	{
		blockdelete = blocktmp;
		writeBlocktoDisk(db_name, blocktmp);
		blocktmp->sfile->count_of_blockinbuffer--;
		blocktmp = blockdelete->nextblock;
		BlockPool[blockdelete->blockid] = nullptr;
		totalblock--;
		delete[] blockdelete->blockdata;
		delete blockdelete;
	}
	sqlFile* filetmp = filehead;
	while (filetmp->nextfile != fileInfo) {
		filetmp = filetmp->nextfile;
	}
	filetmp->nextfile = fileInfo->nextfile;
	delete fileInfo;
	totalfile--;
	return;
}

void BufferManager::setBlockPin(const string file_name, int file_type, int block_num)
{
	const string db_name = this->db_name;
	sqlFile* filetmp = getFileInfo(file_name, file_type);
	sqlBlock* blocktmp = getBlockInBuffer(filetmp, block_num);
	if (blocktmp == nullptr)
	{
		cout << "This Block is not in buffer!" << endl;
	}
	else {
		blocktmp->pin = true;
	}
	return;
}

void BufferManager::unsetBlockPin(const string file_name, int file_type, int block_num)
{
	const string db_name = this->db_name;
	sqlFile* filetmp = getFileInfo(file_name, file_type);
	sqlBlock* blocktmp = getBlockInBuffer(filetmp, block_num);
	if (blocktmp == nullptr)
	{
		cout << "This Block is not in buffer!" << endl;
	}
	else {
		blocktmp->pin = false;
	}
	return;
}

string BufferManager::readFile(const string file_name, int file_type, int block_num, bool setPin, bool first_create)
{
	const string db_name = this->db_name;
	sqlBlock* blocktmp=nullptr;
	string ans = "";
	// if (block_num == -1)
	// {
	// 	int b_num = 0;
	// 	do {
	// 		blocktmp = readFiletoBuffer(db_name, file_name, file_type, b_num, setPin);
	// 		ans += string(blocktmp->blockdata);
	// 		b_num++;
	// 	} while (blocktmp->block_size == BLOCK_SIZE);
	// }
	// else {
	blocktmp = readFiletoBuffer(db_name, file_name, file_type, block_num, setPin, first_create);
	if(blocktmp == nullptr)
	{
		return "";
	}
	ans = string(blocktmp->blockdata);
	// }
	return ans;
}

void BufferManager::writeFile(const string data, const string file_name, int file_type, int block_num, bool setPin)
{
	const string db_name = this->db_name;
	sqlBlock* blocktmp = readFiletoBuffer(db_name, file_name, file_type, block_num, setPin, true);
	if (blocktmp != nullptr)
	{
		memset(blocktmp->blockdata, 0, BLOCK_SIZE);
		// strcpy_s(blocktmp->blockdata,data.length(), data.c_str());
		strcpy(blocktmp->blockdata, data.c_str());
		blocktmp->block_size = strlen(blocktmp->blockdata);
		blocktmp->dirty = 1;
	}
	return;
}

void BufferManager::clearBlock(const string file_name, int file_type, int block_num)
{
	const string db_name = this->db_name;
	sqlBlock* blocktmp = readFiletoBuffer(db_name, file_name, file_type, block_num);
	if (blocktmp != nullptr)
	{	
		memset(blocktmp->blockdata, 0, BLOCK_SIZE);
		blocktmp->block_size = strlen(blocktmp->blockdata);
		blocktmp->dirty = 1;
		writeBlocktoDisk(this->db_name, blocktmp);
		blocktmp->sfile->count_of_blockinbuffer--;
		totalblock--;
		remove_from_block_list(blocktmp, nullptr);
		BlockPool[blocktmp->blockid] = nullptr;
		delete[] blocktmp->blockdata;
		delete blocktmp;
	}
}

void BufferManager::writeBlockAll()
{
	writeBlockAlltoDisk(this->db_name);
}

void BufferManager::writeBlockAlltoDisk(const string db_name)
{
	sqlFile* filetmp = filehead->nextfile;
	sqlFile* filedelete = filetmp;
	while (filetmp != NULL)
	{
		filetmp = filedelete->nextfile;
		closeFile(db_name, filedelete);
		filedelete = filetmp;
	}
	return;
}

void BufferManager::resetDB_name(const string db_name)
{
	writeBlockAlltoDisk(this->db_name);
	this->db_name = db_name;
	this->totalblock = 0;
	this->totalfile = 0;
	filehead = new sqlFile;
	filehead->nextfile = nullptr;
	memset(BlockPool, (int)nullptr, MAX_BLOCK_NUM * sizeof(sqlBlock*));
	cout << "change DB to " + db_name << endl;
	return;
}

void BufferManager::updatePool(int blockid)
{
	for (auto blockptr : BlockPool)
	{
		if (blockptr == nullptr)
			continue;
		if (blockptr->blockid == blockid)
		{
			blockptr->timestamp = 0;
		}
		else {
			blockptr->timestamp++;
		}
	}
}

sqlBlock* BufferManager::getUsableBlock(const string db_name, sqlFile* fileInfo)
{
	sqlBlock* newblock;
	if (totalblock < MAX_BLOCK_NUM)
	{
		newblock = new sqlBlock;
		for (int i = 0; i < MAX_BLOCK_NUM; i++)
		{
			if (BlockPool[i] == nullptr)
			{
				BlockPool[i] = newblock;
				newblock->blockid = i;
				break;
			}
		}
		newblock->blockdata = new char[BLOCK_SIZE];
		memset(newblock->blockdata, 0, BLOCK_SIZE * sizeof(char));
		totalblock++;
		return newblock;
	}
	// LRU
	sqlBlock * blocktmp = nullptr;
	int max_stamp = -1;
	for (auto blockptr : BlockPool)
	{
		if (blockptr->timestamp > max_stamp && blockptr->pin == false) //pin process
		{
			max_stamp = blockptr->timestamp;
			blocktmp = blockptr;
		}
	}
	if (blocktmp == nullptr)
	{
		cout << "ERROR(buffer manager): getUsableBlock error: All Blocks are pinned!" << endl;
		return nullptr;
	}
	writeBlocktoDisk(db_name, blocktmp);
	blocktmp->sfile->count_of_blockinbuffer--;
	//cout << blocktmp->blockid << ": " << string(blocktmp->blockdata) << endl; //For test
	blocktmp->timestamp = 0;
	memset(blocktmp->blockdata, 0, BLOCK_SIZE * sizeof(char));
	remove_from_block_list(blocktmp, fileInfo);
	return blocktmp;
}

void BufferManager::remove_from_block_list(sqlBlock * block, sqlFile* newfileInfo)
{
	sqlBlock * blocktmp = block->sfile->blockhead;
	if (blocktmp->nextblock == nullptr)
	{
		block->sfile->blockhead = nullptr;
		if (block->sfile != newfileInfo) {
			removeFileInfo(block->sfile);
		}
	}
	else {
		while (blocktmp != nullptr)
		{
			if (blocktmp->nextblock == block)
			{
				blocktmp->nextblock = block->nextblock;
				break;
			}
			blocktmp = blocktmp->nextblock;
		}
	}
	return;
}

sqlBlock* BufferManager::readBlocktoBuffer(const string db_name, sqlFile * fileInfo, int offset)
{
	string fpath;
	if (fileInfo->filetype == 0)
	{
		fpath = fileInfo->location + db_name + "\\table\\" + fileInfo->filename + ".txt";
	}
	else if (fileInfo->filetype == 1)
	{
		fpath = fileInfo->location + db_name + "\\index\\" + fileInfo->filename + ".txt";
	}
	else
	{
		fpath = fileInfo->location + db_name + "\\catalog\\" + fileInfo->filename + ".txt";
	}
	fstream file(fpath, ios::out | ios::in);
	if (!file.is_open()) {
		file = fstream(fpath, ios::out); //文件夹已存在，文件不存在
		if (!file.is_open())
		{
			if (fileInfo->is_writing == false) {
				removeFileInfo(fileInfo);
				return nullptr;
			}
			int pos = fpath.find_last_of('\\');
			string dir_str(fpath.substr(0, pos));
			string cmd = "mkdir " + dir_str;
			system(cmd.c_str());
			file = fstream(fpath, ios::out);
			if (!file.is_open()) {
				cout << "ERROR(buffer manager): readBlocktoBuffer cannot open file " + fpath << endl;
				return nullptr;
			}
			cout << "create directory " + dir_str << endl;
		}
	}
	file.seekg(0, ios::end);
	int size = file.tellg()/(int)BLOCK_SIZE;
	size += fileInfo->count_of_blockinbuffer;
	if (size < offset)
	{
		cout << "ERROR(buffer manager): The block number " + to_string(offset) +" is error" << endl;
		file.close();
		return nullptr;
	}
	sqlBlock* block = getUsableBlock(db_name, fileInfo); //ensure it's not in buffer
	block->dirty = 0;
	block->offsetnum = offset;
	block->sfile = fileInfo;
	int offset_byte = offset * BLOCK_SIZE;
	file.seekg(offset_byte, ios::beg);
	file.read(block->blockdata, BLOCK_SIZE);
	block->block_size = strlen(block->blockdata);
	file.close();
	fileInfo->count_of_blockinbuffer++;
	return block;
}

sqlBlock * BufferManager::readFiletoBuffer(const string db_name, const string file_name, int file_type, int block_num, bool setPin, bool is_writing)
{
	sqlFile* fileInfo = getFileInfo(file_name, file_type);
	if(fileInfo == nullptr)
	{
		return nullptr;
	}
	fileInfo->is_writing = is_writing;
	sqlBlock * block;
	if ((block=getBlockInBuffer(fileInfo, block_num))!=nullptr )
	{
		/*if (block->pin == true) {
			cout << "(buffer manager)It's a locked block." << endl;
		}*/
		block->pin = setPin;
		updatePool(block->blockid);
		return block;
	}
	block = readBlocktoBuffer(db_name, fileInfo, block_num);
	if (block == nullptr)
	{
		removeFileInfo(fileInfo);
		cout << "ERROR(buffer manager): readFiletoBuffer error" << endl;
		return nullptr;
	}
	else
	{
		block->pin = setPin;
		if (fileInfo->blockhead == nullptr)
		{
			block->nextblock = nullptr;
			fileInfo->blockhead = block;
		}
		else {
			block->nextblock = fileInfo->blockhead->nextblock;
			fileInfo->blockhead->nextblock = block;
		}
		updatePool(block->blockid);
		return block;
	}
}


const string BufferManager::GetDB_name()
{
	return db_name;
}

void BufferManager::deleteFile(const string file_name, int file_type)
{
	sqlFile* filetmp = filehead->nextfile;
	while (filetmp != nullptr)
	{
		if (filetmp->filename == file_name && filetmp->filetype == file_type)
		{
			break;
		}
		filetmp = filetmp->nextfile;
	}
	if (filetmp == nullptr)
	{
		printf("No such file exists.\n");
		return;
	}
	
	sqlBlock* blocktmp = filetmp->blockhead;
	sqlBlock* blockdelete;
	while (blocktmp != NULL)
	{
		blockdelete = blocktmp;
		blocktmp = blockdelete->nextblock;
		BlockPool[blockdelete->blockid] = nullptr;
		totalblock--;
		delete[] blockdelete->blockdata;
		delete blockdelete;
	}

	string fpath;
	if (filetmp->filetype == 0)
	{
		fpath = filetmp->location + this->db_name + "\\table\\" + filetmp->filename + ".txt";
	}
	else if (filetmp->filetype == 1)
	{
		fpath = filetmp->location + this->db_name + "\\index\\" + filetmp->filename + ".txt";
	}
	else
	{
		fpath = filetmp->location + this->db_name + "\\catalog\\" + filetmp->filename + ".txt";
	}
	removeFileInfo(filetmp);
	totalfile--;
	remove(fpath.c_str());

	return;
}
