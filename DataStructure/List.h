#ifndef LIST_H_
#define LIST_H_

#include <iostream>

namespace trainsys {
    template<class elemType>
    class list {
    public:
        virtual void clear() = 0;

        virtual int length() const = 0;

        virtual void insert(int i, const elemType &x) = 0;

        virtual void remove(int i) = 0;

        virtual elemType visit(int i) const = 0;

        virtual ~list() {
        };
    };

    template<class elemType>
    class seqList : public list<elemType> {
    protected:
        elemType *data;
        int currentLength;
        int maxSize;

        void doubleSpace();

    public:
        seqList(int initSize = 10);

        ~seqList() { delete[] data; }
        void clear() { currentLength = 0; }
        int length() const { return currentLength; }
        bool empty() const { return currentLength == 0; }

        void insert(int i, const elemType &x);

        void pushBack(const elemType &x);

        void popBack();

        void remove(int i);

        elemType back() const;

        elemType visit(int i) const { return data[i]; }
    };

    template<class elemType>
    seqList<elemType>::seqList(int initSize) {
        data = new elemType[initSize];
        maxSize = initSize;
        currentLength = 0;
    }

    template<class elemType>
    void seqList<elemType>::insert(int i, const elemType &x) {
        if (currentLength == maxSize)
            doubleSpace();
        for (int j = currentLength; j > i; j--)
            data[j] = data[j - 1];
        data[i] = x;
        ++currentLength;
    }

    template<class elemType>
    void seqList<elemType>::pushBack(const elemType &x) {
        insert(length(), x);
    }

    template<class elemType>
    void seqList<elemType>::popBack() {
        remove(length() - 1);
    }

    template<class elemType>
    void seqList<elemType>::doubleSpace() {
        elemType *tmp = data;
        maxSize *= 2;
        data = new elemType[maxSize];
        for (int i = 0; i < currentLength; ++i) data[i] = tmp[i];
        delete[] tmp;
    }

    template<class elemType>
    void seqList<elemType>::remove(int i) {
        for (int j = i; j < currentLength - 1; j++)
            data[j] = data[j + 1];
        --currentLength;
    }

    template<class elemType>
    elemType seqList<elemType>::back() const {
        return visit(length() - 1);
    }
}

#endif
