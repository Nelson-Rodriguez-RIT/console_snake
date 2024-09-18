#pragma once

const int PlayfieldWidth = 40;
const int PlayfieldHeight = 20;

const double GlobalTimerLength = 250;

enum Direction {
	RIGHT,
	LEFT,
	UP,
	DOWN,
	NONE
};


enum GameState { // Global game state
	EXIT,
	MENU,
	PLAYING,
	GAMEOVER,
	HIGHSCORE
};
GameState STATE{ MENU };

enum MenuState { // Used for navigating the menu
	MENU_START,
	MENU_SCORES,
	MENU_EXIT
};

enum PlayingState {
	INIT,
	ACTIVE
};

void initializeConsole();

void drawGraphicsBuffer(char graphics[PlayfieldHeight][PlayfieldWidth]);

bool updateDeltaTime(double &dt, double &prevTiming, double &timer);

void updateSnakeBody(Direction newDirection, Direction snake[]);