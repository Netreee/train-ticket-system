#ifndef RED_BLACK_TREE_H_
#define RED_BLACK_TREE_H_

#include "SearchTable.h"

namespace trainsys {
    template<class KeyType, class ValueType>
    class RedBlackTree : public DynamicSearchTable<KeyType, ValueType> {
    private:
        enum Color { RED, BLACK };

        class RedBlackNode {
        public:
            Color color;
            RedBlackNode *parent;
            RedBlackNode *left;
            RedBlackNode *right;

            DataType<KeyType, ValueType> data;

            RedBlackNode() = default;

            RedBlackNode(Color color, const DataType<KeyType, ValueType> &element, RedBlackNode *pt = nullptr,
                         RedBlackNode *lt = nullptr, RedBlackNode *rt = nullptr)
                : data(element) {
                this->color = color;
                parent = pt, left = lt, right = rt;
            }

            ~RedBlackNode() = default;
        };

    private:
        RedBlackNode *root;

    public:
        RedBlackTree();

        ~RedBlackTree();

    private:
        void makeEmpty(RedBlackNode *u);

        void LL(RedBlackNode *t);

        void RR(RedBlackNode *t);

        void LR(RedBlackNode *t);

        void RL(RedBlackNode *t);

        void insertionRebalance(RedBlackNode *u);

        void removalRebalance(RedBlackNode *u);

    public:
        DataType<KeyType, ValueType> *find(const KeyType &x) const;

        void insert(const DataType<KeyType, ValueType> &x);

        void remove(const KeyType &x);

    private:
        void insert(const DataType<KeyType, ValueType> &x, RedBlackNode *p, RedBlackNode *&t);

        void remove(const KeyType &x, RedBlackNode *&t);
    };

    template<class KeyType, class ValueType>
    RedBlackTree<KeyType, ValueType>::RedBlackTree() {
        root = nullptr;
    }

    template<class KeyType, class ValueType>
    RedBlackTree<KeyType, ValueType>::~RedBlackTree() {
        if (root != nullptr) makeEmpty(root);
    }

    template<class KeyType, class ValueType>
    void RedBlackTree<KeyType, ValueType>::makeEmpty(RedBlackNode *u) {
        if (u->left) makeEmpty(u->left);
        if (u->right) makeEmpty(u->right);
        delete u;
    }

    template<class KeyType, class ValueType>
    void RedBlackTree<KeyType, ValueType>::LL(RedBlackNode *t) {
        RedBlackNode *tmp = t->left;
        t->left = tmp->right;
        if (tmp->right) tmp->right->parent = t;
        tmp->right = t;
        tmp->parent = t->parent;
        if (t->parent != nullptr) {
            if (t->parent->left == t)
                t->parent->left = tmp;
            else
                t->parent->right = tmp;
        } else
            root = tmp;
        t->parent = tmp;
        t = tmp;
    }

    template<class KeyType, class ValueType>
    void RedBlackTree<KeyType, ValueType>::RR(RedBlackNode *t) {
        RedBlackNode *tmp = t->right;
        t->right = tmp->left;
        if (tmp->left) tmp->left->parent = t;
        tmp->left = t;
        tmp->parent = t->parent;
        if (t->parent != nullptr) {
            if (t->parent->left == t)
                t->parent->left = tmp;
            else
                t->parent->right = tmp;
        } else
            root = tmp;
        t->parent = tmp;
        t = tmp;
    }

    template<class KeyType, class ValueType>
    void RedBlackTree<KeyType, ValueType>::LR(RedBlackNode *t) {
        RR(t->left);
        LL(t);
    }

    template<class KeyType, class ValueType>
    void RedBlackTree<KeyType, ValueType>::RL(RedBlackNode *t) {
        LL(t->right);
        RR(t);
    }

    template<class KeyType, class ValueType>
    DataType<KeyType, ValueType> *RedBlackTree<KeyType, ValueType>::find(const KeyType &x) const {
        RedBlackNode *t = root;
        while (t != nullptr) {
            if (x == t->data.key) return &t->data;
            if (x < t->data.key)
                t = t->left;
            else
                t = t->right;
        }
        return nullptr;
    }

