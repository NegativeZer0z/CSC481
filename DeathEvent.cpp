#include "DeathEvent.h"

void DeathEvent::onEvent(Event e) {
    if(e.getEventType() == "deathEvent") {
        player->setState(true);
    }
}