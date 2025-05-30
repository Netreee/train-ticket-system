# SchedulerManager 和 TrainScheduler 模块测试

## 概述

这个测试套件全面验证了 `SchedulerManager` 和 `TrainScheduler` 两个核心模块的功能正确性。

## 被测试的模块

### TrainScheduler
- **功能**: 管理单个列车的调度信息
- **主要特性**:
  - 站点管理（增加、插入、删除、查找）
  - 运行时间和票价设置
  - 数据验证和异常处理
  - 操作符重载

### SchedulerManager
- **功能**: 管理多个列车调度器的集合
- **主要特性**:
  - 基于B+树的高效存储
  - CRUD操作（创建、读取、更新、删除）
  - 持久化存储
  - 参数验证

## 测试覆盖范围

### 1. TrainScheduler 基础功能测试
- ✅ 初始状态验证
- ✅ 列车ID和座位数设置
- ✅ 基本getter/setter方法

### 2. TrainScheduler 站点管理测试
- ✅ 添加站点
- ✅ 插入站点（指定位置）
- ✅ 删除站点
- ✅ 查找站点
- ✅ 站点数量管理

### 3. TrainScheduler 时间价格测试
- ✅ 运行时间设置和获取
- ✅ 票价设置和获取
- ✅ 批量获取功能
- ✅ 站点间关系正确性

### 4. TrainScheduler 异常处理测试
- ✅ 无效索引异常
- ✅ 超出站点数量限制异常
- ✅ 空列表操作异常
- ✅ 边界条件处理

### 5. TrainScheduler 操作符测试
- ✅ 相等性比较 (`==`, `!=`)
- ✅ 大小比较 (`<`)
- ✅ 基于trainID的比较逻辑

### 6. SchedulerManager 基础功能测试
- ✅ 调度器添加
- ✅ 存在性检查
- ✅ 调度器获取
- ✅ 数据一致性验证

### 7. SchedulerManager 异常处理测试
- ✅ 无效参数验证
- ✅ 空指针检查
- ✅ 站点数量限制
- ✅ 座位数验证

### 8. SchedulerManager CRUD操作测试
- ✅ 创建调度器
- ✅ 读取调度器
- ✅ 更新调度器（覆盖）
- ✅ 删除调度器
- ✅ 安全删除（不存在的项）

### 9. 综合场景测试
- ✅ 多线路管理
- ✅ 复杂数据验证
- ✅ 实际使用场景模拟
- ✅ 数据完整性检查

## 运行测试

### 方法一：使用批处理脚本（推荐）
```bash
cd test
run_scheduler_test.bat
```

### 方法二：手动编译运行
```bash
cd test
g++ -std=c++17 -Wall -Wextra -I.. -I../DataStructure test_scheduler.cpp ../SchedulerManager.cpp ../TrainScheduler.cpp ../DateTime.cpp -o test_scheduler.exe
test_scheduler.exe
```

## 测试依赖

- C++17 兼容编译器 (推荐 g++ 7.0+)
- 以下源文件:
  - `SchedulerManager.h` / `SchedulerManager.cpp`
  - `TrainScheduler.h` / `TrainScheduler.cpp`
  - `Utils.h`
  - `DateTime.h` / `DateTime.cpp`
  - `DataStructure/BPlusTree.h`

## 测试结果解读

### 成功输出示例
```
🚂 SchedulerManager和TrainScheduler模块测试
================================================

=== 测试案例: TrainScheduler基础功能 ===
✅ 通过: 初始站点数量为0
✅ 通过: 初始座位数为0
...

================================================
测试完成！
通过: 9/9
🎉 所有测试都通过了！
```

### 失败输出示例
```
=== 测试案例: TrainScheduler基础功能 ===
❌ 失败: 初始站点数量为0
...
```

## 代码质量分析

### ✅ 发现的优点
1. **清晰的接口设计**: 两个类的API设计直观易用
2. **良好的异常处理**: 大部分边界情况都有适当的异常抛出
3. **合理的数据结构**: 使用B+树提供高效的查找性能
4. **完整的功能**: 覆盖了列车调度管理的核心需求

### ⚠️ 潜在改进点
1. **SchedulerManager::getScheduler**: 当trainID不存在时的行为需要更明确的定义
2. **边界检查**: 某些方法可以增加更多的参数验证
3. **文档**: 可以增加更多的使用示例

## 性能特征

- **时间复杂度**: 
  - SchedulerManager查找: O(log n)
  - TrainScheduler站点操作: O(n) where n ≤ 30
- **空间复杂度**: O(n) 其中n是调度器数量
- **最大站点数**: 30个站点/列车

## 故障排除

### 编译错误
1. 确保所有头文件路径正确
2. 检查C++17支持
3. 验证所有依赖文件存在

### 运行时错误
1. 检查文件写入权限
2. 确保有足够的磁盘空间存储测试数据文件
3. 验证数据结构实现（BPlusTree等）

## 扩展测试

如需添加更多测试用例，可以在 `test_scheduler.cpp` 中：

1. 添加新的测试函数
2. 在 `main()` 函数中调用新测试
3. 使用现有的测试宏进行断言

```cpp
bool testMyNewFeature() {
    TEST_CASE("我的新功能测试");
    
    // 测试代码
    ASSERT_TRUE(condition, "描述");
    
    testsPassed++;
    return true;
}
``` 