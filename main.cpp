#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include "lib.h"
#include "pieces.h"

using namespace std;
using namespace sf;

constexpr bool real = false;
constexpr bool useDefaultKeyEvents = true;

Board board;

unsigned int score, linesCleared;

unsigned short dropTime, dropTimer;

uint_fast64_t nextUpdateTime = 0, nextCalcTime = 0;
unsigned int frameCount = 0, frameRateCounter = 0;
float frameRate;

bool hitFloor;

RenderWindow *wind;

Clock clk;

map<int, Key> keys;

bool checkPiece(Board board, MovingPiece piece)
{
	Piece activePiece = getPieceFromMovingPiece(piece);
	int PX = piece.x;
	int PY = piece.y;

	for (Vec2i part : activePiece)
	{
		int checkX = PX + part.x;
		if(checkX >= NUM_CELLS_X || checkX < 0) return false;
		
		int checkY = PY + part.y;
		if(checkY >= NUM_CELLS_Y || checkY < 0) return false;

		if (board.rows[checkY].cells[checkX].on) return false;
	}

	return true;
}

bool checkPiece(MovingPiece piece) { return checkPiece(board, piece); }

bool checkPiece() { return checkPiece(board.currPiece); }

bool setPiece(MovingPiece phantom)
{
	if (!checkPiece(phantom)) return false;
	
	board.currPiece = phantom;
	return true;
}

bool movePiece(int dx, int dy)
{
	MovingPiece phantom = board.currPiece;
	phantom.x += dx;
	phantom.y += dy;
	return setPiece(phantom);
}

bool rotatePiece()
{
	MovingPiece phantom = board.currPiece;
	phantom.rot++;
	phantom.rot %= 4;
	return setPiece(phantom);
}

// Check if the key is down and is ready to be processed again
bool checkKey(Key key) { return key.down && key.heldTime % key.repeatTime == 0; }

void repeatableInit()
{
	dropTime = 30;
	dropTimer = 0;

	hitFloor = false;

	score = 0;

	board.hasHeld = false;

	board.shufflePieces();
	board.newPiece();

	for (Row &row : board.rows) row.reset();
}

void init()
{
	srand(time(0));

	repeatableInit();
}

void keyPressed(Keyboard::Key key)
{
	if (keys[key].down) return;

	Key replacement{true, 0, KEY_REPEAT};

	if (key == DOWNA || key == DOWNB) replacement.repeatTime = 5;

	keys[key] = replacement;
}

void keyReleased(Keyboard::Key key)
{
	Key replacement{false, 0};
	keys[key] = replacement;
}

void checkEvents()
{
	// Loop Through all the events that happened this frame
	Event event;
	while (wind->pollEvent(event))
	{
		if (event.type == Event::Closed)
			wind->close();
		else if (event.type == Event::KeyPressed)
			keyPressed(event.key.code);
		else if (event.type == Event::KeyReleased)
			keyReleased(event.key.code);
	}
}

bool canDrop(MovingPiece piece)
{
	int PY = piece.y + 1;
	int PX = piece.x;
	Piece activePiece = getPieceFromMovingPiece(piece);

	for (Vec2i part : activePiece)
	{
		int checkY = PY + part.y;
		int checkX = PX + part.x;
		if (checkY >= NUM_CELLS_Y || board.rows[checkY].cells[checkX].on)
			return true;
	}

	return false;
}

MovingPiece findDrop(MovingPiece piece)
{
	bool hitFloor = false;

	MovingPiece phantom = piece;
	hitFloor = canDrop(phantom);

	while (!hitFloor)
	{
		phantom.y++;
		hitFloor = canDrop(phantom);
	}

	return phantom;
}

void dropPiece()
{
	MovingPiece droppedPiece = findDrop(board.currPiece);
	int PX = droppedPiece.x;
	int PY = droppedPiece.y;
	Piece activePiece = getPieceFromMovingPiece(droppedPiece);
	for (Vec2i part : activePiece)
	{
		Cell *cell = &board.rows[PY + part.y].cells[PX + part.x];
		cell->on = true;
		cell->col = droppedPiece.full.color;
	}

	board.newPiece();
}

void swapHeld()
{
	if (!board.hasHeld)
	{
		board.hasHeld = true;
		board.heldPiece = board.currPiece.type;
		board.newPiece();
	}
	else
	{
		int tempPiece = board.heldPiece;
		board.heldPiece = board.currPiece.type;
		MovingPiece piece;
		piece.x = NUM_CELLS_X / 2;
		piece.y = 0;
		piece.full = pieces[tempPiece];
		piece.rot = 0;
		board.currPiece = piece;
	}
}

void checkInputs()
{
	if (checkKey(keys[LEFTA]) || checkKey(keys[LEFTB]))
		movePiece(-1, 0);
	if (checkKey(keys[RIGHTA]) || checkKey(keys[RIGHTB]))
		movePiece(1, 0);
	if (checkKey(keys[DOWNA]) || checkKey(keys[DOWNB]))
		if (!movePiece(0, 1))
			dropPiece();

	if (checkKey(keys[57])) // Quick Drop
		dropPiece();
	if (checkKey(keys[22]) || checkKey(keys[73])) // Rotate
		rotatePiece();
	if (checkKey(keys[7])) // Swap for held piece
		swapHeld();
}

