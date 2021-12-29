#pragma once

#ifndef UTIL
#define UTIL

#include <SFML/Graphics.hpp>
#include <array>

const sf::Color BLACK(0, 0, 0, 255);
const sf::Color GREY(128, 128, 128, 255);
const sf::Color WHITE(255, 255, 255, 255);
const sf::Color RED(255, 0, 0, 255);
const sf::Color GREEN(0, 255, 0, 255);
const sf::Color BLUE(0, 0, 255, 255);
const sf::Color YELLOW(255, 255, 0, 255);
const sf::Color PURPLE(255, 0, 255, 255);
const sf::Color CYAN(0, 255, 255, 255);
const sf::Color ORANGE(255, 128, 0, 255);

constexpr unsigned short WIDTH = 800;
constexpr unsigned short HEIGHT = 800;
constexpr unsigned short NUM_CELLS_X = 10;
constexpr unsigned short NUM_CELLS_Y = 25;
constexpr unsigned short CELL_SIZE = HEIGHT / NUM_CELLS_Y;
constexpr unsigned short PIECE_SIZE = 25;
constexpr unsigned short FLASH_FRAMES = 20;
constexpr unsigned short FRAMERATE = 60;
constexpr unsigned short UPDATEFRAMES = 30;
constexpr unsigned short KEY_REPEAT = 15;

constexpr float XOFF = ((WIDTH - (NUM_CELLS_X * CELL_SIZE)) / 2);

constexpr int LEFTA = 0;
constexpr int LEFTB = 71;
constexpr int RIGHTA = 3;
constexpr int RIGHTB = 72;
constexpr int DOWNA = 18;
constexpr int DOWNB = 74;

struct Vec2i;
struct Vec2f;

//2D-Vector with ints
struct Vec2i {
	int x;
	int y;

	operator Vec2f() const;
};

struct Vec2f {
	float x;
	float y;

	operator Vec2i() const;
};

//Tetris Piece
using Piece = std::array<Vec2i,4>;

//Tetris Piece with a base piece and a center point and color
struct FullPiece {
	Piece base;
	Vec2f center;
	sf::Color color;
};

struct MovingPiece {
	FullPiece full;
	int type, rot, x, y;
};

struct Cell {
	bool on = false;
	sf::Color col = BLACK;
	sf::Color prevCol = BLACK;

	void reset();
};

struct Row {
	bool filled;
	int flashTimer;
	Cell cells[NUM_CELLS_X];

	void reset();
	void update();
};

struct Board {
	bool hasHeld;
	int heldPiece, nextPieceInd;
	int nextPieces[7];
	MovingPiece currPiece;
	Row rows[NUM_CELLS_Y];

	void newPiece();
	void clearRow(int rowNum);
	void shufflePieces();
};

struct Key {
	bool down;
	int heldTime;
	int repeatTime;
};

Piece getPieceFromFullPieceAndRot(FullPiece fullPiece, int rot);
Piece getPieceFromTypeAndRot(int type, int rot);
Piece getPieceFromMovingPiece(MovingPiece movingPiece);
MovingPiece makeMovingPiece(int type, int rot, int x, int y);

#endif
