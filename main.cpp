#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

constexpr int kPhilosopherCount = 5;
constexpr int kEatingTimes = 10;

std::vector<std::mutex> chopstics(kPhilosopherCount);

class Philosopher {
 public:
  explicit Philosopher(int index) : index_(index) {}

  void operator()() {
    std::unique_lock<std::mutex> left_lock(chopstics[LeftChopstic()], std::defer_lock);
    std::unique_lock<std::mutex> right_lock(chopstics[RightChopstic()], std::defer_lock);

    int i = 0;
    while (true) {
      // 只有左右筷子都可用时，才能开始吃, 避免死锁, 如果左筷子可用但右筷子不可用时，必须释放右筷子
      if (left_lock.try_lock()) {
        if (right_lock.try_lock()) {
          printf("Philosopher %d starts eating\n", index_);
          std::this_thread::sleep_for(std::chrono::seconds(1));
          printf("Philosopher %d finishes eating\n", index_);
          priority_++;
          i++;
          right_lock.unlock();
        }
        left_lock.unlock();
      }
      if (i == kEatingTimes) {
        break;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(50 * priority_));
    }
  }
 private:
  inline int LeftChopstic() {
    return index_;
  }

  inline int RightChopstic() {
    return (index_ + 1) % kPhilosopherCount;
  }
 private:
  int index_;
  int priority_{1}; // 初始为1，值越大，需要等待的时间越长, 避免饥饿
};

int main() {
  std::vector<std::thread> threads;
  threads.reserve(kPhilosopherCount);
  for (int i = 0; i < kPhilosopherCount; i++) {
    threads.emplace_back(Philosopher(i));
  }

  for (int i = 0; i < kPhilosopherCount; i++) {
    threads[i].join();
  }

  return 0;
}