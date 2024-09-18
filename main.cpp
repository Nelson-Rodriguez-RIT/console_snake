#include "main.h"

#define _CRT_SECURE_NO_WARNINGS // Puts a pillow over the compiler's face (sometimes) :)

#include <Windows.h>	// Window related functionality
#include <iostream>		// Console initialization
#include <chrono>		// Deltatime calculations
#include <stdlib.h>		// RNG

HANDLE hStdout;

typedef std::chrono::steady_clock::time_point steady_clock_t;


int WinMain(
	HINSTANCE	hInstance,
	HINSTANCE	hPrevInstance,
	LPSTR		lpCmdLine,
	int			nShowCmd
) {
	// Initialize random seed
	srand(clock());

	// Set up console related functionality
	initializeConsole();

	// Used to help display text by redrawing over previous text
	COORD COORD_HOME = COORD();
	COORD_HOME.X = 0;
	COORD_HOME.Y = 0;


	// Calculating DeltaTime to sync the game speed properly 
	double delta{ 0 }, updateRelevantDelta{ 0 };
	steady_clock_t startTime{}, endTime{};

	// Used to limit how often game code/rendering is done
	// Not needed, but prevents a simple snake console app from eating the CPU and PSU
	double updateTimer{ 0 }, renderTimer{ 0 };


	// Contains text used to display information/game content
	char graphicsBuffer[GraphicsHeight][GraphicsWidth]{};
	for (int index{ 0 }; index < GraphicsHeight * GraphicsWidth; index++)
		graphicsBuffer[0][index] = ' ';


	// Used for navigating the menu
	int menuState{ MENU_START };


	// Used during gameplay
	COORD snakeHeadPosition{ COORD() };
	COORD snakeSegmentPosition{ COORD() };

	Direction snakeHeadDirection;			// Direction of the head is based on the direction the snake is moving
	Direction snakeSegments[MaxSnakeSize];	// Direction of segments is based on the direction from the head to the body segment
	Direction tailDirection{ DOWN };

	double movementTimer{ 0 };

	COORD fruitPosition{ COORD() };


	while (STATE != EXIT) {
		if (STATE != INIT) {	// Calculates delta time, needs at least one cycle beforehand
			delta = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime).count();
			updateRelevantDelta += delta;	// Used to properly update time restricted logic while
											// still avoiding updating every possible moment

			if ((updateTimer += delta) >= UpdateTimerTarget)
				updateTimer = 0;

			if ((renderTimer += delta) >= RenderTimerTarget)
				renderTimer = 0;
		}
		startTime = std::chrono::steady_clock::now();


		// Checks for input constantly, and will remember what the user's input was
		// even during downtimes. Resets to INPUT_NONE once game logic is updated
		registerInput();	// Could you count this as input buffering?

		if (updateTimer == 0) {	// Updates gameplay related logic/functionality
			switch (STATE) {
			case INIT:	
				STATE = MAIN_MENU;
				STATE_INITIALIZED = false;
				break;

			case MAIN_MENU:
				if (!STATE_INITIALIZED) { // Loads menu graphics
					modifyGraphicsBuffer(graphicsBuffer, GRAPHICS_MENU, 0);

					STATE_INITIALIZED = true;
				}


				// Handle user input
				if (USER_INPUT == INPUT_UP)
					menuState -= menuState > 0 ? 1 : 0;

				if (USER_INPUT == INPUT_DOWN)
					menuState += menuState < 1 ? 1 : 0;

				if (USER_INPUT == INPUT_RETURN)
					switch ((MenuState)menuState) {
					case MENU_START:
						STATE = PLAYING;
						STATE_INITIALIZED = false;
						break;

					case MENU_EXIT:
						STATE = EXIT;
						break;
					}


				// Updates drawn cursor
				graphicsBuffer[3 + menuState][7] = '<';
				graphicsBuffer[4 - menuState][7] = ' ';
				break;

			case PLAYING:
				if (!STATE_INITIALIZED) {
					// Draws playfield
					modifyGraphicsBuffer(graphicsBuffer, GRAPHICS_PLAYFIELD_BORDER, 0);
					for (int row{ 1 }; row < GraphicsHeight - 1; row++)
						modifyGraphicsBuffer(graphicsBuffer, GRAPHICS_PLAYFIELD_FIELD, row);
					modifyGraphicsBuffer(graphicsBuffer, GRAPHICS_PLAYFIELD_BORDER, GraphicsHeight - 1);

					// Prepare snake player character
					for (int segment{ 1 }; segment < MaxSnakeSize; segment++)
						snakeSegments[segment] = NONE;

					// Starting orientation
					snakeSegments[0] = LEFT;
					snakeHeadDirection = RIGHT;

					// Initialize fruit
					fruitPosition.X = (rand() % (GraphicsWidth - 1)) + 1;
					fruitPosition.Y = (rand() % (GraphicsHeight - 1)) + 1;

					// Starting position
					snakeHeadPosition.X = 4;
					snakeHeadPosition.Y = 3;

					STATE_INITIALIZED = true;
				}
				snakeSegmentPosition.X = snakeHeadPosition.X;
				snakeSegmentPosition.Y = snakeHeadPosition.Y;


				// Draws the snake
				graphicsBuffer[snakeHeadPosition.Y][snakeHeadPosition.X] = 'x';	// Head
				for (int segment{ 0 }; snakeSegments[segment] != NONE; segment++) {
					switch (snakeSegments[segment]) {							// Body
					case UP:
						graphicsBuffer[--snakeSegmentPosition.Y][snakeSegmentPosition.X] = 'o';
						break;

					case DOWN:
						graphicsBuffer[++snakeSegmentPosition.Y][snakeSegmentPosition.X] = 'o';
						break;

					case RIGHT:
						graphicsBuffer[snakeSegmentPosition.Y][++snakeSegmentPosition.X] = 'o';
						break;

					case LEFT:
						graphicsBuffer[snakeSegmentPosition.Y][--snakeSegmentPosition.X] = 'o';
						break;
					}

					// Check while writing to buffer if body segments ever collide with head
					if (snakeHeadPosition.X == snakeSegmentPosition.X &&
						snakeHeadPosition.Y == snakeSegmentPosition.Y
						) {
						STATE = MAIN_MENU;
						STATE_INITIALIZED = false;
					}
				}
					
					
				// Draws fruit
				graphicsBuffer[fruitPosition.Y][fruitPosition.X] = '@';


				// User input handling
				switch (USER_INPUT) {
				case INPUT_UP:
					snakeHeadDirection = UP;
					break;

				case INPUT_DOWN:
					snakeHeadDirection = DOWN;
					break;

				case INPUT_LEFT:
					snakeHeadDirection = LEFT;
					break;

				case INPUT_RIGHT:
					snakeHeadDirection = RIGHT;
					break;

				case INPUT_ESCAPE:
					STATE = MAIN_MENU;
					STATE_INITIALIZED = false;
					break;
				}
					

				// Moves the player's snake character automatically. Accounts for the seperation between lines
				if ((movementTimer += updateRelevantDelta) >= MovementTimerTarget) {
					movementTimer = 0;

					// Adjust segments direction and removes trailing tail leftovers
					tailDirection = moveSnake(snakeSegments, snakeHeadDirection);
					graphicsBuffer[snakeSegmentPosition.Y][snakeSegmentPosition.X] = ' ';

					// Adjust snake head's coordinates
					switch (snakeHeadDirection) {
					case UP:
						snakeHeadPosition.Y--;
						break;

					case DOWN:
						snakeHeadPosition.Y++;
						break;

					case LEFT:
						snakeHeadPosition.X--;
						break;

					case RIGHT:
						snakeHeadPosition.X++;
						break;
					}
				}


				// Causes a game over if player "collides" with the boundary
				if (snakeHeadPosition.Y <= 0 || snakeHeadPosition.Y >= GraphicsHeight - 1 ||
					snakeHeadPosition.X <= 0 || snakeHeadPosition.X >= GraphicsWidth - 1
					) {
					STATE = MAIN_MENU;
					STATE_INITIALIZED = false;
				}

					

				// Fruit collection logic
				if (snakeHeadPosition.X == fruitPosition.X &&
					snakeHeadPosition.Y == fruitPosition.Y
					) {
					for (int segment{ 1 }; segment < MaxSnakeSize; segment++)
						if (snakeSegments[segment] == NONE) {
							snakeSegments[segment] = tailDirection;
							break;
						}

					fruitPosition.X = (rand() % (GraphicsWidth - 1)) + 1;
					fruitPosition.Y = (rand() % (GraphicsHeight - 1)) + 1;
				}

				break;
			}

			USER_INPUT = INPUT_NONE;
			updateRelevantDelta = 0;
		}

		if (renderTimer == 0) {	// Draw gameplay graphics
			SetConsoleCursorPosition(hStdout, COORD_HOME);	// Sets the cursor to beginning of the console
															// Allows us to redraw instead of clearing the console

			for (int row{ 0 }; row < GraphicsHeight; row++) {
				for (int col{ 0 }; col < GraphicsWidth; col++)
					std::cout << graphicsBuffer[row][col];

				std::cout << '\n';
			}
		}

		endTime = std::chrono::steady_clock::now();
	}
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

