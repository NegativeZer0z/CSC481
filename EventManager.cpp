#include "EventManager.h"

EventManager::EventManager() {
    //default constructor
}

void EventManager::raise(Event e) {
    std::lock_guard<std::mutex> lock(*mutex);
    queue.push(e);
}

void EventManager::registerEvent(std::string s, EventHandler *h) {
    handlers[s] = h;
}

void EventManager::deregisterEvent(std::string s) {
    handlers.erase(s);
}
