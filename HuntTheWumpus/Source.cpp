#pragma once
#include "Game.h"

using namespace std;

short int MINSIZE = 4, MAXSIZE = 30;

int main()
{
	setlocale(LC_ALL, "rus");
	Game::printRules();
	cout << endl;
	cout<< " "; system("PAUSE");
	do
	{
		system("cls");
		int x = 0, y = 0;
		try
		{
			cout << " Введите размер карты" << endl << " X: ";
			cin >> x;
			cout << " Y: ";
			cin >> y;
			if ((x < MINSIZE || y < MINSIZE) || (x > MAXSIZE || y > MAXSIZE))
			{
				string wrongSize;
				wrongSize += "(";
				char tmp[10]{};
				_itoa_s(x, tmp, 10);
				wrongSize += tmp;
				wrongSize += ',';
				_itoa_s(y, tmp, 10);
				wrongSize += tmp;
				wrongSize += ')';
				throw exception(wrongSize.c_str());
			}
		}
		catch (exception e)
		{
			cout << " " << e.what() << " - неправильный размер игровой карты" << endl;
			if (!cin)
			{
				cin.clear();
				cin.ignore(10,'\n');
			}
			cout<< " "; system("PAUSE");
			system("cls");
			continue;
		}
		Game game(x, y);
		system("cls");
		while (true)
		{
			game.printPlayerMap();
			game.makeAnAction();
			if (game.vumpusIsKilled())
			{
				cout << " Вы победили" << endl;
				break;
			}
			if (game.checkPlayerDead() == true)
			{
				cout << " Вы погибли" << endl;
				break;
			}
			system("cls");
		}
		cout << " Хотите сыграть еще раз (y/n)? ";
		char answer = 'n';
		cin >> answer;
		if (answer == 'y')
			continue;
		return 0;
	} while (true);
}
