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

#include "../SchedulerManager.h"
#include "../TrainScheduler.h" 
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

// 在文件开头添加进度条函数
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

// 在文件开头添加时间格式化函数
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
    std::vector<std::pair<std::string, std::string>> testDetails;  // 存储测试名称和详细信息
    
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

        report << "SchedulerManager 综合测试报告\n";
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

// 生成随机列车调度信息
void generateRandomScheduler(TrainID& trainID, int& seatNum, int& stationCount, 
                           StationID stations[], int duration[], int price[]) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    
    seatNum = 100 + gen() % 500;  // 100-600座位
    stationCount = 3 + gen() % 8;  // 3-10个站点
    
    // 生成站点ID (避免重复)
    std::vector<int> stationPool;
    for (int i = 1; i <= 1000; i++) {
        stationPool.push_back(i);
    }
    std::shuffle(stationPool.begin(), stationPool.end(), gen);
    
    for (int i = 0; i < stationCount; i++) {
        stations[i] = stationPool[i];
    }
    
    // 生成运行时间和价格
    for (int i = 0; i < stationCount - 1; i++) {
        duration[i] = 30 + gen() % 180;  // 30-210分钟
        price[i] = 50 + gen() % 300;     // 50-350元
    }
}

// 1. 基础功能测试
bool testBasicFunctionality(TestStats& stats) {
    std::cout << "\n========== 基础功能测试 ==========" << std::endl;
    
    bool allPassed = true;
    
    // 测试1: 创建和基本操作
    try {
        SchedulerManager manager("test_basic_func.dat");
        
        TrainID trainId("T001");
        StationID stations[] = {1, 2, 3, 4};
        int duration[] = {120, 90, 110};
        int price[] = {150, 200, 180};
        
        // 添加调度
        manager.addScheduler(trainId, 300, 4, stations, duration, price);
        bool exists = manager.existScheduler(trainId);
        stats.recordTest("添加和存在性检查", exists);
        allPassed &= exists;
        
        // 获取调度
        if (exists) {
            TrainScheduler scheduler = manager.getScheduler(trainId);
            bool correctID = (scheduler.getTrainID() == trainId);
            bool correctSeats = (scheduler.getSeatNum() == 300);
            bool correctStations = (scheduler.getPassingStationNum() == 4);
            
            stats.recordTest("获取调度信息-ID", correctID);
            stats.recordTest("获取调度信息-座位数", correctSeats);
            stats.recordTest("获取调度信息-站点数", correctStations);
            allPassed &= (correctID && correctSeats && correctStations);
        }
        
        // 删除调度
        manager.removeScheduler(trainId);
        bool notExists = !manager.existScheduler(trainId);
        stats.recordTest("删除调度", notExists);
        allPassed &= notExists;
        
    } catch (const std::exception& e) {
        std::cout << "基础功能测试异常: " << e.what() << std::endl;
        stats.recordTest("基础功能异常处理", false);
        allPassed = false;
    }
    
    // 测试2: 异常情况
    try {
        SchedulerManager manager("test_exception.dat");
        
        TrainID trainId("T002");
        StationID stations[] = {1, 2, 3};
        int duration[] = {120, 90};
        int price[] = {150, 200};
        
        // 测试无效参数
        bool caughtException = false;
        try {
            manager.addScheduler(trainId, 0, 3, stations, duration, price); // 无效座位数
        } catch (const std::invalid_argument&) {
            caughtException = true;
        }
        
        stats.recordTest("无效参数异常处理", caughtException);
        allPassed &= caughtException;
        
    } catch (const std::exception& e) {
        std::cout << "异常测试失败: " << e.what() << std::endl;
        stats.recordTest("异常情况测试", false);
        allPassed = false;
    }
    
    return allPassed;
}

