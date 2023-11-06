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

void Thread::runMovement(std::vector<std::shared_ptr<MovingPlatform>>& moving, std::shared_ptr<Player> player, float deltaTime, std::vector<Entity*>& list, bool move, EventManager *manager) {
    if(identity == 0) { //movement
        try {
            //std::unique_lock<std::mutex> cv_lock(*this->_mutex);
            _mutex->lock();
            if(move) {
                std::string temp = (*player).updateEvent();
                if(temp == "left") {
                    Event e("leftInput", 0);
                    manager->raise(e);
                    
                    InputHandler *ih = new InputHandler;
                    ih->player = player;
                    ih->deltaTime = deltaTime;
                    
                    manager->registerEvent("leftInput", ih);
                }
                else if(temp == "right") {
                    Event e("rightInput", 0);
                    manager->raise(e);
                    
                    InputHandler *ih = new InputHandler;
                    ih->player = player;
                    ih->deltaTime = deltaTime;
                    
                    manager->registerEvent("rightInput", ih);
                }
                else if(temp == "jump") {
                    Event e("jumpInput", 0);
                    manager->raise(e);
                    
                    InputHandler *ih = new InputHandler;
                    ih->player = player;
                    ih->deltaTime = deltaTime;
                    
                    manager->registerEvent("jumpInput", ih);
                }
            }
            // for(int i = 0; i < moving.size(); ++i) {
            //     moving[i]->update(deltaTime);
            // }
            busy = !busy;
            _mutex->unlock();
        }
        catch(...) {
            std::cerr << "Thread " << identity << "caught exception movement." << std::endl;
        }
    }
    else { //identity == 1 collision
        while(other->isBusy()) {
            try {
                //std::unique_lock<std::mutex> lock(*_mutex);
                _mutex->lock();
                for(int i = 0; i < moving.size(); ++i) {
                    moving[i]->checkCollision(player);
                    player->checkCollision(*moving[i]);
                }

                for(int i = 0; i < list.size(); ++i) {
                    (*player).checkCollision(*list[i]);
                }
                _mutex->unlock();
            }
            catch(...) {
                std::cerr << "Thread " << identity << "caught exception collision." << std::endl;
            }
        }
    }
}