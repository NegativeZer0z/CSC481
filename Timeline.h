#include <chrono>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <SFML/System.hpp>

class Timeline {

    private:
        std::mutex m;

        std::chrono::time_point<std::chrono::system_clock> startTime;
        
        std::chrono::duration<int64_t, std::nano> elapsedPausedTime;

        std::chrono::time_point<std::chrono::system_clock> lastPausedTime;

        int64_t tic;

        bool paused;

        Timeline *anchor;

    public:
        Timeline(Timeline *anchor, int64_t tic);

        int64_t getTime();

        void pause();

        void unpause();

        void changeTic(int tic);

        bool isPaused();
};