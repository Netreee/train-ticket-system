#include <iostream>
#include <cassert>
#include <stdexcept>
#include <sstream>
#include "../SchedulerManager.h"
#include "../TrainScheduler.h"
#include "../Utils.h"
#include <windows.h>
using namespace trainsys;

// 测试计数器
int testsTotal = 0; 
int testsPassed = 0;



// 测试辅助宏
#define TEST_CASE(name) \
    std::cout << "\n=== 测试案例: " << name << " ===" << std::endl; \
    testsTotal++;

#define ASSERT_TRUE(condition, message) \
    if (!(condition)) { \
        std::cout << "❌ 失败: " << message << std::endl; \
        return false; \
    } else { \
        std::cout << "✅ 通过: " << message << std::endl; \
    }

#define ASSERT_FALSE(condition, message) \
    if (condition) { \
        std::cout << "❌ 失败: " << message << std::endl; \
        return false; \
    } else { \
        std::cout << "✅ 通过: " << message << std::endl; \
    }

#define ASSERT_EXCEPTION(code, exception_type, message) \
    try { \
        code; \
        std::cout << "❌ 失败: " << message << " (未抛出预期异常)" << std::endl; \
        return false; \
    } catch (const exception_type& e) { \
        std::cout << "✅ 通过: " << message << " (正确抛出异常: " << e.what() << ")" << std::endl; \
    } catch (...) { \
        std::cout << "❌ 失败: " << message << " (抛出了错误类型的异常)" << std::endl; \
        return false; \
    }

// TrainScheduler基础功能测试
bool testTrainSchedulerBasic() {
    TEST_CASE("TrainScheduler基础功能");
    
    TrainScheduler scheduler;
    
    // 测试初始状态
    ASSERT_TRUE(scheduler.getPassingStationNum() == 0, "初始站点数量为0");
    ASSERT_TRUE(scheduler.getSeatNum() == 0, "初始座位数为0");
    
    // 测试设置列车ID和座位数
    TrainID trainId("T001");
    scheduler.setTrainID(trainId);
    scheduler.setSeatNumber(200);
    
    ASSERT_TRUE(scheduler.getTrainID() == trainId, "设置并获取列车ID");
    ASSERT_TRUE(scheduler.getSeatNum() == 200, "设置并获取座位数");
    
    testsPassed++;
    return true;
}

// TrainScheduler站点管理测试
bool testTrainSchedulerStations() {
    TEST_CASE("TrainScheduler站点管理");
    
    TrainScheduler scheduler;
    
    // 添加站点
    scheduler.addStation(1);  // 北京
    scheduler.addStation(2);  // 上海  
    scheduler.addStation(3);  // 广州
    
    ASSERT_TRUE(scheduler.getPassingStationNum() == 3, "添加3个站点后站点数量正确");
    ASSERT_TRUE(scheduler.getStation(0) == 1, "第一个站点正确");
    ASSERT_TRUE(scheduler.getStation(1) == 2, "第二个站点正确");
    ASSERT_TRUE(scheduler.getStation(2) == 3, "第三个站点正确");
    
    // 测试查找站点
    ASSERT_TRUE(scheduler.findStation(2) == 1, "查找站点2，索引为1");
    ASSERT_TRUE(scheduler.findStation(999) == -1, "查找不存在的站点返回-1");
    
    // 测试插入站点
    scheduler.insertStation(1, 4);  // 在位置1插入站点4
    ASSERT_TRUE(scheduler.getPassingStationNum() == 4, "插入站点后数量正确");
    ASSERT_TRUE(scheduler.getStation(1) == 4, "插入的站点位置正确");
    ASSERT_TRUE(scheduler.getStation(2) == 2, "原有站点位置正确移动");
    
    // 测试删除站点
    scheduler.removeStation(1);  // 删除位置1的站点
    ASSERT_TRUE(scheduler.getPassingStationNum() == 3, "删除站点后数量正确");
    ASSERT_TRUE(scheduler.getStation(1) == 2, "删除后站点位置正确");
    
    testsPassed++;
    return true;
}

// TrainScheduler时间和价格测试
bool testTrainSchedulerTimePrice() {
    TEST_CASE("TrainScheduler时间和价格设置");
    
    TrainScheduler scheduler;
    
    // 添加3个站点
    scheduler.addStation(1);
    scheduler.addStation(2);
    scheduler.addStation(3);
    
    // 设置运行时间（站点间）
    int duration[] = {120, 90};  // 1->2: 120分钟, 2->3: 90分钟
    scheduler.setDuration(duration);
    
    // 设置票价（站点间）
    int price[] = {150, 200};    // 1->2: 150元, 2->3: 200元
    scheduler.setPrice(price);
    
    ASSERT_TRUE(scheduler.getDuration(0) == 120, "第一段运行时间正确");
    ASSERT_TRUE(scheduler.getDuration(1) == 90, "第二段运行时间正确");
    ASSERT_TRUE(scheduler.getPrice(0) == 150, "第一段票价正确");
    ASSERT_TRUE(scheduler.getPrice(1) == 200, "第二段票价正确");
    
    // 测试批量获取
    int getDurationArray[3], getPriceArray[3];
    scheduler.getDuration(getDurationArray);
    scheduler.getPrice(getPriceArray);
    
    ASSERT_TRUE(getDurationArray[0] == 120, "批量获取运行时间正确");
    ASSERT_TRUE(getPriceArray[0] == 150, "批量获取票价正确");
    
    testsPassed++;
    return true;
}

