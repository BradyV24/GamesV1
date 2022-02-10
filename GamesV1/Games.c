#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#define HIGHSCORES "highscores.txt"
#define NUMGAMES 2

#define WIDTH 120
#define HEIGHT 30

int Menu();
int SnakeGame();
int Tetris();
void PrintAt(int x, int y, char string[], wchar_t screen[]);
void PrintTetromino(int tetromino, int x, int y, int r, char mode[], wchar_t screen[]);
int fits(int tetromino, int tx, int ty, int tr, wchar_t screen[]);
int lineCheck(wchar_t screen[], HANDLE hConsole, DWORD cWritten, COORD coord);

int main() {
	int Running = 1;
	//Tetris();
	while (Running) {
		int gameNum = Menu();
		if (gameNum == 0) {
			while (SnakeGame());
		}
		if (gameNum == 1) {
			while (Tetris());
		}
		if (gameNum == 2) {

		}
		if (gameNum == 3) {

		}
		if (gameNum == 4) {
			Running = 0;
		}
	}
	return 0;
}

void PrintAt(int x, int y, char string[], wchar_t screen[]) {
	for (int i = 0; i < strlen(string); i++) {
		screen[x + y * WIDTH + i] = string[i];
	}
}

int Menu() {
	wchar_t screenA[HEIGHT*WIDTH];
	for (int i = 0; i < HEIGHT*WIDTH; i++) screenA[i] = ' ';
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	wchar_t *screen = screenA;
	DWORD cWritten;
	COORD coord;
	coord.X = 0;
	coord.Y = 0;
	SetConsoleActiveScreenBuffer(hConsole);

	PrintAt(WIDTH / 2 - 35, 2, " ____                _       _        _____", screen);
	PrintAt(WIDTH / 2 - 35, 3, "|  _ \\              | |     ( )      / ____|", screen);
	PrintAt(WIDTH / 2 - 35, 4, "| |_) |_ __ __ _  __| |_   _|/ ___  | |  __  __ _ _ __ ___   ___  ___", screen);
	PrintAt(WIDTH / 2 - 35, 5, "|  _ <| '__/ _` |/ _` | | | | / __| | | |_ |/ _` | '_ ` _ \\ / _ \\/ __|", screen);
	PrintAt(WIDTH / 2 - 35, 6, "| |_) | | | (_| | (_| | |_| | \\__ \\ | |__| | (_| | | | | | |  __/\\__ \\", screen);
	PrintAt(WIDTH / 2 - 35, 7, "|____/|_|  \\__,_|\\__,_|\\__, | |___/  \\_____|\\__,_|_| |_| |_|\\___||___/", screen);
	PrintAt(WIDTH / 2 - 35, 8, "                        __/ |", screen);
	PrintAt(WIDTH / 2 - 35, 9, "                       |___/", screen);

	PrintAt(WIDTH / 2 - 30, 11, "[0] Snake", screen);
	PrintAt(WIDTH / 2 - 30, 12, "[1] Tetris", screen);
	PrintAt(WIDTH / 2 - 30, 13, "[2] Hangman (WIP)", screen);
	PrintAt(WIDTH / 2 - 30, 14, "[3] Fishing Mini Game (WIP)", screen);
	PrintAt(WIDTH / 2 - 30, 16, "[esc] Quit", screen);
	WriteConsoleOutputCharacter(hConsole, screen, WIDTH*HEIGHT, coord, &cWritten);
	int inMenu = 1;
	int bKey[5];
	//Waiting for key release
	do {
		bKey[0] = (0x8000 & GetAsyncKeyState((unsigned char)('\x1B'))) != 0;
	} while (bKey[0] == 1);
	Sleep(100);
	while (inMenu) {
		for (int k = 0; k < 5; k++) {
			bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("0123\x1B"[k]))) != 0;
		}
		if (bKey[0] == 1) {
			return 0;
		}
		if (bKey[1] == 1) {
			return 1;
		}
		if (bKey[2] == 1) {
			return 2;
		}
		if (bKey[3] == 1) {
			return 3;
		}
		if (bKey[4] == 1) {
			return 4;
		}
	}

}