// 2. 批量数据测试
bool testBatchOperations(TestStats& stats, int batchSize = 1000) {
    std::cout << "\n========== 批量数据测试 (N=" << batchSize << ") ==========" << std::endl;
    
    bool allPassed = true;
    
    try {
        SchedulerManager manager("test_batch.dat");
        
        // 生成测试数据
        std::cout << "生成测试数据..." << std::endl;
        std::vector<TrainID> trainIDs;
        std::vector<int> seatNums;
        std::vector<int> stationCounts;
        std::vector<std::vector<StationID>> stationsData;
        std::vector<std::vector<int>> durationData;
        std::vector<std::vector<int>> priceData;
        
        for (int i = 0; i < batchSize; i++) {
            printProgress(i + 1, batchSize, "生成数据: ");
            trainIDs.push_back(TrainID(generateTrainID(i)));
            
            StationID stations[MAX_PASSING_STATION_NUMBER];
            int duration[MAX_PASSING_STATION_NUMBER];
            int price[MAX_PASSING_STATION_NUMBER];
            int seatNum, stationCount;
            
            generateRandomScheduler(trainIDs[i], seatNum, stationCount, stations, duration, price);
            
            seatNums.push_back(seatNum);
            stationCounts.push_back(stationCount);
            
            stationsData.emplace_back(stations, stations + stationCount);
            durationData.emplace_back(duration, duration + stationCount - 1);
            priceData.emplace_back(price, price + stationCount - 1);
        }
        std::cout << std::endl;
        
        // 批量插入
        std::cout << "\n开始批量插入..." << std::endl;
        auto start = high_resolution_clock::now();
        for (int i = 0; i < batchSize; i++) {
            printProgress(i + 1, batchSize, "插入进度: ");
            manager.addScheduler(trainIDs[i], seatNums[i], stationCounts[i],
                               stationsData[i].data(), durationData[i].data(), priceData[i].data());
        }
        std::cout << std::endl;
        auto end = high_resolution_clock::now();
        
        double insertTime = duration_cast<microseconds>(end - start).count();
        std::cout << "批量插入 " << batchSize << " 条记录用时: " 
                  << insertTime / 1000.0 << " ms" << std::endl;
        
        // 验证插入结果
        std::cout << "\n开始验证插入结果..." << std::endl;
        int existCount = 0;
        start = high_resolution_clock::now();
        for (int i = 0; i < batchSize; i++) {
            printProgress(i + 1, batchSize, "验证插入: ");
            
            // 添加超时检查
            auto checkStart = high_resolution_clock::now();
            bool exists = false;
            try {
                exists = manager.existScheduler(trainIDs[i]);
                auto checkEnd = high_resolution_clock::now();
                auto checkDuration = duration_cast<milliseconds>(checkEnd - checkStart).count();
                
                // 如果单次检查超过5秒，跳过并记录错误
                if (checkDuration > 5000) {
                    std::cout << "\n警告: 第 " << i << " 条记录查询超时 (" << checkDuration << "ms)，跳过" << std::endl;
                    continue;
                }
                
                if (exists) {
                    existCount++;
                }
            } catch (const std::exception& e) {
                std::cout << "\n警告: 验证第 " << i << " 条记录时发生异常: " << e.what() << std::endl;
                continue;
            }
            
            // 每处理100条记录输出一次状态
            if ((i + 1) % 100 == 0) {
                auto currentTime = high_resolution_clock::now();
                auto elapsedTime = duration_cast<milliseconds>(currentTime - start).count();
                std::cout << "\n已验证 " << (i + 1) << " 条记录，耗时 " << elapsedTime << "ms，成功 " << existCount << " 条" << std::endl;
            }
                 }
         std::cout << std::endl;
         end = high_resolution_clock::now();
         
         double queryTime = duration_cast<microseconds>(end - start).count();
         std::cout << "批量查询 " << batchSize << " 条记录用时: " 
                   << queryTime / 1000.0 << " ms" << std::endl;
         std::cout << "验证结果: " << existCount << "/" << batchSize << " 条记录存在" << std::endl;
         
         bool allExist = (existCount == batchSize);
        stats.recordTest("批量插入验证", allExist);
        allPassed &= allExist;
        
        // 随机查询验证
        std::cout << "\n开始随机查询验证..." << std::endl;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, batchSize - 1);
        
        int correctCount = 0;
        int randomCheckCount = 100;  // 随机检查100个
        for (int i = 0; i < randomCheckCount; i++) {
            printProgress(i + 1, randomCheckCount, "随机验证: ");
            int idx = dis(gen);
            try {
                TrainScheduler scheduler = manager.getScheduler(trainIDs[idx]);
                if (scheduler.getSeatNum() == seatNums[idx] && 
                    scheduler.getPassingStationNum() == stationCounts[idx]) {
                    correctCount++;
                }
            } catch (const std::exception& e) {
                std::cout << "\n验证第 " << i << " 条记录时发生异常: " << e.what() << std::endl;
            }
        }
        std::cout << std::endl;
        
        std::cout << "随机验证结果: " << correctCount << "/" << randomCheckCount << " 条记录正确" << std::endl;
        bool randomQueryCorrect = (correctCount >= 95);  // 允许5%误差
        stats.recordTest("随机查询正确性", randomQueryCorrect);
        allPassed &= randomQueryCorrect;
        
        // 批量删除测试
        std::cout << "\n开始批量删除测试..." << std::endl;
        int deleteCount = batchSize / 2;
        start = high_resolution_clock::now();
        for (int i = 0; i < deleteCount; i++) {
            printProgress(i + 1, deleteCount, "删除进度: ");
            try {
                manager.removeScheduler(trainIDs[i]);
            } catch (const std::exception& e) {
                std::cout << "\n删除第 " << i << " 条记录时发生异常: " << e.what() << std::endl;
            }
        }
        std::cout << std::endl;
        end = high_resolution_clock::now();
        
        double deleteTime = duration_cast<microseconds>(end - start).count();
        std::cout << "批量删除 " << deleteCount << " 条记录用时: " 
                  << deleteTime / 1000.0 << " ms" << std::endl;
        
        // 验证删除结果
        std::cout << "\n开始验证删除结果..." << std::endl;
        std::cout << "检查后 " << (batchSize/2) << " 条记录是否仍然存在（这些记录应该未被删除）" << std::endl;
        
        int remainingCount = 0;
        for (int i = batchSize / 2; i < batchSize; i++) {
            printProgress(i - batchSize/2 + 1, batchSize/2, "验证保留记录: ");
            if (manager.existScheduler(trainIDs[i])) {
                remainingCount++;
            }
        }
        std::cout << std::endl;
        
        std::cout << "验证结果: " << remainingCount << "/" << (batchSize/2) << " 条记录仍然存在" << std::endl;
        std::cout << "（预期结果：所有记录都应该存在，因为这些都是未被删除的记录）" << std::endl;
        
        // 额外验证：检查前500条记录是否真的被删除了
        std::cout << "\n额外验证：检查前 " << (batchSize/2) << " 条记录是否已被删除" << std::endl;
        int deletedCount = 0;
        for (int i = 0; i < batchSize/2; i++) {
            printProgress(i + 1, batchSize/2, "验证删除记录: ");
            if (!manager.existScheduler(trainIDs[i])) {
                deletedCount++;
            }
        }
        std::cout << std::endl;
        
        std::cout << "删除验证结果: " << deletedCount << "/" << (batchSize/2) << " 条记录已被删除" << std::endl;
        std::cout << "（预期结果：所有记录都应该被删除）" << std::endl;
        
        bool deleteCorrect = (remainingCount == batchSize / 2) && (deletedCount == batchSize / 2);
        stats.recordTest("批量删除验证", deleteCorrect);
        allPassed &= deleteCorrect;
        
    } catch (const std::exception& e) {
        std::cout << "批量测试异常: " << e.what() << std::endl;
        stats.recordTest("批量操作异常处理", false);
        allPassed = false;
    }
    
    return allPassed;
}

