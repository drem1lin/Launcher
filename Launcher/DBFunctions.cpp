#include <sstream>
#include "..\sqlite\sqlite3.h"
#include "DBFunctions.h"

int CreateTable(sqlite3 *db, const std::string& table_name)
{
	std::string statement("CREATE TABLE IF NOT EXISTS ");
	statement.append(table_name);
	statement.append("(ID integer PRIMARY KEY, HWND integer NOT NULL, FSMOD integer NOT NULL, VIRTUALKEY integer NOT NULL, COMMAND text NOT NULL, COMMANDARGS text, ASADMIN integer);");
	sqlite3_stmt *stmt = NULL;
	int rc = sqlite3_prepare_v2(db, statement.c_str(), -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		printf("prepare failed: %s\n", sqlite3_errmsg(db));
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE)
		printf("execution failed: %s\n", sqlite3_errmsg(db));
	sqlite3_finalize(stmt);
	return 0;
}

// select all data from table 
void getTableData(sqlite3* dbfile, char* tableName, std::vector<HotKey>& keys)
{
	sqlite3_stmt *statement;

	std::string query("select * from ");
	query.append(tableName);

	if (sqlite3_prepare(dbfile, query.c_str(), -1, &statement, 0) == SQLITE_OK)
	{
		int ctotal = sqlite3_column_count(statement);
		int res = 0;

		std::vector<std::string> parts;

		while (1)
		{
			res = sqlite3_step(statement);

			if (res == SQLITE_ROW)
			{
				for (int i = 0; i < ctotal; i++)
				{
					std::string s = (char*)sqlite3_column_text(statement, i);
					// print or format the output as you want 
					parts.push_back(s);
				}
				//TODO: process string to hotkey
				parts.clear();
			}

			if (res == SQLITE_DONE || res == SQLITE_ERROR)
			{
				break;
			}
		}
	}
}

int ConnectDB(const char* dbname, sqlite3** db, int Parameters)
{
	int res = sqlite3_open_v2(dbname, db, Parameters, 0);
	return res;
}

bool ReadSettingFromDatabase(char* dbname, std::vector<HotKey>& keys)
{
	sqlite3 *db = 0;
	//wchar_t *err;
	if (SQLITE_OK != ConnectDB(dbname, &db, SQLITE_OPEN_READONLY))
		return false;
	
	getTableData(db, "COMMANDS", keys);
	return true;
}

int addDataRow(sqlite3* dbname, char* tablename, HotKey& key)
{
	std::stringstream strm;
	strm << "INSERT INTO " <<tablename<< "(ID, HWND, FSMOD, VIRTUALKEY, COMMAND, COMMANDARGS, ASADMIN) values(" << NULL << key.GetHWND()
		<< ", " << key.GetFsModifiers() << ", " << key.GetVK() << ", " << key.GetFsModifiers() << ",'" << key.GetPath() << "'," << ",'" << key.GetArgs() << "'," << key.GetAsAdmin() << ")";

	std::string s = strm.str();
	//char *str = &s[0];

	sqlite3_stmt *statement;
	int result;
	//char *query="insert into student(roll,name,cgpa)values(4,'uuu',6.6)";
	char *query = &s[0];
	{
		if (sqlite3_prepare(dbname, query, -1, &statement, 0) == SQLITE_OK)
		{
			int res = sqlite3_step(statement);
			result = res;
			sqlite3_finalize(statement);
		}
		return result;
	}
	return SQLITE_OK;
}

bool SaveSettingsToDataBase(char* dbname, char* tablename, HotKey& key)
{
	sqlite3 *db = 0;
	//wchar_t *err;
	if (SQLITE_OK != ConnectDB(dbname, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE))
		return false;

	CreateTable(db, tablename);
	if (SQLITE_OK != addDataRow(db, tablename, key))
		return false;
	return true;
}

