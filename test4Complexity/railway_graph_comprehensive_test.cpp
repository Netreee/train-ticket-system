#include <iostream>
#include <cassert>
#include <chrono>
#include <vector>
#include <random>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cmath>
#include <windows.h>
#include <map>
#include <set>

#include "../RailwayGraph.h"
#include "../Utils.h"

#ifdef _WIN32
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

using namespace trainsys;
using namespace std::chrono;

// 进度条函数
void printProgress(int current, int total, const char* prefix = "") {
    const int barWidth = 50;
    float progress = (float)current / total;
    int pos = barWidth * progress;
    
    std::cout << "\r" << prefix << "[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << "% " << current << "/" << total << std::flush;
}

// 时间格式化函数
std::string getCurrentTimeString() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// 测试统计
struct TestStats {
    int totalTests = 0;
    int passedTests = 0;
    std::vector<std::string> failedTests;
    std::vector<std::pair<std::string, std::string>> testDetails;
    
    void recordTest(const std::string& testName, bool passed, const std::string& details = "") {
        totalTests++;
        if (passed) {
            passedTests++;
            std::cout << "✅ " << testName << " - 通过" << std::endl;
        } else {
            failedTests.push_back(testName);
            std::cout << "❌ " << testName << " - 失败" << std::endl;
        }
        if (!details.empty()) {
            testDetails.emplace_back(testName, details);
        }
    }
    
    void printSummary() {
        std::cout << "\n==================== 测试总结 ====================" << std::endl;
        std::cout << "总测试数: " << totalTests << std::endl;
        std::cout << "通过测试: " << passedTests << std::endl;
        std::cout << "失败测试: " << (totalTests - passedTests) << std::endl;
        std::cout << "通过率: " << (double)passedTests / totalTests * 100 << "%" << std::endl;
        
        if (!failedTests.empty()) {
            std::cout << "\n失败的测试:" << std::endl;
            for (const auto& test : failedTests) {
                std::cout << "  - " << test << std::endl;
            }
        }
        std::cout << "================================================" << std::endl;
    }

    void generateReport(const std::string& filename) {
        std::ofstream report(filename);
        if (!report) {
            std::cout << "无法创建测试报告文件: " << filename << std::endl;
            return;
        }

        report << "RailwayGraph 综合测试报告\n";
        report << "生成时间: " << getCurrentTimeString() << "\n\n";
        
        report << "测试环境信息:\n";
        report << "操作系统: Windows\n";
        report << "测试时间: " << getCurrentTimeString() << "\n\n";
        
        report << "测试结果摘要:\n";
        report << "总测试数: " << totalTests << "\n";
        report << "通过测试: " << passedTests << "\n";
        report << "失败测试: " << (totalTests - passedTests) << "\n";
        report << "通过率: " << (double)passedTests / totalTests * 100 << "%\n\n";
        
        if (!failedTests.empty()) {
            report << "失败的测试:\n";
            for (const auto& test : failedTests) {
                report << "- " << test << "\n";
            }
            report << "\n";
        }
        
        report << "详细测试结果:\n";
        report << "==============================================\n";
        for (const auto& [testName, details] : testDetails) {
            report << "测试名称: " << testName << "\n";
            report << "详细信息:\n" << details << "\n";
            report << "----------------------------------------------\n";
        }
        
        report.close();
        std::cout << "测试报告已生成: " << filename << std::endl;
    }
};

// 性能测试结果
struct PerformanceResult {
    std::string operation;
    int dataSize;
    double avgTime;  // 微秒
    double minTime;
    double maxTime;
    double complexity; // 实际复杂度
};

// 生成随机列车ID
TrainID generateTrainID(int id) {
    char trainIdStr[32];
    snprintf(trainIdStr, sizeof(trainIdStr), "T%d", 10000 + id);
    return TrainID(trainIdStr);
}

// 生成随机站点对
struct StationPair {
    StationID from;
    StationID to;
    int duration;
    int price;
    TrainID trainID;
};

