#include "Timeline.h"

Timeline::Timeline(Timeline *anchor, int64_t tic) {
    this->anchor = anchor;
    this->tic = tic;
    paused = false;
    //startTime = std::chrono::system_clock::now();
    startTime = clock.getElapsedTime().asSeconds();
    elapsedPausedTime = 0.0f;
    lastPausedTime = 0.0f;
}

float Timeline::getTime() {
    std::lock_guard<std::mutex> lock(m);

    float time = 0.0f;
    
    //check to see if the game is paused or not initially (fix this)
    if(paused) {
        return time;
    }

    //check to see if there is an anchor (global), in own timeline
    if(anchor == nullptr) {
        time = ((clock.getElapsedTime().asSeconds() - startTime) / tic) - elapsedPausedTime;
        return time;
    }
    else { //has anchor, in anchor timeline
        time = ((*anchor).getTime() - startTime - elapsedPausedTime) / tic;
        return time;
    }
}

void Timeline::pause() {
    std::lock_guard<std::mutex> lock(m);
    paused = true;
    lastPausedTime = clock.getElapsedTime().asSeconds();
    elapsedPausedTime = 0.0f;
    //lastPausedTime = std::chrono::system_clock::now();
}

void Timeline::unpause() {
    std::lock_guard<std::mutex> lock(m);
    paused = false;
    elapsedPausedTime = clock.getElapsedTime().asSeconds() - lastPausedTime;
    //elapsedPausedTime = std::chrono::system_clock::now() - lastPausedTime;
}

void Timeline::changeTic(int tic) {
    std::lock_guard<std::mutex> lock(m);
    this->tic = tic;
}

bool Timeline::isPaused() {
    std::lock_guard<std::mutex> lock(m);
    return paused;
}
