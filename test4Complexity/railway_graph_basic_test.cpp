#include <iostream>
#include <cassert>
#include <chrono>
#include <vector>
#include <windows.h>

#include "../RailwayGraph.h"
#include "../Utils.h"

using namespace trainsys;
using namespace std::chrono;

void testBasicOperations() {
    std::cout << "========== RailwayGraph 基础操作测试 ==========" << std::endl;
    
    RailwayGraph graph;
    
    // 测试1: 添加单条路线
    std::cout << "测试1: 添加单条路线" << std::endl;
    graph.addRoute(1, 2, 120, 150, TrainID("T001"));
    
    bool connected = graph.checkStationAccessibility(1, 2);
    assert(connected);
    std::cout << "✅ 站点1和站点2连通检查通过" << std::endl;
    
    // 测试2: 测试双向连通性
    std::cout << "\n测试2: 双向连通性" << std::endl;
    bool reverseConnected = graph.checkStationAccessibility(2, 1);
    assert(reverseConnected);
    std::cout << "✅ 双向连通性检查通过" << std::endl;
    
    // 测试3: 添加更多路线形成路径
    std::cout << "\n测试3: 多站点路径" << std::endl;
    graph.addRoute(2, 3, 90, 120, TrainID("T002"));
    graph.addRoute(3, 4, 110, 180, TrainID("T003"));
    
    bool pathConnected = graph.checkStationAccessibility(1, 4);
    assert(pathConnected);
    std::cout << "✅ 多站点路径连通性检查通过 (1->2->3->4)" << std::endl;
    
    // 测试4: 不连通站点
    std::cout << "\n测试4: 不连通站点检查" << std::endl;
    bool notConnected = graph.checkStationAccessibility(1, 5);
    assert(!notConnected);
    std::cout << "✅ 不连通站点检查通过" << std::endl;
    
    // 测试5: 独立连通分量
    std::cout << "\n测试5: 独立连通分量" << std::endl;
    graph.addRoute(10, 11, 60, 80, TrainID("T010"));
    graph.addRoute(11, 12, 70, 90, TrainID("T011"));
    
    bool component1 = graph.checkStationAccessibility(1, 3);
    bool component2 = graph.checkStationAccessibility(10, 12);
    bool crossComponent = graph.checkStationAccessibility(1, 10);
    
    assert(component1 && component2 && !crossComponent);
    std::cout << "✅ 独立连通分量检查通过" << std::endl;
    
    std::cout << "\n所有基础操作测试通过！✅" << std::endl;
}

void testShortestPath() {
    std::cout << "\n========== 最短路径算法测试 ==========" << std::endl;
    
    RailwayGraph graph;
    
    // 构建测试网络
    // 创建一个有多条路径的网络来测试最短路径算法
    graph.addRoute(1, 2, 120, 150, TrainID("T001")); // 路径1: 1->2
    graph.addRoute(1, 3, 200, 100, TrainID("T002")); // 路径2: 1->3
    graph.addRoute(2, 4, 80, 120, TrainID("T003"));  // 1->2->4: 总时间200，总价270
    graph.addRoute(3, 4, 60, 200, TrainID("T004"));  // 1->3->4: 总时间260，总价300
    
    std::cout << "测试网络:" << std::endl;
    std::cout << "1 -> 2: 120分钟, 150元 (T001)" << std::endl;
    std::cout << "1 -> 3: 200分钟, 100元 (T002)" << std::endl;
    std::cout << "2 -> 4: 80分钟, 120元 (T003)" << std::endl;
    std::cout << "3 -> 4: 60分钟, 200元 (T004)" << std::endl;
    
    std::cout << "\n期望结果:" << std::endl;
    std::cout << "按时间最短: 1->2->4 (总时间200分钟)" << std::endl;
    std::cout << "按价格最短: 1->3->4 (总价格300元)" << std::endl;
    
    std::cout << "\n测试按时间最短路径:" << std::endl;
    graph.shortestPath(1, 4, 0); // type=0表示按时间
    
    std::cout << "\n测试按价格最短路径:" << std::endl;
    graph.shortestPath(1, 4, 1); // type=1表示按价格
    
    std::cout << "\n测试不连通路径:" << std::endl;
    graph.shortestPath(1, 5, 0);
    
    std::cout << "\n最短路径算法测试完成！✅" << std::endl;
}

