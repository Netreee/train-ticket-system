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
        return idToName.find(stationID);
    }

    StationID StationManager::getStationID(const char *stationName) {
        return nameToID.find(String(stationName));
    }
}
