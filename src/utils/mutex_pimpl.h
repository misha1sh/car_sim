#pragma once

#include <memory>
#include <experimental/propagate_const>

class Mutex;

class MutexLocker {
    friend Mutex;

    class impl;
    std::experimental::propagate_const<std::unique_ptr<impl>> pImpl;

public:
    explicit MutexLocker(Mutex* mutex);
    ~MutexLocker();

    MutexLocker(MutexLocker&&) = default;
    MutexLocker(const MutexLocker&) = delete;
    MutexLocker& operator=(MutexLocker&&) = default;
    MutexLocker& operator=(const MutexLocker&) = delete;
};

class Mutex {
    friend MutexLocker;
    class impl;
    std::experimental::propagate_const<std::unique_ptr<impl>> pImpl;

public:
    Mutex();
    ~Mutex();

    Mutex(Mutex&&) = default;
    Mutex(const Mutex&) = delete;
    Mutex& operator=(Mutex&&) = default;
    Mutex& operator=(const Mutex&) = delete;
};


Mutex GetMutex();