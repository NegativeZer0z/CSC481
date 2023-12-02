#include <SFML/Window.hpp>
#include "Player.h"
#include "Timeline.h"
#include <zmq.hpp>
#include <unordered_map>
#include "StaticPlatform.h"
#include "MovingPlatform.h"
#include <vector>
#include <memory>
#include "Spawnpoint.h"
#include "SpecialZone.h"
#include <pthread.h>
#include <unistd.h>
#include <cassert>
#include "Boundary.h"
#include "Event.h"
#include "EventManager.h"
#include "DeathHandler.h"
#include "SpawnHandler.h"
#include <mutex>

#define SEND zmq::send_flags::none
#define REPLY zmq::recv_flags::none

int main() {
    return 0;
}