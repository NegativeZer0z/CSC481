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

void Thread::runMovement(MovingPlatform *moving, Player *player, float deltaTime) {
    if(identity == 0) {
        try {
            std::unique_lock<std::mutex> cv_lock(*this->_mutex);
            (*player).update(deltaTime);
            busy = !busy;
            //_condition_variable->notify_all();
        }
        catch(...) {
            std::cerr << "Thread " << identity << "caught exception platform." << std::endl;
        }
    }
    else { //identity == 1
        while(other->isBusy()) {
            try {
                std::unique_lock<std::mutex> lock(*_mutex);
                (*moving).update(deltaTime);
                //(*moving).checkCollision(*player);
                (*moving).checkBoundaries();
            }
            catch(...) {
                std::cerr << "Thread " << identity << "caught exception player." << std::endl;
            }
        }
    }
}

