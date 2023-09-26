#include <mutex>
#include <condition_variable>
#include <thread>
#include <SFML/System.hpp>
#include <iostream>

class Timeline {

    private:
        std::mutex m; //mutex due to being multithreaded and user can change tics

        int64_t tic; //the tic size

        float startTime; //initial start time of the timeline

        float elapsedPausedTime; //the total elapsed time between pause and unpause

        float lastPausedTime; //the last time the game was paused

        bool paused; //is the game paused or not

        Timeline *anchor; //anchor to another timeline

        sf::Clock clock; //clock to keep track of time

    public:
        Timeline(Timeline *anchor, int64_t tic); //constructor

        float getTime(); //get the time pasted

        void pause(); //pause the game

        void unpause(); //unpause the game

        void changeTic(int tic); //change the tic rate

        bool isPaused(); //check to see if the game is paused

        int64_t getTic(); //return the current tic of timeline
};