void registerInput() {
	if (GetAsyncKeyState(VK_UP))
		USER_INPUT = INPUT_UP;

	if (GetAsyncKeyState(VK_DOWN))
		USER_INPUT = INPUT_DOWN;

	if (GetAsyncKeyState(VK_LEFT))
		USER_INPUT = INPUT_LEFT;

	if (GetAsyncKeyState(VK_RIGHT))
		USER_INPUT = INPUT_RIGHT;

	if (GetAsyncKeyState(VK_RETURN))
		USER_INPUT = INPUT_RETURN;

	if (GetAsyncKeyState(VK_ESCAPE))
		USER_INPUT = INPUT_ESCAPE;
}

void modifyGraphicsBuffer(
	char graphicsBuffer[GraphicsHeight][GraphicsWidth], 
	const char lineOfText[GraphicsWidth],
	int row
) {
	for (int col{ 0 }; col < GraphicsWidth; col++)
		graphicsBuffer[row][col] = lineOfText[col];
}

void modifyGraphicsBuffer(
	char graphicsBuffer[GraphicsHeight][GraphicsWidth],
	const char blockOfText[GraphicsHeight][GraphicsWidth],
	int row
) {
	for (; row < GraphicsHeight; row++)
		for (int col{ 0 }; col < GraphicsWidth; col++)
			graphicsBuffer[row][col] = blockOfText[row][col];
}

Direction moveSnake(Direction snakeSegments[MaxSnakeSize], Direction towards) {
	Direction tailDirection{ NONE };	// Used to help erase leftover tail text

	for (int segment{ MaxSnakeSize - 1 }; segment > 0; segment--) {
		if (snakeSegments[segment] == NONE)
			continue;

		if (tailDirection == NONE)
			tailDirection = snakeSegments[segment];

		snakeSegments[segment] = snakeSegments[segment - 1];
	}
	
	if (tailDirection == NONE)	// Only matters if snake is 1 segment long (head doesnt count)
		tailDirection = snakeSegments[0];

	snakeSegments[0] = (Direction)(towards % 2 == 0 ? towards + 1 : towards - 1);

	return tailDirection;
}