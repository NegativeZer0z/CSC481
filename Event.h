#pragma once

#include <string>

class Event {

    private:

        std::string type; //the type of event

        float time; //for prio queue handling, a time for handling events
    
    public:

        Event(std::string e, float t); //constructor for an event

        std::string getEventType(); //get the type of event

        float getTime() const; //get time of event needed to be handled
};