#include "min_heap.h"
#include <stdexcept>

void MinHeap::heapifyUp(int idx) {
    while (idx > 0 && heap[parent(idx)] > heap[idx]) {
        std::swap(heap[parent(idx)], heap[idx]);
        idx = parent(idx);
    }
}

void MinHeap::heapifyDown(int idx) {
    int smallest = idx;
    int left = leftChild(idx);
    int right = rightChild(idx);
    int n = heap.size();

    if (left < n && heap[smallest] > heap[left])
        smallest = left;
    if (right < n && heap[smallest] > heap[right])
        smallest = right;

    if (smallest != idx) {
        std::swap(heap[idx], heap[smallest]);
        heapifyDown(smallest);
    }
}

void MinHeap::push(double key, int value) {
    heap.push_back({key, value});
    heapifyUp(heap.size() - 1);
}

HeapNode MinHeap::pop() {
    if (isEmpty()) throw std::runtime_error("Heap is empty");
    HeapNode top = heap[0];
    heap[0] = heap.back();
    heap.pop_back();
    if (!isEmpty()) heapifyDown(0);
    return top;
}

HeapNode MinHeap::peek() const {
    if (isEmpty()) throw std::runtime_error("Heap is empty");
    return heap[0];
}
