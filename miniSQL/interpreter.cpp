#include "interpreter.h"

class SyntaxError {};

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
					cout << "ERROR: No valid table name" << endl;
					return 0;
				}

				Table newtable;
				newtable.name = tableName;
				int totalLength = 0;
				// create table
				word = getWord(s, pos);
				if (word.empty() || word != "(")
				{
					cout << "ERROR: Table with no attributes" << endl;
					return 0;
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
								cout << "ERROR: Unknown Data Type" << endl;
								return 0;
							}
							word = getWord(s, pos);
							istringstream convert(word);
							int mystrlen = 0;
							if (!(convert >> mystrlen))
							{
								cout << "ERROR: illegal number in char()" << endl;
								return 0;
							}
							tmp_attr.type = mystrlen;
							tmp_attr.length = mystrlen;
							if ((word = getWord(s, pos)) != ")") {
								cout << "ERROR: Unknown Data Type" << endl;
								return 0;
							}
						}
						else {
							cout << "ERROR: Unsupported Data Type" << endl;
							return 0;
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
							for (int i = 0; i < newtable.attributes.size() - 1; i++) {
								if (newtable.attributes[i].name == primaryKey) {
									flag = true;
									newtable.attributes[i].isPrimaryKey = true;
								}
								totalLength += newtable.attributes[i].length;
							}
							if (!flag) {
								cout << "ERROR: No primary key" << endl;
								return 0;
							}
							if ((word = getWord(s, pos)) != ")")
								throw SyntaxError();
						}
					}
					else {
						cout << "ERROR: Unsupported syntax" << endl;
						return 0;
					}
					if ((word = getWord(s, pos)) != ")")
						throw SyntaxError();
				}
				newtable.attriNum = static_cast<int>(newtable.attributes.size());
				newtable.totalLength = totalLength;
				//api->createTable(newtable);
				return 1; //success
			}
			else if (word == "index")
			{
				string indexName = "";
				word = getWord(s, pos);
				if (!word.empty()) {
					indexName = word;
				}
				else
					throw SyntaxError();
				word = getWord(s, pos);
			}
		}
		else if (word == "select")
		{
			//vector<string> attrSelected;
			string tableName = "";
			Table select_table;
			if ((word = getWord(s, pos)) != "*") //TODO: only support *
			{
				cout << "Unsupported Syntax." << endl;
				return 0;
			}
			if ((word = getWord(s, pos)) != "from")
				throw SyntaxError();
			word = getWord(s, pos);
			if (!word.empty())
			{
				tableName = word;
				select_table = catalog_manager->getTable(tableName);
				if (select_table.attriNum == 0) {
					cout << "ERROR: No such table " << tableName << "." << endl;
					return 0;
				}
			}
			else {
				throw SyntaxError();
			}

			word = getWord(s, pos);
			if (word.empty()) {		//without condition
				//api->selectRecord(tableName);
				return 1;
			}
			else if (word != "where")
				throw SyntaxError();
			else {
				vector<Condition> ConditionList;
				word = getWord(s, pos);
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
					// ÀàÐÍ×ª»»
					istringstream value(word);
					bool found_attr = false;
					for (auto attr : select_table.attributes) {
						if (attr.name == tmp_cond.AttrName) {
							found_attr = true;
							short tmp_type = attr.type;
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
								return 0;
							}
						}
					}
					if (found_attr == false) {
						cout << "ERROR: No such attribute " << tmp_cond.AttrName << "." << endl;
						return 0;
					}
					ConditionList.push_back(tmp_cond);
					if ((word = getWord(s, pos)).empty()) {
						break;
					}
					else if (word != "and")
						throw SyntaxError();
					word = getWord(s, pos);
				}
				//api->selectRecord(tableName, ConditionList);
				return 1;
			}
		}
		else if (word == "drop")
		{
			word = getWord(s, pos);
			if (word == "table")
			{
				if (!(word = getWord(s, pos)).empty())
				{
					//api->dropTable(word);
					return 1;
				}
				else
					throw SyntaxError();
			}
			else if (word == "index")
			{
				if (!(word = getWord(s, pos)).empty())
				{
					//api->dropIndex(word);
					return 1;
				}
				else 
					throw SyntaxError();
			}
			else {
				return 1;
			}
		}
		else
			return 1;
	}
	catch (SyntaxError &)
	{
		cout << "ERROR: Syntax Error" << endl;
		return 0;
	}
	return 1;
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
	else  // WORDS
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
