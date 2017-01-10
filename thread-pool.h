/**
 * File: thread-pool.h
 * -------------------
 * This class defines the ThreadPool class, which accepts a collection
 * of thunks (which are zero-argument functions that don't return a value)
 * and schedules them in a FIFO manner to be executed by a constant number
 * of child threads that exist solely to invoke previously scheduled thunks.
 */

#ifndef _thread_pool_
#define _thread_pool_

#include <cstddef>    // for size_t
#include <functional> // for the function template used in the schedule signature
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <atomic>
#include <condition_variable>

#include "semaphore.h"

class ThreadPool
{
public:
  /**
 * Constructs a ThreadPool configured to spawn up to the specified
 * number of threads.
 */
  ThreadPool(size_t numThreads);

  /**
 * Schedules the provided thunk (which is something that can
 * be invoked as a zero-argument function without a return value)
 * to be executed by one of the ThreadPool's threads as soon as
 * all previously scheduled thunks have been handled.
 */
  void schedule(const std::function<void(void)> &thunk);

  /**
 * Blocks and waits until all previously scheduled thunks
 * have been executed in full.
 */
  void wait();

private:
  typedef struct {
    bool is_working;
    bool is_active;
    std::unique_ptr<semaphore> sem_wait_task;
    std::function<void(void)> thunk;
  } worker_t;
  std::vector<worker_t> workers;
  // the task queue
  std::queue<std::function<void(void)> > tasks;
  std::unique_ptr<semaphore> sem_wait_task;
  std::unique_ptr<semaphore> sem_worker_res;
  std::unique_ptr<semaphore> sem_wait;
  std::atomic_int num_of_task;
  std::mutex m;
  std::mutex worker_mutex;
  std::mutex task_mutex;
  std::condition_variable task_condition;
  //size_t aaaaaa;

  void do_task(const size_t i);
  void dispatcher(void);
  /**
 * ThreadPools are the type of thing that shouldn't be cloneable, since it's
 * not clear what it means to clone a ThreadPool (should copies of all outstanding
 * functions to be executed be copied?).
 *
 * In order to prevent cloning, we remove the copy constructor and the
 * assignment operator.  By doing so, the compiler will ensure we never clone
 * a ThreadPool.
 */
  ThreadPool(const ThreadPool &original) = delete;
  ThreadPool &operator=(const ThreadPool &rhs) = delete;
};

#endif
