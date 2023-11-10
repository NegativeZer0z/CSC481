CC = g++

platformer: main.o StaticPlatform.o Player.o MovingPlatform.o GeneralPlatform.o Entity.o Thread.o Timeline.o Spawnpoint.o SpecialZone.o Boundary.o Event.o EventManager.o DeathHandler.o SpawnHandler.o CollisionHandler.o InputHandler.o
	$(CC) main.o Entity.o GeneralPlatform.o StaticPlatform.o Player.o MovingPlatform.o Spawnpoint.o SpecialZone.o Thread.o Timeline.o Boundary.o Event.o EventManager.o DeathHandler.o SpawnHandler.o CollisionHandler.o InputHandler.o -o platformer -lsfml-window -lsfml-graphics -lsfml-system -pthread

client: StaticPlatform.o Player.o MovingPlatform.o GeneralPlatform.o Entity.o Thread.o Timeline.o Spawnpoint.o SpecialZone.o client.cpp Boundary.o Event.o EventManager.o DeathHandler.o SpawnHandler.o CollisionHandler.o InputHandler.o
	$(CC) client.cpp Entity.o Spawnpoint.o SpecialZone.o GeneralPlatform.o StaticPlatform.o Player.o MovingPlatform.o Thread.o Timeline.o Boundary.o Event.o EventManager.o DeathHandler.o SpawnHandler.o CollisionHandler.o InputHandler.o -o client -lzmq -lsfml-window -lsfml-graphics -lsfml-system -pthread

server: StaticPlatform.o Player.o MovingPlatform.o GeneralPlatform.o Entity.o Thread.o Timeline.o Spawnpoint.o SpecialZone.o server.cpp Boundary.o Event.o EventManager.o DeathHandler.o SpawnHandler.o CollisionHandler.o InputHandler.o
	$(CC) server.cpp Entity.o Spawnpoint.o SpecialZone.o GeneralPlatform.o StaticPlatform.o Player.o MovingPlatform.o Thread.o Timeline.o Boundary.o Event.o EventManager.o DeathHandler.o SpawnHandler.o CollisionHandler.o InputHandler.o -o server -lzmq -lsfml-window -lsfml-graphics -lsfml-system -pthread

main.o: main.cpp
	$(CC) -c main.cpp

Thread.o: Thread.cpp Thread.h
	$(CC) -c Thread.cpp

Timeline.o: Timeline.h Timeline.cpp
	$(CC) -c Timeline.cpp

Entity.o: Entity.cpp Entity.h
	$(CC) -c Entity.cpp

GeneralPlatform.o: GeneralPlatform.cpp GeneralPlatform.h
	$(CC) -c GeneralPlatform.cpp

StaticPlatform.o: StaticPlatform.cpp StaticPlatform.h
	$(CC) -c StaticPlatform.cpp

Player.o: Player.cpp Player.h
	$(CC) -c Player.cpp

MovingPlatform.o: MovingPlatform.cpp MovingPlatform.h
	$(CC) -c MovingPlatform.cpp

SpecialZone.o: SpecialZone.cpp SpecialZone.h
	$(CC) -c SpecialZone.cpp

Spawnpoint.o: Spawnpoint.cpp Spawnpoint.h
	$(CC) -c Spawnpoint.cpp

Boundary.o: Boundary.cpp Boundary.h
	$(CC) -c Boundary.cpp

Event.o: Event.cpp Event.h
	$(CC) -c Event.cpp

EventManager.o: EventManager.cpp EventManager.h
	$(CC) -c EventManager.cpp

DeathHandler.o: DeathHandler.cpp DeathHandler.h
	$(CC) -c DeathHandler.cpp

SpawnHandler.o: SpawnHandler.cpp SpawnHandler.h
	$(CC) -c SpawnHandler.cpp

InputHandler.o: InputHandler.cpp InputHandler.h
	$(CC) -c InputHandler.cpp

CollisionHandler.o: CollisionHandler.cpp CollisionHandler.h
	$(CC) -c CollisionHandler.cpp

clean:
	rm -f *.o
	rm -f platformer
	rm -f client
	rm -f server