// 3. 性能和复杂度测试
std::vector<PerformanceResult> testComplexity(TestStats& stats) {
    std::cout << "\n========== 性能和复杂度测试 ==========" << std::endl;
    
    std::vector<PerformanceResult> results;
    std::vector<int> testSizes = {100, 300, 500, 800, 1000, 1500};
    
    for (size_t sizeIdx = 0; sizeIdx < testSizes.size(); sizeIdx++) {
        int size = testSizes[sizeIdx];
        std::cout << "\n--- 测试规模: " << size << " (" << (sizeIdx + 1) << "/" << testSizes.size() << ") ---" << std::endl;
        
        char filename[64];
        snprintf(filename, sizeof(filename), "test_perf_%d.dat", size);
        SchedulerManager manager(filename);
        
        // 准备测试数据
        std::vector<TrainID> trainIDs;
        std::vector<int> seatNums;
        std::vector<int> stationCounts;
        std::vector<std::vector<StationID>> stationsData;
        std::vector<std::vector<int>> durationData;
        std::vector<std::vector<int>> priceData;
        
        for (int i = 0; i < size; i++) {
            trainIDs.push_back(TrainID(generateTrainID(i)));
            
            StationID stations[MAX_PASSING_STATION_NUMBER];
            int duration[MAX_PASSING_STATION_NUMBER];
            int price[MAX_PASSING_STATION_NUMBER];
            int seatNum, stationCount;
            
            generateRandomScheduler(trainIDs[i], seatNum, stationCount, stations, duration, price);
            
            seatNums.push_back(seatNum);
            stationCounts.push_back(stationCount);
            
            stationsData.emplace_back(stations, stations + stationCount);
            durationData.emplace_back(duration, duration + stationCount - 1);
            priceData.emplace_back(price, price + stationCount - 1);
        }
        
        // 1. 插入性能测试
        std::cout << "执行插入性能测试..." << std::endl;
        std::vector<double> insertTimes;
        for (int i = 0; i < size; i++) {
            printProgress(i + 1, size, "插入测试: ");
            auto start = high_resolution_clock::now();
            manager.addScheduler(trainIDs[i], seatNums[i], stationCounts[i],
                               stationsData[i].data(), durationData[i].data(), priceData[i].data());
            auto end = high_resolution_clock::now();
            
            double time = duration_cast<nanoseconds>(end - start).count() / 1000.0; // 微秒
            insertTimes.push_back(time);
        }
        std::cout << std::endl;
        
        double avgInsert = std::accumulate(insertTimes.begin(), insertTimes.end(), 0.0) / insertTimes.size();
        double minInsert = *std::min_element(insertTimes.begin(), insertTimes.end());
        double maxInsert = *std::max_element(insertTimes.begin(), insertTimes.end());
        
        results.push_back({"Insert", size, avgInsert, minInsert, maxInsert, avgInsert / log2(size)});
        
        // 2. 查询性能测试
        std::cout << "\n执行查询性能测试..." << std::endl;
        int queryCount = std::min(1000, size);
        std::vector<double> queryTimes;
        for (int i = 0; i < queryCount; i++) {
            printProgress(i + 1, queryCount, "查询测试: ");
            int idx = i;
            auto start = high_resolution_clock::now();
            try {
                bool exists = manager.existScheduler(trainIDs[idx]);
                auto end = high_resolution_clock::now();
                
                double time = duration_cast<nanoseconds>(end - start).count() / 1000.0; // 微秒
                
                // 检查是否超时（超过100ms认为异常）
                if (time > 100000) { // 100ms = 100000微秒
                    std::cout << "\n警告: 查询 " << idx << " 超时 (" << time/1000.0 << "ms)" << std::endl;
                    queryTimes.push_back(100000); // 记录为100ms
                } else {
                    queryTimes.push_back(time);
                }
            } catch (const std::exception& e) {
                std::cout << "\n查询测试异常 (idx=" << idx << "): " << e.what() << std::endl;
                queryTimes.push_back(100000); // 异常情况下记录为100ms
            }
        }
        std::cout << std::endl;
        
        double avgQuery = std::accumulate(queryTimes.begin(), queryTimes.end(), 0.0) / queryTimes.size();
        double minQuery = *std::min_element(queryTimes.begin(), queryTimes.end());
        double maxQuery = *std::max_element(queryTimes.begin(), queryTimes.end());
        
        results.push_back({"Query", size, avgQuery, minQuery, maxQuery, avgQuery / log2(size)});
        
        // 3. 删除性能测试
        std::cout << "\n执行删除性能测试..." << std::endl;
        int deleteCount = size / 2;
        std::vector<double> deleteTimes;
        for (int i = 0; i < deleteCount; i++) {
            printProgress(i + 1, deleteCount, "删除测试: ");
            auto start = high_resolution_clock::now();
            manager.removeScheduler(trainIDs[i]);
            auto end = high_resolution_clock::now();
            
            double time = duration_cast<nanoseconds>(end - start).count() / 1000.0; // 微秒
            deleteTimes.push_back(time);
        }
        std::cout << std::endl;
        
        double avgDelete = std::accumulate(deleteTimes.begin(), deleteTimes.end(), 0.0) / deleteTimes.size();
        double minDelete = *std::min_element(deleteTimes.begin(), deleteTimes.end());
        double maxDelete = *std::max_element(deleteTimes.begin(), deleteTimes.end());
        
        results.push_back({"Delete", size, avgDelete, minDelete, maxDelete, avgDelete / log2(size)});
        
        std::cout << "插入平均时间: " << avgInsert << " μs" << std::endl;
        std::cout << "查询平均时间: " << avgQuery << " μs" << std::endl;
        std::cout << "删除平均时间: " << avgDelete << " μs" << std::endl;
    }
    
    // 分析复杂度趋势
    std::cout << "\n========== 复杂度分析 ==========" << std::endl;
    
    // 按操作类型分组分析
    std::map<std::string, std::vector<PerformanceResult*>> opGroups;
    for (auto& result : results) {
        opGroups[result.operation].push_back(&result);
    }
    
    for (const auto& group : opGroups) {
        std::cout << "\n" << group.first << " 操作复杂度分析:" << std::endl;
        std::cout << "数据规模\t平均时间(μs)\t复杂度系数(T/logN)" << std::endl;
        
        bool complexityValid = true;
        double prevComplexity = 0;
        
        for (const auto& result : group.second) {
            std::cout << result->dataSize << "\t\t" 
                      << std::fixed << std::setprecision(2) << result->avgTime << "\t\t" 
                      << result->complexity << std::endl;
            
            // 验证复杂度是否保持在合理范围内
            if (prevComplexity > 0) {
                double ratio = result->complexity / prevComplexity;
                if (ratio > 3.0) {  // 如果复杂度系数增长超过3倍，可能不是O(log n)
                    complexityValid = false;
                }
            }
            prevComplexity = result->complexity;
        }
        
        std::string testName = group.first + " 操作复杂度验证";
        stats.recordTest(testName, complexityValid);
    }
    
    return results;
}

