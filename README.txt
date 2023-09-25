CSC481 Kevin Zhang

To run the game:

    1. Run Ubuntu 20.04 on your device through the terminal

    2. Unzip all the contents in the zip file into your desired location

    3. Navigate to the directory where you unzipped the contents in your terminal on Ubuntu

    4. Enter "make clean" into the terminal and it should display this in the terminal: 
        rm -f Player.o
        rm -f MovingPlatform.o
        rm -f StaticPlatform.o
        rm -f GeneralPlatform.o
        rm -f Entity.o
        rm -f main.o
        rm -f platformer

    5. Enter "make platformer" into the terminal and it should display something like the following in the terminal:
        g++ -c main.cpp
        g++ -c StaticPlatform.cpp
        g++ -c Player.cpp
        g++ -c MovingPlatform.cpp
        g++ -c GeneralPlatform.cpp
        g++ -c Entity.cpp
        g++ main.o Entity.o GeneralPlatform.o StaticPlatform.o Player.o MovingPlatform.o -o platformer -lsfml-window -lsfml-graphics -lsfml-system

    6. Enter "make client" into the terminal and then after that enter "make server" into the terminal\

    7a. To run the server/client multiplayer game enter "./server" on one terminal and "./client" on another terminal

    7b. Enter "./platformer" into the terminal to do singleplayer
        Note: the entire window may not be on your screen on the first execution of it, if so drag it on the screen


Game Controls:
    Move left: "left arrow" or "a"
    Move right: "right arrow" or "d"
    Jump: "Space", "w", or "up arrow"
    Switch between constant and proportional scaling: "z"
    Speedup: "L"
    Normal Speed: "K"
    Half Speed: "J"


Texture/Art Credits:
    Within the textures file there should be 2 png files -- grass.png and mage.png

    grass.png:
    "29 grounds and walls (and water) (1024x1024) - Grass1.png" by Mysteryem licensed GPL 2.0, GPL 3.0, CC-BY-SA 3.0
    https://opengameart.org/node/8054

    mage.png:
    Four characters: My LPC entries" by Redshrike licensed CC-BY 3.0, CC-BY-SA 3.0, OGA_BY 3.0
    https://opengameart.org/content/four-characters-my-lpc-entries
    note: downloaded and utilize the "mage walking poses sheet copy.png"