// TrainScheduler异常情况测试
bool testTrainSchedulerExceptions() {
    TEST_CASE("TrainScheduler异常情况");
    
    TrainScheduler scheduler;
    
    // 测试无效索引异常
    ASSERT_EXCEPTION(
        scheduler.getStation(-1),
        std::runtime_error,
        "负数索引应抛出异常"
    );
    
    ASSERT_EXCEPTION(
        scheduler.getStation(0),
        std::runtime_error,
        "超出范围索引应抛出异常"
    );
    
    ASSERT_EXCEPTION(
        scheduler.removeStation(0),
        std::runtime_error,
        "删除空列表中的站点应抛出异常"
    );
    
    ASSERT_EXCEPTION(
        scheduler.insertStation(-1, 1),
        std::runtime_error,
        "无效插入位置应抛出异常"
    );
    
    // 测试站点数量限制
    TrainScheduler fullScheduler;
    for (int i = 0; i < MAX_PASSING_STATION_NUMBER; i++) {
        fullScheduler.addStation(i);
    }
    
    ASSERT_EXCEPTION(
        fullScheduler.addStation(999),
        std::runtime_error,
        "超过最大站点数量应抛出异常"
    );
    
    testsPassed++;
    return true;
}

// TrainScheduler操作符测试
bool testTrainSchedulerOperators() {
    TEST_CASE("TrainScheduler操作符");
    
    TrainScheduler scheduler1, scheduler2;
    TrainID trainId1("T001"), trainId2("T002");
    
    scheduler1.setTrainID(trainId1);
    scheduler2.setTrainID(trainId2);
    
    ASSERT_FALSE(scheduler1 == scheduler2, "不同ID的调度器不相等");
    ASSERT_TRUE(scheduler1 != scheduler2, "不同ID的调度器不等操作符正确");
    ASSERT_TRUE(scheduler1 < scheduler2, "字典序比较正确");
    
    scheduler2.setTrainID(trainId1);
    ASSERT_TRUE(scheduler1 == scheduler2, "相同ID的调度器相等");
    
    testsPassed++;
    return true;
}

// SchedulerManager基础功能测试
bool testSchedulerManagerBasic() {
    TEST_CASE("SchedulerManager基础功能");
    
    SchedulerManager manager("test_scheduler.dat");
    
    // 测试添加调度信息
    TrainID trainId("T001");
    StationID stations[] = {1, 2, 3};
    int duration[] = {120, 90};
    int price[] = {150, 200};
    
    manager.addScheduler(trainId, 200, 3, stations, duration, price);
    
    // 测试存在性检查
    ASSERT_TRUE(manager.existScheduler(trainId), "添加后调度器存在");
    
    TrainID nonExistId("T999");
    ASSERT_FALSE(manager.existScheduler(nonExistId), "不存在的调度器");
    
    // 测试获取调度器
    TrainScheduler retrieved = manager.getScheduler(trainId);
    ASSERT_TRUE(retrieved.getTrainID() == trainId, "获取的调度器ID正确");
    ASSERT_TRUE(retrieved.getSeatNum() == 200, "获取的座位数正确");
    ASSERT_TRUE(retrieved.getPassingStationNum() == 3, "获取的站点数正确");
    
    testsPassed++;
    return true;
}

// SchedulerManager异常测试
bool testSchedulerManagerExceptions() {
    TEST_CASE("SchedulerManager异常处理");
    
    SchedulerManager manager("test_scheduler_ex.dat");
    
    TrainID trainId("T001");
    StationID stations[] = {1, 2, 3};
    int duration[] = {120, 90};
    int price[] = {150, 200};
    
    // 测试无效参数
    ASSERT_EXCEPTION(
        manager.addScheduler(trainId, 0, 3, stations, duration, price),
        std::invalid_argument,
        "座位数为0应抛出异常"
    );
    
    ASSERT_EXCEPTION(
        manager.addScheduler(trainId, 200, 0, stations, duration, price),
        std::invalid_argument,
        "站点数为0应抛出异常"
    );
    
    ASSERT_EXCEPTION(
        manager.addScheduler(trainId, 200, MAX_PASSING_STATION_NUMBER + 1, stations, duration, price),
        std::invalid_argument,
        "超过最大站点数应抛出异常"
    );
    
    ASSERT_EXCEPTION(
        manager.addScheduler(trainId, 200, 3, nullptr, duration, price),
        std::invalid_argument,
        "空指针参数应抛出异常"
    );
    
    testsPassed++;
    return true;
}

