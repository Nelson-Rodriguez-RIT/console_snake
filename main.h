#pragma once

enum Direction {
	UP,
	DOWN,
	LEFT,
	RIGHT,
	NONE,
};

enum MenuState {
	MENU_START,
	MENU_EXIT
};

enum InputState {
	INPUT_NONE,
	INPUT_RETURN,
	INPUT_UP,
	INPUT_DOWN,
	INPUT_LEFT,
	INPUT_RIGHT,
	INPUT_ESCAPE
};
InputState USER_INPUT{ INPUT_NONE };

enum GameState {
	INIT,		// Initializes game elements (including delta time)
	MAIN_MENU,
	PLAYING,
	EXIT
};
GameState STATE{ INIT };
bool STATE_INITIALIZED{ false };


// Determines how long (in something) to wait before updating/rendering
const double UpdateTimerTarget = 0.025;
const double RenderTimerTarget = 0.025;

// Determines how big of an area there is to display graphics (text)
const int GraphicsWidth = 40;
const int GraphicsHeight = 20;

const int MaxSnakeSize = 256;
const double MovementTimerTarget = 0.125;

// Static text objects used to aid in drawing gameplay content to the console
const char GRAPHICS_MENU[GraphicsHeight][GraphicsWidth]{
	{'+', '-', '-', '-', '-', '-', '-', '-', '+'},
	{'|', ' ', 'S', 'n', 'a', 'k', 'e', ' ', '|'},
	{'+', '-', '-', '-', '-', '-', '-', '-', '+'},
	{'|', ' ', 'S', 't', 'a', 'r', 't', ' ', '|'},
	{'|', ' ', 'E', 'x', 'i', 't', ' ', ' ', '|'},
	{'+', '-', '-', '-', '-', '-', '-', '-', '+'}
};

const char GRAPHICS_PLAYFIELD_BORDER[GraphicsWidth]{
	'+', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '+'
};
const char GRAPHICS_PLAYFIELD_FIELD[GraphicsWidth]{
	'|', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '|'
};



void initializeConsole();

void registerInput();

void modifyGraphicsBuffer(
	char graphicsBuffer[GraphicsHeight][GraphicsWidth],
	const char lineOfText[GraphicsWidth],
	int row
);
void modifyGraphicsBuffer(
	char graphicsBuffer[GraphicsHeight][GraphicsWidth],
	const char blockOfText[GraphicsHeight][GraphicsWidth],
	int row
);

Direction moveSnake(Direction snakeSegments[MaxSnakeSize], Direction towards);