#include "utils/mutex_pimpl.h"
#include "verify.h"

#include <QMutex>

class MutexLocker::impl : public QMutexLocker<QMutex> {
    using QMutexLocker<QMutex>::QMutexLocker;
};

class Mutex::impl : public QMutex {
    using QMutex::QMutex;
};

Mutex::Mutex():
    pImpl(std::make_unique<impl>())
{
}

Mutex::~Mutex() = default;

MutexLocker::MutexLocker(Mutex *mutex) :
    pImpl(std::make_unique<impl>(&*mutex->pImpl))
{
}
MutexLocker::~MutexLocker() = default;