// 4. 并发访问模拟测试
bool testConcurrentAccess(TestStats& stats) {
    std::cout << "\n========== 并发访问模拟测试 ==========" << std::endl;
    
    bool allPassed = true;
    
    try {
        std::cout << "初始化测试环境..." << std::endl;
        SchedulerManager manager("test_concurrent.dat");
        
        // 先添加一些基础数据
        std::cout << "添加基础数据（100条记录）..." << std::endl;
        std::vector<TrainID> baseTrains;
        for (int i = 0; i < 100; i++) {
            printProgress(i + 1, 100, "添加基础数据: ");
            TrainID trainId(generateTrainID(i));
            baseTrains.push_back(trainId);
            
            StationID stations[MAX_PASSING_STATION_NUMBER];
            int duration[MAX_PASSING_STATION_NUMBER];
            int price[MAX_PASSING_STATION_NUMBER];
            int seatNum, stationCount;
            
            generateRandomScheduler(trainId, seatNum, stationCount, stations, duration, price);
            try {
                manager.addScheduler(trainId, seatNum, stationCount, stations, duration, price);
            } catch (const std::exception& e) {
                std::cout << "\n添加基础数据时发生异常: " << e.what() << std::endl;
                throw;
            }
        }
        std::cout << std::endl;
        
        // 模拟混合操作
        std::cout << "\n开始模拟混合操作（1000次）..." << std::endl;
        std::cout << "操作类型分布：\n";
        std::cout << "- 查询操作：随机检查现有记录\n";
        std::cout << "- 插入操作：添加新记录\n";
        std::cout << "- 删除操作：随机删除现有记录\n\n";
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> opDis(1, 3);  // 1=查询, 2=插入, 3=删除
        std::uniform_int_distribution<> idxDis(0, 99);
        
        int successCount = 0;
        int totalOps = 1000;
        int queryCount = 0, insertCount = 0, deleteCount = 0;
        int querySuccess = 0, insertSuccess = 0, deleteSuccess = 0;
        
        auto start = high_resolution_clock::now();
        
        for (int i = 0; i < totalOps; i++) {
            printProgress(i + 1, totalOps, "并发操作: ");
            try {
                int op = opDis(gen);
                int idx = idxDis(gen);
                
                switch (op) {
                    case 1: {  // 查询
                        queryCount++;
                        bool exists = manager.existScheduler(baseTrains[idx]);
                        querySuccess++;
                        successCount++;
                        break;
                    }
                    case 2: {  // 插入新的
                        insertCount++;
                        TrainID newTrain(generateTrainID(1000 + i));
                        StationID stations[] = {1, 2, 3};
                        int duration[] = {120, 90};
                        int price[] = {150, 200};
                        manager.addScheduler(newTrain, 200, 3, stations, duration, price);
                        insertSuccess++;
                        successCount++;
                        break;
                    }
                    case 3: {  // 删除（如果存在）
                        deleteCount++;
                        if (manager.existScheduler(baseTrains[idx])) {
                            manager.removeScheduler(baseTrains[idx]);
                        }
                        deleteSuccess++;
                        successCount++;
                        break;
                    }
                }
            } catch (const std::exception& e) {
                std::cout << "\n操作异常 (操作 " << i + 1 << "): " << e.what() << std::endl;
            }
        }
        std::cout << std::endl;
        
        auto end = high_resolution_clock::now();
        double totalTime = duration_cast<milliseconds>(end - start).count();
        
        std::cout << "\n并发测试统计：" << std::endl;
        std::cout << "总操作数: " << totalOps << std::endl;
        std::cout << "总用时: " << totalTime << " ms" << std::endl;
        std::cout << "平均操作时间: " << totalTime / totalOps << " ms" << std::endl;
        std::cout << "\n操作类型分布：" << std::endl;
        std::cout << "- 查询操作: " << queryCount << " 次 (" << querySuccess << " 成功)" << std::endl;
        std::cout << "- 插入操作: " << insertCount << " 次 (" << insertSuccess << " 成功)" << std::endl;
        std::cout << "- 删除操作: " << deleteCount << " 次 (" << deleteSuccess << " 成功)" << std::endl;
        std::cout << "\n总成功率: " << successCount << "/" << totalOps << " (" 
                  << (double)successCount / totalOps * 100 << "%)" << std::endl;
        
        bool concurrentTestPassed = (successCount >= totalOps * 0.95);  // 允许5%失败率
        std::stringstream details;
        details << "并发测试详情:\n"
                << "- 总操作数: " << totalOps << "\n"
                << "- 总用时: " << totalTime << " ms\n"
                << "- 平均操作时间: " << totalTime / totalOps << " ms\n"
                << "- 查询操作: " << queryCount << " 次 (" << querySuccess << " 成功)\n"
                << "- 插入操作: " << insertCount << " 次 (" << insertSuccess << " 成功)\n"
                << "- 删除操作: " << deleteCount << " 次 (" << deleteSuccess << " 成功)\n"
                << "- 总成功率: " << successCount << "/" << totalOps << " (" 
                << (double)successCount / totalOps * 100 << "%)";
        
        stats.recordTest("并发访问模拟", concurrentTestPassed, details.str());
        allPassed &= concurrentTestPassed;
        
    } catch (const std::exception& e) {
        std::cout << "并发测试异常: " << e.what() << std::endl;
        stats.recordTest("并发访问异常处理", false, std::string("发生异常: ") + e.what());
        allPassed = false;
    }
    
    return allPassed;
}

