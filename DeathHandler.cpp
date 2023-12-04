#include "DeathHandler.h"

bool DeathHandler::onEvent(Event e) {
    if(e.getEventType() == "deathEvent") {
        player->setState(true);
        player->resetScore();
        //std::cout << "state" << std::endl;
        return true;
    }
    return false;
}