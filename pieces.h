#pragma once

#ifndef PIECES
#define PIECES

#include "lib.h"

// Line Piece
// Pieces: (0, 1), (1, 1), (2, 1), (3, 1)
// Center: (1.5, 1.5)

// L Piece A
// Pieces: (0, 0), (0, 1), (1, 1), (2, 1)
// Center: (1, 1)

// L Piece B
// Pieces: (0, 1), (1, 1), (2, 1), (2, 0)
// Center: (1, 1)

// Square
// Pieces: (1, 1), (1, 2), (2, 1), (2, 2)
// Center: (1.5, 1.5)

// Z Piece A
// Pieces: (0, 1), (1, 1), (1, 0), (2, 0)
// Center: (1, 1)

// T Piece
// Pieces: (0, 1), (1, 1), (1, 0), (2, 1)
// Center: (1, 1)

// Z Piece B
// Pieces: (0, 0), (1, 0), (1, 1), (2, 1)
// Center: (1, 1)

const FullPiece pieces[7] =
{ // Enter the FullPiece Array Constructor
	{ // Enter the FullPiece Constructor
		{ // Enter the Piece Constructor (aka std::array)
			{ // Enter the Vec2i Array Constructor
				{ // Enter the Vec2i Constructor
					0, 1
				}, {1, 1}, {2, 1}, {3, 1}
			}
		}, { // Enter the Center Constructor
			1.5, 1.5
		},
		CYAN
	},
	{{{{0, 0}, {0, 1}, {1, 1}, {2, 1}}}, {1, 1}, ORANGE},
	{{{{0, 1}, {1, 1}, {2, 1}, {2, 0}}}, {1, 1}, BLUE},
	{{{{1, 1}, {1, 2}, {2, 1}, {2, 2}}}, {1.5, 1.5}, YELLOW},
	{{{{0, 1}, {1, 1}, {1, 0}, {2, 0}}}, {1, 1}, GREEN},
	{{{{0, 1}, {1, 1}, {1, 0}, {2, 1}}}, {1, 1}, PURPLE},
	{{{{0, 0}, {1, 0}, {1, 1}, {2, 1}}}, {1, 1}, RED},
};

#endif