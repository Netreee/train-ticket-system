#include <iostream>
#include "DataStructure/BPlusTree.h"

using namespace trainsys;

int main() {
    BPlusTree<int, int> tree("test_intint");
    for (int i = 0; i < 20; ++i) tree.insert(i, i * 10);
    for (int i = 0; i < 20; ++i) {
        auto vals = tree.find(i);
        std::cout << "key " << i << ": ";
        for (int j = 0; j < vals.length(); ++j) std::cout << vals.visit(j) << ' ';
        std::cout << '\n';
    }
    return 0;
} 