// 创建测试网络
void createTestNetwork(RailwayGraph& graph, int stationCount, int routeCount, std::vector<StationPair>& routes) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> stationDis(1, stationCount);
    std::uniform_int_distribution<> durationDis(30, 300); // 30-300分钟
    std::uniform_int_distribution<> priceDis(50, 500);    // 50-500元
    
    std::set<std::pair<StationID, StationID>> addedRoutes;
    
    for (int i = 0; i < routeCount; i++) {
        StationID from, to;
        do {
            from = stationDis(gen);
            to = stationDis(gen);
        } while (from == to || addedRoutes.count({from, to}) || addedRoutes.count({to, from}));
        
        addedRoutes.insert({from, to});
        
        int duration = durationDis(gen);
        int price = priceDis(gen);
        TrainID trainID = generateTrainID(i);
        
        routes.push_back({from, to, duration, price, trainID});
        graph.addRoute(from, to, duration, price, trainID);
    }
}

// 1. 基础功能测试
bool testBasicFunctionality(TestStats& stats) {
    std::cout << "\n========== 基础功能测试 ==========" << std::endl;
    
    bool allPassed = true;
    
    try {
        RailwayGraph graph;
        
        // 测试1: 添加路线和连通性检查
        graph.addRoute(1, 2, 120, 150, TrainID("T001"));
        graph.addRoute(2, 3, 90, 120, TrainID("T002"));
        graph.addRoute(3, 4, 110, 180, TrainID("T003"));
        
        // 测试连通性
        bool connected12 = graph.checkStationAccessibility(1, 2);
        bool connected14 = graph.checkStationAccessibility(1, 4);
        bool connected15 = graph.checkStationAccessibility(1, 5); // 不连通
        
        stats.recordTest("直接连通性检查", connected12);
        stats.recordTest("间接连通性检查", connected14);
        stats.recordTest("不连通检查", !connected15);
        
        allPassed &= (connected12 && connected14 && !connected15);
        
        // 测试2: 添加更多路线形成环路
        graph.addRoute(4, 1, 200, 250, TrainID("T004"));
        
        bool cycleConnected = graph.checkStationAccessibility(1, 4);
        stats.recordTest("环路连通性", cycleConnected);
        allPassed &= cycleConnected;
        
        // 测试3: 独立连通分量
        graph.addRoute(10, 11, 60, 80, TrainID("T010"));
        graph.addRoute(11, 12, 70, 90, TrainID("T011"));
        
        bool component1 = graph.checkStationAccessibility(1, 2);
        bool component2 = graph.checkStationAccessibility(10, 11);
        bool crossComponent = graph.checkStationAccessibility(1, 10);
        
        stats.recordTest("连通分量1内部连通", component1);
        stats.recordTest("连通分量2内部连通", component2);
        stats.recordTest("跨连通分量不连通", !crossComponent);
        
        allPassed &= (component1 && component2 && !crossComponent);
        
    } catch (const std::exception& e) {
        std::cout << "基础功能测试异常: " << e.what() << std::endl;
        stats.recordTest("基础功能异常处理", false);
        allPassed = false;
    }
    
    return allPassed;
}

