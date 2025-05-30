#include <iostream>
#include <string>
#include "../DataStructure/BPlusTree.h"
#include "../DataStructure/List.h"
#include <windows.h>
using namespace trainsys;

template<typename T>
void printResults(const seqList<T>& results) { 
    std::cout << "查询结果: ";
    for (int i = 0; i < results.length(); i++) {
        std::cout << results.visit(i) << " ";
    }
    std::cout << std::endl;
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    // 测试1：基本增删改查功能
    {
        BPlusTree<std::string, std::string> bpt("test_basic");
        
        // 插入测试
        bpt.insert("apple", "fruit");
        bpt.insert("banana", "fruit");
        bpt.insert("cherry", "vegetable"); // 故意不同值测试多值情况
        bpt.insert("date", "fruit");
        
        std::cout << "=== 基本功能测试 ===" << std::endl;
        std::cout << "当前数据量: " << bpt.size() << std::endl; // 应输出4
        
        // 查找存在的键
        auto fruits = bpt.find("apple");
        printResults(fruits); // 应输出fruit
        
        // 查找多值键（虽然示例中可能唯一，但代码支持多值）
        auto multiCheck = bpt.find("cherry");
        printResults(multiCheck); // 应输出vegetable
        
        // 查找不存在的键
        if (bpt.find("grape").empty()) {
            std::cout << "查找不存在的键测试通过" << std::endl;
        }
        
        // 删除测试
        bpt.remove("banana", "fruit");
        if (bpt.size() == 3 && !bpt.contains("banana")) {
            std::cout << "单值删除测试通过" << std::endl;
        }
        
        // 修改测试
        bpt.modify("cherry", "vegetable", "fruit");
        auto modified = bpt.find("cherry");
        if (modified.visit(0) == "fruit") {
            std::cout << "修改测试通过" << std::endl;
        }
        
        // 清空测试
        bpt.clear();
        if (bpt.size() == 0) {
            std::cout << "清空测试通过" << std::endl;
        }
    }

    // 测试2：边界条件测试
    {
        const int TEST_SIZE = 105; // 超过叶子节点容量(L=100)
        BPlusTree<std::string, int> bpt("test_boundary");
        
        // 插入100个元素（刚好填满叶子节点）
        for (int i = 0; i < 100; i++) {
            std::string key = "key_" + std::to_string(i);
            bpt.insert(key, i);
        }
        
        // 插入第101个元素，触发叶子节点分裂
        bpt.insert("key_100", 100);
        if (bpt.size() == 101) {
            std::cout << "\n=== 边界测试（分裂） ===" << std::endl;
            std::cout << "分裂后数据量正确" << std::endl;
        }
        
        // 删除元素直到触发节点合并
        for (int i = 0; i < 50; i++) {
            std::string key = "key_" + std::to_string(i);
            bpt.remove(key, i);
        }
        if (bpt.size() == 51) { // 101-50=51
            std::cout << "合并后数据量正确" << std::endl;
        }
    }

    // 测试3：复杂查询与唯一性测试
    {
        BPlusTree<std::string, std::string> bpt("test_complex");
        
        // 插入重复键（测试多值存储，虽然业务上可能唯一）
        bpt.insert("orange", "sweet");
        bpt.insert("orange", "sour"); // 允许相同键不同值
        
        std::cout << "\n=== 复杂查询测试 ===" << std::endl;
        auto oranges = bpt.find("orange");
        std::cout << "多值查询结果数量: " << oranges.length() << std::endl; // 应输出2
        
        // 删除其中一个值
        bpt.remove("orange", "sweet");
        oranges = bpt.find("orange");
        if (oranges.length() == 1 && oranges.visit(0) == "sour") {
            std::cout << "部分删除测试通过" << std::endl;
        }
        
        // 全删除
        bpt.remove("orange", "sour");
        if (!bpt.contains("orange")) {
            std::cout << "完全删除测试通过" << std::endl;
        }
    }

    std::cout << "\n所有测试完成！" << std::endl;
    return 0;
}