#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;


template <class T>
class MessageQueue
{
public:
	T receive();
    void send(T &&msg);
    
private:
	std::mutex _mtx;
    std::condition_variable _cond;
    std::deque<T> _queue;
    
};

enum TrafficLightPhase { red, green };

class TrafficLight : TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight();

    // getters / setters
    TrafficLightPhase getCurrentPhase();

    // typical behaviour methods
    void waitForGreen();
    void simulate();

private:
    // typical behaviour methods
    void cycleThroughPhases();
    
    // Member Variables
    TrafficLightPhase _currentPhase;
    
    std::condition_variable _condition;
    std::mutex _mutex;
    
    double getRandomCycleDuration(double minDuration, double maxDuration);

	MessageQueue<TrafficLightPhase> _queue;
    
};

#endif