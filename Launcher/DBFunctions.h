#pragma once

#include <windows.h>
#include <vector>
#include "HotKey.h"
#include "..\sqlite\sqlite3.h" 

int ConnectDB(const char* dbname, sqlite3** db, int Parameters);
bool ReadSettingFromDatabase(char* dbname, std::vector<HotKey>& keys);
bool SaveSettingsToDataBase(char* dbname, char* tablename, HotKey& key);
int DeletHotKeyFromDatabase(char* dbname, char* tablename, HotKey& key);
LRESULT GetDataBaseName(std::string& dbName);

