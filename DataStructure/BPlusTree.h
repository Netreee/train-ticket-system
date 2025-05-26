#ifndef BPTREE_HPP_BPTREE_HPP
#define BPTREE_HPP_BPTREE_HPP

#include <vector>
#include <fstream>
#include "Pair.h"
#include "SearchTable.h"
#include "List.h"

namespace trainsys {
    template<class KeyType, class ValueType, int M = 100, int L = 100>
    class BPlusTree : public StorageSearchTable<KeyType, ValueType> {
    private:
        std::fstream treeNodeFile, leafFile;
        int rearTreeNode, rearLeaf;
        int sizeData;
        const int headerLengthOfTreeNodeFile = 2 * sizeof(int);
        const int headerLengthOfLeafFile = 2 * sizeof(int);
        seqList<int> emptyTreeNode;
        seqList<int> emptyLeaf;

        struct TreeNode {
            bool isBottomNode;
            int pos, dataCount;
            int childrenPos[M];
            Pair<KeyType, ValueType> septal[M - 1];
        };

        struct Leaf {
            int nxt, pos;
            int dataCount;
            Pair<KeyType, ValueType> value[L];
        };

        std::string treeNodeFileName, leafFileName;
        TreeNode root;

    public:
        explicit BPlusTree(const std::string &name) {
            treeNodeFileName = name + "_treeNodeFile", leafFileName = name + "_leafFile";
            // 原实现 ↓（文本模式，换行转换会破坏二进制数据）
            // treeNodeFile.open(treeNodeFileName);
            // leafFile.open(leafFileName);
            treeNodeFile.open(treeNodeFileName, std::ios::in | std::ios::out | std::ios::binary);
            leafFile.open(leafFileName, std::ios::in | std::ios::out | std::ios::binary);
            
            if (!leafFile || !treeNodeFile) {
                initialize();
            } else {
                treeNodeFile.seekg(0), leafFile.seekg(0);
                int rootPos;
                treeNodeFile.read(reinterpret_cast<char *>(&rootPos), sizeof(int));
                treeNodeFile.read(reinterpret_cast<char *>(&rearTreeNode), sizeof(int));
                treeNodeFile.seekg(headerLengthOfTreeNodeFile + rootPos * sizeof(TreeNode));
                treeNodeFile.read(reinterpret_cast<char *>(&root), sizeof(TreeNode));
                int treeNodeEmptySize, leafEmptySize;
                treeNodeFile.seekg(headerLengthOfTreeNodeFile + (rearTreeNode + 1) * sizeof(TreeNode));
                treeNodeFile.read(reinterpret_cast<char *>(&treeNodeEmptySize), sizeof(int));
                for (int i = 0; i < treeNodeEmptySize; i++) {
                    int data;
                    treeNodeFile.read(reinterpret_cast<char *>(&data), sizeof(int));
                    emptyTreeNode.pushBack(data);
                }
                leafFile.read(reinterpret_cast<char *>(&rearLeaf), sizeof(int));
                leafFile.read(reinterpret_cast<char *>(&sizeData), sizeof(int));
                leafFile.seekg(headerLengthOfLeafFile + (rearLeaf + 1) * sizeof(Leaf));
                leafFile.read(reinterpret_cast<char *>(&leafEmptySize), sizeof(int));
                for (int i = 0; i < leafEmptySize; i++) {
                    int data;
                    leafFile.read(reinterpret_cast<char *>(&data), sizeof(int));
                    emptyLeaf.pushBack(data);
                }
            }
        }

