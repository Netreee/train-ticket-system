#include <iostream>
#include <sstream>
#include <string>
#include <cassert>
#include <windows.h>
#include "../RailwayGraph.h"
#include "../Utils.h"

using namespace trainsys;

// 计数器
int testsTotal = 0;

int testsPassed = 0;

// 宏
#define TEST_CASE(name) \
    std::cout << "\n=== 测试案例: " << name << " ===" << std::endl; \
    testsTotal++;

#define ASSERT_TRUE(cond, msg) \
    if (!(cond)) { \
        std::cout << "❌ 失败: " << msg << std::endl; \
        return false; \
    } else { \
        std::cout << "✅ 通过: " << msg << std::endl; \
    }

#define ASSERT_FALSE(cond, msg) ASSERT_TRUE(!(cond), msg)

#define CAPTURE_OUTPUT(code, outputStr) { \
    std::stringstream _buffer; \
    std::streambuf* _old = std::cout.rdbuf(_buffer.rdbuf()); \
    code; \
    std::cout.rdbuf(_old); \
    outputStr = _buffer.str(); \
}

// 基础添加与连通性测试
bool testAddRouteAccessibility() {
    TEST_CASE("基础添加与连通性");

    RailwayGraph graph;

    ASSERT_FALSE(graph.checkStationAccessibility(1, 2), "初始车站不连通");

    graph.addRoute(1, 2, 30, 100, TrainID("T001"));

    ASSERT_TRUE(graph.checkStationAccessibility(1, 2), "添加线路后车站连通");
    ASSERT_TRUE(graph.checkStationAccessibility(2, 1), "反向连通性正确");

    testsPassed++;
    return true;
}

// 多线路连通性测试
bool testMultiRouteConnectivity() {
    TEST_CASE("多线路连通性");

    RailwayGraph graph;
    graph.addRoute(1, 2, 10, 20, TrainID("A"));
    graph.addRoute(2, 3, 10, 20, TrainID("B"));

    ASSERT_TRUE(graph.checkStationAccessibility(1, 3), "间接连通性正确");
    ASSERT_FALSE(graph.checkStationAccessibility(1, 4), "孤立站点不连通");

    testsPassed++;
    return true;
}

// displayRoute 输出测试
bool testDisplayRoute() {
    TEST_CASE("displayRoute 路径输出");

    RailwayGraph graph;
    graph.addRoute(1, 2, 10, 20, TrainID("A"));
    graph.addRoute(2, 3, 10, 20, TrainID("B"));

    std::string output;
    CAPTURE_OUTPUT(graph.displayRoute(1, 3);, output);

    ASSERT_TRUE(output.find("1 -> 2 -> 3") != std::string::npos, "输出包含正确路径");

    testsPassed++;
    return true;
}

// shortestPath 时间与价格测试
bool testShortestPath() {
    TEST_CASE("shortestPath 时间与价格");

    RailwayGraph graph;
    // 构建两条不同权重的路径
    graph.addRoute(1, 2, 10, 100, TrainID("A"));
    graph.addRoute(2, 3, 10, 100, TrainID("B"));
    graph.addRoute(1, 3, 30, 50,  TrainID("C"));

    // 按时间
    std::string outTime;
    CAPTURE_OUTPUT(graph.shortestPath(1, 3, 0);, outTime);
    ASSERT_TRUE(outTime.find("总时间: 20") != std::string::npos, "按时间最短路径花费正确");

    // 按价格
    std::string outPrice;
    CAPTURE_OUTPUT(graph.shortestPath(1, 3, 1);, outPrice);
    ASSERT_TRUE(outPrice.find("总花费: 50") != std::string::npos, "按价格最短路径花费正确");

    testsPassed++;
    return true;
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    std::cout << "🚂 RailwayGraph 模块测试" << std::endl;
    std::cout << "================================" << std::endl;

    bool allPassed = true;
    allPassed &= testAddRouteAccessibility();
    allPassed &= testMultiRouteConnectivity();
    allPassed &= testDisplayRoute();
    allPassed &= testShortestPath();

    std::cout << "\n================================" << std::endl;
    std::cout << "测试完成！通过: " << testsPassed << "/" << testsTotal << std::endl;

    if (allPassed && testsPassed == testsTotal) {
        std::cout << "🎉 所有测试通过！" << std::endl;
        return 0;
    } else {
        std::cout << "❌ 存在失败测试" << std::endl;
        return 1;
    }
}
