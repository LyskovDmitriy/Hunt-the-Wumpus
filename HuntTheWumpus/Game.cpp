#pragma once

#include "Game.h"

Game::Game(short int x,short int y) : mapX(x), mapY(y), killedVumpus(false), arrows(5)
{
	dungeonMap = new short int*[mapX];
	for (int i = 0;i < mapX;i++)
	{
		dungeonMap[i] = new short int[mapY];
		for (int j = 0;j < mapY;j++)
		{
			dungeonMap[i][j] = NOTROOM; //creates a map filled with ' '
		}
	}

	playerMap = new short int*[mapX];
	for (int i = 0;i < mapX;i++)
	{
		playerMap[i] = new short int[mapY];
		for (int j = 0;j < mapY;j++)
		{
			playerMap[i][j] = NOTROOM;
		}
	}

	addCircle();
	rooms.push_back(Room()); //for deleteDeadEnds
	fillMap();
	deleteDeadEnds();
	rooms.clear();
	fillRooms();
}

void Game::makeAnAction()
{
	vector<const Room*>adjacentRooms = checkAdjacentRooms(playerX, playerY);
	cout << " Введите \'q\', чтобы стрелять или \'h\', чтобы ознакомиться с правилами" << endl;
	cout << " У вас осталось " << arrows << " стрел";
	if (arrows == 1)
		cout << "а";
	else if (arrows < 5)
		cout << "ы";
	cout << endl;

	string surroundings = ""; //displays content of adjacent rooms
	for (int i = 0;i < adjacentRooms.size();i++)
	{
		switch (adjacentRooms[i]->getContent())
		{
		case EMPTY:
			break;
		case BAT:
			surroundings += " Я слышу летучую мышь\n";
			break;
		case BOTTOMLESS_PIT:
			surroundings += " Я чувствую ветерок\n";
			break;
		case VUMPUS:
			surroundings += " Я чувствую запах вампуса\n";
		}
	}
	cout << endl;
	if (surroundings!="")
		cout << surroundings.c_str() << endl;

	char answer = 'n';
	while (answer=_getch())
	{
		bool madeAction = false;//stays false if player press wrong button
		switch (answer)
		{
		case 'w':
		case 'a':
		case 's':
		case 'd':
		case 72:
		case 75:
		case 77:
		case 80:
			move(answer);
			madeAction = true;
			break;
		case -32:
			continue;
		case 'q':
			shoot();
			madeAction = true;
			break;
		case 'h':
			cout << endl;
			Game::printRules();
			madeAction = true;
			cout << endl;
			cout<< " "; system("PAUSE");
			break;
		default:
			cout << " Ошибка ввода";
		}
		cout << endl;
		if (madeAction)
			break;
	}
}

void Game::move(char ch)
{
	int x = playerX;
	int y = playerY;
	switch (ch)
	{
	case 'w':
	case 72:
		x--;
		break;
	case 'a':
	case 75:
		y--;
		break;
	case 's':
	case 80:
		x++;
		break;
	case 'd':
	case 77:
		y++;
	}
	if ((x < 0 || y < 0) || (x >= mapX || y >= mapY) || dungeonMap[x][y] == NOTROOM)
		return;
	playerX = x;
	playerY = y;
	checkBat();
	updatePlayerMap();
}

void Game::shoot()
{
	cout << " Выберите направление для стрельбы (нажмите \'n\', чтобы отменить выстрел) " << endl;
	char answer = '0';
	while (answer = _getch())
	{
		bool playerShot = true;
		int dirX = 0;
		int dirY = 0;
		switch (answer)
		{
		case -32:
			continue;
		case 'w':
		case 72:
			dirX--;
			break;
		case 'a':
		case 75:
			dirY--;
			break;
		case 's':
		case 80:
			dirX++;
			break;
		case 'd':
		case 77:
			dirY++;
			break;
		case 'n':
			return;
		default:
			cout << " Ошибка. Выберите направление стрельбы с помощью клавиш WASD или стрелок" << endl;
			playerShot = false;
		}
		if (!playerShot)//if player pressed wrong button
			continue;
		arrows--;
		int x = playerX + dirX;
		int y = playerY + dirY;
		for (int i = 0;i < 3;x += dirX, y += dirY, i++)
		{
			shootRoom(x, y);
		}

		bool vumpusWoken = wakeVumpus(playerX, playerY);
		for (int i = 0;i < 3 && !vumpusWoken;x -= dirX, y -= dirY, i++)
		{
			vumpusWoken = wakeVumpus(x, y);
		}
		return;
	}
}

