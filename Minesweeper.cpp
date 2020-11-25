// Minesweeper.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <windows.h>
#include <thread>
#include <chrono>
#include <ctime>

using namespace std;

class Map
{
public:

	int width, height;

	void setSize(int h, int w) {

		height = h;
		width = w;
	}

	int countThisElement(char ele) {

		int c = 0;
		for (int i = 0; i < height; i++) {

			for (int j = 0; j < width; j++) {

				char ch = mapArray[i][j];
				if (ch == 'm') ch = 'M';
				if (ch == ele) {
					c++;
				}
			}
		}
		return c;
	}

	void layMineField(int mineNumber) {

		fillMapWith(' ');

		int totalCells = width * height;
		if (mineNumber > totalCells) mineNumber = totalCells;

		mapArray.resize(height);

		int mineCount = 0;
		boolean re_gen = false;

	generateMine:
		for (int i = 0; i < height; i++) {

			if (!re_gen) mapArray[i].resize(width);
			for (int j = 0; j < width; j++) {

				if (mapArray[i][j] != 'M') {

					if (mineCount < mineNumber) {
						int hasMine = rand() % 100 + 1;
						if (hasMine <= 8) {
							mapArray[i][j] = 'M';
							mineCount++;
						}
						else {
							mapArray[i][j] = ' ';
						}
					}
				}
			}
		}
		if (mineCount < mineNumber) {
			re_gen = true;
			goto generateMine;
		}
	}

	void setRandomMap(int mineNum) {

		layMineField(mineNum);
		char digit[] = { ' ','1','2','3','4','5','6' };

		for (int y = 0; y < height; y++) {

			for (int x = 0; x < width; x++) {

				if (mapArray[y][x] != 'M') {

					int mineCount = 0;

					for (int i = y - 1; i <= y + 1; i++) {

						for (int j = x - 1; j <= x + 1; j++) {

							if (i >= 0 && i < height && j >= 0 && j < width) {

								if (mapArray[i][j] == 'M') {

									mineCount++;
								}
							}
						}
					}
					mapArray[y][x] = digit[mineCount];
				}
			}
		}
	}

	void fillMapWith(char c) {

		mapArray.resize(height);
		for (int i = 0; i < height; i++) {

			mapArray[i].resize(width);
			for (int j = 0; j < width; j++) {

				mapArray[i][j] = c;
			}
		}
	}

	char getCharAt(int y, int x) {

		if (y < mapArray.size() && x < mapArray[y].size())
			return mapArray[y][x];
	}

	void setChar(int y, int x, char c) {

		mapArray[y][x] = c;
	}

private:
	vector < vector<char> > mapArray;
};

