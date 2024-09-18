#include "old_main.h"

#include <Windows.h>
#include <iostream>
#include <time.h>

#include <cstdint>
#include <iomanip>
#include <utility>

#pragma warning(disable : 4996)

HANDLE hStdout;

int WinMain(
	HINSTANCE	hInstance,
	HINSTANCE	hPrevInstance,
	LPSTR		lpCmdLine,
	int			nShowCmd
) {
	// Set up console related functionality
	initializeConsole();

	COORD COORD_HOME = COORD();
	COORD_HOME.X = 0;
	COORD_HOME.Y = 0;

	// Prepare timing elements
	double previousTiming{ (double)clock() };
	double deltaTime{ 0 };

	double updateTimer{ 0 }; // Resets after reaching 1


	while (STATE != EXIT) {
		// For navigating the menu
		int menuState{ MENU_START };

		// Used to store both gameplay graphics and elements
		char playfield[PlayfieldHeight][PlayfieldWidth];

		COORD snakeHeadPos{ COORD() };
		COORD snakeSegmentPos{ COORD() };
		Direction snakeSegments[256]{};


		switch (STATE) {
		case MENU:
			while (STATE == MENU) {
				// Resets cursor to allow for redrawing
				SetConsoleCursorPosition(hStdout, COORD_HOME);

				std::cout <<
					" +=+ Snake +=+\n"
					" (Used Arrows Keys To Navigate)\n"
					"  Start   " << (menuState == MENU_START ? '<' : ' ') <<
					"\n  Scores  " << (menuState == MENU_SCORES ? '<' : ' ') <<
					"\n  Exit    " << (menuState == MENU_EXIT ? '<' : ' ');
				

				// Menu navigation
				if (GetAsyncKeyState(VK_UP) & 0b1 && menuState != MENU_START)
					menuState--;
				
				if (GetAsyncKeyState(VK_DOWN) & 0b1 && menuState != MENU_EXIT)
					menuState++;
				
				if (GetAsyncKeyState(VK_RETURN)) {
					switch (menuState) {
					case MENU_START:
						STATE = PLAYING;
						break;

					case MENU_SCORES:
						STATE = HIGHSCORE;
						break;

					case MENU_EXIT:
						STATE = EXIT;
						break;
					}
				}
					
			}
			break;

		case PLAYING:
			snakeHeadPos.X = 6;
			snakeHeadPos.Y = 4;

			for (int segment{ 0 }; segment < 256; segment++)
				snakeSegments[segment] = NONE;

			snakeSegments[0] = DOWN;
			snakeSegments[1] = DOWN;


			while (STATE == PLAYING) {
				if (!updateDeltaTime(deltaTime, previousTiming, updateTimer))
					continue;

				SetConsoleCursorPosition(hStdout, COORD_HOME);
				
				
				// Add snake head to playfield
				playfield[snakeHeadPos.Y][snakeHeadPos.X] = 'x';

				// Add snake body to playfield
				snakeSegmentPos.X = snakeHeadPos.X;
				snakeSegmentPos.Y = snakeHeadPos.Y;

				for (int segment{ 0 }; segment < 256; segment++) {
					if (snakeSegments[segment] == NONE)
						break;

					switch (snakeSegments[segment]) {
					case RIGHT:
						playfield[snakeSegmentPos.Y][++snakeSegmentPos.X] = 'o';
						break;

					case LEFT:
						playfield[snakeSegmentPos.Y][--snakeSegmentPos.X] = 'o';
						break;

					case UP:
						playfield[--snakeSegmentPos.Y][snakeSegmentPos.X] = 'o';
						break;

					case DOWN:
						playfield[++snakeSegmentPos.Y][snakeSegmentPos.X] = 'o';
						break;
					}
				}

				// Draw playfield
				for (int row{ 0 }; row < PlayfieldHeight; row++) {
					for (int col{ 0 }; col < PlayfieldWidth; col++)
						std::cout << playfield[row][col];

					std::cout << '\n';
				}

				// Get player input
				// TODO: Update consistently but without updating extremely fast as it stands
				//		 the 0b1 prevents reading a held input rapidly until a few moments have passed


				if (GetAsyncKeyState(VK_UP)) {
					updateSnakeBody(UP, snakeSegments);
					// Removes extra snake segment leftover from moving snake
					playfield[snakeSegmentPos.Y][snakeSegmentPos.X] = ' ';	
					snakeHeadPos.Y--;
				}

				else if (GetAsyncKeyState(VK_DOWN)) {
					updateSnakeBody(DOWN, snakeSegments);
					playfield[snakeSegmentPos.Y][snakeSegmentPos.X] = ' ';
					snakeHeadPos.Y++;
				}

				else if (GetAsyncKeyState(VK_RIGHT)) {
					updateSnakeBody(RIGHT, snakeSegments);
					playfield[snakeSegmentPos.Y][snakeSegmentPos.X] = ' ';
					snakeHeadPos.X++;
				}

				else if (GetAsyncKeyState(VK_LEFT)) {
					updateSnakeBody(LEFT, snakeSegments);
					playfield[snakeSegmentPos.Y][snakeSegmentPos.X] = ' ';
					snakeHeadPos.X--;
				}
				
				else
					switch (snakeSegments[0]) {
					case DOWN:
						updateSnakeBody(UP, snakeSegments);
						// Removes extra snake segment leftover from moving snake
						playfield[snakeSegmentPos.Y][snakeSegmentPos.X] = ' ';
						snakeHeadPos.Y--;
						break;

					case UP:
						updateSnakeBody(DOWN, snakeSegments);
						playfield[snakeSegmentPos.Y][snakeSegmentPos.X] = ' ';
						snakeHeadPos.Y++;
						break;

					case RIGHT:
						updateSnakeBody(LEFT, snakeSegments);
						playfield[snakeSegmentPos.Y][snakeSegmentPos.X] = ' ';
						snakeHeadPos.X--;
						break;

					case LEFT:
						updateSnakeBody(RIGHT, snakeSegments);
						playfield[snakeSegmentPos.Y][snakeSegmentPos.X] = ' ';
						snakeHeadPos.X++;
						break;
					}
			}

			break;
		}

		
	}

	return 0;
}

void initializeConsole() {
	// Prepare and load console
	AllocConsole();

	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr);
	std::cout.sync_with_stdio();

	// Console handle allows us to use more features of the console
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

	// Turn off cursor visability (blinks annoyingly otherwise)
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(hStdout, &cursorInfo);
	cursorInfo.bVisible = false;
	SetConsoleCursorInfo(hStdout, &cursorInfo);
}

void drawGraphicsBuffer(char graphics[PlayfieldHeight][PlayfieldWidth]) {

}

bool updateDeltaTime(double &dt, double &prevTiming, double &timer) {
	dt = clock() - prevTiming;
	prevTiming = (double)clock();

	if (timer += dt >= GlobalTimerLength) {
		timer = 0;
		return true;
	}

	return false;
}

void updateSnakeBody(Direction towardsDirection, Direction snake[]) {
	for (int segment{ 255 }; segment > 0; segment--) {
		if (snake[segment] == NONE)
			continue;

		snake[segment] = snake[segment - 1];
	}

	snake[0] = (Direction)(towardsDirection % 2 == 0 ? towardsDirection + 1 : towardsDirection - 1);
}