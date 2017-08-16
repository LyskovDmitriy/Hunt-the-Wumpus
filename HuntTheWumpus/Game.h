#pragma once

#include "Room.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <list>
#include <fstream>
#include <conio.h>

struct Line //a structure for lines. Is used to fill the map of the dungeon
{
	char lineType; //h - horisontal, v - vertical
	short int X; //X, Y - coordinates of line's first room
	short int Y;
	short int length;
	Line() : lineType('h'), X(0), Y(0), length(0)
	{}
	Line(char lT, int x, int y, int l) :lineType(lT), X(x), Y(y), length(l)
	{}
};

class Game
{
private:
	short int NOTROOM = -1; //constants that are used to fill the map
	short int ROOM = 0;

	short int arrows; //number of arrows. The game is lost when you run out of arrows
	short int playerX;// player coordinates
	short int playerY;

	bool killedVumpus; //marks whether vumpus is killed

	short int** dungeonMap; //visual map of the dungeon
	short int** playerMap; // is dynamically drawn
	
	int mapX; //length of the map
	int mapY; //height of the map
	vector<Room> rooms; //vector that contains all rooms of the dungeon

	Line getRandomLine(int randomizer = 0, char lineType = ' ', int X = -1, int Y = -1, int length = 0) const; //returns a line with random parameters
	
	bool wakeVumpus(int x, int y); //is used when player shoots. Makes vumpus move to adjacent room
	void checkBat();//interaction with a bat

public:
	Game(short int x, short int y);

	void makeAnAction();//is called each turn
	void move(char ch);//for WASD
	void shoot();//shoot in particular direction
	void shootRoom(int x, int y);

	void fillMap(); //fills dungeon map with empty rooms
	void fillRooms(); // fills vector rooms with empty rooms
	void updatePlayerMap(); //adds adjacent rooms to playerMap

	void addCircle(); //adds a frame of lines around the map
	void addLine(Line line); //draws a line on visual map
	void deleteDeadEnds(); //deletes all rooms that have only 1 adjacent room (are dead ends)

	bool checkLine(Line line) const; //checks whether line has adjacent rooms
	vector<const Room*> checkAdjacentRooms(int x, int y) const; //returns a vector of pointers to adjacent rooms
	const Room* checkRoom(int x, int y) const; //returns a pointer to the room

	void printPlayerMap() const; //draws a playerMap
	void printActualMap() const; //draws a dungeonMap
	bool vumpusIsKilled() const; //returns killedVumpus
	bool checkPlayerDead() const; //player is dead when he falls in a pit or meets vumpus
	static void printRules(); //prints the rules of the game
	
	~Game();
};

