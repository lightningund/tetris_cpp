#include "lib.h"
#include "pieces.h"
#include <utility>

Piece getPieceFromFullPieceAndRot(FullPiece fullPiece, int rot)
{
	Piece base = fullPiece.base;
	Vec2f center = fullPiece.center;

	int s = rot == 3 ? -1 : rot % 2;
	int c = rot == 2 ? -1 : (rot + 1) % 2;

	for(int i = 0; i < 4; i ++)
	{
		Vec2f part = base[i];
		part.x -= center.x;
		part.y -= center.y;
		float xnew = part.x * c - part.y * s;
		float ynew = part.x * s + part.y * c;
		base[i].x = center.x + xnew;
		base[i].y = center.y + ynew;
	}

	return base;
}

Piece getPieceFromTypeAndRot(int type, int rot) { return getPieceFromFullPieceAndRot(pieces[type], rot); }

Piece getPieceFromMovingPiece(MovingPiece movingPiece) { return getPieceFromFullPieceAndRot(movingPiece.full, movingPiece.rot); }

// Creates a MovingPiece from a type, rotation, x, and y
MovingPiece makeMovingPiece(int type, int rot, int x, int y)
{
	FullPiece fullPiece = pieces[type];
	MovingPiece piece;

	piece.x = x;
	piece.y = y;
	piece.full = fullPiece;
	piece.rot = rot;

	return piece;
}

Vec2i::operator Vec2f() const {
	return Vec2f {(float)x, (float)y};
};

Vec2f::operator Vec2i() const {
	return Vec2i {(int)x, (int)y};
};

void Cell::reset()
{
	col = BLACK;
	prevCol = BLACK;
	on = false;
}

void Row::reset()
{
	filled = false;
	flashTimer = -1;
	for (Cell& cell : cells) cell.reset();
}

void Row::update()
{
	if (filled || flashTimer != -1) return;
	for (const Cell& cell : cells) if (!cell.on) return;

	filled = true;
	flashTimer = FLASH_FRAMES;
}

// Assign the current piece to the next piece and generates a new random next piece
void Board::newPiece()
{
	currPiece.full = pieces[nextPieces[nextPieceInd]];
	currPiece.rot = 0;
	currPiece.x = NUM_CELLS_X / 2;
	currPiece.y = 0;
	nextPieceInd ++;
	if(nextPieceInd == 7) shufflePieces();
}

// Clear a row and shifts down all other rows above it
void Board::clearRow(int rowNum)
{
	for (int j = rowNum; j > 1; j--) rows[j] = rows[j - 1];
	rows[0].reset();
}

void Board::shufflePieces()
{
	nextPieceInd = 0;
	int pieceNumbers[] = {0, 1, 2, 3, 4, 5, 6};
	for(int i = 0; i < 7; i ++)
	{
		int randInd = (int)(rand() % 7);
		std::swap(pieceNumbers[i], pieceNumbers[randInd]);
	}
	for(int i = 0; i < 7; i ++)
	{
		nextPieces[i] = pieceNumbers[i];
	}
}