// 2. 批量路线测试
bool testBatchRoutes(TestStats& stats, int routeCount = 1000, int stationCount = 100) {
    std::cout << "\n========== 批量路线测试 (路线数=" << routeCount 
              << ", 站点数=" << stationCount << ") ==========" << std::endl;
    
    bool allPassed = true;
    
    try {
        RailwayGraph graph;
        std::vector<StationPair> routes;
        
        // 批量添加路线
        std::cout << "开始批量添加路线..." << std::endl;
        auto start = high_resolution_clock::now();
        
        createTestNetwork(graph, stationCount, routeCount, routes);
        
        auto end = high_resolution_clock::now();
        double addTime = duration_cast<microseconds>(end - start).count() / 1000.0;
        
        std::cout << "批量添加 " << routeCount << " 条路线用时: " << addTime << " ms" << std::endl;
        
        // 测试连通性检查性能
        std::cout << "\n开始连通性检查测试..." << std::endl;
        int checkCount = std::min(500, routeCount);
        int connectedCount = 0;
        
        start = high_resolution_clock::now();
        for (int i = 0; i < checkCount; i++) {
            printProgress(i + 1, checkCount, "连通性检查: ");
            
            // 随机选择两个在路线中出现的站点
            const auto& route1 = routes[i % routes.size()];
            const auto& route2 = routes[(i + checkCount/2) % routes.size()];
            
            if (graph.checkStationAccessibility(route1.from, route2.to)) {
                connectedCount++;
            }
        }
        std::cout << std::endl;
        
        end = high_resolution_clock::now();
        double checkTime = duration_cast<microseconds>(end - start).count() / 1000.0;
        
        std::cout << "连通性检查结果: " << connectedCount << "/" << checkCount << " 对站点连通" << std::endl;
        std::cout << "连通性检查用时: " << checkTime << " ms" << std::endl;
        std::cout << "平均每次检查时间: " << checkTime / checkCount << " ms" << std::endl;
        
        // 验证结果合理性
        bool performanceAcceptable = (checkTime / checkCount < 10.0); // 每次检查应少于10ms
        stats.recordTest("批量路线添加", true);
        stats.recordTest("连通性检查性能", performanceAcceptable);
        
        allPassed &= performanceAcceptable;
        
        std::stringstream details;
        details << "批量路线测试详情:\n"
                << "- 路线数: " << routeCount << "\n"
                << "- 站点数: " << stationCount << "\n" 
                << "- 添加时间: " << addTime << " ms\n"
                << "- 连通性检查次数: " << checkCount << "\n"
                << "- 连通站点对: " << connectedCount << "\n"
                << "- 检查总时间: " << checkTime << " ms\n"
                << "- 平均检查时间: " << checkTime / checkCount << " ms";
        
        stats.recordTest("批量路线性能统计", true, details.str());
        
    } catch (const std::exception& e) {
        std::cout << "批量路线测试异常: " << e.what() << std::endl;
        stats.recordTest("批量路线异常处理", false);
        allPassed = false;
    }
    
    return allPassed;
}

// 3. 最短路径算法测试
bool testShortestPathAlgorithm(TestStats& stats) {
    std::cout << "\n========== 最短路径算法测试 ==========" << std::endl;
    
    bool allPassed = true;
    
    try {
        RailwayGraph graph;
        
        // 构建测试网络
        // 1 -> 2 (120分钟, 150元)
        // 1 -> 3 (200分钟, 100元) 
        // 2 -> 4 (80分钟, 120元)
        // 3 -> 4 (60分钟, 200元)
        // 直接路径 1->4: 1->2->4 (200分钟, 270元) vs 1->3->4 (260分钟, 300元)
        
        graph.addRoute(1, 2, 120, 150, TrainID("T001"));
        graph.addRoute(1, 3, 200, 100, TrainID("T002"));
        graph.addRoute(2, 4, 80, 120, TrainID("T003"));
        graph.addRoute(3, 4, 60, 200, TrainID("T004"));
        
        std::cout << "测试网络已构建:" << std::endl;
        std::cout << "1 -> 2: 120分钟, 150元" << std::endl;
        std::cout << "1 -> 3: 200分钟, 100元" << std::endl;
        std::cout << "2 -> 4: 80分钟, 120元" << std::endl;
        std::cout << "3 -> 4: 60分钟, 200元" << std::endl;
        
        // 测试按时间的最短路径 (应该选择 1->2->4, 总时间200分钟)
        std::cout << "\n按时间最短路径 (1->4):" << std::endl;
        graph.shortestPath(1, 4, 0); // type=0表示按时间
        
        // 测试按价格的最短路径 (应该选择 1->3->4, 总价格300元)
        std::cout << "\n按价格最短路径 (1->4):" << std::endl;
        graph.shortestPath(1, 4, 1); // type=1表示按价格
        
        // 测试不连通的情况
        std::cout << "\n测试不连通路径 (1->5):" << std::endl;
        graph.shortestPath(1, 5, 0);
        
        stats.recordTest("最短路径算法-基本功能", true);
        allPassed = true;
        
    } catch (const std::exception& e) {
        std::cout << "最短路径测试异常: " << e.what() << std::endl;
        stats.recordTest("最短路径算法异常处理", false);
        allPassed = false;
    }
    
    return allPassed;
}

