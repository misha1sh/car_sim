#pragma once

#include <memory>

class CounterPointer {
public:
    explicit CounterPointer(std::shared_ptr<int> counter):
            counter_(counter) {
        *counter_ += 1;
    }
    CounterPointer(const CounterPointer& other) {
        counter_ = other.counter_;
        *counter_ += 1;
    }

    ~CounterPointer() {
        *counter_ -= 1;
    }

private:
    std::shared_ptr<int> counter_;
};

class AutomaticCounter {
public:
    explicit AutomaticCounter():
            internal_{std::make_shared<int>(0)}{
    }

    int Count() {
        return *internal_;
    }

    CounterPointer GetCounterPointer() {
        return CounterPointer{internal_};
    }

private:
    std::shared_ptr<int> internal_;
};