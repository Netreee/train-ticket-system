#ifndef QUEUE_H_
#define QUEUE_H_

namespace trainsys {
    template<class elemType>
    class queue {
    public:
        virtual bool isEmpty() const = 0;

        virtual void enQueue(const elemType &x) = 0;

        virtual elemType deQueue() = 0;

        virtual elemType getHead() const = 0;

        virtual ~queue() {
        }
    };

    template<class elemType>
    class linkQueue : public queue<elemType> {
    private:
        struct node {
            elemType data;
            node *next;

            node(const elemType &x, node *N = nullptr) {
                data = x;
                next = N;
            }

            node() : next(nullptr) {
            }

            ~node() {
            }
        };

        node *front, *rear;

    public:
        linkQueue();

        ~linkQueue();

        bool isEmpty() const;

        void enQueue(const elemType &x);

        elemType deQueue();

        elemType getHead() const;
    };

    template<class elemType>
    linkQueue<elemType>::linkQueue() {
        front = rear = nullptr;
    }

    template<class elemType>
    linkQueue<elemType>::~linkQueue() {
        node *tmp;
        while (front != nullptr) {
            tmp = front;
            front = front->next;
            delete tmp;
        }
    }

    template<class elemType>
    bool linkQueue<elemType>::isEmpty() const {
        return front == nullptr;
    }

    template<class elemType>
    elemType linkQueue<elemType>::getHead() const {
        return front->data;
    }

    template<class elemType>
    void linkQueue<elemType>::enQueue(const elemType &x) {
        if (rear == nullptr)
            front = rear = new node(x);
        else
            rear = rear->next = new node(x);
    }

    template<class elemType>
    elemType linkQueue<elemType>::deQueue() {
        node *tmp = front;
        elemType value = front->data;
        front = front->next;
        if (front == nullptr) rear = nullptr;
        delete tmp;
        return value;
    }

    template<class Type>
    class priorityQueue : public queue<Type> {
    public:
        priorityQueue(int capacity = 100);

        priorityQueue(const Type data[], int size) {
        }

        ~priorityQueue();

        bool isEmpty() const;

        void enQueue(const Type &x);

        Type deQueue();

        Type getHead() const;

    private:
        int currentSize;
        Type *array;
        int maxSize;

        void doubleSpace();

        void percolateDown(int hole);
    };

    template<class Type>
    priorityQueue<Type>::priorityQueue(int capacity) {
        array = new Type[capacity];
        maxSize = capacity;
        currentSize = 0;
    }

    template<class Type>
    priorityQueue<Type>::~priorityQueue() {
        delete[] array;
    }

    template<class Type>
    bool priorityQueue<Type>::isEmpty() const {
        return currentSize == 0;
    }

    template<class Type>
    Type priorityQueue<Type>::getHead() const {
        return array[1];
    }

    template<class Type>
    void priorityQueue<Type>::doubleSpace() {
        Type *tmp = array;

        maxSize *= 2;
        array = new Type[maxSize];
        for (int i = 0; i <= currentSize; ++i) array[i] = tmp[i];
        delete[] tmp;
    }

    template<class Type>
    void priorityQueue<Type>::enQueue(const Type &x) {
        if (currentSize == maxSize - 1) doubleSpace();

        int hole = ++currentSize;
        for (; hole > 1 && x < array[hole / 2]; hole /= 2) array[hole] = array[hole / 2];
        array[hole] = x;
    }

    template<class Type>
    Type priorityQueue<Type>::deQueue() {
        Type minItem;
        minItem = array[1];
        array[1] = array[currentSize--];
        percolateDown(1);
        return minItem;
    }

    template<class Type>
    void priorityQueue<Type>::percolateDown(int hole) {
        int child;
        Type tmp = array[hole];

        for (; hole * 2 <= currentSize; hole = child) {
            child = hole * 2;
            if (child != currentSize && array[child + 1] < array[child]) child++;
            if (array[child] < tmp)
                array[hole] = array[child];
            else
                break;
        }
        array[hole] = tmp;
    }
}

#endif