int SnakeGame() {
	// SEEDING rand()
	srand(time(NULL));

	// LOADING HIGH SCORE
	FILE* HighScores = fopen(HIGHSCORES, "r+");
	int gameNum;
	char game[NUMGAMES][10];
	int highscore[NUMGAMES];
	char highscoreStr[10];
	for (int i = 0; fscanf(HighScores, "%s %d", &game[i], &highscore[i]) == 2; i++) {
		if (strcmp(game[i], "snake:") == 0) {
			gameNum = i;
		}
	}
	_itoa(highscore[gameNum], highscoreStr, 10);

	wchar_t screenA[HEIGHT*WIDTH];
	for (int i = 0; i < HEIGHT*WIDTH; i++) screenA[i] = ' ';

	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	wchar_t *screen = screenA;
	DWORD cWritten;
	COORD coord;
	coord.X = 0;
	coord.Y = 0;

	SetConsoleActiveScreenBuffer(hConsole);

	int SnakeCoords[WIDTH*HEIGHT][2];
	SnakeCoords[4][0] = WIDTH / 2 + 2;
	SnakeCoords[4][1] = (HEIGHT - 8) / 2;
	SnakeCoords[3][0] = WIDTH / 2 + 1;
	SnakeCoords[3][1] = (HEIGHT - 8) / 2;
	SnakeCoords[2][0] = WIDTH / 2;
	SnakeCoords[2][1] = (HEIGHT - 8) / 2;
	SnakeCoords[1][0] = WIDTH / 2 - 1;
	SnakeCoords[1][1] = (HEIGHT - 8) / 2;
	SnakeCoords[0][0] = WIDTH / 2 - 2;
	SnakeCoords[0][1] = (HEIGHT - 8) / 2;

	int length = 5;
	int direction = 2;
	int PelletActive = 0;
	COORD PelletPos;
	PelletPos.X = 0;
	PelletPos.Y = 0;

	int bKey[5];

	int lose = 0;
	int playing = 0;
	int score = 0;

	while (!lose) {

		// INITIALIZING FRAME AND TITLE
		if (!playing) {
			for (int i = 0; i < WIDTH; i++) {
				PrintAt(i, 1, "~", screen);
			}
			PrintAt(WIDTH / 2 - 2, HEIGHT / 2 - 4, "snake", screen);
			PrintAt(WIDTH / 2 - 11, HEIGHT / 2 - 2, "Press [enter] to start", screen);
			PrintAt(WIDTH / 2 - 10, HEIGHT / 2, "Press [esc] to quit.", screen);
			PrintAt(WIDTH - strlen(highscoreStr) - 13, 0, "HIGH SCORE: ", screen);
			PrintAt(WIDTH - strlen(highscoreStr) - 1, 0, highscoreStr, screen);
			PrintAt(1, 0, "SCORE: ", screen);
			PrintAt(8, 0, "0", screen);
			WriteConsoleOutputCharacter(hConsole, screen, WIDTH*HEIGHT, coord, &cWritten);
			while (!playing) {
				for (int k = 0; k < 2; k++) {
					bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x0D\x1B"[k]))) != 0;
				}
				if (bKey[0] == 1) {
					playing = 1;
					for (int i = 0; i < 22; i++) {
						screen[WIDTH / 2 - 11 + (HEIGHT / 2 - 2) * WIDTH + i] = ' ';
						screen[WIDTH / 2 - 11 + (HEIGHT / 2) * WIDTH + i] = ' ';
						WriteConsoleOutputCharacter(hConsole, screen, WIDTH*HEIGHT, coord, &cWritten);
						Sleep(1);
					}
				}
				if (bKey[1] == 1) {
					return 0;
				}
			}
		}

		// INPUT		==================================================================
		for (int k = 0; k < 5; k++) {
			bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x26\x25\x28\x1B"[k]))) != 0;
		}

		// GAME LOGIC	 ================================================================
			// MOVEMENT ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		if (bKey[0] == 1 && direction != 2) {
			direction = 0;
		}
		else if (bKey[1] == 1 && direction != 3) {
			direction = 1;
		}
		else if (bKey[2] == 1 && direction != 0) {
			direction = 2;
		}
		else if (bKey[3] == 1 && direction != 1) {
			direction = 3;
		}

		// PRINT SCORE ~~~~~~~~~~~~~~~~~~~~~~~~~~~
		char scoreStr[5];
		_itoa(score, scoreStr, 10);
		PrintAt(1, 0, "SCORE: ", screen);
		PrintAt(8, 0, scoreStr, screen);

		//REFRESH FRAME
		screen[SnakeCoords[length - 1][0] + SnakeCoords[length - 1][1] * WIDTH] = 'e';
		screen[SnakeCoords[length - 2][0] + SnakeCoords[length - 2][1] * WIDTH] = 'k';
		for (int i = length - 3; i > 1; i--) {
			screen[SnakeCoords[i][0] + SnakeCoords[i][1] * WIDTH] = 'a';
		}
		screen[SnakeCoords[1][0] + SnakeCoords[1][1] * WIDTH] = 'n';
		screen[SnakeCoords[0][0] + SnakeCoords[0][1] * WIDTH] = 's';

		WriteConsoleOutputCharacter(hConsole, screen, WIDTH*HEIGHT, coord, &cWritten);
		Sleep(50);

		// GAME LOGIC (CONT) ================================================================

		// PAUSE
		if (bKey[4] == 1) {
			//Waiting for key release
			while (bKey[4] == 1) {
				for (int k = 0; k < 5; k++) {
					bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x26\x25\x28\x1B"[k]))) != 0;
				}
			}
			PrintAt(WIDTH / 2 - 13, HEIGHT / 2 - 2, "Press [enter] to continue", screen);
			PrintAt(WIDTH / 2 - 10, HEIGHT / 2, "Press [esc] to quit.", screen);
			WriteConsoleOutputCharacter(hConsole, screen, WIDTH*HEIGHT, coord, &cWritten);
			int paused = 1;
			while (paused) {
				for (int k = 0; k < 2; k++) {
					bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x0D\x1B"[k]))) != 0;
				}
				if (bKey[0] == 1) {
					paused = 0;
					for (int i = 0; i < 26; i++) {
						screen[WIDTH / 2 - 13 + (HEIGHT / 2 - 2) * WIDTH + i] = ' ';
						screen[WIDTH / 2 - 13 + (HEIGHT / 2) * WIDTH + i] = ' ';
						WriteConsoleOutputCharacter(hConsole, screen, WIDTH*HEIGHT, coord, &cWritten);
						Sleep(1);
					}
				}
				if (bKey[1] == 1) {
					return 0;
				}
			}

		}

		// PELLET SPAWNING ~~~~~~~~~~~~~~~~~~~~~~~
		while (PelletActive == 0) {
			PelletPos.X = rand() % (WIDTH / 2) * 2;
			PelletPos.Y = rand() % (HEIGHT - 2) + 2;
			int intersects = 0;
			if (screen[PelletPos.X + PelletPos.Y * WIDTH] != ' ') {
				intersects = 1;
			}
			if (intersects != 1) {
				screen[PelletPos.X + PelletPos.Y * WIDTH] = '@';
				PelletActive = 1;
			}
		}
		// SELF COLLISION ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		if (direction == 0) {
			for (int c = 0; c < 5; c++) {
				if (screen[(SnakeCoords[0][0] + 2 + SnakeCoords[0][1] * WIDTH + (HEIGHT - 2) * WIDTH) % ((HEIGHT - 2)*WIDTH)] == "snake"[c]) {
					lose = 1;
				}
			}
		}
		else if (direction == 1) {
			for (int c = 0; c < 5; c++) {
				if (screen[(SnakeCoords[0][0] + (SnakeCoords[0][1] - 1) * WIDTH + (HEIGHT - 2) * WIDTH) % ((HEIGHT - 2)*WIDTH)] == "snake"[c]) {
					lose = 1;
				}
			}
		}
		else if (direction == 2) {
			for (int c = 0; c < 5; c++) {
				if (screen[(SnakeCoords[0][0] - 2 + SnakeCoords[0][1] * WIDTH + (HEIGHT - 2) * WIDTH) % ((HEIGHT - 2)*WIDTH)] == "snake"[c]) {
					lose = 1;
				}
			}
		}
		else {
			for (int c = 0; c < 5; c++) {
				if (screen[(SnakeCoords[0][0] + (SnakeCoords[0][1] + 1) * WIDTH + (HEIGHT - 2) * WIDTH) % ((HEIGHT - 2)*WIDTH)] == "snake"[c]) {
					lose = 1;
				}
			}
		}

		// CLEAR PREVIOUS SNAKE ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		for (int i = 0; i < length; i++) {
			screen[SnakeCoords[i][0] + SnakeCoords[i][1] * WIDTH] = ' ';
		}
		for (int i = length - 1; i > 0; i--) {
			SnakeCoords[i][0] = SnakeCoords[i - 1][0];
			SnakeCoords[i][1] = SnakeCoords[i - 1][1];
		}

		// MOVE SNAKE AND CHECK FOR APPLES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		if (direction == 0) {
			if (screen[(SnakeCoords[0][0] + 2 + SnakeCoords[0][1] * WIDTH) % (HEIGHT*WIDTH)] == '@') {
				PelletActive = 0;
				SnakeCoords[length][0] = SnakeCoords[length - 1][0];
				SnakeCoords[length][1] = SnakeCoords[length - 1][1];
				length++;
				score++;
			}
			SnakeCoords[0][0] += 2;
		}
		else if (direction == 1) {
			if (screen[(SnakeCoords[0][0] + (SnakeCoords[0][1] - 1) * WIDTH + HEIGHT * WIDTH) % (HEIGHT*WIDTH)] == '@') {
				PelletActive = 0;
				SnakeCoords[length][0] = SnakeCoords[length - 1][0];
				SnakeCoords[length][1] = SnakeCoords[length - 1][1];
				length++;
				score++;
			}
			SnakeCoords[0][1]--;
		}
		else if (direction == 2) {
			if (screen[(SnakeCoords[0][0] - 2 + SnakeCoords[0][1] * WIDTH + HEIGHT * WIDTH) % (HEIGHT*WIDTH)] == '@') {
				PelletActive = 0;
				SnakeCoords[length][0] = SnakeCoords[length - 1][0];
				SnakeCoords[length][1] = SnakeCoords[length - 1][1];
				length++;
				score++;
			}
			SnakeCoords[0][0] -= 2;
		}
		else {
			if (screen[(SnakeCoords[0][0] + (SnakeCoords[0][1] + 1) * WIDTH + HEIGHT * WIDTH) % (HEIGHT*WIDTH)] == '@') {
				PelletActive = 0;
				SnakeCoords[length][0] = SnakeCoords[length - 1][0];
				SnakeCoords[length][1] = SnakeCoords[length - 1][1];
				length++;
				score++;
			}
			SnakeCoords[0][1]++;
		}

		//COORDINATE WRAP-AROUND ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		if (SnakeCoords[0][0] < 0) {
			SnakeCoords[0][0] += WIDTH;
		}
		else if (SnakeCoords[0][0] >= WIDTH) {
			SnakeCoords[0][0] %= WIDTH;
		}
		if (SnakeCoords[0][1] < 2) {
			SnakeCoords[0][1] += (HEIGHT - 2);
		}
		else if (SnakeCoords[0][1] >= HEIGHT) {
			SnakeCoords[0][1] %= HEIGHT;
			SnakeCoords[0][1] += 2;
		}
	}

	//CHECKING FOR NEW HIGH SCORE
	char scoreStr[5];
	_itoa(score, scoreStr, 10);
	if (score > highscore[gameNum]) {
		PrintAt(WIDTH / 2 - 8, HEIGHT / 2 - 6, "NEW HIGH SCORE!", screen);
		//WRITE NEW HIGH SCORE TO FILE
		rewind(HighScores);
		highscore[gameNum] = score;
		for (int i = 0; i < NUMGAMES; i++) {
			fprintf(HighScores, "%s %d\n", game[i], highscore[i]);
		}
	}

	fclose(HighScores);

	// END FRAME
	PrintAt(WIDTH / 2 - 4, HEIGHT / 2 - 4, "SCORE: ", screen);
	PrintAt(WIDTH / 2 + 3, HEIGHT / 2 - 4, scoreStr, screen);
	PrintAt(WIDTH / 2 - 14, HEIGHT / 2 - 2, "Press [enter] to play again.", screen);
	PrintAt(WIDTH / 2 - 10, HEIGHT / 2, "Press [esc] to quit.", screen);
	WriteConsoleOutputCharacter(hConsole, screen, WIDTH*HEIGHT, coord, &cWritten);
	int checking = 1;
	while (checking) {
		for (int k = 0; k < 2; k++) {
			bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x0D\x1B"[k]))) != 0;
		}
		if (bKey[0] == 1) {
			checking = 0;
			return 1;
		}
		if (bKey[1] == 1) {
			checking = 0;
			return 0;
		}
	}
}

