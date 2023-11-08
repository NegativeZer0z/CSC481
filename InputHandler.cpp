#include "InputHandler.h"

bool InputHandler::onEvent(Event e) {
    if(e.getEventType().find("leftInput") != std::string::npos) {
        player->update(deltaTime, "left");
        return true;
    }
    else if(e.getEventType().find("rightInput") != std::string::npos) {
        player->update(deltaTime, "right");
        return true;
    }
    else if(e.getEventType().find("jumpInput") != std::string::npos) {
        player->update(deltaTime, "jump");
        return true;
    }
    return false;
}