#include <sstream>
#include <locale>
#include <codecvt>
#include <string>
#include "DBFunctions.h"

int CreateTable(sqlite3 *db, const std::string& table_name)
{
	std::string statement("CREATE TABLE IF NOT EXISTS ");
	statement.append(table_name);
	statement.append("(ID integer PRIMARY KEY, FSMOD integer NOT NULL, VIRTUALKEY integer NOT NULL, COMMAND text NOT NULL, COMMANDARGS text, ASADMIN integer NOT NULL);");
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

int DeletHotKeyFromDatabase(char* dbname, char* tablename, HotKey& key)
{
	sqlite3 *db = 0;
	std::wstring path(key.GetPath());
	if (SQLITE_OK != ConnectDB(dbname, &db, SQLITE_OPEN_READWRITE))
		return false;

	std::stringstream ss;
	sqlite3_stmt *stmt = NULL;
	ss << "DELETE FROM " <<tablename << "WHERE FSMOD = ?1 AND VIRTUALKEY = ?2 AND COMMAND = ?3" << std::endl;
	int rc = sqlite3_prepare_v2(db, ss.str().c_str(), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		printf("prepare failed: %s\n", sqlite3_errmsg(db));
		goto exit;
	}

	rc = sqlite3_bind_int(stmt, 1, key.GetFsModifiers());
	if (rc != SQLITE_OK)
	{
		printf("sqlite3_bind_int failed: %s\n", sqlite3_errmsg(db));
		goto exit;
	}
	rc = sqlite3_bind_int(stmt, 2, key.GetVK());
	if (rc != SQLITE_OK)
	{
		printf("sqlite3_bind_int failed: %s\n", sqlite3_errmsg(db));
		goto exit;
	}
	rc = sqlite3_bind_text16(stmt, 3, (void*)(path.c_str()), (int)path.size()*sizeof(wchar_t), SQLITE_TRANSIENT);
	if (rc != SQLITE_OK)
	{
		printf("sqlite3_bind_int failed: %s\n", sqlite3_errmsg(db));
		goto exit;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE)
		printf("execution failed: %s\n", sqlite3_errmsg(db));
exit:
	sqlite3_finalize(stmt);
	sqlite3_close(db);
	return 0;
}

// select all data from table 
void GetHotKeysFromDatabase(sqlite3* db, char* tableName, std::vector<HotKey>& keys)
{
	sqlite3_stmt *statement;
	std::string sql("SELECT * FROM ");
	sql.append(tableName);
	sql.append(";");

	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &statement, 0) != SQLITE_OK)
	{
		printf("prepare failed: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return;
	}

	int result = 0;
	while (true)
	{
		result = sqlite3_step(statement);

		if (result == SQLITE_ROW)
		{
			int columntype = sqlite3_column_type(statement, 0);
			if (columntype != SQLITE_INTEGER)
				break;
			int id = (int)sqlite3_column_int64(statement, 0);

			columntype = sqlite3_column_type(statement, 1);
			if (columntype != SQLITE_INTEGER)
				break;
			int fsMod = (int)sqlite3_column_int64(statement, 1);

			columntype = sqlite3_column_type(statement, 2);
			if (columntype != SQLITE_INTEGER)
				break;
			int vKey = (int)sqlite3_column_int64(statement, 2);

			columntype = sqlite3_column_type(statement, 3);
			if (columntype != SQLITE_TEXT)
				break;
			wchar_t* ProgramName = (wchar_t*)sqlite3_column_text16(statement, 3);

			wchar_t* ProgramArgs = nullptr;
			columntype = sqlite3_column_type(statement, 4);
			if(columntype == SQLITE_TEXT)
				ProgramArgs = (wchar_t*)sqlite3_column_text16(statement, 4);
			if (ProgramArgs == nullptr && columntype != SQLITE_NULL)
				break;
			//read asAdmin
			columntype = sqlite3_column_type(statement, 5);
			if (columntype != SQLITE_INTEGER)
				break;
			bool asAdmin = sqlite3_column_int64(statement, 5)!=0;

			HotKey* k = new HotKey(id, fsMod, vKey, ProgramName, wcslen(ProgramName), ProgramArgs, wcslen(ProgramArgs), asAdmin);
			keys.push_back(*k);
		}
		else
			break;
	}
	sqlite3_finalize(statement);
	return;
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
	
	GetHotKeysFromDatabase(db, "COMMANDS", keys);
	sqlite3_close(db);
	return true;
}

void WStringToString(std::string& outParameter, const std::wstring& inParameter)
{
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;

	std::string converted_str = converter.to_bytes(inParameter);
	outParameter.clear();
	outParameter.append(converted_str);
}

void WcharToString(std::string& outParameter,const wchar_t* param)
{
	std::wstring wParam;
	wParam.append(param);
	WStringToString(outParameter, wParam);
}

int AddHotKeyToDatabase(sqlite3* db, char* tablename, HotKey& key)
{
	std::wstringstream ss;
	sqlite3_stmt *stmt = NULL;
	ss << "INSERT INTO " << tablename <<"(FSMOD, VIRTUALKEY, COMMAND, COMMANDARGS, ASADMIN) VALUES(?1,?2,?3,?4,?5);" << std::endl;
	int rc = sqlite3_prepare16_v2(db, ss.str().c_str(), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		MessageBoxA(NULL, sqlite3_errmsg(db), sqlite3_errmsg(db),MB_OK);
		goto exit;
	}
	else
	{
		rc = sqlite3_bind_int(stmt, 1, (int)key.GetFsModifiers());
		if (rc != SQLITE_OK)
		{
			printf("sqlite3_bind_int failed: %s\n", sqlite3_errmsg(db));
			goto exit;
		}
		rc = sqlite3_bind_int(stmt, 2, (int)key.GetVK());
		if (rc != SQLITE_OK)
		{
			printf("sqlite3_bind_int failed: %s\n", sqlite3_errmsg(db));
			goto exit;
		}
		std::wstring tmp(key.GetPath());
		rc = sqlite3_bind_text16(stmt, 3, (void*)(tmp.c_str()), (int)tmp.size()*sizeof(wchar_t), SQLITE_TRANSIENT);
		if (rc != SQLITE_OK)
		{
			printf("sqlite3_bind_text failed: %s\n", sqlite3_errmsg(db));
			goto exit;
		}
		
		tmp.clear();
		if (key.GetArgs() != nullptr)
		{
			tmp.append(key.GetArgs());
			rc = sqlite3_bind_text16(stmt, 4, (void*)(tmp.c_str()), (int)tmp.size()*sizeof(wchar_t), SQLITE_TRANSIENT);
		}
		else
			rc = sqlite3_bind_null(stmt, 4);
		if (rc != SQLITE_OK)
		{
			printf("sqlite3_bind_text failed: %s\n", sqlite3_errmsg(db));
			goto exit;
		}
		rc = sqlite3_bind_int(stmt, 5, (int)key.GetAsAdmin());
		if (rc != SQLITE_OK)
		{
			printf("sqlite3_bind_int failed: %s\n", sqlite3_errmsg(db));
			goto exit;
		}
	}
	
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE)
		printf("execution failed: %s\n", sqlite3_errmsg(db));
exit:
	sqlite3_finalize(stmt);
	return SQLITE_OK;
}

bool SaveSettingsToDataBase(char* dbname, char* tablename, HotKey& key)
{
	sqlite3 *db = 0;
	if (SQLITE_OK != ConnectDB(dbname, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE))
		return false;

	CreateTable(db, tablename);
	int res = AddHotKeyToDatabase(db, tablename, key);
	sqlite3_close(db);
	if (SQLITE_OK != res)
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