        ~BPlusTree() {
            treeNodeFile.seekp(0), leafFile.seekp(0);
            treeNodeFile.write(reinterpret_cast<char *>(&root.pos), sizeof(int));
            treeNodeFile.write(reinterpret_cast<char *>(&rearTreeNode), sizeof(int));
            writeTreeNode(root);
            leafFile.write(reinterpret_cast<char *>(&rearLeaf), sizeof(int));
            leafFile.write(reinterpret_cast<char *>(&sizeData), sizeof(int));
            treeNodeFile.seekp(headerLengthOfTreeNodeFile + (rearTreeNode + 1) * sizeof(TreeNode));
            int emptyTreeNodeCount = emptyTreeNode.length(), emptyLeafCount = emptyLeaf.length();
            treeNodeFile.write(reinterpret_cast<char *>(&emptyTreeNodeCount), sizeof(int));
            for (int i = 0; i < emptyTreeNode.length(); i++) {
                int tmp = emptyTreeNode.visit(i);
                treeNodeFile.write(reinterpret_cast<char *>(&tmp), sizeof(int));
            }
            leafFile.seekp(headerLengthOfLeafFile + (rearLeaf + 1) * sizeof(Leaf));
            leafFile.write(reinterpret_cast<char *>(&emptyLeafCount), sizeof(int));
            for (int i = 0; i < emptyLeaf.length(); i++) {
                int tmp = emptyLeaf.visit(i);
                leafFile.write(reinterpret_cast<char *>(&tmp), sizeof(int));
            }
            leafFile.close();
            treeNodeFile.close();
        }

        int size() { return sizeData; }

        void insert(const KeyType &key, const ValueType &value) {
            if (insert(Pair<KeyType, ValueType>(key, value), root)) {
                TreeNode newRoot;
                TreeNode newNode;
                newNode.pos = getNewTreeNodePos();
                newNode.isBottomNode = root.isBottomNode;
                newNode.dataCount = M / 2;
                int mid = M / 2;
                for (int i = 0; i < mid; i++) {
                    newNode.childrenPos[i] = root.childrenPos[mid + i];
                }
                for (int i = 0; i < mid - 1; i++) {
                    newNode.septal[i] = root.septal[mid + i];
                }
                root.dataCount = mid;
                writeTreeNode(root);
                writeTreeNode(newNode);
                newRoot.dataCount = 2;
                newRoot.pos = getNewTreeNodePos();
                newRoot.isBottomNode = false;
                newRoot.childrenPos[0] = root.pos;
                newRoot.childrenPos[1] = newNode.pos;
                newRoot.septal[0] = root.septal[mid - 1];
                root = newRoot;
                writeTreeNode(root);
            }
        }

        seqList<ValueType> find(const KeyType &key) {
            seqList<ValueType> ans;
            TreeNode p = root;
            Leaf leaf;
            if (p.dataCount == 0) {
                return ans;
            }
            while (!p.isBottomNode) {
                readTreeNode(p, p.childrenPos[binarySearchTreeNode(key, p)]);
            }
            readLeaf(leaf, p.childrenPos[binarySearchTreeNode(key, p)]);
            int now = binarySearchLeaf(key, leaf);
            while (now < leaf.dataCount && leaf.value[now].first == key) {
                ans.pushBack(leaf.value[now++].second);
            }
            while (leaf.nxt && now == leaf.dataCount) {
                readLeaf(leaf, leaf.nxt);
                now = 0;
                while (now < leaf.dataCount && leaf.value[now].first == key) {
                    ans.pushBack(leaf.value[now++].second);
                }
            }
            return ans;
        }

        bool contains(const KeyType &key) {
            return !find(key).empty();
        }

        ValueType findFirst(const KeyType &key) {
            return find(key).visit(0);
        }

        void remove(const KeyType &key, const ValueType &value) {
            if (remove(Pair<KeyType, ValueType>(key, value), root)) {
                if (!root.isBottomNode && root.dataCount == 1) {
                    TreeNode son;
                    readTreeNode(son, root.childrenPos[0]);
                    emptyTreeNode.pushBack(root.pos);
                    root = son;
                }
            }
        }

        void removeFirst(const KeyType &key) {
            remove(key, findFirst(key));
        }

        void modify(const KeyType &key, const ValueType &oldValue, const ValueType &newValue) {
            remove(key, oldValue);
            insert(key, newValue);
        }

