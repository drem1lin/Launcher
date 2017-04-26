#include <sstream>
#include <locale>
#include <codecvt>
#include <string>
#include "..\sqlite\sqlite3.h"
#include "DBFunctions.h"

int CreateTable(sqlite3 *db, const std::string& table_name)
{
	std::string statement("CREATE TABLE IF NOT EXISTS ");
	statement.append(table_name);
	statement.append("(ID integer PRIMARY KEY, HWND integer NOT NULL, FSMOD integer NOT NULL, VIRTUALKEY integer NOT NULL, COMMAND text NOT NULL, COMMANDARGS text, ASADMIN integer NOT NULL);");
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
		keys.clear();
		//keys.insert(keys.end(), parts.begin(), parts.end());
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

void CreateStringParameterFromWstring(std::string& outParameter, wchar_t* param)
{
	std::wstring wParam;
	if (param[0] == 0)
	{
		wParam.append(L"NULL");
	}
	else
	{
		wParam.append(L"'");
		wParam.append(param);
		wParam.append(L"'");
	}
	//setup converter
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;

	//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
	std::string converted_str = converter.to_bytes(wParam);
	outParameter.clear();
	outParameter.append(converted_str);
}

int addDataRow(sqlite3* dbname, char* tablename, HotKey& key)
{
	std::stringstream strm;
	
	std::string Path;
	CreateStringParameterFromWstring(Path, key.GetPath());
	std::string Argument;
	CreateStringParameterFromWstring(Argument, key.GetArgs());
//	std::string asAdmin((key.GetAsAdmin() != 0) ? "TRUE" : "FALSE");

	strm << "INSERT INTO " << tablename << "(ID, HWND, FSMOD, VIRTUALKEY, COMMAND, COMMANDARGS, ASADMIN) values(" << 0 << "," << 
							(UINT_PTR)key.GetHWND()<< ", " << 
							key.GetFsModifiers() << ", " << 
							key.GetVK() << ", " << 
							Path.c_str() << ", " <<
							Argument.c_str() << ", " <<
							key.GetAsAdmin() << ")";

	std::string s = strm.str();
	//char *str = &s[0];

	sqlite3_stmt *statement;
	int result = 0;
	//char *query="insert into student(roll,name,cgpa)values(4,'uuu',6.6)";
	char *query = &s[0];
	{
		result = sqlite3_prepare(dbname, query, -1, &statement, 0);
		if (result == SQLITE_OK)
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

LRESULT GetDataBaseName(std::string& dbName)
{
	char ModuleName[MAX_PATH];
	GetModuleFileNameA(NULL, ModuleName, MAX_PATH);
	std::string TMPdbName(ModuleName);
	dbName = TMPdbName.substr(0, TMPdbName.find_last_of('\\') + 1).append("Settings.db");
	return 0;
}