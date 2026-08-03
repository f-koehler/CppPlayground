#ifndef CPP_PLAYGROUND_THREADING_MUTEX_HPP
#define CPP_PLAYGROUND_THREADING_MUTEX_HPP

#include <cerrno>
#include <system_error>
extern "C" {
#include <pthread.h>
}

namespace CppPlayground {

class PThreadsMutex {
private:
  pthread_mutex_t m_mutex{};

public:
  PThreadsMutex() {
    pthread_mutexattr_t attr{};
    if (int err = pthread_mutexattr_init(&attr); err != 0) {
      throw std::system_error(err, std::system_category(),
                              "pthread_mutexattr_init");
    }

    try {
      // PTHREAD_MUTEX_ERRORCHECK: detects deadlock scenario
      // PTHREAD_MUTEX_NORMAL: does not detect deadlock scenario
      if (int err = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
          err != 0) {
        throw std::system_error(err, std::system_category(),
                                "pthread_mutexattr_settype");
      }

      if (int err = pthread_mutex_init(&m_mutex, &attr); err != 0) {
        throw std::system_error(err, std::system_category(),
                                "pthread_mutex_init");
      }

    } catch (...) {
      pthread_mutexattr_destroy(&attr);
      throw;
    }

    pthread_mutexattr_destroy(&attr);
  }
  ~PThreadsMutex() { pthread_mutex_destroy(&m_mutex); }

  PThreadsMutex(const PThreadsMutex &) = delete;
  PThreadsMutex(PThreadsMutex &&) = delete;
  PThreadsMutex &operator=(const PThreadsMutex &) = delete;
  PThreadsMutex &operator=(PThreadsMutex &&) = delete;

  void lock() {
    if (int err = pthread_mutex_lock(&m_mutex); err != 0) {
      throw std::system_error(err, std::system_category(),
                              "pthread_mutex_lock");
    }
  }
  bool try_lock() {
    int err = pthread_mutex_trylock(&m_mutex);
    if (err == 0) {
      return true;
    }
    if (err == EBUSY) {
      return false;
    }
    throw std::system_error(err, std::system_category(),
                            "pthread_mutex_trylock");
  }
  void unlock() {
    if (int err = pthread_mutex_unlock(&m_mutex); err != 0) {
      // should only happen if we try to call it on uninitialized m_mutex, which
      // can only happen if the constructor has thrown and access to this object
      // is UB
      throw std::system_error(err, std::system_category(),
                              "pthread_mutex_unlock");
    }
  }

  pthread_mutex_t &native_handle() { return m_mutex; }
};

} // namespace CppPlayground

#endif