        void clear() {
            treeNodeFile.close();
            leafFile.close();
            emptyTreeNode.clear();
            emptyLeaf.clear();
            initialize();
        }

    private:
        bool insert(const Pair<KeyType, ValueType> &val, TreeNode &currentNode) {
            if (currentNode.isBottomNode) {
                Leaf leaf;
                int nodePos = binarySearchTreeNodeValue(val, currentNode);
                readLeaf(leaf, currentNode.childrenPos[nodePos]);
                int leafPos = binarySearchLeafValue(val, leaf);
                leaf.dataCount++, sizeData++;
                for (int i = leaf.dataCount - 1; i > leafPos; i--) {
                    leaf.value[i] = leaf.value[i - 1];
                }
                leaf.value[leafPos] = val;
                if (leaf.dataCount == L) {
                    Leaf newLeaf;
                    newLeaf.pos = getNewLeafPos();
                    newLeaf.nxt = leaf.nxt;
                    leaf.nxt = newLeaf.pos;
                    int mid = L / 2;
                    for (int i = 0; i < mid; i++) {
                        newLeaf.value[i] = leaf.value[i + mid];
                    }
                    leaf.dataCount = newLeaf.dataCount = mid;
                    writeLeaf(leaf);
                    writeLeaf(newLeaf);
                    for (int i = currentNode.dataCount; i > nodePos + 1; i--) {
                        currentNode.childrenPos[i] = currentNode.childrenPos[i - 1];
                    }
                    currentNode.childrenPos[nodePos + 1] = newLeaf.pos;
                    for (int i = currentNode.dataCount - 1; i > nodePos; i--) {
                        currentNode.septal[i] = currentNode.septal[i - 1];
                    }
                    currentNode.septal[nodePos] = leaf.value[mid - 1];
                    currentNode.dataCount++;
                    if (currentNode.dataCount == M) {
                        return true;
                    } else writeTreeNode(currentNode);
                    return false;
                }
                writeLeaf(leaf);
                return false;
            }
            TreeNode son;
            int now = binarySearchTreeNodeValue(val, currentNode);
            readTreeNode(son, currentNode.childrenPos[now]);
            if (insert(val, son)) {
                TreeNode newNode;
                newNode.pos = getNewTreeNodePos(), newNode.isBottomNode = son.isBottomNode;
                int mid = M / 2;
                for (int i = 0; i < mid; i++) {
                    newNode.childrenPos[i] = son.childrenPos[mid + i];
                }
                for (int i = 0; i < mid - 1; i++) {
                    newNode.septal[i] = son.septal[mid + i];
                }
                newNode.dataCount = son.dataCount = mid;
                writeTreeNode(son);
                writeTreeNode(newNode);
                for (int i = currentNode.dataCount; i > now + 1; i--) {
                    currentNode.childrenPos[i] = currentNode.childrenPos[i - 1];
                }
                currentNode.childrenPos[now + 1] = newNode.pos;
                for (int i = currentNode.dataCount - 1; i > now; i--) {
                    currentNode.septal[i] = currentNode.septal[i - 1];
                }
                currentNode.septal[now] = son.septal[mid - 1];
                currentNode.dataCount++;
                if (currentNode.dataCount == M) {
                    return true;
                } else writeTreeNode(currentNode);
                return false;
            } else return false;
        }