int Tetris() {
	// SEEDING rand()
	srand(time(NULL));
	// LOAD HIGH SCORE
	FILE* HighScores = fopen(HIGHSCORES, "r+");
	int gameNum;
	char game[NUMGAMES][10];
	int highscore[NUMGAMES];
	char highscoreStr[10];
	for (int i = 0; fscanf(HighScores, "%s %d", &game[i], &highscore[i]) == 2; i++) {
		if (strcmp(game[i], "tetris:") == 0) {
			gameNum = i;
		}
	}
	_itoa(highscore[gameNum], highscoreStr, 10);

	// VARIABLE INITIALIZATION
	int bKey[7];
	int framesPerGridCell[29] = { 48, 43, 38, 33, 28, 23, 18, 13, 8, 6, 5, 5, 5, 4, 4, 4, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1 };
	const int FRAMETIME = 17;
	COORD BoardCorner;
	BoardCorner.X = WIDTH / 2 - 9;
	BoardCorner.Y = 5;

	// SETUP DISPLAY
	wchar_t screenA[HEIGHT*WIDTH];
	for (int i = 0; i < HEIGHT*WIDTH; i++) screenA[i] = ' ';
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	wchar_t *screen = screenA;
	DWORD cWritten;
	COORD coord;
	coord.X = 0;
	coord.Y = 0;
	SetConsoleActiveScreenBuffer(hConsole);


	// MAIN MENU
	{
		PrintAt(WIDTH / 2 - 19, 1, " _____ _____ ___________ _____ _____", screen);
		PrintAt(WIDTH / 2 - 19, 2, "|_===_|==___|_===_|=___=\\_===_/==___|", screen);
		PrintAt(WIDTH / 2 - 19, 3, "==|=|=|=|__===|=|=|=|_/=/=|=|=\\=`--.=", screen);
		PrintAt(WIDTH / 2 - 19, 4, "~~|~|~|~~__|~~|~|~|~~~~/~~|~|~~`--.~\\", screen);
		PrintAt(WIDTH / 2 - 19, 5, "~~|~|~|~|___~~|~|~|~|\\~\\~_|~|_/\\__/~/", screen);
		PrintAt(WIDTH / 2 - 19, 6, "..\\_/.\\____/..\\_/.\\_|.\\_|\\___/\\____/.", screen);
		PrintAt(WIDTH / 2 - 19, 7, ".....................................", screen);

		PrintAt(WIDTH / 2 - 11, 12, "Press [enter] to start", screen);
		PrintAt(WIDTH / 2 - 10, 14, "Press [esc] to quit.", screen);
		PrintAt(WIDTH / 2 - (strlen(highscoreStr) + 13) / 2, 10, "HIGH SCORE: ", screen);
		PrintAt(WIDTH / 2 + (strlen(highscoreStr) + 9) / 2, 10, highscoreStr, screen);
		WriteConsoleOutputCharacter(hConsole, screen, WIDTH*HEIGHT, coord, &cWritten);

		int InMenu = 1;
		while (InMenu) {
			for (int k = 0; k < 2; k++) {
				bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x0D\x1B"[k]))) != 0;
			}
			if (bKey[0] == 1) {
				InMenu = 0;
				for (int i = 0; i <= 38; i++) {
					for (int j = 0; j <= 14; j++) {
						screen[WIDTH / 2 - 19 + (j)* WIDTH + i] = ' ';
						WriteConsoleOutputCharacter(hConsole, screen, WIDTH*HEIGHT, coord, &cWritten);
					}
				}
			}
			if (bKey[1] == 1) {
				return 0;
			}
		}
	}

	// TETROMINOS
	char Tetrominos[7][4][3] = {
	{	{"I."},
		{"I."},
		{"I."},
		{"I."}	},

	{	{".J"},
		{".J"},
		{"JJ"},
		{".."}	},

	{	{"L."},
		{"L."},
		{"LL"},
		{".."}	},

	{	{"S."},
		{"SS"},
		{".S"},
		{".."}	},

	{	{".Z"},
		{"ZZ"},
		{"Z."},
		{".."}	},

	{	{"T."},
		{"TT"},
		{"T."},
		{".."}	},

	{	{"OO"},
		{"OO"},
		{".."},
		{".."}	}
	};

	// INITIALIZE BOARD
	{
		// PLAY AREA
		for (int i = 5; i <= 25; i++) {
			PrintAt(WIDTH / 2 - 10, i, "| . . . . . . . . . . |", screen);
		}
		PrintAt(WIDTH / 2 - 10, 26, "|~~~~~~~~~~~~~~~~~~~~~|", screen);

		// LEFT SIDE
		PrintAt(WIDTH / 2 + 13, 7, "|~~~~~~~~~~~~|", screen);
		PrintAt(WIDTH / 2 + 13, 8, "|SCORE:      |", screen);
		PrintAt(WIDTH / 2 + 13, 9, "|            |", screen);
		PrintAt(WIDTH / 2 + 13, 10, "|~~~~~~~~~~~~|", screen);
		PrintAt(WIDTH / 2 + 13, 11, "|LEVEL:      |", screen);
		PrintAt(WIDTH / 2 + 13, 12, "|            |", screen);
		PrintAt(WIDTH / 2 + 13, 13, "|~~~~~~~~~~~~|", screen);
		PrintAt(WIDTH / 2 + 13, 14, "|LINES:      |", screen);
		PrintAt(WIDTH / 2 + 13, 15, "|            |", screen);
		PrintAt(WIDTH / 2 + 13, 16, "|~~~~~~~~~~~~|", screen);
		PrintAt(WIDTH / 2 + 13, 17, "|HIGH SCORE: |", screen);
		PrintAt(WIDTH / 2 + 13, 18, "|            |", screen);
		PrintAt(WIDTH / 2 + 13, 19, "|~~~~~~~~~~~~|", screen);
		PrintAt(WIDTH / 2 + 20 - strlen(highscoreStr) / 2, 18, highscoreStr, screen);

		// RIGHT SIDE
		PrintAt(WIDTH / 2 - 19, 7, "|~~~~~~~|", screen);
		PrintAt(WIDTH / 2 - 19, 8, "|NEXT:  |", screen);
		PrintAt(WIDTH / 2 - 19, 9, "|       |", screen);
		PrintAt(WIDTH / 2 - 19, 10, "|       |", screen);
		PrintAt(WIDTH / 2 - 19, 11, "|       |", screen);
		PrintAt(WIDTH / 2 - 19, 12, "|       |", screen);
		PrintAt(WIDTH / 2 - 19, 13, "|~~~~~~~|", screen);

		// CONTROLS
		PrintAt(WIDTH / 2 - 46, 8, "|~~~~~~~~~~~~~~~~~~~~|", screen);
		PrintAt(WIDTH / 2 - 46, 9, "| CONTROLS:          |", screen);
		PrintAt(WIDTH / 2 - 46, 10, "|                    |", screen);
		PrintAt(WIDTH / 2 - 46, 11, "|   ARROW KEYS:      |", screen);
		PrintAt(WIDTH / 2 - 46, 12, "|                    |", screen);
		PrintAt(WIDTH / 2 - 46, 13, "|     ROTATE CW      |", screen);
		PrintAt(WIDTH / 2 - 46, 14, "|         ^          |", screen);
		PrintAt(WIDTH / 2 - 46, 15, "|  LEFT < v > RIGHT  |", screen);
		PrintAt(WIDTH / 2 - 46, 16, "|     SOFT DROP      |", screen);
		PrintAt(WIDTH / 2 - 46, 17, "|                    |", screen);
		PrintAt(WIDTH / 2 - 46, 18, "|  Z:   ROTATE CCW   |", screen);
		PrintAt(WIDTH / 2 - 46, 19, "|  ESC: PAUSE        |", screen);
		PrintAt(WIDTH / 2 - 46, 20, "|~~~~~~~~~~~~~~~~~~~~|", screen);
	};

	WriteConsoleOutputCharacter(hConsole, screen, WIDTH*HEIGHT, coord, &cWritten);
	Sleep(17); // ~60 FPS

	// GAME LOOP
	int lose = 0;
	int level = 0;
	int score = 0;
	int rotation = 0;
	int CurrentPiece = -1;
	int NextPiece = rand() % 7;
	int pX, pY;
	long long int numFrames = 0;
	int lines = 0;

	while (!lose) {

		// SPAWNING PIECES
		int justSpawned = 1;
		if (CurrentPiece == -1) {
			if (NextPiece == 1) {
				PrintTetromino(NextPiece, WIDTH / 2 - 14, 9, 0, "dErase", screen);
			}
			else if (NextPiece == 6) {
				PrintTetromino(NextPiece, WIDTH / 2 - 16, 10, 0, "dErase", screen);
			}
			else {
				PrintTetromino(NextPiece, WIDTH / 2 - 16, 9, 0, "dErase", screen);
			}
			CurrentPiece = NextPiece;
			NextPiece = rand() % 7;
			rotation = 0;
			pX = BoardCorner.X + 9;
			pY = BoardCorner.Y + 0;
			justSpawned = 5;
			if (NextPiece == 1) {
				PrintTetromino(NextPiece, WIDTH / 2 - 14, 9, 0, "print", screen);
			}
			else if (NextPiece == 6) {
				PrintTetromino(NextPiece, WIDTH / 2 - 16, 10, 0, "print", screen);
			}
			else {
				PrintTetromino(NextPiece, WIDTH / 2 - 16, 9, 0, "print", screen);
			}
		}

		// MOVEMENT & ROTATION
		for (int k = 0; k < 7; k++) {							//  R   U   L   D   Z  Ent Esc 
			bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x26\x25\x28\x5A\x0D\x1B"[k]))) != 0;
		}

		// MOVEMENT
		static long long int lastFrameUnmoving = 0;
		static int pressed = 0;
		if (bKey[0] == 1 && bKey[2] == 0) {
			if (fits(CurrentPiece, pX + 2, pY, rotation, screen)) {
				if (lastFrameUnmoving > numFrames - 8 && pressed == 0) {
					pX += 2;
					pressed = 1;
				}
				else if (numFrames - lastFrameUnmoving > 8 && ((numFrames - 8) - lastFrameUnmoving) % 2 == 0) {
					pX += 2;
				}
			}
		}
		else if (bKey[2] == 0) {
			lastFrameUnmoving = numFrames;
			pressed = 0;
		}
		else if (bKey[2] == 1) {
			if (fits(CurrentPiece, pX - 2, pY, rotation, screen)) {

				if (lastFrameUnmoving > numFrames - 8 && pressed == 0) {
					pX -= 2;
					pressed = 1;
				}
				else if (numFrames - lastFrameUnmoving > 8 && ((numFrames - 8) - lastFrameUnmoving) % 2 == 0) {
					pX -= 2;
				}
			}
		}
		else {
			lastFrameUnmoving = numFrames;
			pressed = 0;
		}

		// ROTATION
		if (bKey[1] == 1) {
			while (bKey[1] == 1) bKey[1] = (0x8000 & GetAsyncKeyState((unsigned char)('\x26'))) != 0;
			if (fits(CurrentPiece, pX, pY, rotation - 1, screen)) {
				rotation--;
			}
			else if (fits(CurrentPiece, pX - 2, pY, rotation - 1, screen)) {
				rotation--;
				pX -= 2;
			}
			else if (fits(CurrentPiece, pX + 2, pY, rotation - 1, screen)) {
				rotation--;
				pX += 2;
			}
			else if (CurrentPiece == 0) {
				if (fits(CurrentPiece, pX - 4, pY, rotation + 1, screen)) {
					rotation--;
					pX -= 4;
				}
				else if (fits(CurrentPiece, pX + 4, pY, rotation + 1, screen)) {
					rotation--;
					pX += 4;
				}
			}
			if (rotation < 0) {
				rotation += 4;
			}
		}
		if (bKey[4] == 1) {
			while (bKey[4] == 1) bKey[4] = (0x8000 & GetAsyncKeyState((unsigned char)('\x5A'))) != 0;
			if (fits(CurrentPiece, pX, pY, rotation + 1, screen)) {
				rotation++;
			}
			else if (fits(CurrentPiece, pX - 2, pY, rotation + 1, screen)) {
				rotation++;
				pX -= 2;
			}
			else if (fits(CurrentPiece, pX + 2, pY, rotation + 1, screen)) {
				rotation++;
				pX += 2;
			}
			else if (CurrentPiece == 0) {
				if (fits(CurrentPiece, pX - 4, pY, rotation + 1, screen)) {
					rotation++;
					pX -= 4;
				}
				else if (fits(CurrentPiece, pX + 4, pY, rotation + 1, screen)) {
					rotation++;
					pX += 4;
				}
			}
			rotation %= 4;
		}

		// REDUNDANT CHECK TO PREVENT INTERSECTIONS
		if (!fits(CurrentPiece, pX, pY, rotation, screen)) {
			if (fits(CurrentPiece, pX - 2, pY, rotation, screen)) {
				pX -= 2;
			}
			else if (fits(CurrentPiece, pX + 2, pY, rotation, screen)) {
				pX += 2;
			}
			else if (CurrentPiece == 0) {
				if (fits(CurrentPiece, pX - 4, pY, rotation, screen)) {
					pX -= 4;
				}
				else if (fits(CurrentPiece, pX + 4, pY, rotation, screen)) {
					pX += 4;
				}
			}
		}

		// PRINT STATS
		char scoreStr[7];
		_itoa(score, scoreStr, 10);
		PrintAt(WIDTH / 2 + 20 - strlen(scoreStr) / 2, 9, scoreStr, screen);
		char levelStr[3];
		_itoa(level, levelStr, 10);
		PrintAt(WIDTH / 2 + 20 - strlen(levelStr) / 2, 12, levelStr, screen);
		char linesStr[4];
		_itoa(lines, linesStr, 10);
		PrintAt(WIDTH / 2 + 20 - strlen(linesStr) / 2, 15, linesStr, screen);

		PrintTetromino(CurrentPiece, pX, pY, rotation, "print", screen);

		// REFRESH FRAME
		WriteConsoleOutputCharacter(hConsole, screen, WIDTH*HEIGHT, coord, &cWritten);
		Sleep(FRAMETIME * justSpawned);

		PrintTetromino(CurrentPiece, pX, pY, rotation, "erase", screen);

		// PAUSE
		if (bKey[6] == 1) {
			//Waiting for key release
			while (bKey[6] == 1) {
				bKey[6] = (0x8000 & GetAsyncKeyState((unsigned char)('\x1B'))) != 0;
			}
			// SAVE STATE
			wchar_t saveState[WIDTH*HEIGHT];
			for (int i = 0; i < WIDTH; i++) {
				for (int j = 0; j < HEIGHT; j++) {
					saveState[j * WIDTH + i] = screen[j * WIDTH + i];
				}
			}
			// WIPE SCREEN
			for (int i = 0; i < WIDTH; i++) {
				for (int j = 0; j < HEIGHT; j++) {
					screen[j * WIDTH + i] = ' ';
				}
			}
			// PRINT PAUSE MENU
			PrintAt(WIDTH / 2 - 13, HEIGHT / 2 - 2, "Press [enter] to continue", screen);
			PrintAt(WIDTH / 2 - 10, HEIGHT / 2, "Press [esc] to quit.", screen);
			WriteConsoleOutputCharacter(hConsole, screen, WIDTH*HEIGHT, coord, &cWritten);
			int paused = 1;
			while (paused) {
				for (int k = 0; k < 2; k++) {
					bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x0D\x1B"[k]))) != 0;
				}
				// RESUME
				if (bKey[0] == 1) {
					paused = 0;
					// LOAD STATE
					for (int i = 0; i < WIDTH; i++) {
						for (int j = 0; j < HEIGHT; j++) {
							screen[j * WIDTH + i] = saveState[j * WIDTH + i];
						}
					}
					WriteConsoleOutputCharacter(hConsole, screen, WIDTH*HEIGHT, coord, &cWritten);
				}
				// QUIT
				else if (bKey[1] == 1) {
					lose = 1;
					paused = 0;
				}
			}

		}

		// SOFT DROP
		int softdropping = 0;
		if (bKey[3] == 1) {
			softdropping = framesPerGridCell[level] - 2;
		}

		// GRAVITY
		if (numFrames % (framesPerGridCell[level] - softdropping) == 0) {
			if (softdropping != 0) {
				score += (level + 1);
			}
			if (fits(CurrentPiece, pX, pY + 1, rotation, screen)) {
				pY++;
			}
			else {
				if (pY == BoardCorner.Y) {
					lose = 1;
				}
				int linesCleared = 0;
				PrintTetromino(CurrentPiece, pX, pY, rotation, "print", screen);
				linesCleared = lineCheck(screen, hConsole, cWritten, coord);
				lines += linesCleared;

				if (linesCleared == 1) {
					score += 40 * (level + 1);
				}
				if (linesCleared == 2) {
					score += 100 * (level + 1);
				}
				if (linesCleared == 3) {
					score += 300 * (level + 1);
				}
				if (linesCleared == 4) {
					score += 1200 * (level + 1);
				}

				level = lines / 10;
				CurrentPiece = -1;
			}
		}

		numFrames++;
	}

	// WIPE SCREEN
	for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < HEIGHT; j++) {
			screen[j * WIDTH + i] = ' ';
			WriteConsoleOutputCharacter(hConsole, screen, WIDTH*HEIGHT, coord, &cWritten);
		}
	}

	//CHECKING FOR NEW HIGH SCORE
	char scoreStr[7];
	_itoa(score, scoreStr, 10);
	if (score > highscore[gameNum]) {
		PrintAt(WIDTH / 2 - 8, 9, "NEW HIGH SCORE!", screen);
		//WRITE NEW HIGH SCORE TO FILE
		rewind(HighScores);
		highscore[gameNum] = score;
		for (int i = 0; i < NUMGAMES; i++) {
			fprintf(HighScores, "%s %d\n", game[i], highscore[i]);
		}
	}

	fclose(HighScores);

	// END FRAME
	PrintAt(WIDTH / 2 - 19, 1, " _____ _____ ___________ _____ _____", screen);
	PrintAt(WIDTH / 2 - 19, 2, "|_===_|==___|_===_|=___=\\_===_/==___|", screen);
	PrintAt(WIDTH / 2 - 19, 3, "==|=|=|=|__===|=|=|=|_/=/=|=|=\\=`--.=", screen);
	PrintAt(WIDTH / 2 - 19, 4, "~~|~|~|~~__|~~|~|~|~~~~/~~|~|~~`--.~\\", screen);
	PrintAt(WIDTH / 2 - 19, 5, "~~|~|~|~|___~~|~|~|~|\\~\\~_|~|_/\\__/~/", screen);
	PrintAt(WIDTH / 2 - 19, 6, "..\\_/.\\____/..\\_/.\\_|.\\_|\\___/\\____/.", screen);
	PrintAt(WIDTH / 2 - 19, 7, ".....................................", screen);

	char fscoreStr[7];
	_itoa(score, fscoreStr, 10);
	PrintAt(WIDTH / 2 - 4, 10, "SCORE: ", screen);
	PrintAt(WIDTH / 2 + 3, 10, fscoreStr, screen);
	PrintAt(WIDTH / 2 - strlen("Press [enter] to play again") / 2, 14, "Press [enter] to play again", screen);
	PrintAt(WIDTH / 2 - 10, 16, "Press [esc] to quit.", screen);
	PrintAt(WIDTH / 2 - (strlen(highscoreStr) + 13) / 2, 12, "HIGH SCORE: ", screen);
	PrintAt(WIDTH / 2 + (strlen(highscoreStr) + 2) / 2, 12, highscoreStr, screen);
	WriteConsoleOutputCharacter(hConsole, screen, WIDTH*HEIGHT, coord, &cWritten);

	int InMenu = 1;
	int playAgain;
	while (InMenu) {
		for (int k = 0; k < 2; k++) {
			bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x0D\x1B"[k]))) != 0;
		}
		if (bKey[0] == 1) {
			playAgain = 1;
			InMenu = 0;
		}
		if (bKey[1] == 1) {
			playAgain = 0;
			InMenu = 0;
		}
	}
	return playAgain;
}

