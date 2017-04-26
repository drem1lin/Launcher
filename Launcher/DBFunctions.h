#pragma once

#include <windows.h>
#include <vector>
#include "HotKey.h"

bool ReadSettingFromDatabase(char* dbname, std::vector<HotKey>& keys);
bool SaveSettingsToDataBase(char* dbname, char* tablename, HotKey& key);
LRESULT GetDataBaseName(std::string& dbName);

