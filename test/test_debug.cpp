#include <iostream>
#include <string>
#include "../DataStructure/BPlusTree.h"

using namespace trainsys;
using namespace std;

int main() {
    cout << "Testing BPlusTree with string..." << endl;
    
    try {
        BPlusTree<int, string> tree("test_debug");
        
        cout << "Inserting (1, \"value1\")..." << endl;
        tree.insert(1, "value1");
        
        cout << "Inserting (2, \"value2\")..." << endl;
        tree.insert(2, "value2");
        
        cout << "Searching for key 2..." << endl;
        auto result = tree.find(2);
        
        cout << "Result empty: " << result.empty() << endl;
        if (!result.empty()) {
            cout << "Result[0]: " << result.visit(0) << endl;
        }
        
    } catch (const exception& e) {
        cout << "Exception: " << e.what() << endl;
    } catch (...) {
        cout << "Unknown exception" << endl;
    }
    
    return 0;
} 