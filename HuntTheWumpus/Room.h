#pragma once

#include <iostream>

using namespace std;

enum CONTENT { VUMPUS, BOTTOMLESS_PIT, BAT, EMPTY }; //list of things a room can contain

class Room
{
private:
	int number;
	CONTENT content;
	int X; //room coordinates
	int Y;
public:
	Room(int n, CONTENT c, int x, int y) :number(n), content(c), X(x), Y(y) {}
	Room() :number(0), content(EMPTY), X(0), Y(0){}

	void setContent(CONTENT c) { content = c; }

	int getNumber() const { return number; }
	CONTENT getContent() const { return content; }
	int getRoomX() const { return X; }
	int getRoomY() const { return Y; }

	friend ostream& operator<<(ostream& os, const Room& room) 
	{
		os << room.number << ": " << room.content;
		return os;
	}
	~Room() {}
};