void Game::shootRoom(int x, int y)
{
	if ((x < 0 || y < 0) || (x >= mapX || y >= mapY) || dungeonMap[x][y] == NOTROOM) //checks room coordinates
		return;
	//cout << "Shot room " << dungeonMap[x][y] << endl;
	int roomNumber = dungeonMap[x][y] - 1;
	if (rooms[roomNumber].getContent() == EMPTY)//does nothing with empty rooms
		return;
	if (rooms[roomNumber].getContent() == BAT)//bat is killed
		rooms[roomNumber].setContent(EMPTY);
	else if (rooms[roomNumber].getContent() == VUMPUS)
	{
		killedVumpus = true;
		rooms[roomNumber].setContent(EMPTY);
	}
}

bool Game::wakeVumpus(int x, int y)
{
	vector<const Room*>adjacentRooms = checkAdjacentRooms(x, y);
	for (int i = 0;i < adjacentRooms.size();i++)
	{
		if (adjacentRooms[i]->getContent() == VUMPUS)//vumpus moves to an empty room if he is in adjacent one
		{
			x = adjacentRooms[i]->getRoomX();
			y = adjacentRooms[i]->getRoomY();
			adjacentRooms = checkAdjacentRooms(x, y);
			while (true)
			{
				int room = rand() % adjacentRooms.size();
				if (adjacentRooms[room]->getContent() == EMPTY)
				{
					rooms[adjacentRooms[room]->getNumber() - 1].setContent(VUMPUS);
					rooms[dungeonMap[x][y] - 1].setContent(EMPTY);
					return true;
				}
			}
		}
	}
	return false;
}

void Game::checkBat()
{
	if (rooms[dungeonMap[playerX][playerY] - 1].getContent() == BAT) //if player meets bat it moves him to random empty room
	{
		while (true)
		{
			int newRoom = rand() % rooms.size();
			if (rooms[newRoom].getContent() == EMPTY)
			{
				playerX = rooms[newRoom].getRoomX();
				playerY = rooms[newRoom].getRoomY();
				cout << " Вас схватила летучая мышь" << endl;
				cout<< " "; system("PAUSE");
				return;
			}
		}
	}
}

void Game::fillMap()
{
	for (int i = 0;i < ((mapX + mapY) / 1.4);i++)
	{
		Line line;
		line = getRandomLine(i);

		while (true)
		{
			if (checkLine(line)) //if it is connected, it is added to the map
			{
				addLine(line);
				break;
			}
			else
			{
				if (i % 2 == 0) // 1 of 2 lines is made longer, the other is moved up or left
				{
					if (line.lineType == 'v')
						line.X--;
					else
						line.Y--;
				}
				else
					line.length++;
			}
		}

	}
}

void Game::fillRooms()
{
	short int currentSize = 0;
	for (int i = 0;i < mapX;i++)
	{
		for (int j = 0;j < mapY;j++)
		{
			if (dungeonMap[i][j] == ROOM)
			{
				dungeonMap[i][j] = ++currentSize;
				rooms.push_back(Room(currentSize, EMPTY, i, j));//fills vector with empty rooms
			}
		}
	}

	//insert Vumpus
	rooms[rand() % rooms.size()].setContent(VUMPUS);
	//insert Bats
	for (int i = 0;i < rooms.size() / 10;i++)
	{
		int roomNumber = rand() % rooms.size();
		if (rooms[roomNumber].getContent() == EMPTY)
			rooms[roomNumber].setContent(BAT);
		else
			i--;
	}
	//insert Pits
	for (int i = 0;i < rooms.size() / 10;i++)
	{
		int roomNumber = rand() % rooms.size();
		if (rooms[roomNumber].getContent() == EMPTY)
			rooms[roomNumber].setContent(BOTTOMLESS_PIT);
		else
			i--;
	}
	//insert Player
	bool playerAdded = false;
	while (!playerAdded)
	{
		int roomNumber = rand() % rooms.size();
		if (rooms[roomNumber].getContent() == EMPTY)
		{
			playerX = rooms[roomNumber].getRoomX();
			playerY = rooms[roomNumber].getRoomY();
			playerAdded = true;
		}
	}
	updatePlayerMap();
}

