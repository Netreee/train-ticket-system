#include <iostream>
#include <string>
#include <cassert>
#include <vector>
#include <algorithm>
#include <random>
#include <filesystem>
#include <Windows.h>
#include "DataStructure/BPlusTree.h"

using namespace trainsys;
using namespace std;

// 测试用的简单类型
void testBasicOperations() {
    cout << "=== 测试基本操作 ===" << endl;
    
    // 清理之前可能存在的测试文件
    try {
        if (std::filesystem::exists("test_basic_treeNodeFile")) {
            std::filesystem::remove("test_basic_treeNodeFile");
        }
        if (std::filesystem::exists("test_basic_leafFile")) {
            std::filesystem::remove("test_basic_leafFile");
        }
    } catch (const std::exception& e) {
        cout << "清理文件时出错: " << e.what() << endl;
    }
    
    try {
        BPlusTree<int, string> tree("test_basic");
        
        // 测试初始状态
        assert(tree.size() == 0);
        cout << "✓ 初始大小为0" << endl;
        
        // 测试插入
        tree.insert(1, "value1");
        tree.insert(2, "value2");
        tree.insert(3, "value3");
        assert(tree.size() == 3);
        cout << "✓ 插入3个元素后大小为3" << endl;
        
        // 测试查找
        auto result = tree.find(1);
        assert(!result.empty() && result.visit(0) == "value1");
        cout << "✓ 查找key=1成功" << endl;
        
        result = tree.find(2);
        assert(!result.empty() && result.visit(0) == "value2");
        cout << "✓ 查找key=2成功" << endl;
        
        // 测试contains
        assert(tree.contains(1));
        assert(tree.contains(2));
        assert(tree.contains(3));
        assert(!tree.contains(4));
        cout << "✓ contains功能正常" << endl;
        
        // 测试findFirst
        assert(tree.findFirst(1) == "value1");
        assert(tree.findFirst(2) == "value2");
        cout << "✓ findFirst功能正常" << endl;
    } catch (const std::exception& e) {
        cout << "测试过程中出错: " << e.what() << endl;
        throw;
    }
}
void testMultipleValues() {
    cout << "\n=== 测试相同键的多个值 ===" << endl;
    
    try {
        std::filesystem::remove("test_multi_treeNodeFile");
        std::filesystem::remove("test_multi_leafFile");
    } catch (...) {}
    
    BPlusTree<int, string> tree("test_multi");
    
    // 为同一个键插入多个值
    tree.insert(1, "value1_1");
    tree.insert(1, "value1_2");
    tree.insert(1, "value1_3");
    
    assert(tree.size() == 3);
    cout << "✓ 同一键插入3个值" << endl;
    
    auto result = tree.find(1);
    assert(result.length() == 3);
    cout << "✓ 找到3个值" << endl;
    
    // 验证所有值都存在
    bool found1 = false, found2 = false, found3 = false;
    for (int i = 0; i < result.length(); i++) {
        string val = result.visit(i);
        if (val == "value1_1") found1 = true;
        if (val == "value1_2") found2 = true;
        if (val == "value1_3") found3 = true;
    }
    assert(found1 && found2 && found3);
    cout << "✓ 所有值都能找到" << endl;
}

void testRemoveOperations() {
    cout << "\n=== 测试删除操作 ===" << endl;
    
    try {
        std::filesystem::remove("test_remove_treeNodeFile");
        std::filesystem::remove("test_remove_leafFile");
    } catch (...) {}
    
    BPlusTree<int, string> tree("test_remove");
    
    // 插入测试数据
    tree.insert(1, "value1");
    tree.insert(2, "value2");
    tree.insert(3, "value3");
    tree.insert(1, "value1_2");
    
    assert(tree.size() == 4);
    cout << "✓ 插入4个元素" << endl;
    
    // 测试删除特定值
    tree.remove(1, "value1");
    assert(tree.size() == 3);
    auto result = tree.find(1);
    assert(result.length() == 1 && result.visit(0) == "value1_2");
    cout << "✓ 删除特定值成功" << endl;
    
    // 测试removeFirst
    tree.removeFirst(1);
    assert(tree.size() == 2);
    assert(!tree.contains(1));
    cout << "✓ removeFirst功能正常" << endl;
    
    // 验证其他元素仍存在
    assert(tree.contains(2));
    assert(tree.contains(3));
    cout << "✓ 其他元素未受影响" << endl;
}