void PrintTetromino(int tetromino, int x, int y, int r, char mode[], wchar_t screen[]) {
	// Tetrominos
	char Tetrominos[7][4][3] = {
	{	{"I."},
		{"I."},
		{"I."},
		{"I."}	},

	{	{".J"},
		{".J"},
		{"JJ"},
		{".."}	},

	{	{"L."},
		{"L."},
		{"LL"},
		{".."}	},

	{	{"S."},
		{"SS"},
		{".S"},
		{".."}	},

	{	{".Z"},
		{"ZZ"},
		{"Z."},
		{".."}	},

	{	{"T."},
		{"TT"},
		{"T."},
		{".."}	},

	{	{"OO"},
		{"OO"},
		{".."},
		{".."}	}
	};
	if (strcmp(mode, "print") == 0) {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 2; j++) {
				if (tetromino == 0) {
					if (Tetrominos[tetromino][i][j] != '.') {
						if (r == 0) {
							screen[(x + j * 2) + (y + i) * WIDTH] = Tetrominos[tetromino][i][j];
						}
						else if (r == 1) {
							screen[(x + i * 2 - 2) + (y - j + 1) * WIDTH] = Tetrominos[tetromino][i][j];
						}
						else if (r == 2) {
							screen[(x - j * 2 + 2) + (y - i + 3) * WIDTH] = Tetrominos[tetromino][i][j];
						}
						else if (r == 3) {
							screen[(x - i * 2 + 4) + (y + j + 2) * WIDTH] = Tetrominos[tetromino][i][j];
						}
					}
				}
				if (tetromino == 1) {
					if (Tetrominos[tetromino][i][j] != '.') {
						if (r == 0) {
							screen[(x + j * 2 - 2) + (y + i) * WIDTH] = Tetrominos[tetromino][i][j];
						}
						else if (r == 1) {
							screen[(x + i * 2 - 2) + (y - j + 2) * WIDTH] = Tetrominos[tetromino][i][j];
						}
						else if (r == 2) {
							screen[(x - j * 2 + 2) + (y - i + 2) * WIDTH] = Tetrominos[tetromino][i][j];
						}
						else if (r == 3) {
							screen[(x - i * 2 + 2) + (y + j) * WIDTH] = Tetrominos[tetromino][i][j];
						}
					}
				}
				if (tetromino == 2 || tetromino == 3 || tetromino == 4 || tetromino == 5) {
					if (Tetrominos[tetromino][i][j] != '.') {
						if (r == 0) {
							screen[(x + j * 2) + (y + i) * WIDTH] = Tetrominos[tetromino][i][j];
						}
						else if (r == 1) {
							screen[(x + i * 2 - 2) + (y - j + 1) * WIDTH] = Tetrominos[tetromino][i][j];
						}
						else if (r == 2) {
							screen[(x - j * 2) + (y - i + 2) * WIDTH] = Tetrominos[tetromino][i][j];
						}
						else if (r == 3) {
							screen[(x - i * 2 + 2) + (y + j + 1) * WIDTH] = Tetrominos[tetromino][i][j];
						}
					}
				}
				if (tetromino == 6) {
					if (Tetrominos[tetromino][i][j] == 'O') {
						screen[(x + j * 2) + (y + i) * WIDTH] = Tetrominos[tetromino][i][j];
					}
				}
			}
		}
	}
	else if (strcmp(mode, "erase") == 0) {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 2; j++) {
				if (tetromino == 0) {
					if (Tetrominos[tetromino][i][j] != '.') {
						if (r == 0) {
							screen[(x + j * 2) + (y + i) * WIDTH] = '.';
						}
						else if (r == 1) {
							screen[(x + i * 2 - 2) + (y - j + 1) * WIDTH] = '.';
						}
						else if (r == 2) {
							screen[(x - j * 2 + 2) + (y - i + 3) * WIDTH] = '.';
						}
						else if (r == 3) {
							screen[(x - i * 2 + 4) + (y + j + 2) * WIDTH] = '.';
						}
					}
				}
				if (tetromino == 1) {
					if (Tetrominos[tetromino][i][j] != '.') {
						if (r == 0) {
							screen[(x + j * 2 - 2) + (y + i) * WIDTH] = '.';
						}
						else if (r == 1) {
							screen[(x + i * 2 - 2) + (y - j + 2) * WIDTH] = '.';
						}
						else if (r == 2) {
							screen[(x - j * 2 + 2) + (y - i + 2) * WIDTH] = '.';
						}
						else if (r == 3) {
							screen[(x - i * 2 + 2) + (y + j) * WIDTH] = '.';
						}
					}
				}
				if (tetromino == 2 || tetromino == 3 || tetromino == 4 || tetromino == 5) {
					if (r == 0) {
						if (Tetrominos[tetromino][i][j] != '.') {
							screen[(x + j * 2) + (y + i) * WIDTH] = '.';
						}
					}
					else if (r == 1) {
						if (Tetrominos[tetromino][i][j] != '.') {
							screen[(x + i * 2 - 2) + (y - j + 1) * WIDTH] = '.';
						}
					}
					else if (r == 2) {
						if (Tetrominos[tetromino][i][j] != '.') {
							screen[(x - j * 2) + (y - i + 2) * WIDTH] = '.';
						}
					}
					else if (r == 3) {
						if (Tetrominos[tetromino][i][j] != '.') {
							screen[(x - i * 2 + 2) + (y + j + 1) * WIDTH] = '.';
						}
					}
				}
				if (tetromino == 6) {
					if (Tetrominos[tetromino][i][j] == 'O') {
						screen[(x + j * 2) + (y + i) * WIDTH] = '.';
					}
				}
			}
		}
	}
	else if (strcmp(mode, "dErase") == 0) {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 2; j++) {
				if (tetromino == 0) {
					if (Tetrominos[tetromino][i][j] != '.') {
						if (r == 0) {
							screen[(x + j * 2) + (y + i) * WIDTH] = ' ';
						}
						else if (r == 1) {
							screen[(x + i * 2 - 2) + (y - j + 1) * WIDTH] = ' ';
						}
						else if (r == 2) {
							screen[(x - j * 2 + 2) + (y - i + 3) * WIDTH] = ' ';
						}
						else if (r == 3) {
							screen[(x - i * 2 + 4) + (y + j + 2) * WIDTH] = ' ';
						}
					}
				}
				if (tetromino == 1) {
					if (Tetrominos[tetromino][i][j] != '.') {
						if (r == 0) {
							screen[(x + j * 2 - 2) + (y + i) * WIDTH] = ' ';
						}
						else if (r == 1) {
							screen[(x + i * 2 - 2) + (y - j + 2) * WIDTH] = ' ';
						}
						else if (r == 2) {
							screen[(x - j * 2 + 2) + (y - i + 2) * WIDTH] = ' ';
						}
						else if (r == 3) {
							screen[(x - i * 2 + 2) + (y + j) * WIDTH] = ' ';
						}
					}
				}
				if (tetromino == 2 || tetromino == 3 || tetromino == 4 || tetromino == 5) {
					if (r == 0) {
						if (Tetrominos[tetromino][i][j] != '.') {
							screen[(x + j * 2) + (y + i) * WIDTH] = ' ';
						}
					}
					else if (r == 1) {
						if (Tetrominos[tetromino][i][j] != '.') {
							screen[(x + i * 2 - 2) + (y - j + 1) * WIDTH] = ' ';
						}
					}
					else if (r == 2) {
						if (Tetrominos[tetromino][i][j] != '.') {
							screen[(x - j * 2) + (y - i + 2) * WIDTH] = ' ';
						}
					}
					else if (r == 3) {
						if (Tetrominos[tetromino][i][j] != '.') {
							screen[(x - i * 2 + 2) + (y + j + 1) * WIDTH] = ' ';
						}
					}
				}
				if (tetromino == 6) {
					if (Tetrominos[tetromino][i][j] != '.') {
						screen[(x + j * 2) + (y + i) * WIDTH] = ' ';
					}
				}
			}
		}
	}
}

