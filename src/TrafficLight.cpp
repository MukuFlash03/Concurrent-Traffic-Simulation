#include <bits/stdc++.h>
#include <iostream>
#include <random>
#include <future>
#include <queue>
#include <mutex>
#include <chrono>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
  	std::unique_lock<std::mutex> uLock(_mtx);
  _cond.wait(uLock, [this] { return !_queue.empty(); });
  
  T msg = std::move(_queue.back());
  _queue.pop_back();
  
  return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
  std::lock_guard<std::mutex> uLock(_mtx);
  _queue.clear();
  _queue.emplace_back(msg);
  _cond.notify_one();
  
}


/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
	while(true) {
      auto currentPhase = _queue.receive();
      if (currentPhase == green)
        return;
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{ 
  threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{  
  // Cycle time limits in milliseconds(ms)
  double minCycleTime = 4.0 * 1000;
  double maxCycleTime = 6.0 * 1000;
  double currentCycleTime = getRandomCycleDuration(minCycleTime, maxCycleTime);
  
  // Initialize a stop watch with latest time update
  // std::chrono::time_point<std::chrono::system_clock> lastUpdate;
  auto lastUpdate = std::chrono::system_clock::now();
  
  // Infinite while loop to measure time between two loop cycles
  while (true) {
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
    
    auto timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastUpdate).count();
    
    if (timeSinceLastUpdate >= currentCycleTime) {
      
      // Toggle TrafficLight Phase
      _currentPhase = (_currentPhase == TrafficLightPhase::red) ? TrafficLightPhase::green : TrafficLightPhase::red;
      
      // Pass Update to MessageQueue
      auto updateSent = std::async(std::launch::async, &MessageQueue<TrafficLightPhase>::send, &_queue, std::move(_currentPhase));
        updateSent.wait();
      
        
  // Reset Stop Watch
  lastUpdate = std::chrono::system_clock::now();
    }
  }
  
 // Choose cycle duration for next cycle
  currentCycleTime = getRandomCycleDuration(minCycleTime, maxCycleTime);
}

// Helper function for cycleThroughPhases random value generation
double TrafficLight::getRandomCycleDuration(double minDuration, double maxDuration) {

  // Uniformly-distributed integer random number generator that produces non-deterministic random numbers.
  	static std::random_device rd;
  
    // A Mersenne Twister pseudo-random generator of 32-bit numbers with a state size of 19937 bits.
  	static std::mt19937 gen(rd());
  
  // Uniform distribution
  	static std::uniform_real_distribution<double> uniDist(minDuration, maxDuration);
  
  // Return generated random value
  return uniDist(gen);
}