void Game::updatePlayerMap()
{
	auto adjacentRooms = checkAdjacentRooms(playerX, playerY);
	for (int i = 0;i < adjacentRooms.size();i++)
	{
		int x = adjacentRooms[i]->getRoomX();
		int y = adjacentRooms[i]->getRoomY();
		playerMap[x][y] = dungeonMap[x][y];
	}
	playerMap[playerX][playerY] = dungeonMap[playerX][playerY];
}

void Game::addCircle()
{
	addLine(Line('h', 0, 0, mapY));
	addLine(Line('v', 0, 0, mapX));
	addLine(Line('v', 0, mapY - 1, mapX));
	addLine(Line('h', mapX - 1, 0, mapY));
}

Line Game::getRandomLine(int randomizer, char lineType, int X, int Y, int length) const
{
	srand(time(NULL) + randomizer);
	Line line(lineType, X, Y, length);
	if (line.lineType == ' ')
	{
		if (rand() % 2 == 1)
			line.lineType = 'h'; //line is horisontal
		else
			line.lineType = 'v'; //line is vertical
	}
	if (line.X == -1)
	{
		line.X = rand() % (mapX - 2); //to make lines start left and up
	}
	if (line.Y == -1)
	{
		line.Y = rand() % (mapY - 2);
	}
	if (line.length <= 0)
	{
		if (line.lineType == 'h')
			line.length = rand() % (mapX - 3) + 3;
		else
			line.length = rand() % (mapY - 3) + 3;
	}
	return line;
}

void Game::addLine(Line line)
{
	int x = line.X, y = line.Y;
	for (int i = 0;i < line.length;i++)
	{
		if (dungeonMap[x][y] == NOTROOM)
			dungeonMap[x][y] = ROOM;
		if (line.lineType == 'h')
			y++;
		else
			x++;
		if (y == mapY || x == mapX)
			return;
	}
}

void Game::deleteDeadEnds()
{
	for (int i = 0;i < mapX;i++)
	{
		for (int j = 0;j < mapY;j++)
		{
			if (checkAdjacentRooms(i, j).size() == 1) //deletes a room if it has only 1 adjacent room (is a dead end)
				dungeonMap[i][j] = NOTROOM;
		}
	}
	for (int i = mapX - 1;i > 0;i--)
	{
		for (int j = mapY - 1;j > 0;j--)
		{
			if (checkAdjacentRooms(i, j).size() == 1) 
				dungeonMap[i][j] = NOTROOM;
		}
	}
}

bool Game::checkLine(Line line) const
{
	for (int j = 0, x = line.X, y = line.Y;j < line.length;j++)
	{
		if (checkAdjacentRooms(x, y).size() > 0)
			return true;
		if (line.lineType == 'h')
			y++;
		else
			x++;
	}
	return false;
}

vector<const Room*> Game::checkAdjacentRooms(int x, int y) const
{
	vector<const Room*>adjacentRooms(0);
	const Room* room = nullptr;
	if (x - 1 >= 0)
	{
		room = checkRoom(x - 1, y);
		if (room != nullptr)
		{
			adjacentRooms.push_back(room);
			room = nullptr;
		}
	}
	if (y - 1 >= 0)
	{
		room = checkRoom(x, y - 1);
		if (room != nullptr)
		{
			adjacentRooms.push_back(room);
			room = nullptr;
		}
	}
	if (y + 1 < mapY)
	{
		room = checkRoom(x, y + 1);
		if (room != nullptr)
		{
			adjacentRooms.push_back(room);
			room = nullptr;
		}
	}
	if (x + 1 < mapX)
	{
		room = checkRoom(x + 1, y);
		if (room != nullptr)
		{
			adjacentRooms.push_back(room);
			room = nullptr;
		}
	}
	return adjacentRooms;
}

