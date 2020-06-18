#include "interpreter.h"

int Interpreter::interprete (string &s)
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
						tmp_attr.type = 0;
					else if (word == "float")
						tmp_attr.type = -1;
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
						{
							cout << "ERROR: Syntax error" << endl;
							return 0;
						}
						else
							break;
					}
					word = getWord(s, pos);
				}
				if (word == "primary")
				{
					if ((word = getWord(s, pos)) != "key") {
						cout << "ERROR: Syntax error" << endl;
						return 0;
					}
					else {
						if ((word = getWord(s, pos)) != "(") {
							cout << "ERROR: Syntax error" << endl;
							return 0;
						}
						word = getWord(s, pos);
						primaryKey = word;
						bool flag = false;
						for (auto attr : newtable.attributes) {
							if (attr.name == primaryKey) {
								flag = true;
								attr.isPrimaryKey = true;
								break;
							}
						}
						if (!flag) {
							cout << "ERROR: No primary key" << endl;
							return 0;
						}
						if ((word = getWord(s, pos)) != ")")
						{
							cout << "ERROR: Syntax error" << endl;
							return 0;
						}
					}
				}
				else {
					cout << "ERROR: Unsupported syntax" << endl;
					return 0;
				}
				if ((word = getWord(s, pos)) != ")")
				{
					cout << "ERROR: Syntax error" << endl;
					return 0;
				}
			}
			newtable.attriNum = static_cast<int>(newtable.attributes.size());
			api->createTable(newtable);
			return 1; //success
		}
		else if (word == "index")
		{
			string indexName = "";
			word = getWord(s, pos);
			if (!word.empty()) {
				indexName = word;
			}
			else {
				cout << "ERROR: Syntax error." << endl;
				return 0;
			}
			word = getWord(s, pos);
		}
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
