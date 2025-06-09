#include <fstream>
#include <iostream>
#include "StationManager.h"

namespace trainsys {
    StationManager::StationManager(const char *filename) {
        std::ifstream fin;
        fin.open(filename, std::ios::in);
        if (!fin.is_open()) {
            puts("station info not found");
            return;
        }
        char stationName[MAX_STATIONNAME_LEN + 1];
        StationID stationID;
        puts("loading station info");
        while (fin >> stationName >> stationID) {
            idToName.insertEntry(stationID, String(stationName));
            nameToID.insertEntry(String(stationName), stationID);
        }
        idToName.sortEntry();
        nameToID.sortEntry();
    }

    String StationManager::getStationName(const StationID &stationID) {
        String ans = idToName.find(stationID);
        if (ans == String()) { // 如果未找到（BinarySearchTable::find 在未找到时返回 ValueType()，即空字符串）
            throw std::runtime_error("站点不存在");
        }
        return ans;
    }

    StationID StationManager::getStationID(const char *stationName) {
        StationID ans = nameToID.find(String(stationName));
        if (ans == 0) { // 如果未找到（BinarySearchTable::find 在未找到时返回 ValueType()，即 0）
            throw std::runtime_error("站点不存在");
        }
        return ans;
    }
}
