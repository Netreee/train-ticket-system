#include <iostream>
#include "../StationManager.h"
#include <windows.h>

using namespace trainsys;

int main() {
    SetConsoleOutputCP(CP_UTF8);
    StationManager stationManager("station.txt");
    try {
        // 测试 getStationName
        StationID sid = 1; // 假设站点ID为1存在
        String name = stationManager.getStationName(sid);
        std::cout << "站点ID " << sid << " 对应的站点名: " << name << std::endl;

        // 测试 getStationID
        const char *sname = "SiyuanGate"; // 假设站点名"SiyuanGate"存在
        StationID id = stationManager.getStationID(sname);
        std::cout << "站点名 " << sname << " 对应的站点ID: " << id << std::endl;

        // 测试异常情况（可选，例如查询不存在的站点）
        // StationID inexistID = 999; // 假设站点ID 999不存在
        // String inexistName = stationManager.getStationName(inexistID); // 应抛出异常
        // const char *inexistSName = "NonExistentStation"; // 假设站点名"NonExistentStation"不存在
        // StationID inexistSID = stationManager.getStationID(inexistSName); // 应抛出异常

    } catch (const std::exception &e) {
        std::cerr << "测试异常: " << e.what() << std::endl;
    }
    return 0;
} 