        bool remove(const Pair<KeyType, ValueType> &val, TreeNode &currentNode) {
            if (currentNode.isBottomNode) {
                Leaf leaf;
                int nodePos = binarySearchTreeNodeValue(val, currentNode);
                readLeaf(leaf, currentNode.childrenPos[nodePos]);
                int leafPos = binarySearchLeafValue(val, leaf);
                if (leafPos == leaf.dataCount || !checkPairEqual(leaf.value[leafPos], val)) {
                    return false;
                }
                leaf.dataCount--, sizeData--;
                for (int i = leafPos; i < leaf.dataCount; i++) {
                    leaf.value[i] = leaf.value[i + 1];
                }
                if (leaf.dataCount < L / 2) {
                    Leaf pre, nxt;
                    if (nodePos - 1 >= 0) {
                        readLeaf(pre, currentNode.childrenPos[nodePos - 1]);
                        if (pre.dataCount > L / 2) {
                            leaf.dataCount++, pre.dataCount--;
                            for (int i = leaf.dataCount - 1; i > 0; i--) {
                                leaf.value[i] = leaf.value[i - 1];
                            }
                            leaf.value[0] = pre.value[pre.dataCount];
                            currentNode.septal[nodePos - 1] = pre.value[pre.dataCount - 1];
                            writeLeaf(leaf);
                            writeLeaf(pre);
                            writeTreeNode(currentNode);
                            return false;
                        }
                    }
                    if (nodePos + 1 < currentNode.dataCount) {
                        readLeaf(nxt, currentNode.childrenPos[nodePos + 1]);
                        if (nxt.dataCount > L / 2) {
                            leaf.dataCount++, nxt.dataCount--;
                            leaf.value[leaf.dataCount - 1] = nxt.value[0];
                            currentNode.septal[nodePos] = nxt.value[0];
                            for (int i = 0; i < nxt.dataCount; i++) {
                                nxt.value[i] = nxt.value[i + 1];
                            }
                            writeLeaf(leaf);
                            writeLeaf(nxt);
                            writeTreeNode(currentNode);
                            return false;
                        }
                    }
                    if (nodePos - 1 >= 0) {
                        for (int i = 0; i < leaf.dataCount; i++) {
                            pre.value[pre.dataCount + i] = leaf.value[i];
                        }
                        pre.dataCount += leaf.dataCount;
                        pre.nxt = leaf.nxt;
                        writeLeaf(pre);
                        emptyLeaf.pushBack(leaf.pos);
                        currentNode.dataCount--;
                        for (int i = nodePos; i < currentNode.dataCount; i++) {
                            currentNode.childrenPos[i] = currentNode.childrenPos[i + 1];
                        }
                        for (int i = nodePos - 1; i < currentNode.dataCount - 1; i++) {
                            currentNode.septal[i] = currentNode.septal[i + 1];
                        }
                        if (currentNode.dataCount < M / 2) {
                            return true;
                        }
                        writeTreeNode(currentNode);
                        return false;
                    }
                    if (nodePos + 1 < currentNode.dataCount) {
                        for (int i = 0; i < nxt.dataCount; i++) {
                            leaf.value[leaf.dataCount + i] = nxt.value[i];
                        }
                        leaf.dataCount += nxt.dataCount;
                        leaf.nxt = nxt.nxt;
                        writeLeaf(leaf);
                        emptyLeaf.pushBack(nxt.pos);
                        currentNode.dataCount--;
                        for (int i = nodePos + 1; i < currentNode.dataCount; i++) {
                            currentNode.childrenPos[i] = currentNode.childrenPos[i + 1];
                        }
                        for (int i = nodePos; i < currentNode.dataCount - 1; i++) {
                            currentNode.septal[i] = currentNode.septal[i + 1];
                        }
                        if (currentNode.dataCount < M / 2) {
                            return true;
                        }
                        writeTreeNode(currentNode);
                        return false;
                    }
                    writeLeaf(leaf);
                } else writeLeaf(leaf);
                return false;
            }
            TreeNode son;
            int now = binarySearchTreeNodeValue(val, currentNode);
            readTreeNode(son, currentNode.childrenPos[now]);
            if (remove(val, son)) {
                TreeNode pre, nxt;
                if (now - 1 >= 0) {
                    readTreeNode(pre, currentNode.childrenPos[now - 1]);
                    if (pre.dataCount > M / 2) {
                        son.dataCount++, pre.dataCount--;
                        for (int i = son.dataCount - 1; i > 0; i--) {
                            son.childrenPos[i] = son.childrenPos[i - 1];
                        }
                        for (int i = son.dataCount - 2; i > 0; i--) {
                            son.septal[i] = son.septal[i - 1];
                        }
                        son.childrenPos[0] = pre.childrenPos[pre.dataCount];
                        son.septal[0] = currentNode.septal[now - 1];
                        currentNode.septal[now - 1] = pre.septal[pre.dataCount - 1];
                        writeTreeNode(son);
                        writeTreeNode(pre);
                        writeTreeNode(currentNode);
                        return false;
                    }
                }
                if (now + 1 < currentNode.dataCount) {
                    readTreeNode(nxt, currentNode.childrenPos[now + 1]);
                    if (nxt.dataCount > M / 2) {
                        son.dataCount++, nxt.dataCount--;
                        son.childrenPos[son.dataCount - 1] = nxt.childrenPos[0];
                        son.septal[son.dataCount - 2] = currentNode.septal[now];
                        currentNode.septal[now] = nxt.septal[0];
                        for (int i = 0; i < nxt.dataCount; i++) {
                            nxt.childrenPos[i] = nxt.childrenPos[i + 1];
                        }
                        for (int i = 0; i < nxt.dataCount - 1; i++) {
                            nxt.septal[i] = nxt.septal[i + 1];
                        }
                        writeTreeNode(son);
                        writeTreeNode(nxt);
                        writeTreeNode(currentNode);
                        return false;
                    }
                }
                if (now - 1 >= 0) {
                    for (int i = 0; i < son.dataCount; i++) {
                        pre.childrenPos[pre.dataCount + i] = son.childrenPos[i];
                    }
                    pre.septal[pre.dataCount - 1] = currentNode.septal[now - 1];
                    for (int i = 0; i < son.dataCount - 1; i++) {
                        pre.septal[pre.dataCount + i] = son.septal[i];
                    }
                    pre.dataCount += son.dataCount;
                    writeTreeNode(pre);
                    emptyTreeNode.pushBack(son.pos);
                    currentNode.dataCount--;
                    for (int i = now; i < currentNode.dataCount; i++) {
                        currentNode.childrenPos[i] = currentNode.childrenPos[i + 1];
                    }
                    for (int i = now - 1; i < currentNode.dataCount - 1; i++) {
                        currentNode.septal[i] = currentNode.septal[i + 1];
                    }
                    if (currentNode.dataCount < M / 2) {
                        return true;
                    }
                    writeTreeNode(currentNode);
                    return false;
                }
                if (now + 1 < currentNode.dataCount) {
                    for (int i = 0; i < nxt.dataCount; i++) {
                        son.childrenPos[son.dataCount + i] = nxt.childrenPos[i];
                    }
                    son.septal[son.dataCount - 1] = currentNode.septal[now];
                    for (int i = 0; i < nxt.dataCount - 1; i++) {
                        son.septal[son.dataCount + i] = nxt.septal[i];
                    }
                    son.dataCount += nxt.dataCount;
                    writeTreeNode(son);
                    emptyTreeNode.pushBack(nxt.pos);
                    currentNode.dataCount--;
                    for (int i = now + 1; i < currentNode.dataCount; i++) {
                        currentNode.childrenPos[i] = currentNode.childrenPos[i + 1];
                    }
                    for (int i = now; i < currentNode.dataCount - 1; i++) {
                        currentNode.septal[i] = currentNode.septal[i + 1];
                    }
                    if (currentNode.dataCount < M / 2) {
                        return true;
                    }
                    writeTreeNode(currentNode);
                    return false;
                }
            }
            return false;
        }

