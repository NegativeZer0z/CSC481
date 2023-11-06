#include "InputHandler.h"

void InputHandler::onEvent(Event e) {
    if(e.getEventType().find("leftInput") != std::string::npos) {
        player->update(deltaTime, "left");
    }
    else if(e.getEventType().find("rightInput") != std::string::npos) {
        player->update(deltaTime, "right");
    }
    else if(e.getEventType().find("jumpInput") != std::string::npos) {
        player->update(deltaTime, "jump");
    }
}