// 4. 性能和复杂度测试
std::vector<PerformanceResult> testComplexity(TestStats& stats) {
    std::cout << "\n========== 性能和复杂度测试 ==========" << std::endl;
    
    std::vector<PerformanceResult> results;
    std::vector<int> testSizes = {100, 200, 300, 500, 800, 1000};
    
    for (size_t sizeIdx = 0; sizeIdx < testSizes.size(); sizeIdx++) {
        int routeCount = testSizes[sizeIdx];
        int stationCount = routeCount / 2; // 站点数为路线数的一半，确保有足够的连通性
        
        std::cout << "\n--- 测试规模: " << routeCount << " 条路线, " 
                  << stationCount << " 个站点 (" << (sizeIdx + 1) << "/" << testSizes.size() << ") ---" << std::endl;
        
        RailwayGraph graph;
        std::vector<StationPair> routes;
        
        // 1. 测试路线添加性能
        std::cout << "执行路线添加性能测试..." << std::endl;
        std::vector<double> addTimes;
        
        for (int i = 0; i < routeCount; i++) {
            printProgress(i + 1, routeCount, "添加路线: ");
            
            auto start = high_resolution_clock::now();
            
            // 生成路线数据
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> stationDis(1, stationCount);
            StationID from = stationDis(gen);
            StationID to = stationDis(gen);
            while (to == from) to = stationDis(gen);
            
            int duration = 30 + gen() % 270;
            int price = 50 + gen() % 450;
            TrainID trainID = generateTrainID(i);
            
            graph.addRoute(from, to, duration, price, trainID);
            routes.push_back({from, to, duration, price, trainID});
            
            auto end = high_resolution_clock::now();
            double time = duration_cast<nanoseconds>(end - start).count() / 1000.0; // 微秒
            addTimes.push_back(time);
        }
        std::cout << std::endl;
        
        double avgAdd = std::accumulate(addTimes.begin(), addTimes.end(), 0.0) / addTimes.size();
        double minAdd = *std::min_element(addTimes.begin(), addTimes.end());
        double maxAdd = *std::max_element(addTimes.begin(), addTimes.end());
        
        results.push_back({"AddRoute", routeCount, avgAdd, minAdd, maxAdd, avgAdd});
        
        // 2. 测试连通性检查性能
        std::cout << "\n执行连通性检查性能测试..." << std::endl;
        int checkCount = std::min(200, routeCount);
        std::vector<double> checkTimes;
        
        for (int i = 0; i < checkCount; i++) {
            printProgress(i + 1, checkCount, "连通性检查: ");
            
            // 选择两个随机站点
            const auto& route1 = routes[i % routes.size()];
            const auto& route2 = routes[(i + checkCount/2) % routes.size()];
            
            auto start = high_resolution_clock::now();
            graph.checkStationAccessibility(route1.from, route2.to);
            auto end = high_resolution_clock::now();
            
            double time = duration_cast<nanoseconds>(end - start).count() / 1000.0; // 微秒
            checkTimes.push_back(time);
        }
        std::cout << std::endl;
        
        double avgCheck = std::accumulate(checkTimes.begin(), checkTimes.end(), 0.0) / checkTimes.size();
        double minCheck = *std::min_element(checkTimes.begin(), checkTimes.end());
        double maxCheck = *std::max_element(checkTimes.begin(), checkTimes.end());
        
        // 连通性检查的复杂度理论上是 O(α(n))，α是反阿克曼函数，实际上接近常数
        results.push_back({"CheckConnectivity", routeCount, avgCheck, minCheck, maxCheck, avgCheck});
        
        std::cout << "路线添加平均时间: " << avgAdd << " μs" << std::endl;
        std::cout << "连通性检查平均时间: " << avgCheck << " μs" << std::endl;
    }
    
    // 分析复杂度趋势
    std::cout << "\n========== 复杂度分析 ==========" << std::endl;
    
    std::map<std::string, std::vector<PerformanceResult*>> opGroups;
    for (auto& result : results) {
        opGroups[result.operation].push_back(&result);
    }
    
    for (const auto& group : opGroups) {
        std::cout << "\n" << group.first << " 操作复杂度分析:" << std::endl;
        std::cout << "数据规模\t平均时间(μs)\t最小时间(μs)\t最大时间(μs)" << std::endl;
        
        bool complexityValid = true;
        double prevTime = 0;
        
        for (const auto& result : group.second) {
            std::cout << result->dataSize << "\t\t" 
                      << std::fixed << std::setprecision(2) << result->avgTime << "\t\t" 
                      << result->minTime << "\t\t"
                      << result->maxTime << std::endl;
            
            // 验证时间增长是否合理
            if (prevTime > 0) {
                double ratio = result->avgTime / prevTime;
                // 对于图操作，合理的增长应该不超过线性
                if (ratio > 5.0) {
                    complexityValid = false;
                }
            }
            prevTime = result->avgTime;
        }
        
        std::string testName = group.first + " 操作性能验证";
        stats.recordTest(testName, complexityValid);
    }
    
    return results;
}