void clearLines()
{
	int lines = 0;
	for (Row &row : board.rows)
	{
		row.update();
		if (row.filled)
		{
			if (row.flashTimer == 0)
			{
				lines++;
				row.flashTimer = -1;
				board.clearRow(&row - board.rows);
			}
			else
			{
				short flashCol = ((row.flashTimer >> 2) % 2) * 255;
				row.flashTimer--;
				for (Cell &cell : row.cells)
					cell.col = Color(flashCol, flashCol, flashCol);
			}
		}
	}

	score += lines * lines;

	if (lines != 0)
		cout << score << endl;
}

void update()
{
	checkEvents();
	checkInputs();

	clearLines();

	for (auto &[code, val] : keys)
		if (val.down)
		{
			if (useDefaultKeyEvents)
			{
				val.down = false;
				val.heldTime = 0;
			}
			else val.heldTime++;
		}
}

void drawUIPiece(bool isHeld)
{
	unsigned int xoff = isHeld ? PIECE_SIZE : (WIDTH - 5 * PIECE_SIZE);

	RectangleShape bg(Vector2f(PIECE_SIZE * 4, PIECE_SIZE * 4));
	bg.setPosition(xoff, PIECE_SIZE);
	bg.setFillColor(BLACK);
	bg.setOutlineColor(WHITE);
	bg.setOutlineThickness(1);
	wind->draw(bg);

	if ((isHeld && board.hasHeld) || !isHeld)
	{
		int selInd = isHeld ? board.heldPiece : board.nextPieces[board.nextPieceInd];
		FullPiece selPiece = pieces[selInd];
		const Color drawCol = selPiece.color;
		RectangleShape piecePart(Vector2f(PIECE_SIZE, PIECE_SIZE));
		Piece activePiece = selPiece.base;

		for (Vec2i part : activePiece)
		{
			piecePart.setPosition(xoff + (part.x * PIECE_SIZE), PIECE_SIZE + (part.y * PIECE_SIZE));
			piecePart.setOutlineThickness(0);
			piecePart.setFillColor(drawCol);
			wind->draw(piecePart);
		}
	}
}

void drawNext() { drawUIPiece(false); }
void drawHeld() { drawUIPiece(true); }

void renderUI()
{
	wind->clear(GREY); //Clear the window
	drawHeld();
	drawNext();
}

void drawPiece(MovingPiece piece, Color col)
{
	int PX = piece.x;
	int PY = piece.y;
	Piece activePiece = getPieceFromMovingPiece(piece);

	for (Vec2i part : activePiece)
	{
		RectangleShape pp(Vector2f(CELL_SIZE, CELL_SIZE));
		pp.setPosition(XOFF + ((PX + part.x) * CELL_SIZE), ((PY + part.y) * CELL_SIZE));
		pp.setFillColor(col);
		wind->draw(pp);
	}
}

void drawPiece(MovingPiece piece) { drawPiece(piece, piece.full.color); }

void drawPiece() { drawPiece(board.currPiece, board.currPiece.full.color); }

void pseudoDrop(MovingPiece piece)
{
	Color col = piece.full.color;
	col.a = 100;

	MovingPiece dropped = findDrop(piece);

	drawPiece(dropped, col);
}

void pseudoDrop() { pseudoDrop(board.currPiece); }

void renderBoard()
{
	// Draw the black background
	RectangleShape rect(Vector2f(NUM_CELLS_X * CELL_SIZE, HEIGHT));
	rect.setPosition(XOFF, 0);
	rect.setFillColor(BLACK);
	wind->draw(rect);

	for (Row &row : board.rows)
	{
		int i = &row - board.rows;
		for (Cell &cell : row.cells)
		{
			if (!cell.on) continue;

			// If the cell is on but black revert it to it's previous color
			if (cell.col == BLACK) cell.col = cell.prevCol;

			int j = &cell - row.cells;

			RectangleShape cellRect(Vector2f(CELL_SIZE, CELL_SIZE));
			cellRect.setPosition(j * CELL_SIZE + XOFF, i * CELL_SIZE);
			cellRect.setFillColor(cell.col);
			wind->draw(cellRect);
		}
	}

	drawPiece();
	pseudoDrop();
}

void render()
{
	renderUI();
	renderBoard();

	wind->display();
}

int main()
{
	RenderWindow window(VideoMode(WIDTH, HEIGHT), "Tetris");
	wind = &window;

	init();

	// Main Loop
	while (wind->isOpen())
	{
		uint_fast64_t currTime = clk.getElapsedTime().asMicroseconds();

		// Check if we should update
		if (currTime >= nextUpdateTime)
		{
			nextUpdateTime = currTime + (1000000 / FRAMERATE);
			frameCount++;
			frameRateCounter++;

			update();
			render();
		}

		if (currTime >= nextCalcTime)
		{
			nextCalcTime = currTime + 1000000;
			frameRate = frameRateCounter;
			frameRateCounter = 0;
		}
	}

	return 0;
}
