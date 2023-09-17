CC = g++

platformer: main.o StaticPlatform.o Player.o MovingPlatform.o GeneralPlatform.o Entity.o Thread.o Timeline.o
	$(CC) main.o Entity.o GeneralPlatform.o StaticPlatform.o Player.o MovingPlatform.o Thread.o Timeline.o -o platformer -lsfml-window -lsfml-graphics -lsfml-system -pthread

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
	rm -f Player.o
	rm -f MovingPlatform.o
	rm -f StaticPlatform.o
	rm -f GeneralPlatform.o
	rm -f Entity.o
	rm -f main.o
	rm -f Thread.o
	rm -f platformer