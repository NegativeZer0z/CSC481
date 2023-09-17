#pragma once

#include "Entity.h"
#include "MovingPlatform.h"
#include "Player.h"
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>

class Thread {

    private:
        bool busy;

        int identity;

        Thread *other;

        std::mutex *_mutex;

        std::condition_variable *_condition_variable;

    public:
        Thread(int i, Thread *other, std::mutex *m, std::condition_variable *cv);

        bool isBusy();

        void runMovement(MovingPlatform *moving, Player *player, float deltaTime);
};
