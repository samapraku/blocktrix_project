/****************************************************************************
 **
 ** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 ** All rights reserved.
 ** Contact: Nokia Corporation (qt-info@nokia.com)
 **
 ** This file is part of the examples of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:BSD$
 ** You may use this file under the terms of the BSD license as follows:
 **
 ** "Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 **   * Redistributions of source code must retain the above copyright
 **     notice, this list of conditions and the following disclaimer.
 **   * Redistributions in binary form must reproduce the above copyright
 **     notice, this list of conditions and the following disclaimer in
 **     the documentation and/or other materials provided with the
 **     distribution.
 **   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
 **     the names of its contributors may be used to endorse or promote
 **     products derived from this software without specific prior written
 **     permission.
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/


#ifndef BLOCKTRIXPIECE_H
#define BLOCKTRIXPIECE_H

#include "Interfaces.h"

class BlockTrixPiece
{
public:
    BlockType_E NoShape;   // used to initialise our board

    BlockTrixPiece() {
        NoShape = (BlockType_E)7;  // cast NoShape to 7 which is not included in enum BlockType_E
        setShape(NoShape);  //  Initialise board with no shape
    }

    void setShape(BlockType_E shape);

    BlockType_E shape() const { return pieceShape; }
    int x(int index) const { return coords[index][0]; }   //  returns the coordinate of x when a block (index) is given and the block is known
    int y(int index) const { return coords[index][1]; }    // returns the coordinate of y when the block(index) is given
    int minX() const;   //the minimum value of x
    int maxX() const;
    int minY() const;
    int maxY() const;
    BlockTrixPiece rotatedLeft() const;   // -90 rotation
    BlockTrixPiece rotatedRight() const;   // +90 rotation

private:
    void setX(int index, int x) { coords[index][0] = x; }  // x position of the block
    void setY(int index, int y) { coords[index][1] = y; }   // y position of the block

    BlockType_E pieceShape; // an object of the enumeration
    int coords[4][2];       // 4 blocks with 2 points (x,y) each.
};

#endif
