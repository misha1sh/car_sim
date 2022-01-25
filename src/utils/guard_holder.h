#pragma once

#include <QMutex>
#include <QMutexLocker>

#include <memory>

namespace guard_holder {

template <typename T>
struct GuardHolderInnerData {
    std::unique_ptr<T> value{nullptr};
    QMutex mutex{};
};

template <typename T>
using GuardHolderInnerDataPtr = std::shared_ptr<GuardHolderInnerData<T>> ;


template <typename T>
class Guard {
public:
    explicit Guard(GuardHolderInnerDataPtr<T> data, std::unique_ptr<T>& place_where_was_moved) :
            data_(data),
            place_where_was_moved_(place_where_was_moved),
            locker_(&data->mutex)
    {
        VERIFY(data_ != nullptr);
        VERIFY(place_where_was_moved_ == nullptr);
        VERIFY(data_->value != nullptr);
        data_->value.swap(place_where_was_moved_);
    }


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexceptions"
    ~Guard() {
        VERIFY(place_where_was_moved_ != nullptr);
        VERIFY(data_->value == nullptr);
        data_->value.swap(place_where_was_moved_);
    }
#pragma clang diagnostic pop


private:
    GuardHolderInnerDataPtr<T> data_;
    std::unique_ptr<T>& place_where_was_moved_;
    QMutexLocker<QMutex> locker_;
};

template <typename T>
class GuardHolder {
public:
    void Init(std::unique_ptr<T> value) {
        VERIFY(data_ == nullptr);

        data_ = std::make_shared<GuardHolderInnerData<T>>();
        QMutexLocker<QMutex> locker(&data_->mutex);
        data_->value = std::move(value);
    }

    Guard<T> Get(std::unique_ptr<T>& place) {
        VERIFY(data_ != nullptr);
        return Guard<T>(data_, place);
    }

    operator bool() const {
        return data_ != nullptr;
    }

private:
    GuardHolderInnerDataPtr<T> data_{nullptr};
};

}  // namespace guard_holder