const Room* Game::checkRoom(int x, int y) const
{
	if ((x < 0 || x >= mapX) || (y < 0 || y >= mapY))
		return nullptr;
	if (dungeonMap[x][y] == NOTROOM)
		return nullptr;
	const Room* room = nullptr;
	if (rooms.size() == 1)
		room = &rooms[0];
	else
		room = &rooms[dungeonMap[x][y] - 1];
	return room;
}

void Game::printPlayerMap() const
{
	cout << endl;
	short int bufferSize = 3; //to print room numbers properly
	if (rooms.size() < 100)
		bufferSize = 2;

	cout << "  ";
	double max = 2.0;
	if (bufferSize == 3)
		max = 2.5;
	for (int i = 0;i < mapY * max;i++)
			cout << " -";
	cout << endl;

	for (int i = 0;i < mapX;i++)
	{
		cout << "  |";
		for (int j = 0;j < mapY;j++)
		{
			cout << " " << setw(bufferSize);
			if (playerMap[i][j] != NOTROOM)
			{
				if (i == playerX && j == playerY)
					cout << ":)";
				else
					cout << playerMap[i][j];
			}
			else
				cout << " ";
			cout << " ";
		}
		cout << "|" << endl;
	}

	cout << "  ";
	max = 2.0;
	if (bufferSize == 3)
		max = 2.5;
	for (int i = 0;i < mapY * max;i++)
		cout << " -";
	cout << endl;
	cout << endl;
}

void Game::printActualMap() const
{
	cout << endl;
	short int bufferSize = 3; //to print room numbers properly
	if (rooms.size() < 100)
		bufferSize = 2;

	cout << "  ";
	double max = 2.0;
	if (bufferSize == 3)
		max = 2.5;
	for (int i = 0;i < mapY * max;i++)
		cout << " -";
	cout << endl;

	for (int i = 0;i < mapX;i++)
	{
		cout << "  |";
		for (int j = 0;j < mapY;j++)
		{
			cout << " " << setw(bufferSize);
			if (dungeonMap[i][j] != NOTROOM)
			{
				if (i == playerX && j == playerY)
					cout << ":)";
				else
					cout << dungeonMap[i][j];
			}
			else
				cout << " ";
			cout << " ";
		}
		cout << "|" << endl;
	}

	cout << "  ";
	max = 2.0;
	if (bufferSize == 3)
		max = 2.5;
	for (int i = 0;i < mapY * max;i++)
		cout << " -";
	cout << endl;
	cout << endl;
	
	short int currentSize = 0;
	for (int i = 0;i < mapX;i++)
	{
		for (int j = 0;j < mapY;j++)
		{
			if (dungeonMap[i][j] != NOTROOM)
			{
				cout << rooms[currentSize++] << endl;
			}
		}
	}
}

bool Game::vumpusIsKilled() const
{
	return killedVumpus;
}

bool Game::checkPlayerDead() const
{
	if (rooms[dungeonMap[playerX][playerY] - 1].getContent() == VUMPUS)
	{
		cout << " Вас съел Вампус" << endl;
		return true;
	}
	else if (rooms[dungeonMap[playerX][playerY] - 1].getContent() == BOTTOMLESS_PIT)
	{
		cout << " Вы упали в яму" << endl;
		return true;
	}
	else if (arrows == 0)
	{
		cout << " У вас закончились стрелы" << endl;
		return true;
	}
	return false;
}

void Game::printRules()
{
	ifstream is("rules.txt");
	cout << is.rdbuf();
}

Game::~Game()
{
	for (int i = 0;i < mapX;i++)
	{
		delete[]dungeonMap[i];
	}
	delete[] dungeonMap;

	for (int i = 0;i < mapX;i++)
	{
		delete[]playerMap[i];
	}
	delete[] playerMap;
}