    template<class KeyType, class ValueType>
    void RedBlackTree<KeyType, ValueType>::insertionRebalance(RedBlackNode *u) {
        if (u == nullptr) return;

        while (u != root) {
            RedBlackNode *p = u->parent;
            if (p->color == BLACK) break;

            RedBlackNode *g = p->parent;
            RedBlackNode *s = (p == g->left) ? g->right : g->left;

            if (s != nullptr && s->color == RED) {
                p->color = BLACK;
                s->color = BLACK;
                g->color = RED;
                u = g;
            } else {
                if (p == g->left && u == p->left) {
                    p->color = BLACK;
                    g->color = RED;
                    LL(g);
                } else if (p == g->left && u == p->right) {
                    u->color = BLACK;
                    g->color = RED;
                    LR(g);
                } else if (p == g->right && u == p->left) {
                    u->color = BLACK;
                    g->color = RED;
                    RL(g);
                } else if (p == g->right && u == p->right) {
                    p->color = BLACK;
                    g->color = RED;
                    RR(g);
                }
                break;
            }
        }
        root->color = BLACK;
    }

    template<class KeyType, class ValueType>
    void RedBlackTree<KeyType, ValueType>::insert(const DataType<KeyType, ValueType> &x) {
        insert(x, nullptr, root);
    }

    template<class KeyType, class ValueType>
    void RedBlackTree<KeyType, ValueType>::insert(const DataType<KeyType, ValueType> &x, RedBlackNode *p,
                                                  RedBlackNode *&t) {
        if (t == nullptr) {
            t = new RedBlackNode(RED, x, p);
            insertionRebalance(t);
        } else if (x.key < t->data.key) {
            insert(x, t, t->left);
        } else if (x.key > t->data.key) {
            insert(x, t, t->right);
        }
    }

    template<class KeyType, class ValueType>
    void RedBlackTree<KeyType, ValueType>::removalRebalance(RedBlackNode *u) {
        if (u == nullptr) return;
        while (u != root) {
            RedBlackNode *p = u->parent;
            RedBlackNode *s = (u == p->left) ? p->right : p->left;

            if (s->color == RED) {
                p->color = RED;
                s->color = BLACK;
                if (s == p->left)
                    LL(p);
                else
                    RR(p);
            } else {
                bool leftBlackOrNil = s->left == nullptr || s->left->color == BLACK;
                bool rightBlackOrNil = s->right == nullptr || s->right->color == BLACK;

                if (leftBlackOrNil && rightBlackOrNil) {
                    if (p->color == RED) {
                        s->color = RED;
                        p->color = BLACK;
                        break;
                    } else {
                        s->color = RED;
                        u = p;
                    }
                } else {
                    if (u == p->left) {
                        if (rightBlackOrNil) {
                            s->left->color = BLACK;
                            s->color = RED;
                            LL(s);
                        } else {
                            s->color = p->color;
                            p->color = BLACK;
                            s->right->color = BLACK;
                            RR(p);
                            break;
                        }
                    } else {
                        if (leftBlackOrNil) {
                            s->right->color = BLACK;
                            s->color = RED;
                            RR(s);
                        } else {
                            s->color = p->color;
                            p->color = BLACK;
                            s->left->color = BLACK;
                            LL(p);
                            break;
                        }
                    }
                }
            }
        }
        root->color = BLACK;
    }

    template<class KeyType, class ValueType>
    void RedBlackTree<KeyType, ValueType>::remove(const KeyType &x) {
        remove(x, root);
    }

    template<class KeyType, class ValueType>
    void RedBlackTree<KeyType, ValueType>::remove(const KeyType &x, RedBlackNode *&t) {
        if (!t) return;

        if (x == t->data.key) {
            if (t->left != nullptr && t->right != nullptr) {
                RedBlackNode *tmp = t->right;
                while (tmp->left != nullptr) tmp = tmp->left;
                t->data = tmp->data;
                remove(tmp->data.key, t->right);
            } else {
                RedBlackNode *v = nullptr;
                if (t->left) v = t->left;
                if (t->right) v = t->right;

                if (t->color == BLACK) {
                    if (v && v->color == RED)
                        v->color = BLACK;
                    else
                        removalRebalance(t);
                }

                if (v) v->parent = t->parent;
                delete t;
                t = v;
            }
        } else if (x < t->data.key) {
            remove(x, t->left);
        } else {
            remove(x, t->right);
        }
    }
} // namespace trainsys

#endif  // REB_BLACK_TREE_H_