        void writeTreeNode(TreeNode &node) {
            // 原实现 ↓（用 get 指针）
            // treeNodeFile.seekg(node.pos * sizeof(TreeNode) + headerLengthOfTreeNodeFile);
            treeNodeFile.seekp(node.pos * sizeof(TreeNode) + headerLengthOfTreeNodeFile);
            treeNodeFile.write(reinterpret_cast<char *>(&node), sizeof(TreeNode));
        }

        void writeLeaf(Leaf &leaf) {
            // 原实现 ↓
            // leafFile.seekg(leaf.pos * sizeof(Leaf) + headerLengthOfLeafFile);
            leafFile.seekp(leaf.pos * sizeof(Leaf) + headerLengthOfLeafFile);
            leafFile.write(reinterpret_cast<char *>(&leaf), sizeof(Leaf));
        }

        void readTreeNode(TreeNode &node, int pos) {
            treeNodeFile.seekg(pos * sizeof(TreeNode) + headerLengthOfTreeNodeFile);
            treeNodeFile.read(reinterpret_cast<char *>(&node), sizeof(TreeNode));
        }

        void readLeaf(Leaf &lef, int pos) {
            leafFile.seekg(pos * sizeof(Leaf) + headerLengthOfLeafFile);
            leafFile.read(reinterpret_cast<char *>(&lef), sizeof(Leaf));
        }

