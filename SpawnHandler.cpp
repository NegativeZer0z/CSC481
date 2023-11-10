#include "SpawnHandler.h"

bool SpawnHandler::onEvent(Event e) {
    if(e.getEventType() == "spawnEvent") {
        sp->spawn(player);
        return true;
    }
    return false;
}