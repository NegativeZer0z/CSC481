#include "Thread.h"

Thread::Thread(int i, Thread *other, std::mutex *m, std::condition_variable *cv) {
    this->identity = i;
    if(i == 0) {
        this->busy = true;
    }
    else {
        this->other = other;
    }
    this->_mutex = m;
    this->_condition_variable = cv;
}

bool Thread::isBusy() {
    //std::lock_guard<std::mutex> lock(*_mutex);
    return busy;
}

void Thread::runMovement(MovingPlatform *moving, Player *player, float deltaTime, std::vector<Entity>& list) {
    if(identity == 0) { //movement
        try {
            std::unique_lock<std::mutex> cv_lock(*_mutex);
            (*player).update(deltaTime);
            (*moving).update(deltaTime);
            busy = !busy;
        }
        catch(...) {
            std::cerr << "Thread " << identity << "caught exception platform." << std::endl;
        }
    }
    else { //identity == 1 collision
        while(other->isBusy()) {
            try {
                std::unique_lock<std::mutex> lock(*_mutex);
                (*player).wallCollision();
                (*moving).checkCollision(*player);

                for(int i = 0; i < list.size(); ++i) {
                    (*player).checkCollision(list[i]);
                }
            }
            catch(...) {
                std::cerr << "Thread " << identity << "caught exception player." << std::endl;
            }
        }
    }
}