// SchedulerManager删除和更新测试
bool testSchedulerManagerCRUD() {
    TEST_CASE("SchedulerManager CRUD操作");
    
    SchedulerManager manager("test_scheduler_crud.dat");
    
    TrainID trainId("T001");
    StationID stations1[] = {1, 2, 3};
    StationID stations2[] = {4, 5, 6, 7};
    int duration1[] = {120, 90};
    int duration2[] = {100, 80, 110};
    int price1[] = {150, 200};
    int price2[] = {180, 220, 160};
    
    // 添加第一个调度
    manager.addScheduler(trainId, 200, 3, stations1, duration1, price1);
    ASSERT_TRUE(manager.existScheduler(trainId), "添加第一个调度成功");
    
    TrainScheduler first = manager.getScheduler(trainId);
    ASSERT_TRUE(first.getPassingStationNum() == 3, "第一个调度站点数正确");
    
    // 更新调度（用新的覆盖旧的） 
    manager.addScheduler(trainId, 300, 4, stations2, duration2, price2);
    ASSERT_TRUE(manager.existScheduler(trainId), "更新后调度仍存在");
    
    TrainScheduler updated = manager.getScheduler(trainId);
    ASSERT_TRUE(updated.getSeatNum() == 300, "更新后座位数正确");
    ASSERT_TRUE(updated.getPassingStationNum() == 4, "更新后站点数正确");
    
    // 删除调度
    manager.removeScheduler(trainId);
    ASSERT_FALSE(manager.existScheduler(trainId), "删除后调度不存在");
    
    // 删除不存在的调度（应该不抛出异常）
    manager.removeScheduler(trainId);  // 这应该安全执行
    
    testsPassed++;
    return true;
}

// 综合功能测试
bool testIntegratedScenario() {
    TEST_CASE("综合场景测试");
    
    SchedulerManager manager("test_integrated.dat");
    
    // 创建多条线路
    struct TrainRoute {
        const char* trainId;
        int seatNum;
        int stationCount;
        StationID stations[5];
        int duration[4];
        int price[4];
    };
    
    TrainRoute routes[] = {
        {"G001", 300, 3, {1, 2, 3}, {120, 90}, {150, 200}},
        {"G002", 250, 4, {1, 4, 5, 6}, {100, 80, 110}, {180, 220, 160}},
        {"D001", 200, 5, {2, 7, 8, 9, 10}, {90, 75, 85, 95}, {120, 140, 130, 110}}
    };
    
    // 添加所有线路
    for (int i = 0; i < 3; i++) {
        TrainID id(routes[i].trainId);
        manager.addScheduler(id, routes[i].seatNum, routes[i].stationCount,
                           routes[i].stations, routes[i].duration, routes[i].price);
        
        ASSERT_TRUE(manager.existScheduler(id), 
                   std::string("线路 ") + routes[i].trainId + " 添加成功");
    }
    
    // 验证所有线路都存在
    for (int i = 0; i < 3; i++) {
        TrainID id(routes[i].trainId);
        TrainScheduler scheduler = manager.getScheduler(id);
        
        ASSERT_TRUE(scheduler.getSeatNum() == routes[i].seatNum,
                   std::string("线路 ") + routes[i].trainId + " 座位数正确");
        ASSERT_TRUE(scheduler.getPassingStationNum() == routes[i].stationCount,
                   std::string("线路 ") + routes[i].trainId + " 站点数正确");
    }
    
    // 输出线路信息
    std::cout << "\n--- 所有线路信息 ---" << std::endl;
    for (int i = 0; i < 3; i++) {
        TrainID id(routes[i].trainId);
        TrainScheduler scheduler = manager.getScheduler(id);
        std::cout << scheduler << std::endl;
    }
    
    testsPassed++;
    return true;
}

// 主测试函数
int main() {
    SetConsoleOutputCP(CP_UTF8);
    std::cout << "🚂 SchedulerManager和TrainScheduler模块测试" << std::endl;
    std::cout << "================================================" << std::endl;
    
    // 运行所有测试
    bool allPassed = true;
    
    allPassed &= testTrainSchedulerBasic();
    allPassed &= testTrainSchedulerStations();
    allPassed &= testTrainSchedulerTimePrice();
    allPassed &= testTrainSchedulerExceptions();
    allPassed &= testTrainSchedulerOperators();
    allPassed &= testSchedulerManagerBasic();
    allPassed &= testSchedulerManagerExceptions();
    allPassed &= testSchedulerManagerCRUD();
    allPassed &= testIntegratedScenario();
    
    // 输出测试结果
    std::cout << "\n================================================" << std::endl;
    std::cout << "测试完成！" << std::endl;
    std::cout << "通过: " << testsPassed << "/" << testsTotal << std::endl;
    
    if (allPassed && testsPassed == testsTotal) {
        std::cout << "🎉 所有测试都通过了！" << std::endl;
        return 0;
    } else {
        std::cout << "❌ 有测试失败" << std::endl;
        return 1;
    }
} 