int fits(int tetromino, int tx, int ty, int tr, wchar_t screen[]) {
	char Tetrominos[7][4][3] = {
{	{"I."},
	{"I."},
	{"I."},
	{"I."}	},

{	{".J"},
	{".J"},
	{"JJ"},
	{".."}	},

{	{"L."},
	{"L."},
	{"LL"},
	{".."}	},

{	{"S."},
	{"SS"},
	{".S"},
	{".."}	},

{	{".Z"},
	{"ZZ"},
	{"Z."},
	{".."}	},

{	{"T."},
	{"TT"},
	{"T."},
	{".."}	},

{	{"OO"},
	{"OO"},
	{".."},
	{".."}	}
	};

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 2; j++) {
			if (tetromino == 0) {
				if (Tetrominos[tetromino][i][j] != '.') {
					if (tr == 0) {
						if (screen[(tx + j * 2) + (ty + i) * WIDTH] != '.') {
							return 0;
						}
					}
					else if (tr == 1) {
						if (screen[(tx + i * 2 - 2) + (ty - j + 1) * WIDTH] != '.') {
							return 0;
						}
					}
					else if (tr == 2) {
						if (screen[(tx - j * 2 + 2) + (ty - i + 3) * WIDTH] != '.') {
							return 0;
						}
					}
					else if (tr == 3) {
						if (screen[(tx - i * 2 + 4) + (ty + j + 2) * WIDTH] != '.') {
							return 0;
						}
					}
				}
			}
			if (tetromino == 1) {
				if (Tetrominos[tetromino][i][j] != '.') {
					if (tr == 0) {
						if (screen[(tx + j * 2 - 2) + (ty + i) * WIDTH] != '.') {
							return 0;
						}
					}
					else if (tr == 1) {
						if (screen[(tx + i * 2 - 2) + (ty - j + 2) * WIDTH] != '.') {
							return 0;
						}
					}
					else if (tr == 2) {
						if (screen[(tx - j * 2 + 2) + (ty - i + 2) * WIDTH] != '.') {
							return 0;
						}
					}
					else if (tr == 3) {
						if (screen[(tx - i * 2 + 2) + (ty + j) * WIDTH] != '.') {
							return 0;
						}
					}
				}
			}
			if (tetromino == 2 || tetromino == 3 || tetromino == 4 || tetromino == 5) {
				if (Tetrominos[tetromino][i][j] != '.') {
					if (tr == 0) {
						if (screen[(tx + j * 2) + (ty + i) * WIDTH] != '.') {
							return 0;
						}
					}
					else if (tr == 1) {
						if (screen[(tx + i * 2 - 2) + (ty - j + 1) * WIDTH] != '.') {
							return 0;
						}
					}
					else if (tr == 2) {
						if (screen[(tx - j * 2) + (ty - i + 2) * WIDTH] != '.') {
							return 0;
						}
					}
					else if (tr == 3) {
						if (screen[(tx - i * 2 + 2) + (ty + j + 1) * WIDTH] != '.') {
							return 0;
						}
					}
				}
			}
			if (tetromino == 6) {
				if (Tetrominos[tetromino][i][j] != '.') {
					if (screen[(tx + j * 2) + (ty + i) * WIDTH] != '.') {
						return 0;
					}
				}
			}
		}
	}
	return 1;
}

