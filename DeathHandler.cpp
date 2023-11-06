#include "DeathHandler.h"

void DeathHandler::onEvent(Event e) {
    if(e.getEventType() == "deathEvent") {
        player->setState(true);
        std::cout << "state" << std::endl;
    }
}