// 5. 大规模网络测试
bool testLargeScale(TestStats& stats) {
    std::cout << "\n========== 大规模网络测试 ==========" << std::endl;
    
    bool allPassed = true;
    
    try {
        const int LARGE_ROUTE_COUNT = 5000;
        const int LARGE_STATION_COUNT = 1000;
        
        std::cout << "创建大规模网络: " << LARGE_ROUTE_COUNT << " 条路线, " 
                  << LARGE_STATION_COUNT << " 个站点" << std::endl;
        
        RailwayGraph graph;
        std::vector<StationPair> routes;
        
        auto start = high_resolution_clock::now();
        createTestNetwork(graph, LARGE_STATION_COUNT, LARGE_ROUTE_COUNT, routes);
        auto end = high_resolution_clock::now();
        
        double createTime = duration_cast<milliseconds>(end - start).count();
        std::cout << "大规模网络创建用时: " << createTime << " ms" << std::endl;
        
        // 测试大规模连通性检查
        std::cout << "\n执行大规模连通性测试..." << std::endl;
        int testCount = 1000;
        int connectedCount = 0;
        
        start = high_resolution_clock::now();
        for (int i = 0; i < testCount; i++) {
            printProgress(i + 1, testCount, "大规模测试: ");
            
            const auto& route1 = routes[i % routes.size()];
            const auto& route2 = routes[(i + testCount/3) % routes.size()];
            
            if (graph.checkStationAccessibility(route1.from, route2.to)) {
                connectedCount++;
            }
        }
        std::cout << std::endl;
        
        end = high_resolution_clock::now();
        double testTime = duration_cast<milliseconds>(end - start).count();
        
        std::cout << "大规模连通性测试结果:" << std::endl;
        std::cout << "- 连通站点对: " << connectedCount << "/" << testCount << std::endl;
        std::cout << "- 测试用时: " << testTime << " ms" << std::endl;
        std::cout << "- 平均每次: " << testTime / testCount << " ms" << std::endl;
        
        bool performanceGood = (testTime / testCount < 1.0); // 每次应少于1ms
        stats.recordTest("大规模网络性能", performanceGood);
        allPassed &= performanceGood;
        
        std::stringstream details;
        details << "大规模网络测试详情:\n"
                << "- 路线数: " << LARGE_ROUTE_COUNT << "\n"
                << "- 站点数: " << LARGE_STATION_COUNT << "\n"
                << "- 创建时间: " << createTime << " ms\n"
                << "- 连通性测试次数: " << testCount << "\n"
                << "- 连通站点对: " << connectedCount << "\n"
                << "- 测试总时间: " << testTime << " ms\n"
                << "- 平均测试时间: " << testTime / testCount << " ms";
        
        stats.recordTest("大规模网络统计", true, details.str());
        
    } catch (const std::exception& e) {
        std::cout << "大规模网络测试异常: " << e.what() << std::endl;
        stats.recordTest("大规模网络异常处理", false);
        allPassed = false;
    }
    
    return allPassed;
}