void testModifyOperation() {
    cout << "\n=== 测试修改操作 ===" << endl;
    
    try {
        std::filesystem::remove("test_modify_treeNodeFile");
        std::filesystem::remove("test_modify_leafFile");
    } catch (...) {}
    
    BPlusTree<int, string> tree("test_modify");
    
    tree.insert(1, "oldValue");
    tree.insert(2, "value2");
    
    // 测试修改
    tree.modify(1, "oldValue", "newValue");
    
    auto result = tree.find(1);
    assert(!result.empty() && result.visit(0) == "newValue");
    assert(tree.size() == 2); // 大小应该保持不变
    cout << "✓ 修改操作成功" << endl;
}

void testLargeDataSet() {
    cout << "\n=== 测试大数据集 ===" << endl;
    
    try {
        std::filesystem::remove("test_large_treeNodeFile");
        std::filesystem::remove("test_large_leafFile");
    } catch (...) {}
    
    BPlusTree<int, int> tree("test_large");
    
    const int testSize = 1000;
    vector<int> keys;
    
    // 生成随机键值
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, testSize * 2);
    
    for (int i = 0; i < testSize; i++) {
        int key = dis(gen);
        keys.push_back(key);
        tree.insert(key, i);
    }
    
    assert(tree.size() == testSize);
    cout << "✓ 插入" << testSize << "个元素" << endl;
    
    // 验证所有插入的元素都能找到
    int foundCount = 0;
    for (int i = 0; i < testSize; i++) {
        if (tree.contains(keys[i])) {
            foundCount++;
        }
    }
    assert(foundCount == testSize);
    cout << "✓ 所有元素都能找到" << endl;
    
    // 删除一半元素
    for (int i = 0; i < testSize / 2; i++) {
        auto result = tree.find(keys[i]);
        if (!result.empty()) {
            tree.remove(keys[i], result.visit(0));
        }
    }
    
    cout << "✓ 删除操作完成，剩余元素数量：" << tree.size() << endl;
}

void testStringKeys() {
    cout << "\n=== 测试字符串键 ===" << endl;
    
    try {
        std::filesystem::remove("test_string_treeNodeFile");
        std::filesystem::remove("test_string_leafFile");
    } catch (...) {}
    
    BPlusTree<string, int> tree("test_string");
    
    tree.insert("apple", 1);
    tree.insert("banana", 2);
    tree.insert("cherry", 3);
    tree.insert("date", 4);
    
    assert(tree.contains("apple"));
    assert(tree.contains("banana"));
    assert(tree.contains("cherry"));
    assert(tree.contains("date"));
    assert(!tree.contains("elderberry"));
    
    assert(tree.findFirst("apple") == 1);
    assert(tree.findFirst("banana") == 2);
    
    cout << "✓ 字符串键功能正常" << endl;
}

void testClearOperation() {
    cout << "\n=== 测试清空操作 ===" << endl;
    
    try {
        std::filesystem::remove("test_clear_treeNodeFile");
        std::filesystem::remove("test_clear_leafFile");
    } catch (...) {}
    
    BPlusTree<int, string> tree("test_clear");
    
    // 插入一些数据
    for (int i = 1; i <= 10; i++) {
        tree.insert(i, "value" + to_string(i));
    }
    
    assert(tree.size() == 10);
    cout << "✓ 插入10个元素" << endl;
    
    // 清空
    tree.clear();
    assert(tree.size() == 0);
    
    // 验证所有元素都被删除
    for (int i = 1; i <= 10; i++) {
        assert(!tree.contains(i));
    }
    
    cout << "✓ 清空操作成功" << endl;
    
    // 清空后应该还能正常插入
    tree.insert(100, "newValue");
    assert(tree.size() == 1);
    assert(tree.contains(100));
    cout << "✓ 清空后可以正常插入新数据" << endl;
}

