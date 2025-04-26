#include <unordered_map>
#include <boost/thread.hpp>
#include <boost/regex.hpp>

#include <iostream>

using std::unordered_map;
using boost::shared_mutex;

// an enum for whether the lock type is a reader or a writer lock
enum guardStatus {
    READER,
    WRITER
};

// Class used to unlock when we go out of scope for a given lock (RAII)
class UnlockGuard {
public:
    UnlockGuard(shared_mutex& mutexVal, guardStatus statusVal)
        : mutex(&mutexVal), status(statusVal) {}

    ~UnlockGuard() {
        if (status == WRITER) { // if was writer lock
            mutex->unlock();
        }
        else if (status == READER) { // if was reader lock
            mutex->unlock_shared();
        }
    }
private:
    shared_mutex* mutex;
    guardStatus status;
};