void testDisplayRoute() {
    std::cout << "\n========== 路径显示测试 ==========" << std::endl;
    
    RailwayGraph graph;
    
    // 构建简单网络
    graph.addRoute(1, 2, 60, 100, TrainID("T001"));
    graph.addRoute(2, 3, 70, 120, TrainID("T002"));
    graph.addRoute(1, 3, 150, 200, TrainID("T003"));
    
    std::cout << "测试网络:" << std::endl;
    std::cout << "1 -> 2: 60分钟, 100元" << std::endl;
    std::cout << "2 -> 3: 70分钟, 120元" << std::endl;
    std::cout << "1 -> 3: 150分钟, 200元" << std::endl;
    
    std::cout << "\n显示从站点1到站点3的所有路径:" << std::endl;
    graph.displayRoute(1, 3);
    
    std::cout << "\n显示不连通路径:" << std::endl;
    graph.displayRoute(1, 5);
    
    std::cout << "\n路径显示测试完成！✅" << std::endl;
}

void testPerformance() {
    std::cout << "\n========== 性能测试 ==========" << std::endl;
    
    const int ROUTE_COUNT = 1000;
    const int STATION_COUNT = 100;
    
    RailwayGraph graph;
    
    std::cout << "添加 " << ROUTE_COUNT << " 条路线..." << std::endl;
    
    auto start = high_resolution_clock::now();
    
    // 生成随机路线
    for (int i = 0; i < ROUTE_COUNT; i++) {
        StationID from = (i % STATION_COUNT) + 1;
        StationID to = ((i + 1) % STATION_COUNT) + 1;
        if (from == to) to = (to % STATION_COUNT) + 1;
        
        int duration = 30 + (i % 300);
        int price = 50 + (i % 450);
        
        char trainIdStr[32];
        snprintf(trainIdStr, sizeof(trainIdStr), "T%d", 10000 + i);
        
        graph.addRoute(from, to, duration, price, TrainID(trainIdStr));
        
        if ((i + 1) % 100 == 0) {
            std::cout << "已添加 " << (i + 1) << " 条路线" << std::endl;
        }
    }
    
    auto end = high_resolution_clock::now();
    double addTime = duration_cast<milliseconds>(end - start).count();
    
    std::cout << "路线添加完成，用时: " << addTime << " ms" << std::endl;
    std::cout << "平均每条路线: " << addTime / ROUTE_COUNT << " ms" << std::endl;
    
    // 测试连通性检查性能
    std::cout << "\n测试连通性检查性能..." << std::endl;
    const int CHECK_COUNT = 500;
    
    start = high_resolution_clock::now();
    
    int connectedCount = 0;
    for (int i = 0; i < CHECK_COUNT; i++) {
        StationID from = (i % STATION_COUNT) + 1;
        StationID to = ((i + CHECK_COUNT/2) % STATION_COUNT) + 1;
        
        if (graph.checkStationAccessibility(from, to)) {
            connectedCount++;
        }
        
        if ((i + 1) % 100 == 0) {
            std::cout << "已检查 " << (i + 1) << " 对站点" << std::endl;
        }
    }
    
    end = high_resolution_clock::now();
    double checkTime = duration_cast<milliseconds>(end - start).count();
    
    std::cout << "连通性检查完成:" << std::endl;
    std::cout << "- 检查次数: " << CHECK_COUNT << std::endl;
    std::cout << "- 连通对数: " << connectedCount << std::endl;
    std::cout << "- 总用时: " << checkTime << " ms" << std::endl;
    std::cout << "- 平均每次: " << checkTime / CHECK_COUNT << " ms" << std::endl;
    
    std::cout << "\n性能测试完成！✅" << std::endl;
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    
    std::cout << "🚄 RailwayGraph 基础功能测试程序" << std::endl;
    std::cout << "======================================\n" << std::endl;
    
    try {
        // 1. 基础操作测试
        testBasicOperations();
        
        // 2. 最短路径测试
        testShortestPath();
        
        // 3. 路径显示测试
        testDisplayRoute();
        
        // 4. 性能测试
        testPerformance();
        
        std::cout << "\n🎉 所有测试完成！RailwayGraph 模块功能正常。" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ 测试过程中发生异常: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << "❌ 测试过程中发生未知异常" << std::endl;
        return 1;
    }
    
    return 0;
} 