        int binarySearchLeafValue(const Pair<KeyType, ValueType> &val, const Leaf &lef) {
            int l = 0, r = lef.dataCount - 1, ans = lef.dataCount;
            while (l <= r) {
                int mid = (l + r) / 2;
                if (checkPairLess(lef.value[mid], val)) l = mid + 1;
                else r = mid - 1, ans = mid;
            }
            return ans;
        }

        int binarySearchTreeNodeValue(const Pair<KeyType, ValueType> &val, const TreeNode &node) {
            int l = -1, r = node.dataCount - 2, ans = node.dataCount - 1;
            while (l <= r) {
                int mid = (l + r) / 2;
                if (checkPairLess(node.septal[mid], val)) l = mid + 1;
                else r = mid - 1, ans = mid;
            }
            return ans;
        }

        int binarySearchLeaf(const KeyType &key, const Leaf &lef) {
            int l = 0, r = lef.dataCount - 1, ans = lef.dataCount;
            while (l <= r) {
                int mid = (l + r) / 2;
                if (lef.value[mid].first < key) l = mid + 1;
                else r = mid - 1, ans = mid;
            }
            return ans;
        }

        int binarySearchTreeNode(const KeyType &key, const TreeNode &node) {
            int l = 0, r = node.dataCount - 2, ans = node.dataCount - 1;
            while (l <= r) {
                int mid = (l + r) / 2;
                if (node.septal[mid].first < key) l = mid + 1;
                else r = mid - 1, ans = mid;
            }
            return ans;
        }

        void initialize() {
            // 原实现 ↓
            // treeNodeFile.open(treeNodeFileName, std::ios::out);
            // leafFile.open(leafFileName, std::ios::out);
            treeNodeFile.open(treeNodeFileName, std::ios::out | std::ios::binary | std::ios::trunc);
            leafFile.open(leafFileName, std::ios::out | std::ios::binary | std::ios::trunc);
            root.isBottomNode = (root.pos = (root.childrenPos[0] = 1)), sizeData = 0;
            root.dataCount = 1;
            rearLeaf = rearTreeNode = 1;
            Leaf initLeaf;
            initLeaf.nxt = initLeaf.dataCount = 0;
            initLeaf.pos = 1;
            writeLeaf(initLeaf);
            treeNodeFile.close();
            leafFile.close();
            // 原实现 ↓
            // treeNodeFile.open(treeNodeFileName);
            // leafFile.open(leafFileName);
            treeNodeFile.open(treeNodeFileName, std::ios::in | std::ios::out | std::ios::binary);
            leafFile.open(leafFileName, std::ios::in | std::ios::out | std::ios::binary);
        }

        int getNewTreeNodePos() {
            if (emptyTreeNode.empty()) {
                return ++rearTreeNode;
            } else {
                int newIndex = emptyTreeNode.back();
                emptyTreeNode.popBack();
                return newIndex;
            }
        }

        int getNewLeafPos() {
            if (emptyLeaf.empty()) {
                return ++rearLeaf;
            } else {
                int newIndex = emptyLeaf.back();
                emptyLeaf.popBack();
                return newIndex;
            }
        }
    };
}

#endif //BPTREE_HPP_BPTREE_HPP