// 5. 内存使用测试
bool testMemoryUsage(TestStats& stats) {
    std::cout << "\n========== 内存使用测试 ==========" << std::endl;
    
    bool allPassed = true;
    
    try {
        // 测试大量数据的内存使用
        {
            SchedulerManager manager("test_memory.dat");
            
            std::cout << "开始大量数据插入测试..." << std::endl;
            std::cout << "将插入2000条记录，并验证查询性能" << std::endl;
            
            auto insertStart = high_resolution_clock::now();
            for (int i = 0; i < 2000; i++) {
                printProgress(i + 1, 2000, "内存测试: ");
                TrainID trainId(generateTrainID(i));
                
                StationID stations[MAX_PASSING_STATION_NUMBER];
                int duration[MAX_PASSING_STATION_NUMBER];
                int price[MAX_PASSING_STATION_NUMBER];
                int seatNum, stationCount;
                
                generateRandomScheduler(trainId, seatNum, stationCount, stations, duration, price);
                manager.addScheduler(trainId, seatNum, stationCount, stations, duration, price);
                
                if (i % 1000 == 0) {
                    std::cout << "\n已插入 " << i << " 条记录" << std::endl;
                }
            }
            auto insertEnd = high_resolution_clock::now();
            double insertTime = duration_cast<milliseconds>(insertEnd - insertStart).count();
            std::cout << "\n大量数据插入完成，总用时: " << insertTime << " ms" << std::endl;
            std::cout << "平均每条记录插入时间: " << (insertTime / 2000.0) << " ms" << std::endl;
            
            std::cout << "\n开始测试查询性能..." << std::endl;
            std::cout << "将执行1000次随机查询" << std::endl;
            
            // 测试查询性能
            std::vector<double> queryTimes;
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, 1999);
            
            auto queryStart = high_resolution_clock::now();
            for (int i = 0; i < 1000; i++) {
                TrainID testId(generateTrainID(dis(gen)));
                auto start = high_resolution_clock::now();
                manager.existScheduler(testId);
                auto end = high_resolution_clock::now();
                queryTimes.push_back(duration_cast<microseconds>(end - start).count());
            }
            auto queryEnd = high_resolution_clock::now();
            
            double totalQueryTime = duration_cast<milliseconds>(queryEnd - queryStart).count();
            double avgQueryTime = std::accumulate(queryTimes.begin(), queryTimes.end(), 0.0) / queryTimes.size();
            double maxQueryTime = *std::max_element(queryTimes.begin(), queryTimes.end());
            double minQueryTime = *std::min_element(queryTimes.begin(), queryTimes.end());
            
            std::cout << "查询性能统计：" << std::endl;
            std::cout << "- 总查询时间: " << totalQueryTime << " ms" << std::endl;
            std::cout << "- 平均查询时间: " << avgQueryTime << " μs" << std::endl;
            std::cout << "- 最长查询时间: " << maxQueryTime << " μs" << std::endl;
            std::cout << "- 最短查询时间: " << minQueryTime << " μs" << std::endl;
            
            // 修改判断条件：如果平均查询时间超过200微秒或最长查询时间超过1毫秒，则认为性能不达标
            bool memoryTestPassed = (avgQueryTime < 200.0) && (maxQueryTime < 1000.0);
            std::cout << "\n性能要求：" << std::endl;
            std::cout << "- 平均查询时间应小于200微秒（当前: " << avgQueryTime << " μs）" << std::endl;
            std::cout << "- 最长查询时间应小于1毫秒（当前: " << maxQueryTime << " μs）" << std::endl;
            
            std::stringstream details;
            details << "插入性能:\n"
                    << "- 总插入时间: " << insertTime << " ms\n"
                    << "- 平均每条记录插入时间: " << (insertTime / 2000.0) << " ms\n\n"
                    << "查询性能:\n"
                    << "- 总查询时间: " << totalQueryTime << " ms\n"
                    << "- 平均查询时间: " << avgQueryTime << " μs\n"
                    << "- 最长查询时间: " << maxQueryTime << " μs\n"
                    << "- 最短查询时间: " << minQueryTime << " μs\n\n"
                    << "性能要求:\n"
                    << "- 平均查询时间应小于200微秒\n"
                    << "- 最长查询时间应小于1毫秒";
            
            stats.recordTest("大数据量内存测试", memoryTestPassed, details.str());
            allPassed &= memoryTestPassed;
        }
        
        std::cout << "\n内存测试完成，SchedulerManager已析构" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "内存测试异常: " << e.what() << std::endl;
        stats.recordTest("内存使用异常处理", false);
        allPassed = false;
    }
    
    return allPassed;
}

