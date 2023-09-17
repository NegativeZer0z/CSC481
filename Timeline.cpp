#include "Timeline.h"

Timeline::Timeline(Timeline *anchor, int64_t tic) {
    this->anchor = anchor;
    this->tic = tic;
    paused = false;
    startTime = std::chrono::system_clock::now();
}

int64_t Timeline::getTime() {
    std::lock_guard<std::mutex> lock(m);

    if (paused) {

        return std::chrono::duration_cast<std::chrono::seconds>(lastPausedTime - startTime).count();
    } 
    else {
        //find total time elapsed
        int64_t elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - startTime).count();

        //subtract anchor timeline
        if (anchor != nullptr) {
            elapsed -= anchor->getTime();
        }

        elapsed = elapsed / tic;
        return elapsed;
    }
}

void Timeline::pause() {
    std::lock_guard<std::mutex> lock(m);
    lastPausedTime = std::chrono::system_clock::now();
    paused = true;
}

void Timeline::unpause() {
    std::lock_guard<std::mutex> lock(m);
    elapsedPausedTime = std::chrono::system_clock::now() - lastPausedTime;
    paused = false;
}

void Timeline::changeTic(int tic) {
    std::lock_guard<std::mutex> lock(m);
    this->tic = tic;
}

bool Timeline::isPaused() {
    std::lock_guard<std::mutex> lock(m);
    return paused;
}