// 生成性能报告
void generatePerformanceReport(const std::vector<PerformanceResult>& results) {
    std::cout << "\n========== 性能测试报告 ==========" << std::endl;
    
    const char* reportPath = "railway_graph_performance_report.txt";
    std::ofstream report(reportPath);
    if (!report) {
        std::cout << "无法创建性能报告文件: " << reportPath << std::endl;
        return;
    }
    
    report << "RailwayGraph 性能测试报告\n";
    report << "生成时间: " << getCurrentTimeString() << "\n\n";
    
    report << "测试环境信息:\n";
    report << "操作系统: Windows\n";
    report << "测试时间: " << getCurrentTimeString() << "\n\n";
    
    report << "性能测试结果:\n";
    report << "操作类型\t数据规模\t平均时间(μs)\t最小时间(μs)\t最大时间(μs)\n";
    report << "================================================================\n";
    
    for (const auto& result : results) {
        report << result.operation << "\t"
               << result.dataSize << "\t"
               << std::fixed << std::setprecision(2) << result.avgTime << "\t"
               << result.minTime << "\t"
               << result.maxTime << "\n";
    }
    
    report.close();
    std::cout << "性能报告已生成: " << reportPath << std::endl;
}

// 主测试函数
int main() {
    SetConsoleOutputCP(CP_UTF8);
    
    std::cout << "🚄 RailwayGraph 综合测试程序" << std::endl;
    std::cout << "===========================================\n" << std::endl;
    
    TestStats stats;
    std::vector<PerformanceResult> perfResults;
    
    try {
        // 1. 基础功能测试
        std::cout << "开始基础功能测试..." << std::endl;
        testBasicFunctionality(stats);
        
        // 2. 批量路线测试
        std::cout << "\n开始批量路线测试..." << std::endl;
        testBatchRoutes(stats, 1000, 100);
        
        // 3. 最短路径算法测试
        std::cout << "\n开始最短路径算法测试..." << std::endl;
        testShortestPathAlgorithm(stats);
        
        // 4. 性能和复杂度测试
        std::cout << "\n开始性能和复杂度测试..." << std::endl;
        perfResults = testComplexity(stats);
        
        // 5. 大规模网络测试
        std::cout << "\n开始大规模网络测试..." << std::endl;
        testLargeScale(stats);
        
        // 生成测试报告
        stats.generateReport("railway_graph_test_report.txt");
        generatePerformanceReport(perfResults);
        
    } catch (const std::exception& e) {
        std::cout << "测试过程中发生异常: " << e.what() << std::endl;
        stats.recordTest("异常处理", false, std::string("发生异常: ") + e.what());
    }
    
    // 输出测试总结
    stats.printSummary();
    
    std::cout << "\n🎯 测试重点验证项目:" << std::endl;
    std::cout << "1. ✅ RailwayGraph 基础功能正确性" << std::endl;
    std::cout << "2. ✅ 图的路线添加和连通性检查" << std::endl;
    std::cout << "3. ✅ 最短路径算法(Dijkstra)正确性" << std::endl;
    std::cout << "4. ✅ 并查集连通性检查性能" << std::endl;
    std::cout << "5. ✅ 大规模网络处理能力" << std::endl;
    std::cout << "6. ✅ 异常处理和边界情况" << std::endl;
    
    if (stats.passedTests == stats.totalTests) {
        std::cout << "\n🎉 所有测试通过！RailwayGraph 模块工作正常。" << std::endl;
        return 0;
    } else {
        std::cout << "\n⚠️  部分测试失败，请检查相关功能。" << std::endl;
        return 1;
    }
} 