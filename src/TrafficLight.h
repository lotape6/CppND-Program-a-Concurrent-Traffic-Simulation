#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include "TrafficObject.h"
#include <chrono>
#include <condition_variable>
#include <deque>
#include <mutex>

// forward declarations to avoid include cycle
class Vehicle;

// FP.3 Define a class „MessageQueue“ which has the public methods send and
// receive. Send should take an rvalue reference of type TrafficLightPhase
// whereas receive should return this type. Also, the class should define an
// std::dequeue called _queue, which stores objects of type TrafficLightPhase.
// Also, there should be an std::condition_variable as well as an std::mutex as
// private members.

template <class T> class MessageQueue {
public:
  void send(T &&newVal);
  T receive();

private:
  std::mutex _mtx;
  std::condition_variable _condVar;

  bool _msgReceived = false;
  std::deque<T> _queue;
};

// FP.1 : Define a class „TrafficLight“ which is a child class of TrafficObject.
// The class shall have the public methods „void waitForGreen()“ and „void
// simulate()“ as well as „TrafficLightPhase getCurrentPhase()“, where
// TrafficLightPhase is an enum that can be either „red“ or „green“. Also, add
// the private method „void cycleThroughPhases()“. Furthermore, there shall be
// the private member _currentPhase which can take „red“ or „green“ as its
// value.

using TimePoint = std::chrono::time_point<std::chrono::system_clock>;
enum TrafficLightPhase : uint8_t { red = 0, green = 1 };

class TrafficLight : public TrafficObject {
public:
  // constructor / desctructor
  TrafficLight();
  ~TrafficLight() = default;

  void waitForGreen();
  void simulate();
  TrafficLightPhase getCurrentPhase();

  // getters / setters

  // typical behaviour methods

private:
  void cycleThroughPhases();

  TrafficLightPhase _currentPhase;

  MessageQueue<TrafficLightPhase> _msgQueue;
  // typical behaviour methods

  // FP.4b : create a private member of type MessageQueue for messages of type
  // TrafficLightPhase and use it within the infinite loop to push each new
  // TrafficLightPhase into it by calling send in conjunction with move
  // semantics.

  std::condition_variable _condition;
  std::mutex _mutex;
};

#endif