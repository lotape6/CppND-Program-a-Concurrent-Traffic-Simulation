#include "TrafficLight.h"
#include <cstdlib>
#include <iostream>
#include <random>
#include <thread>

using namespace std::chrono_literals;

/* Implementation of class "MessageQueue" */

template <typename T> T MessageQueue<T>::receive() {
  // FP.5a : The method receive should use std::unique_lock<std::mutex> and

  std::unique_lock<std::mutex> lock(_mtx);

  while (!_msgReceived) {
    _condVar.wait(lock);
  }
  _msgReceived = false;

  auto lastMsg = std::move(_queue.front());
  _queue.pop_front();
  // to wait for and receive new messages and pull them from the queue using
  // The received object should then be returned by the receive function.
  return lastMsg;
}

template <typename T> void MessageQueue<T>::send(T &&msg) {
  // FP.4a : The method send should use the mechanisms
  std::unique_lock<std::mutex> lock(_mtx);

  _queue.emplace_back(std::move(msg));

  _msgReceived = true;
  _condVar.notify_one();

  // as well as _condition.notify_one() to add a new message to the queue
  // and afterwards send a notification.
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight()
    : _currentPhase(static_cast<TrafficLightPhase>(rand() % 2)) {}

void TrafficLight::waitForGreen() {
  while (_currentPhase != TrafficLightPhase::green) {
    _currentPhase = _msgQueue.receive();
  }
}

TrafficLightPhase TrafficLight::getCurrentPhase() { return _currentPhase; }

void TrafficLight::simulate() {
  // FP.2b : Finally, the private method „cycleThroughPhases“ should be started
  // in a thread when the public method „simulate“ is called. To do this, use
  // the thread queue in the base class.

  threads.emplace_back(&TrafficLight::cycleThroughPhases, this);
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases() {

  TimePoint lastToggleStamp;
  std::cout << "Entering cycleThrough" << std::endl;
  lastToggleStamp = std::chrono::system_clock::now();
  while (true) {
    TimePoint now = std::chrono::system_clock::now();
    float elapsedSecs =
        std::chrono::duration_cast<std::chrono::seconds>(now - lastToggleStamp)
            .count();
    if (elapsedSecs >= ((rand() % 2) + 4)) {
      TrafficLightPhase newPhase = (_currentPhase == TrafficLightPhase::red)
                                       ? TrafficLightPhase::green
                                       : TrafficLightPhase::red;

      //   std::cout << "Sending new phase" << std::endl;

      _currentPhase = newPhase;
      _msgQueue.send(std::move(newPhase));
      lastToggleStamp = now;
    }

    std::this_thread::sleep_for(1ms);
  }
  // FP.2a : Implement the function with an infinite loop that measures the time
  // between two loop cycles and toggles the current phase of the traffic light
  // between red and green and sends an update method to the message queue using
  // move semantics. The cycle duration should be a random value between 4 and 6
  // seconds. Also, the while-loop should use std::this_thread::sleep_for to
  // wait 1ms between two cycles.
}
