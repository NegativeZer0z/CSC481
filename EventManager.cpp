#include "EventManager.h"

EventManager::EventManager() {
    //default constructor
}

void EventManager::raise(Event e) {
    std::lock_guard<std::mutex> lock(*mutex);
    queue.push(e);
}

void EventManager::onEvent(Event e, std::shared_ptr<Player> player, Spawnpoint* sp) {
    if(e.getEventType() == "collisionEvent") {

    }
    else if(e.getEventType() == "inputEvent") {

    }
    else if(e.getEventType() == "deathEvent") {
        player->setState(true);
    }
    else if(e.getEventType() == "spawnEvent") {
        sp->spawn(player);
    } 
}

void EventManager::registerEvent(std::string s, EventHandler *h) {
    handlers[s] = h;
}

void EventManager::deregisterEvent(std::string s) {
    handlers.erase(s);
}
