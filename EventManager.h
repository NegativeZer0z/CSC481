#pragma once

#include "Event.h"
#include <queue>
#include <vector>
#include <mutex>
#include "Player.h"
#include "Spawnpoint.h"
#include "SpecialZone.h"
#include <memory>
#include <functional>
#include <unordered_map>
#include "EventHandler.h"

class EventManager {

    private:

        struct compare {
            bool operator()(const Event& e, const Event& e2) const {
                return e.getTime() > e2.getTime();
            }
        };

    public:
        EventManager();

        std::mutex *mutex; //mutex

        std::priority_queue<Event, std::vector<Event>, compare> queue; //prio queue of events

        std::unordered_map<std::string, EventHandler*> handlers;

        void raise(Event e); //raise events by adding to queue

        void onEvent(Event e, std::shared_ptr<Player> player, Spawnpoint* sp); //handle the events

        void registerEvent(std::string s, EventHandler* h);

        void deregisterEvent(std::string s);
};