void gotoxy(int y, int x) {

	COORD c = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void hideCursor(bool hide) {

	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cci;
	cci.dwSize = 25;
	cci.bVisible = !hide;
	SetConsoleCursorInfo(hStdout, &cci);
}

void prepareWindow(int y, int x) {

	HWND console = GetConsoleWindow();
	RECT r;
	GetWindowRect(console, &r); //stores the console's current dimensions
	MoveWindow(console, r.left, r.top, x, y, TRUE); // 800 width, 100 height

	HWND scroll = GetConsoleWindow();
	ShowScrollBar(scroll, SB_BOTH, FALSE);

	hideCursor(true);
}

class Renderer {

public:

	void colorize(char c = ' ') {

		HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

		switch (c) {

		case 'M': {
			SetConsoleTextAttribute(h, FOREGROUND_RED | BACKGROUND_INTENSITY);
			break;
		}

		case 'm': {
			SetConsoleTextAttribute(h, 12 | BACKGROUND_GREEN);
			break;
		}

		case 'F': {
			SetConsoleTextAttribute(h, 14);
			break;
		}

		case 'f': {
			SetConsoleTextAttribute(h, 14 | BACKGROUND_RED);
			break;
		}

		case ' ': {
			SetConsoleTextAttribute(h, 8);
			break;
		}

		case  '1': {
			SetConsoleTextAttribute(h, 9);
			break;
		}

		case  '2': {
			SetConsoleTextAttribute(h, 10);
			break;
		}

		case  '3': {
			SetConsoleTextAttribute(h, 12);
			break;
		}

		case  '4': {
			SetConsoleTextAttribute(h, 15);
			break;
		}
		case  '5': {
			SetConsoleTextAttribute(h, 13);
			break;
		}
		case  '6': {
			SetConsoleTextAttribute(h, 11);
			break;
		}
		default:
			SetConsoleTextAttribute(h, int(c) - 48);
			break;
		}
	}

	void printMap(Map map) {

		gotoxy(2, 0);
		for (int i = 0; i < map.width + 2; i++) {
			colorize('F');
			cout << "=" << " "; //space
		}
		cout << endl;
		for (int i = 0; i < map.height; i++) {

			for (int j = 0; j < map.width; j++) {

				char c = map.getCharAt(i, j);
				//colorize(c);

				if (j == 0) {
					colorize('F');
					cout << "|" << " "; //space
					colorize(c);

					cout << c;
					colorize(' ');
					cout << " "; //space
				}
				else if (j == map.width - 1) {

					colorize(c);
					if (c == 'm') c = 'M';
					cout << c;
					colorize('F');
					cout << " " << "|"; //space
				}
				else {

					colorize(c);
					if (c == 'm') c = 'M';
					if (c == 'f') c = 'F';
					cout << c;

					colorize(' ');
					cout << " "; //space
				}
				colorize();
			}
			cout << endl;
		}
		gotoxy(map.height + 3, 0);
		for (int i = 0; i < map.width + 2; i++) {
			colorize('F');
			cout << "=" << " "; //space
		}
		cout << endl;
	}
};

struct Cell {

	char value;
	int y;
	int x;
};

struct Action {

	string type;
	vector<Cell> cells;
};

class AutoSolver {

public:

	vector<Cell> getAllHiddenCells(Map map) {
	
		vector<Cell> hiddenCells;
		for (int i = 0; i < map.height; i++) {

			for (int j = 0; j < map.width; j++) {

				if (map.getCharAt(i, j) == '?') {

					Cell hiddenCell;
					hiddenCell.value = map.getCharAt(i, j);
					hiddenCell.y = i;
					hiddenCell.x = j;

					hiddenCells.push_back(hiddenCell);
				}
			}
		}
		return hiddenCells;
	}

	vector<Cell> getAllValuedCells(Map map) {

		vector<Cell> valuedCells;
		for (int i = 0; i < map.height; i++) {

			for (int j = 0; j < map.width; j++) {

				int value = getValOf(map.getCharAt(i, j));
				if (value > 0) {

					Cell valuedCell;
					valuedCell.value = map.getCharAt(i, j);
					valuedCell.y = i;
					valuedCell.x = j;

					valuedCells.push_back(valuedCell);
				}
			}
		}
		return valuedCells;
	}

	int getValOf(char c) {

		if (c == ' ') return 0;
		if (c == '?' || c == 'F' || c == 'f') return -1;
		return c - '0';
	}

	vector<Cell> getFlaggedCells(Map map, int y, int x, int &max_size) {

		vector<Cell> flaggedCells;
		max_size = 0;

		for (int i = y - 1; i <= y + 1; i++) {

			for (int j = x - 1; j <= x + 1; j++) {

				if (i >= 0 && i < map.height && j >= 0 && j < map.width) {

					if (!(i == y && j == x)) {

						max_size++;

						if (map.getCharAt(i, j) == 'F' || map.getCharAt(i, j) == 'f') {

							Cell flagged;
							flagged.value = 'F';
							flagged.y = i;
							flagged.x = j;
							flaggedCells.push_back(flagged);
						}
					}
				}
			}
		}
		return flaggedCells;
	}

	vector<Cell> getHiddenCells(Map map, int y, int x) {

		vector<Cell> hiddenCells;

		for (int i = y - 1; i <= y + 1; i++) {

			for (int j = x - 1; j <= x + 1; j++) {

				if (i >= 0 && i < map.height && j >= 0 && j < map.width) {

					if (!(i == y && j == x)) {

						if (map.getCharAt(i, j) == '?') {

							Cell hidden;
							hidden.value = '?';
							hidden.y = i;
							hidden.x = j;
							hiddenCells.push_back(hidden);
						}
					}
				}
			}
		}
		return hiddenCells;
	}
};

class GameManager
{
public:
	GameManager();
	~GameManager();

	bool isGameOver = false;
	Map trueMap, hiddenMap, checkedMap;
	Renderer renderer;

	int fieldWidth;
	int fieldHeight;
	int mineNum;
	int flagLeft;

	HANDLE hStdin;
	DWORD fdwSaveOldMode;
	DWORD cNumRead, fdwMode, i;
	INPUT_RECORD irInBuf;
	int counter = 0;

	bool clicked = false;
	bool exit = false;
	bool restart = false;
	bool beginGame = false;

	string level;

	vector<Cell> allOpenedCells;

	void initGame() {

		srand(time(NULL));

		flagLeft = mineNum;

		restart = false;
		isGameOver = false;
		beginGame = false;

		allOpenedCells.clear();

		trueMap.setSize(fieldHeight, fieldWidth);
		hiddenMap.setSize(fieldHeight, fieldWidth);
		checkedMap.setSize(fieldHeight, fieldWidth);

		trueMap.setRandomMap(mineNum);
		hiddenMap.fillMapWith('?');
		checkedMap.fillMapWith(' ');

		hStdin = GetStdHandle(STD_INPUT_HANDLE);
		GetConsoleMode(hStdin, &fdwSaveOldMode);
		fdwMode = ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS;
		SetConsoleMode(hStdin, fdwMode);

	}

	void setGame() {

		level = "Beginner";

		fieldHeight = 15;
		fieldWidth = 20;
		mineNum = 35;
		prepareWindow(500, 600);
	
	}

	void checkWay() {

		renderer.printMap(hiddenMap);

		gotoxy(fieldHeight + 4, 0);
		renderer.colorize('F');
		cout << "Scanning...                          ";
		renderer.colorize(); 

		gotoxy(fieldHeight + 5, 0);
		cout << "                                            ";

		AutoSolver solver;
		vector<Cell> valuedCells = solver.getAllValuedCells(hiddenMap);

		for (int i = 0; i < valuedCells.size(); i++) {

			bool checked = false;

			for (int j = 0; j < allOpenedCells.size(); j++) {

				if (allOpenedCells[j].value == valuedCells[i].value)
					if (allOpenedCells[j].y == valuedCells[i].y)
						if (allOpenedCells[j].y == valuedCells[i].y)
						{
							checked = true;
							break;
						}
			}

			if (!checked) {

				int y = valuedCells[i].y;
				int x = valuedCells[i].x;
				int val = solver.getValOf(valuedCells[i].value);

				int max_size;

				vector<Cell> hidden = solver.getHiddenCells(hiddenMap, y, x);
				vector<Cell> flagged = solver.getFlaggedCells(hiddenMap, y, x, max_size);

				if (val > 0 && hidden.size() < 8) {

					if (hidden.size() > 0) {

						if (val == flagged.size() || max_size - flagged.size() == 1) {

							for (int k = 0; k < hidden.size(); k++) {

								openCell(hidden[k].y, hidden[k].x);
							}
							renderer.printMap(hiddenMap);						}

						hidden.clear();
						flagged.clear();

						hidden = solver.getHiddenCells(hiddenMap, y, x);
						flagged = flagged = solver.getFlaggedCells(hiddenMap, y, x, max_size);

						if (val - flagged.size() == hidden.size()) {

							gotoxy(fieldHeight + 4, 0);
							for (int k = 0; k < hidden.size(); k++) {

								flagCell(hidden[k].y, hidden[k].x);
							}
							renderer.printMap(hiddenMap);
							checkWay();
						}
					}
				}
			}
		}

			gotoxy(fieldHeight + 4, 0);
			cout << "               ";

			showUI();
	}

	void AutoPlay() {

		while (true) {

			system("cls");
			initGame();
			showUI();
			renderer.printMap(hiddenMap);
		
			Sleep(1000);

			AutoSolver solver;
			vector<Cell> hiddenCells = solver.getAllHiddenCells(hiddenMap);
			int id = rand() % hiddenCells.size();
			openCell(hiddenCells[id].y, hiddenCells[id].x);
			renderer.printMap(hiddenMap);
			Sleep(10);

			while (!isGameOver) {
			
				showUI();

				ReadConsoleInput(hStdin, &irInBuf, 1, &cNumRead);
				switch (irInBuf.EventType) {
				case KEY_EVENT:

					if (irInBuf.Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE) {
						exit = true;
						goto end;
					}
				}

				checkWay();

				gotoxy(fieldHeight + 5, 0);
				renderer.colorize('F');
				cout << "There's no way, choosing a random pos...";
				renderer.colorize();

				hiddenCells = solver.getAllHiddenCells(hiddenMap);
				if (hiddenCells.size() > 0) {

					id = rand() % hiddenCells.size();
					openCell(hiddenCells[id].y, hiddenCells[id].x);
					renderer.printMap(hiddenMap);
				}
					
				if (!isGameOver && flagLeft == 0) {
					
					gotoxy(fieldHeight + 4, 0);
					renderer.colorize('2');
					cout << "                 BOT WIN                  " << endl;
					cout << "                                                       ";
					while (true) {

						if (GetKeyState(VK_ESCAPE) & 0x8000) goto end;
						else if (GetKeyState(VK_RETURN) & 0x8000) goto next;
					}
				}
			}
			if (isGameOver) {
				gameOver();
				renderer.printMap(trueMap);
				while (true) {
				
					if (GetKeyState(VK_ESCAPE) & 0x8000) goto end;
					else if (GetKeyState(VK_RETURN) & 0x8000) goto next;
				}
			}
		next: {}
		}	
		end:
		system("cls");
	}


	void flagCell(int y, int x) {

		if (hiddenMap.getCharAt(y, x) != 'F') {
			hiddenMap.setChar(y, x, 'F');
			flagLeft--;
		}
		else {
			hiddenMap.setChar(y, x, '?');
			flagLeft++;
		}
	}

	void openCell(int y, int x) {

		if (hiddenMap.getCharAt(y, x) != 'F' && hiddenMap.getCharAt(y, x) != ' ') {
			clicked = true;
			if (trueMap.getCharAt(y, x) == 'M') {

				isGameOver = true;
				trueMap.setChar(y, x, 'm');
			}
			else {
				oilSpills(y, x);
			}
		}
	}

private:


	bool checkWin() {

		bool isWin = true;
		for (int i = 0; i < fieldHeight; i++) {

			for (int j = 0; j < fieldWidth; j++) {

				if (trueMap.getCharAt(i, j) == 'M') {

					if (hiddenMap.getCharAt(i, j) != 'F') {

						isWin = false;
						break;
					}
				}
			}
		}
		return isWin;
	}

	void checkInput() {

		int x; int y;

		ReadConsoleInput(hStdin, &irInBuf, 1, &cNumRead);
		switch (irInBuf.EventType) {
		case KEY_EVENT:

			if (irInBuf.Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE) {
				exit = true;
			}
			else if (irInBuf.Event.KeyEvent.wVirtualKeyCode == VK_RETURN) {
				Sleep(100);
				if (restart == false) restart = true;
			}
			break;

		case MOUSE_EVENT:

			if (!isGameOver) {
				MOUSE_EVENT_RECORD mer = irInBuf.Event.MouseEvent;

				switch (mer.dwEventFlags) {
				case 0:

					if (mer.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED) {
						x = mer.dwMousePosition.X;
						y = mer.dwMousePosition.Y;

						if (x % 2 == 0) {
							if (x >= 2 && x <= fieldWidth * 2 && y >= 2 && y <= fieldHeight + 2) {
								x = (x - 2) / 2;
								y = y - 3;

								openCell(y, x);
							}
						}
					}
					else if (mer.dwButtonState == RIGHTMOST_BUTTON_PRESSED) {
						x = mer.dwMousePosition.X;
						y = mer.dwMousePosition.Y;
						if (x % 2 == 0) {
							if (x >= 2 && x <= fieldWidth * 2 && y >= 2 && y <= fieldHeight + 2) {
								x = (x - 2) / 2;
								y = y - 3;
								clicked = true;
								if (hiddenMap.getCharAt(y, x) == '?' || hiddenMap.getCharAt(y, x) == 'F') {
									flagCell(y, x);
								}
							}
						}
					}
					break;
				default:
					break;
				}
			}
			break;

		case WINDOW_BUFFER_SIZE_EVENT:
			break;

		case FOCUS_EVENT:
			break;

		case MENU_EVENT:
			break;

		default:
			break;
		}
		FlushConsoleInputBuffer(hStdin);
	}

	void showUI() {

		gotoxy(0, trueMap.width - 6);
		renderer.colorize('3');
		cout << "MINESWEEPERS" << endl;
		renderer.colorize('?');
		gotoxy(1, trueMap.width - 17);
		cout << endl;
		renderer.colorize();
		
		gotoxy(8, trueMap.width * 2 + 5);
		renderer.colorize('4'); cout << " - ";
		renderer.colorize('M'); cout << "M";
		renderer.colorize('4'); cout << ": Mine";

		gotoxy(12, trueMap.width * 2 + 5);
		renderer.colorize('4'); cout << " - ";
		renderer.colorize('F'); cout << "F";
		renderer.colorize('4'); cout << ": Flag";

	}

	void gameOver() {

		for (int i = 0; i < trueMap.height; i++) {

			for (int j = 0; j < trueMap.width; j++) {

				if (hiddenMap.getCharAt(i, j) != 'F') {
					hiddenMap.setChar(i, j, trueMap.getCharAt(i, j));
				}
				else {
					if (trueMap.getCharAt(i, j) != 'M') {
						hiddenMap.setChar(i, j, 'f');
					}
				}
			}
		}
		showUI();
		renderer.printMap(hiddenMap);
		renderer.colorize('3');
		gotoxy(trueMap.height + 4, trueMap.width - 14);
		cout << "   BOOM!! GAME OVER!! :))" << endl;
		renderer.colorize('F');
		gotoxy(trueMap.height + 5, trueMap.width - 17);
		cout << "Please press Enter to restart game";
		gotoxy(trueMap.height + 6, trueMap.width - 12);
		cout << "or press ESC to exit...";
	}

	void oilSpills(int y, int x) {

		checkedMap.setChar(y, x, 'x');
		char c = trueMap.getCharAt(y, x);

		if (c == ' ') {

			hiddenMap.setChar(y, x, ' ');
			string s = to_string(y) + "-" + to_string(x) + "-v=" + c;

			if (x - 1 >= 0) {

				char tc = trueMap.getCharAt(y, x - 1);
				char xc = checkedMap.getCharAt(y, x - 1);
				char hc = hiddenMap.getCharAt(y, x - 1);

				if (xc != 'x' && hc != 'F' && tc != 'M') {
					oilSpills(y, x - 1);
				}
			}
			if (x + 1 < hiddenMap.width) {

				char tc = trueMap.getCharAt(y, x + 1);
				char xc = checkedMap.getCharAt(y, x + 1);
				char hc = hiddenMap.getCharAt(y, x + 1);

				if (xc != 'x' && hc != 'F' && tc != 'M') {
					oilSpills(y, x + 1);
				}
			}
			if (y - 1 >= 0) {

				char tc = trueMap.getCharAt(y - 1, x);
				char xc = checkedMap.getCharAt(y - 1, x);
				char hc = hiddenMap.getCharAt(y - 1, x);

				if (xc != 'x' && hc != 'F' && tc != 'M') {
					oilSpills(y - 1, x);
				}
			}
			if (y + 1 < hiddenMap.height) {

				char tc = trueMap.getCharAt(y + 1, x);
				char xc = checkedMap.getCharAt(y + 1, x);
				char hc = hiddenMap.getCharAt(y + 1, x);
				if (xc != 'x' && hc != 'F' && tc != 'M') {
					oilSpills(y + 1, x);
				}
			}
		}
		else {
			hiddenMap.setChar(y, x, c);
		}
	}
};
GameManager::GameManager()
{
}
GameManager::~GameManager()
{
}

class Menu {

public:

	Renderer renderer;
	int choice;

	vector<string> mainText = { "Watch Bot playing", "Exit" };

	Menu() {

		choice = 0;
	}

	void showBanner() {

		prepareWindow(450, 450);
		gotoxy(0, 24 - 5);
		renderer.colorize('3');
		cout << "MINESWEEPERS" << endl;
		renderer.colorize('?');
		gotoxy(1, 34 - 27);
	}

	void showMain() {
		showBanner();
		vector<string> text = mainText;
		bool gone_up = false;
		bool gone_down = false;
		bool entered = false;
		bool esc_ed = false;

		bool choiceLevel = false;

		while (true) {

			if ((GetKeyState(VK_UP) & 1) != gone_up) {
				choice--;
				gone_up = !gone_up;
			}
			else if ((GetKeyState(VK_DOWN) & 1) != gone_down) {
				choice++;
				gone_down = !gone_down;
			}
			else if ((GetKeyState(VK_RETURN) & 1) != entered) {
				entered = !entered;
					
					if (choice == 0) {

						GameManager gameManager;
						gameManager.setGame();
						system("cls");
						gameManager.AutoPlay();
					}
					else if (choice == 1) {
						goto end;
					}
			}
			else if ((GetKeyState(VK_ESCAPE) & 1) != esc_ed) {
				esc_ed = !esc_ed;
				text = mainText;
				if (choiceLevel) {
					system("cls");
					choiceLevel = false;
				}
			}

			if (choice < 0) {
				choice = text.size() - 1;
			}
			else if (choice > text.size() - 1) {
				choice = 0;
			}

			showBanner();
			for (int i = 0; i < text.size(); i++) {
				renderer.colorize('2');
				gotoxy(i * 2 + 3, 24 - text[i].length() / 2);
				if (choice == i) renderer.colorize('f');
				cout << text[i];
				renderer.colorize(' ');
			}
			FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
		}
	end: {}
	}
};

int main() {

	Menu menu;
	menu.showMain();
	return 0;
}