void testEdgeCases() {
    cout << "\n=== 测试边界情况 ===" << endl;
    
    try {
        std::filesystem::remove("test_edge_treeNodeFile");
        std::filesystem::remove("test_edge_leafFile");
    } catch (...) {}
    
    BPlusTree<int, string> tree("test_edge");
    
    // 测试空树查找
    auto result = tree.find(1);
    assert(result.empty());
    assert(!tree.contains(1));
    cout << "✓ 空树查找返回空结果" << endl;
    
    // 测试删除不存在的元素
    tree.remove(999, "nonexistent");
    assert(tree.size() == 0);
    cout << "✓ 删除不存在元素不影响树" << endl;
    
    // 测试单个元素
    tree.insert(1, "single");
    assert(tree.size() == 1);
    assert(tree.contains(1));
    
    tree.remove(1, "single");
    assert(tree.size() == 0);
    assert(!tree.contains(1));
    cout << "✓ 单元素插入删除正常" << endl;
}

void testPersistence() {
    cout << "\n=== 测试持久化 ===" << endl;
    
    try {
        std::filesystem::remove("test_persist_treeNodeFile");
        std::filesystem::remove("test_persist_leafFile");
    } catch (...) {}
    
    // 创建树并插入数据
    {
        BPlusTree<int, string> tree("test_persist");
        tree.insert(1, "persistent1");
        tree.insert(2, "persistent2");
        tree.insert(3, "persistent3");
        cout << "✓ 第一次创建树并插入数据" << endl;
    } // 树析构，数据应该被保存
    
    // 重新创建树，数据应该被恢复
    {
        BPlusTree<int, string> tree("test_persist");
        assert(tree.size() == 3);
        assert(tree.contains(1));
        assert(tree.contains(2));
        assert(tree.contains(3));
        assert(tree.findFirst(1) == "persistent1");
        assert(tree.findFirst(2) == "persistent2");
        assert(tree.findFirst(3) == "persistent3");
        cout << "✓ 数据持久化成功" << endl;
    }
}

int main() {
    // 设置控制台编码
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    std::system("chcp 65001 > nul");
    std::system("cls");
    
    cout << "开始 BPlusTree 全面测试...\n" << endl;
    
    try {
        testBasicOperations();
        testMultipleValues();
        testRemoveOperations();
        testModifyOperation();
        testLargeDataSet();
        testStringKeys();
        testClearOperation();
        testEdgeCases();
        testPersistence();
        
        cout << "\n🎉 所有测试通过！BPlusTree 实现正确。" << endl;
        
        // 清理测试文件
        cout << "\n清理测试文件..." << endl;
        vector<string> testFiles = {
            "test_basic_treeNodeFile", "test_basic_leafFile",
            "test_multi_treeNodeFile", "test_multi_leafFile",
            "test_remove_treeNodeFile", "test_remove_leafFile",
            "test_modify_treeNodeFile", "test_modify_leafFile",
            "test_large_treeNodeFile", "test_large_leafFile",
            "test_string_treeNodeFile", "test_string_leafFile",
            "test_clear_treeNodeFile", "test_clear_leafFile",
            "test_edge_treeNodeFile", "test_edge_leafFile",
            "test_persist_treeNodeFile", "test_persist_leafFile"
        };
        
        for (const auto& file : testFiles) {
            try {
                std::filesystem::remove(file);
            } catch (...) {}
        }
        cout << "✓ 测试文件清理完成" << endl;
        
    } catch (const exception& e) {
        cout << "❌ 测试失败: " << e.what() << endl;
        return 1;
    } catch (...) {
        cout << "❌ 测试失败: 未知错误" << endl;
        return 1;
    }
    
    return 0;
} 