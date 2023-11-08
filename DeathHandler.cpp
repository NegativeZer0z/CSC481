#include "DeathHandler.h"

bool DeathHandler::onEvent(Event e) {
    if(e.getEventType() == "deathEvent") {
        player->setState(true);
        //std::cout << "state" << std::endl;
        return true;
    }
    return false;
}