int lineCheck(wchar_t screen[], HANDLE hConsole, DWORD cWritten, COORD coord) {
	COORD BoardCorner;
	BoardCorner.X = WIDTH / 2 - 9;
	BoardCorner.Y = 5;

	int ClearLines[7];
	int numLines = 0;
	int emptyLine;
	int numELines = 1;
	for (int i = 20; i >= 0; i--) {
		int fullLine = 0;
		for (int j = 1; j < 20; j += 2) {
			if (screen[(BoardCorner.X + j) + (BoardCorner.Y + i) * WIDTH] != '.') {
				fullLine++;
			}
		}
		if (fullLine == 0) {
			emptyLine = i;
		}
		if (fullLine >= 10) {
			ClearLines[numLines] = i;
			ClearLines[numLines + 1] = i;
			ClearLines[numLines + 2] = i;
			ClearLines[numLines + 3] = i;
			numLines++;
		}
	}

	if (numLines >= 1) {
		for (int i = 0; i < 21; i++) {
			PrintAt(BoardCorner.X + i, BoardCorner.Y + ClearLines[0], "=", screen);
			PrintAt(BoardCorner.X + i, BoardCorner.Y + ClearLines[1], "=", screen);
			PrintAt(BoardCorner.X + i, BoardCorner.Y + ClearLines[2], "=", screen);
			PrintAt(BoardCorner.X + i, BoardCorner.Y + ClearLines[3], "=", screen);
			WriteConsoleOutputCharacter(hConsole, screen, WIDTH*HEIGHT, coord, &cWritten);
			Sleep(1);
		}
	}

	for (int i = numLines - 1; i >= 0; i--) {
		for (int h = ClearLines[i]; h > emptyLine; h--) {
			for (int j = 0; j <= 20; j++) {
				screen[BoardCorner.X + j + (BoardCorner.Y + h) * WIDTH] = screen[BoardCorner.X + j + (BoardCorner.Y + h - 1) * WIDTH];
				screen[BoardCorner.X + j + (BoardCorner.Y + h - 1) * WIDTH] = screen[BoardCorner.X + j + (BoardCorner.Y + emptyLine) * WIDTH];
			}
		}
	}

	return numLines;
}