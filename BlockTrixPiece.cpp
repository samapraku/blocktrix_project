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

/*
 * Institute of Automation - University of Bremen
 *BLOCKTRIX GAME AUTO PLAYER
 * GROUP MEMBERS:
 *  ERIC GAMOR
 * JIBIN JOHN
 * SAMUEL  APRAKU
 *
*/

#include <QtCore>

#include <stdlib.h>

#include "BlockTrixPiece.h"
#include<QDebug>


void BlockTrixPiece::setShape(BlockType_E shape)
{
    static const int coordsTable[8][4][2] = {
        /* block 1*/  /* block 2*/   /* block 3*/  /* block 4*/
        { { 0, -1 },  { 0, 0 },     { 0, 1 },   { 0, 2 } },  /*  shape 1 I */
        { { -1, 0 },  { 0, 0 },     { 1, 0 },   { 1, -1 } },  /*  shape 2 L*/
        { { -1, 0 },  { 0, 0 },     { 1, 0 },   { 0, -1 } },  /*  shape 3  T*/
        { { -1, -1},  { -1, 0},     { 0, 0 },   { 1,  0 } }, /*  shape 4 J*/
        { { 0, -1 },  { 0, 0 },     { 1, 0 },   { 1,  1 } },  /*  shape 5 S */
        { { 0, 0 },   { 1, 0 },     { 0, 1 },   { 1,  1 } },  /*  shape 6  O*/
        { { 0, -1 },  { 0, 0 },     { -1,0 },   {-1,  1 } },  /*  shape 7  Z*/  /*  */
        { { 0, 0 },   { 0, 0 },     { 0, 0 },   { 0,  0 } }  /*  shape 8  no shape*/
    };

    for (int i = 0; i < 4 ; i++) {  // for each of the 4 blocks
        for (int j = 0; j < 2; ++j)     // positions (x,y)
            coords[i][j] = coordsTable[shape][i][j];   // each block is asigned an x and y coordinate depeding on the she
        //cords[0][0]= cordsTable[2][]
    }
    pieceShape = shape;
}


// to give us the  starting and ending point and ending pointing of each piece.
// with this the appropriate translation can be applied to position the the pieces correctly
// on the board
int BlockTrixPiece::minX() const
{
    int min = coords[0][0];   // starting from block 0
    for (int i = 1; i < 4; ++i)
        min = qMin(min, coords[i][0]);   /*cords[i][0] means x coordinates */
    //  qDebug()<<"in minX()";
    //  qDebug()<<min;

    return min;
}

int BlockTrixPiece::maxX() const
{
    int max = coords[0][0];
    for (int i = 1; i < 4; ++i)
        max = qMax(max, coords[i][0]);
    return max;
}

int BlockTrixPiece::minY() const
{
    int min = coords[0][1];
    for (int i = 1; i < 4; ++i)
        min = qMin(min, coords[i][1]);
    return min;
}

int BlockTrixPiece::maxY() const
{
    int max = coords[0][1];
    for (int i = 1; i < 4; ++i)
        max = qMax(max, coords[i][1]);
    return max;
}


// rotation of -90 about the origion
//(x,y)--->(y,-x)  so (3,4)--->(4,-3)
//
BlockTrixPiece BlockTrixPiece::rotatedLeft() const
{
    if (pieceShape == O_Block_e)   // for square there is no change
        return *this;


    BlockTrixPiece result;
    result.pieceShape = pieceShape;
    for (int i = 0; i < 4; ++i) {
        result.setX(i, y(i));
        result.setY(i, -x(i));
    }
    return result;
}

// rotation of +90 about the origion
//(x,y)--->(-y,x)  so (3,4)--->(-4,3)

BlockTrixPiece BlockTrixPiece::rotatedRight() const
{
    if (pieceShape == O_Block_e)
        return *this;

    BlockTrixPiece result;
    result.pieceShape = pieceShape;
    for (int i = 0; i < 4; ++i) {
        result.setX(i, -y(i));
        result.setY(i, x(i));
    }
    return result;
}