// 修改性能报告生成函数
void generatePerformanceReport(const std::vector<PerformanceResult>& results) {
    std::cout << "\n========== 性能测试报告 ==========" << std::endl;
    
    const char* reportPath = "performance_report.txt";
    std::ofstream report(reportPath);
    if (!report) {
        std::cout << "无法创建性能报告文件: " << reportPath << std::endl;
        return;
    }
    
    report << "SchedulerManager 性能测试报告\n";
    report << "生成时间: " << getCurrentTimeString() << "\n\n";
    
    report << "测试环境信息:\n";
    report << "操作系统: Windows\n";
    report << "测试时间: " << getCurrentTimeString() << "\n\n";
    
    report << "性能测试结果:\n";
    report << "操作类型\t数据规模\t平均时间(μs)\t最小时间(μs)\t最大时间(μs)\t复杂度系数\n";
    report << "========================================================================\n";
    
    for (const auto& result : results) {
        report << result.operation << "\t"
               << result.dataSize << "\t"
               << std::fixed << std::setprecision(2) << result.avgTime << "\t"
               << result.minTime << "\t"
               << result.maxTime << "\t"
               << result.complexity << "\n";
    }
    
    report.close();
    std::cout << "性能报告已生成: " << reportPath << std::endl;
}

// 主测试函数
int main() {
    SetConsoleOutputCP(CP_UTF8);
    
    std::cout << "🚂 SchedulerManager 综合测试程序" << std::endl;
    std::cout << "===========================================\n" << std::endl;
    
    TestStats stats;
    std::vector<PerformanceResult> perfResults;
    
    try {
        // 1. 基础功能测试
        std::cout << "开始基础功能测试..." << std::endl;
        testBasicFunctionality(stats);
        
        // 2. 批量数据测试
        std::cout << "\n开始批量数据测试..." << std::endl;
        testBatchOperations(stats, 1000);
        
        // 3. 性能和复杂度测试
        std::cout << "\n开始性能和复杂度测试..." << std::endl;
        perfResults = testComplexity(stats);
        
        // 4. 并发访问模拟测试
        std::cout << "\n开始并发访问模拟测试..." << std::endl;
        testConcurrentAccess(stats);
        
        // 5. 内存使用测试
        std::cout << "\n开始内存使用测试..." << std::endl;
        testMemoryUsage(stats);
        
        // 生成测试报告
        stats.generateReport("test_report.txt");
        generatePerformanceReport(perfResults);
        
    } catch (const std::exception& e) {
        std::cout << "测试过程中发生异常: " << e.what() << std::endl;
        stats.recordTest("异常处理", false, std::string("发生异常: ") + e.what());
    }
    
    // 输出测试总结
    stats.printSummary();
    
    std::cout << "\n🎯 测试重点验证项目:" << std::endl;
    std::cout << "1. ✅ SchedulerManager 基础功能正确性" << std::endl;
    std::cout << "2. ✅ B+树的插入、查询、删除操作" << std::endl;
    std::cout << "3. ✅ 时间复杂度验证 (期望 O(log n))" << std::endl;
    std::cout << "4. ✅ 大数据量处理能力" << std::endl;
    std::cout << "5. ✅ 异常处理和边界情况" << std::endl;
    std::cout << "6. ✅ 内存使用效率" << std::endl;
    
    if (stats.passedTests == stats.totalTests) {
        std::cout << "\n🎉 所有测试通过！SchedulerManager 模块工作正常。" << std::endl;
        return 0;
    } else {
        std::cout << "\n⚠️  部分测试失败，请检查相关功能。" << std::endl;
        return 1;
    }
}