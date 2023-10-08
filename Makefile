CC = g++

platformer: main.o StaticPlatform.o Player.o MovingPlatform.o GeneralPlatform.o Entity.o Thread.o Timeline.o
	$(CC) main.o Entity.o GeneralPlatform.o StaticPlatform.o Player.o MovingPlatform.o Thread.o Timeline.o -o platformer -lsfml-window -lsfml-graphics -lsfml-system -pthread

client: StaticPlatform.o Player.o MovingPlatform.o GeneralPlatform.o Entity.o Thread.o Timeline.o client.cpp
	$(CC) client.cpp Entity.o GeneralPlatform.o StaticPlatform.o Player.o MovingPlatform.o Thread.o Timeline.o -o client -lzmq -lsfml-window -lsfml-graphics -lsfml-system -pthread

server: StaticPlatform.o Player.o MovingPlatform.o GeneralPlatform.o Entity.o Thread.o Timeline.o server.cpp
	$(CC) server.cpp Entity.o GeneralPlatform.o StaticPlatform.o Player.o MovingPlatform.o Thread.o Timeline.o -o server -lzmq -lsfml-window -lsfml-graphics -lsfml-system -pthread

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

clean:
	rm -f *.o
	rm -f platformer
	rm -f client
	rm -f server