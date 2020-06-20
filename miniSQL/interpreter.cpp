#include "interpreter.h"

class SyntaxError 
{
public:
	string prompt = "";
	SyntaxError(){}
	SyntaxError(string s):prompt(s){}
};

int Interpreter::readinCondition(vector<Condition>& ConditionList, Table & table, string & s, int & pos)
{
	string word = getWord(s, pos);
	while (1) {
		if (word.empty())
			throw SyntaxError();
		Condition tmp_cond;
		tmp_cond.AttrName = word;
		word = getWord(s, pos);
		if (word == ">")
			tmp_cond.relation = Relation::GREATER;
		else if (word == ">=")
			tmp_cond.relation = Relation::GREATER_EQUAL;
		else if (word == "=")
			tmp_cond.relation = Relation::EQUAL;
		else if (word == "<")
			tmp_cond.relation = Relation::LESS;
		else if (word == "<=")
			tmp_cond.relation = Relation::LESS_EQUAL;
		else if (word == "<>")
			tmp_cond.relation = Relation::NOT_EQUAL;
		else
			throw SyntaxError();

		if ((word = getWord(s, pos)).empty())
			throw SyntaxError();
		// 类型转换
		istringstream value(word);
		bool found_attr = false;
		for (auto attr : table.attributes) {
			if (attr.name == tmp_cond.AttrName) {
				found_attr = true;
				int tmp_type = attr.type;
				tmp_cond.item.type = tmp_type;
				if (tmp_type == -1) {
					value >> tmp_cond.item.f_data;
				}
				else if (tmp_type == 0) {
					value >> tmp_cond.item.i_data;
				}
				else if (tmp_type >= 1 && tmp_type <= 255) {
					value >> tmp_cond.item.str_data;
				}
				else {
					cout << "ERROR: Unsupported Attribute Type " << word << "." << endl;
					return 1;
				}
			}
		}
		if (found_attr == false) {
			cout << "ERROR: No such attribute " << tmp_cond.AttrName << "." << endl;
			return 1;
		}
		ConditionList.push_back(tmp_cond);
		if ((word = getWord(s, pos)).empty()) {
			break;
		}
		else if (word != "and")
			throw SyntaxError();
		word = getWord(s, pos);
	}
	return 0;
}

