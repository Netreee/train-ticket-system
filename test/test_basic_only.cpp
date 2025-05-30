#include <iostream>
#include <string>
#include "../DataStructure/BPlusTree.h"
#include <filesystem>

using namespace trainsys;
using namespace std;

int main() {
    try {
        std::filesystem::remove("test_basic_treeNodeFile");
        std::filesystem::remove("test_basic_leafFile");
    } catch (...) {}

    BPlusTree<int, string> tree("test_basic");

    tree.insert(1, "value1");
    tree.insert(2, "value2");
    tree.insert(3, "value3");

    cout << "size=" << tree.size() << endl;

    auto res1 = tree.find(1);
    cout << "res1 len=" << res1.length() << (
        res1.empty()?" empty":" not empty") << " first=" << (res1.empty()?"":res1.visit(0)) << endl;
    auto res2 = tree.find(2);
    cout << "res2 len=" << res2.length() << (
        res2.empty()?" empty":" not empty") << " first=" << (res2.empty()?"":res2.visit(0)) << endl;

    return 0;
} 