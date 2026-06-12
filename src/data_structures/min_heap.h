#pragma once
#include <vector>
#include <stdexcept>

struct HeapNode {
    double key;
    int value;

    bool operator>(const HeapNode& other) const {
        return key > other.key;
    }
};

// Custom Min-Heap built from scratch (no STL priority_queue)
class MinHeap {
private:
    std::vector<HeapNode> heap;

    void heapifyUp(int idx);
    void heapifyDown(int idx);
    int parent(int i) const { return (i - 1) / 2; }
    int leftChild(int i) const { return 2 * i + 1; }
    int rightChild(int i) const { return 2 * i + 2; }

public:
    MinHeap() = default;

    void push(double key, int value);
    HeapNode pop();
    HeapNode peek() const;
    bool isEmpty() const { return heap.empty(); }
    int size() const { return heap.size(); }
    void clear() { heap.clear(); }
};