int Interpreter::interprete (string &s)
{
	try
	{
		int pos = 0;
		string word;

		word = getWord(s, pos);
		if (word == "create")
		{
			word = getWord(s, pos);
			if (word == "table")
			{
				string primaryKey = "";
				string tableName = "";
				word = getWord(s, pos);
				if (!word.empty())
					tableName = word;
				else {
					throw SyntaxError("Invalid table name.");
				}

				Table newtable;
				newtable.name = tableName;
				int totalLength = 0;
				// create table
				word = getWord(s, pos);
				if (word.empty() || word != "(")
				{
					cout << "ERROR: Table with no attributes" << endl;
					return 1;
				}
				else
				{
					word = getWord(s, pos);
					while (!word.empty() && word != "primary" && word != ")")
					{
						Attribute tmp_attr(word);
						word = getWord(s, pos);
						if (word == "int")
						{
							tmp_attr.type = 0;
							tmp_attr.length = 4;
						}
						else if (word == "float")
						{
							tmp_attr.type = -1;
							tmp_attr.length = 4;
						}
						else if (word == "char")
						{
							word = getWord(s, pos);
							if (word != "(")
							{
								throw SyntaxError("Error near the char()");
							}
							word = getWord(s, pos);
							istringstream convert(word);
							int mystrlen = 0;
							if (!(convert >> mystrlen))
							{
								cout << "ERROR: illegal number in char()" << endl;
								return 1;
							}
							tmp_attr.type = mystrlen;
							tmp_attr.length = mystrlen;
							if ((word = getWord(s, pos)) != ")") {
								throw SyntaxError("Error near the char()");
							}
						}
						else {
							cout << "Unsupported data type '" << word << "'" << endl;
							return 1;
						}
						word = getWord(s, pos);
						if (word == "unique")
						{
							tmp_attr.isUnique = true;
							word = getWord(s, pos);
						}
						newtable.attributes.push_back(tmp_attr);
						if (word != ",") {
							if (word != ")")
								throw SyntaxError();
							else
								break;
						}
						word = getWord(s, pos);
					}
					if (word == "primary")
					{
						if ((word = getWord(s, pos)) != "key")
							throw SyntaxError();
						else {
							if ((word = getWord(s, pos)) != "(") {
								throw SyntaxError();
							}
							word = getWord(s, pos);
							primaryKey = word;
							bool flag = false;
							for (int i = 0; i < newtable.attributes.size(); i++) {
								if (newtable.attributes[i].name == primaryKey) {
									flag = true;
									newtable.attributes[i].isPrimaryKey = true;
								}
								totalLength += newtable.attributes[i].length;
							}
							if (!flag) {
								cout << "ERROR: No primary key." << endl;
								return 1;
							}
							if ((word = getWord(s, pos)) != ")")
								throw SyntaxError();
						}
					}
					else {
						throw SyntaxError("unsupported");
					}
					if ((word = getWord(s, pos)) != ")")
						throw SyntaxError();
				}
				newtable.attriNum = static_cast<int>(newtable.attributes.size());
				newtable.totalLength = totalLength;
				api->createTable(newtable);
				return 0; //success
			}
			else if (word == "index")
			{
				string indexName = "";
				string tableName = "";
				string attrName = "";
				word = getWord(s, pos);
				if (!word.empty()) {
					indexName = word;
				}
				else
					throw SyntaxError();
				Index newIndex(indexName);
				if ((word = getWord(s, pos)) != "on")
					throw SyntaxError();
				if ((word = getWord(s, pos)).empty())
					throw SyntaxError();
				tableName = word;
				newIndex.tableName = tableName;
				if ((word = getWord(s, pos)) != "(")
					throw SyntaxError();
				if ((word = getWord(s, pos)).empty())
					throw SyntaxError();
				attrName = word;
				if (!catalog_manager->existTable(tableName)) {
					cout << "Table '" << tableName << "' doesn't exist." << endl;
					return 1;
				}
				Table tmp_table = catalog_manager->getTable(tableName);
				int colnum = catalog_manager->getColumn(tmp_table, attrName);
				newIndex.type = tmp_table.attributes[colnum].type;
				newIndex.column = colnum;
				if ((word = getWord(s, pos)) != ")")
					throw SyntaxError();
				api->createIndex(newIndex);
				return 1;
			}else{
				throw SyntaxError();
			}
		}
		else if (word == "select")
		{
			//vector<string> attrSelected;
			string tableName = "";
			Table select_table;
			if ((word = getWord(s, pos)) != "*") //TODO: only support *
			{
				throw SyntaxError("Unsupported Select.");
			}
			if ((word = getWord(s, pos)) != "from")
				throw SyntaxError();
			word = getWord(s, pos);
			if (!word.empty())
			{
				tableName = word;
				if (!catalog_manager->existTable(tableName)) {
					cout << "Table '" << tableName << "' doesn't exist." << endl;
					return 1;
				}
				select_table = catalog_manager->getTable(tableName);
			}
			else {
				throw SyntaxError();
			}

			word = getWord(s, pos);
			if (word.empty()) {		//without condition
				api->selectRecord(tableName);
				return 0;
			}
			else if (word != "where")
				throw SyntaxError();
			else {
				vector<Condition> ConditionList;
				int ret = readinCondition(ConditionList, select_table, s, pos);
				if (ret == 0) return 1;
				api->selectRecord(tableName, ConditionList);
				return 0;
			}
		}
		else if (word == "drop")
		{
			word = getWord(s, pos);
			if (word == "table")
			{
				if (!(word = getWord(s, pos)).empty())
				{
					api->dropTable(word);
					return 0;
				}
				else
					throw SyntaxError();
			}
			else if (word == "index")
			{
				if (!(word = getWord(s, pos)).empty())
				{
					api->dropIndex(word);
					return 0;
				}
				else 
					throw SyntaxError();
			}
			else {
				throw SyntaxError();
			}
		}
		else if (word == "delete")
		{
			string tableName = "";
			Table delete_table;
			if ((word = getWord(s, pos)) != "from")
				throw SyntaxError();
			word = getWord(s, pos);
			if (!word.empty())
			{
				tableName = word;
			}
			else
				throw SyntaxError();
			
			if (!catalog_manager->existTable(tableName)) {
				cout << "Table '" << tableName << "' doesn't exist." << endl;
				return 1;
			}
			delete_table = catalog_manager->getTable(tableName);
			if ((word = getWord(s, pos)).empty())
			{
				api->deleteRecord(tableName);
				return 0;
			}
			else if (word == "where")
			{
				vector<Condition> ConditionList;
				int ret = readinCondition(ConditionList, delete_table, s, pos);
				if (ret == 0) return 1;
				api->deleteRecord(tableName, ConditionList);
				return 0;
			}
		}
		else if (word == "insert")
		{
			string tableName = "";
			Tuple insertTuple;
			if ((word = getWord(s, pos))!="into")
				throw SyntaxError();
			if ((word = getWord(s, pos)).empty())
				throw SyntaxError();
			tableName = word;
			if (!catalog_manager->existTable(tableName)) {
				cout << "Table '" << tableName << "' doesn't exist." << endl;
				return 1;
			}
			Table insertTable = catalog_manager->getTable(tableName);
			if ((word = getWord(s, pos)) != "values")
				throw SyntaxError();
			if ((word = getWord(s, pos)) != "(")
				throw SyntaxError();
			word = getWord(s, pos);
			int i = 0;
			while (!word.empty() && word != ")")
			{
				if (i >= insertTable.attriNum) {
					throw SyntaxError("Too many attributes.");
				}
				istringstream value(word);
				int type = insertTable.attributes[i].type;
				if (type = -1) {
					float f_data;
					value >> f_data;
					insertTuple.AddItem(f_data);
				}
				else if (type == 0) {
					int i_data;
					value >> i_data;
					insertTuple.AddItem(i_data);
				}
				else if (type >= 1 && type <= 255) {
					string s_data;
					value >> s_data;
					insertTuple.AddItem(s_data);
				}
				else
					throw SyntaxError();
				word = getWord(s, pos);
				if (word == ",") {
					word = getWord(s, pos);
				}
			}
			if (word != ")")
				throw SyntaxError();
			api->insertRecord(tableName, insertTuple);
			return 0;
		}
		else if (word == "quit") {
			return 200;
		}
		else if (word == "execfile") {
			execFile = getWord(s, pos);
			cout << "exec " << execFile << "..." << endl;
			return 2;
		}
		else {
			if (word != "")
			{
				throw SyntaxError();
				//cout << "ERROR: command " << word << " not found." << endl;
			}
			return 0; //敲入一个回车
		}
	}
	catch (SyntaxError & err)
	{
		cout << "You have an error in your SQL syntax.";
		if (err.prompt != "") {
			cout << " (" << err.prompt << ")";
		}
		cout << endl;
		return 1;
	}
	return 0;
}

string Interpreter::getWord(string & s, int & pos)
{
	int idx1=pos, idx2=pos;
	while (s[pos] != '\0' && (s[pos] == ' ' || s[pos] == '\t' || s[pos] == '\n'))
	{
		pos++;
	}
	idx1 = pos;

	if (s[pos] == '(' || s[pos] == ',' || s[pos] == ')')
	{
		pos++;
		idx2 = pos;
		return s.substr(idx1, idx2 - idx1);
	}
	else if (s[pos] == '\'')  // insert with  '
	{
		pos++;
		while (s[pos] != '\0' && s[pos != '\'']) {
			pos++;
		}
		if (s[pos] == '\'')
		{
			idx1++;
			idx2 = pos;
			pos++;
			return s.substr(idx1, idx2 - idx1);
		}
		else {
			return "";
		}
	}
	else  // words
	{
		while (s[pos] != '\0' && s[pos] != ' ' && s[pos] != '\n' && s[pos] != ',' && s[pos] != '(' && s[pos] != ')')
		{
			pos++;
		}
		idx2 = pos;
		if (idx1 != idx2) {
			return s.substr(idx1, idx2 - idx1);
		}
		else {
			return "";
		}
	}
	return string();
}
