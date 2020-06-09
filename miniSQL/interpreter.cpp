#include "interpreter.h"

int Interpreter::interprete(string & s)
{
	int pos = 0;
	string word, tableName="";

	word = getWord(s, pos);
	if (word == "create")
	{
		word = getWord(s, pos);
		if (word == "table")
		{
			string primaryKey = "";
			word = getWord(s, pos);
			if (!word.empty())
				tableName = word;
			else {
				cout << "ERROR: No valid table name" << endl;
				return 1;
			}
			// create table
			word = getWord(s, pos);
			if (word.empty() || word != "(")
			{
				cout << "ERROR: Table with no attributes" << endl;
				return 1;
			}
			else
			{
				// ²åÈëAPI
			}

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
				return 1;
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
