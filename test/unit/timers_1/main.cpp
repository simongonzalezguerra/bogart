#include "bogart/async/message_queue.hpp"
#include "bogart/async/timer.hpp"

#include <iostream>
#include <chrono>

bogart::async::message_queue q;
bool done = false;

void func1() {
  std::cout << "Func1\n";
}

void func2() {
  std::cout << "Func2\n";
  done = true;
}

void timer_handler1() {
  std::cout << "Timer 1\n";
}

void timer_handler2() {
  std::cout << "Timer 2\n";
}

void timer_handler3() {
  std::cout << "Timer 3\n";
  q.post(bogart::async::make_callable(func1));
  q.post(bogart::async::make_callable(func1));
  q.post(bogart::async::make_callable(func2));
}

int main() {
  bogart::async::timer tm1(q);
  bogart::async::timer tm2(q);
  bogart::async::timer tm3(q);
  bogart::async::timer::time_point now = std::chrono::high_resolution_clock::now();
  tm2.async_wait(now + std::chrono::seconds(2), bogart::async::make_callable(timer_handler2));
  tm3.async_wait(now + std::chrono::seconds(3), bogart::async::make_callable(timer_handler3));
  tm1.async_wait(now + std::chrono::seconds(1), bogart::async::make_callable(timer_handler1));

  // run() will timeout every half a second while waiting for the timers to be dispatched, but we keep running with an external flag
  while (!done) {
    q.run(std::chrono::milliseconds(500));
  }

  return 0;
}
