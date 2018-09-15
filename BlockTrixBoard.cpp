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

#include <QtWidgets>

#include "BlockTrixBoard.h"
#include <QDebug>

BlockTrixBoard::BlockTrixBoard(QWidget *parent)
    : QFrame(parent)
{
    // Initialize board with new block
    Empty_Block_e = (BlockType_E)7;  // we need a new

    setFrameStyle(QFrame::Panel | QFrame::Plain );
    setContentsMargins(0,0,0,0);
    setFocusPolicy(Qt::StrongFocus);

    bIsStarted = false;      // start button not yet clicked
    bIsPaused = false;       // pause button not clicked
    bAutoPlay = false;      // autoplayer has not started yet

    InitBoard();  // we clear the board and initialise the scores and lines removed

}

void BlockTrixBoard::setNextPieceLabel(QLabel *label)
{
    nextPieceLabel = label;
}

QSize BlockTrixBoard::sizeHint() const
{
    return QSize(BoardWidth * 15 + frameWidth() * 2, BoardHeight * 15 + frameWidth() * 2);
}

QSize BlockTrixBoard::minimumSizeHint() const
{
    return QSize(BoardWidth * 5 + frameWidth() * 2,BoardHeight * 5 + frameWidth() * 2);
}


void BlockTrixBoard::start(int player)
{

    // if player == 1, autoplay else manual player
    bAutoPlay =  player == 1 ? true:false;

    // if game is paused, don't continue
    if (bIsPaused)
        return;

    // reset all column heights to zero
    for(int hh=0; hh<10; hh++)
    {
        ColumnHeights[hh]=0;
    }


    BlockType_E newBlock = GetNextBlockType(); //randomly generate a new block

    // if it is autoplayer, we do not want z and s blocks to follow consecutively
    if(bAutoPlay)
    {
        while(newBlock ==S_Block_e || newBlock ==Z_Block_e)
        {
            newBlock = GetNextBlockType();
        }
    }
    else {
        // we only need the timer for auto player. it is is user, stop the timer
        timer.stop();  }

    nextPiece.setShape(newBlock); // set the next block to the nextpiece

    bIsStarted = true;  // game has started
    bIsWaitingAfterLine = false;  // a line has not been removed here
    iTotalRowsRemoved = 0; // reset total rows removed
    iScore = 0;
    InitBoard();

    emit linesRemovedChanged(iTotalRowsRemoved);
    emit scoreChanged(iScore);

    newPiece();

    if(bAutoPlay)
        timer.start(iTimerInterval(), this); // start timer for auto player

    // startBtn->setEnabled(false);

}


// WORKS BY STOPPING THE INTERNAL TIMER OF THE CLOCK
// PERFORMS OTHER CHECKS BEFORE DOING THAT

void BlockTrixBoard::pause()
{
    if (!bIsStarted)
        return;

    bIsPaused = !bIsPaused;
    if (bIsPaused) {
        //check if it is auto play before stopping
        if(bAutoPlay)
            timer.stop();
    } else {

        //check if it is auto play before starting
        if(bAutoPlay)
            timer.start(iTimerInterval(), this);
    }
    update();
}

void BlockTrixBoard::paintEvent(QPaintEvent *event)
{
    QFrame::paintEvent(event);

    QPainter painter(this);
    QRect rect = contentsRect();   // returns the innner area

    if (bIsPaused) {
        painter.drawText(rect, Qt::AlignCenter, tr("Pause"));   // dsiplay pause the board
        return;
    }



    int boardTop = rect.bottom() - BoardHeight*iSquareHeight();   // get the value at the top. FOR WHAT?

    // GIVEN ANY POSITION ON THE BOARD (WIDTH, HEIGHT) WE SHOULD BE ABLE TO LOCATE THE SHAPE THERE
    // IN THIS CASE IT IS RUNNING THROUGH THE WHOLE BOARD, LOCATING THE SHAPE THERE AND DRAWING IT IF ANY
    // DRAW THE PIECES ON THE BOARD
    for (int i = 0; i < BoardHeight; ++i) {
        for (int j = 0; j < BoardWidth; ++j) {
            BlockType_E shape = shapeAt(j, BoardHeight - i - 1);
            if (shape != Empty_Block_e)
                drawSquare(painter, rect.left() + j * iSquareWidth(),
                           boardTop + i * iSquareHeight(), shape);
            //
        }

    }
    // this draws only the current piece. that piece that has not been dropped
    // if a piece is DROPPED, the  x,y position on the board is SET TO THAT PIECE
    // A PIECE NOT DROPPED does does not have it set.
    // The board is an ARRAY OF BlockTrixpieces
    // if a shape is selected draw it the specified position
    if (curPiece.shape() != Empty_Block_e) {
        for (int i = 0; i < 4; ++i) { // every piece is made up of 4 blocks of 2 cordinates (x,y ) each
            int x = iCurX + curPiece.x(i);   // this x is in pixels. so this converts the given x cordinate to pixels so
            // so that the painter.drawRect can use
            int y = iCurY - curPiece.y(i);      //convert to pixel
            drawSquare(painter, rect.left() + x * iSquareWidth(),boardTop + (BoardHeight - y - 1) * iSquareHeight(),
                       curPiece.shape());
        }
    }


}

// for keyboard input

void BlockTrixBoard::keyPressEvent(QKeyEvent *event)
{
    // we don't want to take key inputs when these conditions are set
    if (!bIsStarted || bIsPaused || curPiece.shape() == Empty_Block_e || bAutoPlay) {
        QFrame::keyPressEvent(event);
        return;
    }


    switch (event->key()) {
    case Qt::Key_Left:
        Move(MoveLeft_e);  // left key pressed move left
        break;
    case Qt::Key_Right:
        Move(MoveRight_e); // right key pressed move right
        break;
    case Qt::Key_Down:
        Move(RotateClockwise_e);        // down key pressed rotate right
        break;
    case Qt::Key_Up:
        Move(RotateCounterClockwise_e); // up key pressed rotate  left
        break;
    case Qt::Key_Space:
        Move(Release_e);        // space key pressed release  block
        break;
    case Qt::Key_D:
        oneLineDown();      // move one step down. it is for emergencies
        break;
    default:
        QFrame::keyPressEvent(event);
    }
}


void BlockTrixBoard::autoPlayer()
{
    bAutoPlay = true;
    start(1); // 1 is used to set auto play in the start method
}

void BlockTrixBoard::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == timer.timerId()) {
        if (bIsWaitingAfterLine) {
            bIsWaitingAfterLine = false;
            newPiece();
            timer.start(iTimerInterval(), this);
        } else {
            oneLineDown();
        }
    } else {
        QFrame::timerEvent(event);
    }
}


// CALLED WHEN THE SPACE BAR IS PRESSED
void BlockTrixBoard::releaseBlock()
{
    int newY = iCurY;
    while (newY > 0) {   // first check if its not negative
        if (!tryMove(curPiece, iCurX, newY - 1))   // then check to see if movement is possible
            break;    // if it is not possible , exit
        --newY;     // if movement is possible decrement the y coordinate
    }
    // update board on about the piece that is at current position
    pieceDropped();
}

void BlockTrixBoard::oneLineDown()
{
    if (!tryMove(curPiece, iCurX, iCurY - 1))
        pieceDropped();
}



/* 1. IT SETS THE BOARD TO A PIECE WHICH IS DROPPED.
 * 2. IT COUNTS THE NUMBER OF PIECES DROPPED
  * 3. REMOVES FULL LINES.
 * 4. CREATES NEW PIECES.
 */
void BlockTrixBoard::pieceDropped()
{
    for (int i = 0; i < 4; ++i) {
        int x = iCurX + curPiece.x(i);    // get the position of each block of the piece
        int y = iCurY - curPiece.y(i);
        shapeAt(x, y) = curPiece.shape();   // set that position to the current piece.
        // THE BOARD IS AN ARRAY OF BlockTrixPIECES
    }

    RemoveRows(); // check if there is any full row, remove and update number of rows removed

    int removedFullRows = GetRemovedRows();  // get the number of rows removed

    if(removedFullRows > 0)
    {
        // If rows were removed, calulate points
        SetPoints(removedFullRows);  // update current points
        bIsWaitingAfterLine = false;
        iTotalRowsRemoved += removedFullRows;  // update total number of rows removed
        emit linesRemovedChanged(iTotalRowsRemoved); // notify UI to update rows removed
    }


    if (!bIsWaitingAfterLine)
        newPiece(); // generate a new piece
}

/**
 * @brief BlockTrixBoard::newPiece()
 * 1. Sets the current piece to the next piece
 * 2. Generates a new block randomly and assigns it to the nextpiece
 * 3. Does calculation of column heights and surfaces to determine where to place blocks for auto player
 *
  *
 *
 */

void BlockTrixBoard::newPiece()
{
    curPiece = nextPiece;    // THE NEXT PIECE CREATED IS ASSIGNED TO THE CURRENT PEACE.

    // get next block type
    BlockType_E nextType =  GetNextBlockType(); //Generate a new block;

    // This section is for autoplayer
    if(bAutoPlay)
    {
        GetColumnHeights();  // calculate heights of all the columns

        // we don't want same blocks to appear consecutively
        while(nextType == curPiece.shape())
        {
            nextType = GetNextBlockType();
        }

        // check how high the stacks are and send rescue
        // If the difference between the highest and lowest columns is more than 5, send line block to clear
        if(ColumnHeights[iHighestColumn] - ColumnHeights[iShortestColumn] >=5)
        { nextType = I_Block_e; }

    }

    // assign the block to the next piece
    nextPiece.setShape(nextType);  // THE DEFAULT IS NO SHAPE, SO NOW GIVE IT A RANDOM SHAPE

    showNextPiece();  // show the next piece on the board

    iCurX = BoardWidth / 2 ;    // position the new piece at the middle

    iCurY = BoardHeight - 1 + curPiece.minY();   // SO THAT PART OF THE PIECE IS NOT LOST
    // IT ENSURES THE CORRECT PLACEMENT OF THE PIECES

    if (!tryMove(curPiece, iCurX, iCurY)) {   // IF MOVEMENT IS NOT POSSIBLE WITH THE CURRENT POSITION (X,Y)
        curPiece.setShape(Empty_Block_e);    // CHANGE TO NO SHAPE SO THAT IT IS NOT DRAWN

        if(bAutoPlay)
            timer.stop();    // STOP THE TIMER if it is player playing

        bIsStarted = false;  // LET THE PROGRAM know THAT THE TIMER IS NOT STARTED
        // when this happens the game is over
    }

    if(bAutoPlay)
    {
        // if we have reached here, it means the game is not over
        if(curPiece.shape() != Empty_Block_e)
        {
            GetColumnHeights();  // calculate heights of the columns
            BestFitColumn();    // We determine the best fitting position to place the block
            int move =   SortColumns(curPiece.shape());  // get the column the block should be moved to

            tryMove(curPiece,move, iCurY);   // move the block piece to the column that was returned.
            //  dropDown();
        }
    }

}
/**
 * @brief BlockTrixBoard::showNextPiece
 *
 * Calls the drawsquare function, which draws the piece on a label.
 * PROCEDURE (lOGIC)
 * 1. gETS THE DISTANCE BETWEEN THE MINX AND MAXX OF THE PIECE,
 * THIS CONVERTED TO PIXELS FOR PIXMAP. THE CONVERSTION IS DONE USING THE SQUAREWIDTH
 * SAME THING IS DONE TO THE MINY
 * 2.   GOES THROUGH EACH BLOCK OF THE PIECE GET THE DISTANCES, WHICH ARE THEN CONVERTED
 * TO PIXELS FOR DRAWING
 *
 *
 */
// WE JUST DISPLAY, DRAW THE NEXTPIECE
void BlockTrixBoard::showNextPiece()
{
    if (!nextPieceLabel)  //if the label has not been set no piece has been generated, return
        return;

    int dx = nextPiece.maxX() - nextPiece.minX() + 1;   // THE DISTANCE BETWEEN THE TWO x POINTS
    int dy = nextPiece.maxY() - nextPiece.minY() + 1;
    // CONVERTS THE HEIGHT AND WITH OF THE BLOCK TO PIXELS
    QPixmap pixmap(dx * iSquareWidth(), dy * iSquareHeight());
    QPainter painter(&pixmap);
    painter.fillRect(pixmap.rect(), nextPieceLabel->palette().background());

    for (int i = 0; i < 4; ++i) {   // DRAW EACH BLOCK THAT MAKE UP THE PIECE
        int x = nextPiece.x(i) - nextPiece.minX();  // THE HORIZONTAL DISTANCE OF THE BLOCK
        int y = nextPiece.y(i) - nextPiece.minY();   // THE VERTICAL DISTANCE OF THE BLOCK
        drawSquare(painter, x * iSquareWidth(), y * iSquareHeight(),   // PASS THESE VALUES TO THE DRAWSQUARE FUNCTION
                   nextPiece.shape());  // TOGETHER WITH THE SHAPE AND DRAW
    }
    nextPieceLabel->setPixmap(pixmap);    //  THE BLOCKS ARE DRAWN UNTO A PIXMAP WHICH IS SET UNTO A LABEL
}


/**
 * @brief BlockTrixBoard::tryMove
 * @param newPiece
 * @param newX
 * @param newY
 * @return
 *  1.GIVEN A POSITION X AND Y, IT CHECKS TO SEE IF MOVEMENT IS POSSIBLE IN THAT POSITION
 * IF MOVEMENT IS POSSIBLE, THAT POSITION (newX,newY) is then USED AS THE curX, and CurY.
 *
 *
 */
bool BlockTrixBoard::tryMove(const BlockTrixPiece &newPiece, int newX, int newY)
{
    for (int i = 0; i < 4; ++i) {
        int x = newX + newPiece.x(i);   // UPDATE THE X POSITON in pixel
        int y = newY - newPiece.y(i);   // UPDATE THE Y POSITION FOR US

        // if movement is not possible, Game over
        // there is a block in the top four rows
        if(IsFinished())
        {
            nextPieceLabel->setText(tr("Game Over"));
            return false;
        }

        if (x < 0 || x >= BoardWidth || y < 0 || y >= BoardHeight)   // IF THIS POSITION OUTSIDE THE BOARD
            return false;   // THEN MOVEMENT IS NOT POSSIBLE

        if (shapeAt(x, y) != Empty_Block_e)    // ALSO THAT PARTICULAR POSITION IS ALREADY FILLED WITH SHAPE
            return false;    // THEN NO MOVEMENT IS POSSIBLE
    }

    // If MOVEMENT IS POSSIBLE
    curPiece = newPiece;  // WE NOW SET THE NEW PIECE AS THE CURRENT PIECE
    iCurX = newX;  // SET THE newX AS THE CURRENT X POSITION
    iCurY = newY;
    update();    // UPDATE THE SCREEN
    return true;
}

/**
 * @brief BlockTrixBoard::drawSquare
 * @param painter
 * @param x
 * @param y
 * @param shape
 *
 * Draw the blocks that make up the piece given the the x,y (in pixels
 * 2. it maps the shape type to a table of colors
 */

void BlockTrixBoard::drawSquare(QPainter &painter, int x, int y, BlockType_E shape)
{
    // each shape is mapped unto a particular color
    static const QRgb colorTable[8] = {
        0x6666CC, 0xCC6666, 0x66CC66,
        0xCCCC66, 0xCC66CC, 0x66CCCC, 0xDAAA00,0x000000
    };

    QColor color = colorTable[int(shape)];   // set the color to the one that corresponds the shape
    // paint each square with the color given
    painter.fillRect(x + 1, y + 1, iSquareWidth() - 2, iSquareHeight() - 2, color);

    painter.setPen(color.light());
    painter.drawLine(x, y + iSquareHeight() - 1, x, y);
    painter.drawLine(x, y, x + iSquareWidth() - 1, y);

    painter.setPen(color.dark());
    painter.drawLine(x + 1, y + iSquareHeight() - 1, x + iSquareWidth() - 1, y + iSquareHeight() - 1);
    painter.drawLine(x + iSquareWidth() - 1, y + iSquareHeight() - 1, x + iSquareWidth() - 1, y + 1);
}


// returns a number from 0 to 7 Representing the shapes stored on the board
// 0 means empty
unsigned int BlockTrixBoard::GetField(unsigned int PosX, unsigned int PosY) const
{
    unsigned int field =  board[(PosY * BoardWidth) + PosX];   // converting to pixel to get the position on the board

    // we used 7 for no shapes, so if the value is 7,
    //we return 0 else we increase it by 1 to indicate there is a block
    field = field < 7 ? field+1:0;
    return field;
}



bool BlockTrixBoard::IsFinished() const
{
    // checks if there is a block at the given position
    unsigned int y= bAutoPlay ? BoardHeight-1 : BoardHeight-3;    // the first row.

    for(unsigned int x=0;x<BoardWidth;++x){
        if(GetField(x,y)==0)   // if the position is empty
            ;       // do nothing , just continue to the next
        else
            return true;  // return true if a block is found in that row
    }
    return false;
}

void BlockTrixBoard::RemoveRows()
{
    int iNumFullLines = 0;   // reset number of full lines to 0.

    // STARTS THE COUNT FROM THE BOTTOM. WELL THATS WHERE THE PIECES ARE DROPPED
    // ITS MOVING BOTTOM UP
    for (int i = BoardHeight - 1; i >= 0; --i) {
        // we first assume that a particular line is filled with shapes
        bool bLineIsFull = true;
        // we go through the width to see if there is a shape stored there
        for (int j = 0; j < BoardWidth; ++j) {
            if (shapeAt(j, i) == Empty_Block_e) {
                // if there is no shape
                bLineIsFull = false;
                // if a particular line has a shape missing we exit.
                break;
            }
        }

        // if all the lines have been filled with shapes FOR THAT PARTICULAR ROW (i). OR HEIGHT
        if (bLineIsFull) {
            ++iNumFullLines;   // we increase the number of filled lines
            for (int k = i; k < BoardHeight - 1; ++k) { // THE PARTICULAR HEIGHT OR ROW FOUND TO BE FILLED
                for (int j = 0; j < BoardWidth; ++j)   // GO THROUGH THE WIDTH OF THAT ROW
                    shapeAt(j, k) = shapeAt(j, k + 1);   // FETCH ALL THE SHAPE ABOVE THAT PARTICULAR ROW (HEIGHT) K+1
                // AND REPLACE IT WITH THE CURRENT SHAPE
            }
            for (int j = 0; j < BoardWidth; ++j)  // THE BOTTOM OF THE BOARD IS CLEARED
                shapeAt(j, BoardHeight - 1) = Empty_Block_e;  // THAT IS REMOVING ALL SHAPES FROM THE BOTTOM
        }
    }


    // SETTING THE SCORE IF LINES HAVE BEEN REMOVED
    iRowsRemoved = iNumFullLines;   // INCREASE THE NUMBER OF LINES REMOVED

}

unsigned int BlockTrixBoard::GetRemovedRows() const
{
    return iRowsRemoved;   // return the number of rows removed
}

void BlockTrixBoard::SetPoints(unsigned int pt)
{
    iScore += 100 * pow(pt,2);  // we square the number of rows removed and multiply by 10 and add to the score
    emit linesRemovedChanged(iTotalRowsRemoved);  // UI, UPDATING TEXT
    emit scoreChanged(iScore); // update score changed in UI

    bIsWaitingAfterLine = true;    // used to tell the game that lines were just removed.
    curPiece.setShape(Empty_Block_e);   // RESET THE CURRENT SHAPE TO NO SHAPE TO ENSURE THAT IT IS NOT DRAWN AGAIN
    update();    // UPDATE THE SCREEN


}

unsigned int BlockTrixBoard::GetPoints() const
{
    return iScore; // return the total score earned
}

BlockType_E BlockTrixBoard::GetNextBlockType() const
{
    return BlockType_E(qrand() % 7);  // return a random block enum
}


void BlockTrixBoard::Move(Movement_E Movement)
{
    switch (Movement) {
    case MoveLeft_e:
        tryMove(curPiece, iCurX - 1, iCurY);  // move left
        break;
    case MoveRight_e:
        tryMove(curPiece, iCurX + 1, iCurY);  // move right
        break;
    case RotateClockwise_e:
        tryMove(curPiece.rotatedRight(), iCurX, iCurY);  // rotate right
        break;
    case RotateCounterClockwise_e:
        tryMove(curPiece.rotatedLeft(), iCurX, iCurY);  // rotate left
        break;
    case Release_e:
        releaseBlock();  // release block
        break;
    case static_cast<Movement_E>(Qt::Key_D):
        oneLineDown();
        break;
    default:
        break;
    }
}

// sets all values on the board to NOSHAPE
void BlockTrixBoard::InitBoard()
{
    for (int i = 0; i < BoardHeight * BoardWidth; ++i)
        board[i] = Empty_Block_e;
}

// should return the column that the new block should be placed
int BlockTrixBoard::GetColumnHeights()
{
    iShortestColumn =0;
    iHighestColumn = 0;
    resetColumnHeight();
    // unsigned int y=BoardHeight-2;    // the first row.

    // STARTS THE COUNT FROM THE BOTTOM.
    // ITS MOVING BOTTOM UP
    for (int h =0 ; h <BoardHeight-1; h++)
    {
        // we go through the width to see if there is a shape stored there
        for (int w = 0; w < BoardWidth; ++w) {
            if (shapeAt(w, h) == Empty_Block_e) {
                // if there is no shape, do nothing
            }
            else {
                // if there is a shape, we take the height of the column as h
                ColumnHeights[w]=h+1;
            }
        }
    }

    // Get shortest column
    // used to keep the stacks evenly distributed
    for (int w = 0; w < BoardWidth; ++w) {
        if(w>0)
        {
            // compare the column heights for the shortest column
            if(ColumnHeights[w] < ColumnHeights[iShortestColumn])
            {
                iShortestColumn = w;
            }
        }
    }

    // We need the highest column too for rescue missions the rescue
    for (int w = 0; w < BoardWidth; ++w) {
        if(w>0)
        {
            if(ColumnHeights[w] > ColumnHeights[iHighestColumn])
            {   iHighestColumn = w;
            }
        }
    }

    HeightDifferences(ColumnHeights);  // this function calculates the differences in height of the columns
    return false;
}



/**
 * @brief BlockTrixBoard::HeightDifferences(int *array)
 * @param array
 *
 * This is where the AI algorithm begins
 * It calculates the differences in height of adjacent columns and determine which block can fit there
 * The columns are then stored in placement vectors
 * vecRightStepDown - stores columns that can take T, and S
   vecRightStepUp - stores columns that can take T, and Z
   vecRightWellUp - stores columns that can take inverted J
   vecRightWellDown - stores columns that can take inverted L and I
   vecLineLeft - stores columns that can take inverted J and I
   vecLineRight - stores columns that can take I
   vecFlatThree - stores columns that can take flat blocks with 3 base
   vecLeftStepTwo - stores columns that can take blocks with 2 base
   vecRightStepTwo - stores columns that can take blocks with 2 base
 */


void BlockTrixBoard::HeightDifferences(int *array)
{
    clearVectors();   // reset the vectors that are used for block placement
    int *colHeights = array;  //

    for(int iCol=0; iCol<10; iCol++)
    {
        // we start from second column because first column doesn't have second left neighbour
        if(iCol>0)
        {
            //if the adjacent columns are equal to the current column, it can take a block
            // with 3 bases
            if(iCol<=8)
            {
                // check for three flat base
                if(colHeights[iCol] == colHeights[iCol-1] && colHeights[iCol] == colHeights[iCol+1])
                {
                    // vecFlatThree is a vector that holds positions for blocks with 3 bases
                    vecFlatThree.push_back(iCol-1); // shift to the left column to give a 3 base
                }
            }

            // if the column number is more than 2, we can subtract 2 from it
            if(iCol>=2)
            {
                if((colHeights[iCol] - colHeights[iCol-1]) == 1 && (colHeights[iCol] - colHeights[iCol-2]) == 1)
                {
                    // vecLeftStepTwo holds positions for two bases blocks
                    vecLeftStepTwo.push_back(iCol-1);
                }
            }

            // this section deals with the columns to the left of the current column
            int heightDiffToLeft =  colHeights[iCol] - colHeights[iCol-1]; // takes the height difference between the column on the left
            // we consider up to difference in height of 3 or -3
            if(heightDiffToLeft > -3 && heightDiffToLeft <3)
            {
                switch(heightDiffToLeft)
                {
                case 0:
                    vecFlatBaseLeft.push_back(iCol); // same column height as the left, they form a surface for O and L an J blocks
                    break;
                case 1:
                    vecLeftStepDown.push_back(iCol); // 1 difference in height to the left, it forms a surface for T and Z
                    break;
                case -1:
                    vecLeftStepUp.push_back(iCol);  //  for S and T
                    break;
                case -2:
                    vecLeftWellUp.push_back(iCol);      // for L and I
                    vecLineLeft.push_back(iCol);
                    break;
                case 2:
                    vecLeftWellDown.push_back(iCol); // surface for J and I
                    vecLineLeft.push_back(iCol);
                    break;
                default:
                    break;
                }
            }
            else if(heightDiffToLeft >= 3) {
                vecLineLeft.push_back(iCol-1); }
            else { vecLineLeft.push_back(iCol); }
        }

        if(iCol<9)
        {
            if(iCol<=7)
            {
                if((colHeights[iCol] - colHeights[iCol+1]) == 1 &&(colHeights[iCol] - colHeights[iCol+2]) == 1)
                {
                    vecRightStepTwo.push_back(iCol);
                }
            }

            // To the right  = ColumnHeightDiffs[cl][R1]
            int heightDiffToRight  = colHeights[iCol] - colHeights[iCol+1];

            if(heightDiffToRight>-3 && heightDiffToRight < 3)
            {
                switch(heightDiffToRight)
                {
                case 0:
                    vecFlatBaseRight.push_back(iCol); // surface for O, L and J
                    break;
                case -1:
                    vecRightStepUp.push_back(iCol); // surface for T and Z
                    break;
                case 1:
                    vecRightStepDown.push_back(iCol);  // surface for S and T
                    break;
                case -2:
                    vecRightWellUp.push_back(iCol);  // surface for J and I
                    vecLineRight.push_back(iCol);
                    break;
                case 2:
                    vecRightWellDown.push_back(iCol);  // surface for L and I
                    vecLineRight.push_back(iCol+1);
                    break;
                default:

                    break;
                }
            }
            // if the height difference is more than 3, it is stored for I
            else if(heightDiffToRight >= 3) {
                vecLineRight.push_back(iCol+1); }

            else {vecLineRight.push_back(iCol);}
        }
    }

}

void BlockTrixBoard::resetColumnHeight()
{
    for(int ch=0; ch<10; ch++)
    {
        ColumnHeights[ch]=0;
    }
}

int BlockTrixBoard::SortColumns(BlockType_E blk)
{
    int moveTo= iShortestColumn;  // thakke the shortes column

    switch(blk)
    {
    case I_Block_e:
        // I block can fit into any column in vecLine;
        // we pick the shortest column and move I there
        moveTo = iShortestColumn;
        break;
    case O_Block_e:

        // O block can fit into any double space.
        // towards the left or right
        moveTo= MoveToDouble(vecFlatBaseLeft,vecFlatBaseRight);
        //    qDebug()<<"MoveTo :"<< moveTo;
        break;
    case L_Block_e:
        // four rotations are possible, we consider only 3 flat, sitting and standing

        // check if there are any columns that can take the inverted L
        if(!vecLeftWellUp.empty() || !vecRightWellDown.empty())
        {
            // rotate L to the Left, to get the inverted shape
            tryMove(curPiece.rotatedLeft(), iCurX, iCurY);

            // we find the column that it can fit
            if(!vecLeftWellUp.empty() && !vecRightWellDown.empty())
            {
                int ret = iShortestColumn;
                if(ColumnHeights[vecLeftWellUp[0]] <= ColumnHeights[vecRightWellDown[0]])
                {
                    ret = vecLeftWellUp[0];
                } else {ret = vecRightWellDown[0]; }

                moveTo = ret;
            }
            else if(!vecLeftWellUp.empty())
            {
                moveTo = vecLeftWellUp[0]; // take the default value
            }
            else if (!vecRightWellDown.empty())
            {
                moveTo = vecRightWellDown[0]; // take the default value
            }


        }
        else if(!vecFlatBaseLeft.empty() || !vecFlatBaseRight.empty())
        {
            tryMove(curPiece.rotatedRight(), iCurX, iCurY);
            if(!vecFlatBaseLeft.empty() && !vecFlatBaseRight.empty())
            {
                moveTo = MoveToDouble(vecFlatBaseLeft,vecFlatBaseRight);
            }
            else if(!vecFlatBaseLeft.empty())
            {
                moveTo = vecFlatBaseLeft[0];
            }
            else if(!vecFlatBaseRight.empty())
            { moveTo = vecFlatBaseRight[0]; }
        }

        else if(!vecFlatThree.empty())
        {
            // if there is a 3 flat surface, place the block there
            moveTo =vecFlatThree[0];
        }
        // if no shape is available, place it randomly
        else {tryMove(curPiece.rotatedLeft(), iCurX, iCurY);}

        break;

    case J_Block_e:
        // four rotations are possible but we consider only 3: flat, sitting and standing
        if(!vecLeftWellDown.empty() || !vecRightWellUp.empty())
        {
            // rotate to right
            tryMove(curPiece.rotatedRight(), iCurX, iCurY);
            if(!vecLeftWellDown.empty() && !vecRightWellUp.empty())
            {
                //  int arrayMove[2] = {0,0};
                int ret = iShortestColumn;
                if(ColumnHeights[vecLeftWellDown[0]] <= ColumnHeights[vecRightWellUp[0]])
                {
                    ret = vecLeftWellDown[0];
                } else {ret = vecRightWellUp[0]; }

                moveTo = ret;
            }
            else if(!vecLeftWellDown.empty())
            {
                moveTo = vecLeftWellDown[0];
            }
            else if(!vecRightWellUp.empty())
            {
                moveTo = vecRightWellUp[0];
            }
        }
        else if(!vecFlatBaseLeft.empty() || !vecFlatBaseRight.empty())
        {
            // rotate left
            tryMove(curPiece.rotatedLeft(), iCurX, iCurY);

            if(!vecFlatBaseLeft.empty() && !vecFlatBaseRight.empty())
            {
                // there are two possibilites to the left and right of the column
                // we choose the shortest column from both
                int iGto =MoveToDouble(vecFlatBaseLeft,vecFlatBaseRight);
                moveTo = iGto +1;
            }
            else if (!vecFlatBaseLeft.empty())
            {
                moveTo = vecFlatBaseLeft[0]-1;
            }
            else if(!vecFlatBaseRight.empty())
            {
                moveTo = vecFlatBaseRight[0];
            }

        }

        else if(!vecFlatThree.empty())
        {
            moveTo =vecFlatThree[0];
        }

        else {
            // there is no fitting column so we rotate it randomly and place it on the board
            int randCol = qrand() % 1;
            if(randCol==0)
            {
                tryMove(curPiece.rotatedRight(), iCurX, iCurY);}

            else {tryMove(curPiece.rotatedLeft(), iCurX, iCurY);}
        }
        break;

    case T_Block_e:

        // T rotated right and z rotated left
        if(!vecLeftStepDown.empty() || !vecRightStepUp.empty())
        {
            // rotate right
            tryMove(curPiece.rotatedRight(), iCurX, iCurY);

            if(!vecLeftStepDown.empty() && !vecRightStepUp.empty())
            {

                int ret = iShortestColumn;
                if(ColumnHeights[vecLeftStepDown[0]] <= ColumnHeights[vecRightStepUp[0]])
                {
                    ret = vecLeftStepDown[0]-1;
                } else {ret = vecRightStepUp[0]; }

                moveTo = ret;
            }

            else  if(!vecLeftStepDown.empty())
            {
                moveTo = vecLeftStepDown[0] -1;
            }
            else  if(!vecRightStepUp.empty())
            {
                moveTo = vecRightStepUp[0] ;
            }

        }
        else if(!vecLeftStepUp.empty() || !vecRightStepDown.empty())
        {
            tryMove(curPiece.rotatedLeft(), iCurX, iCurY);
            if(!vecLeftStepUp.empty() && !vecRightStepDown.empty())
            {
                int ret = iShortestColumn;
                if(ColumnHeights[vecLeftStepUp[0]] <= ColumnHeights[vecRightStepDown[0]])
                {
                    ret = vecLeftStepUp[0];
                } else {ret = vecRightStepDown[0]; }

                moveTo = ret;
            }
            else if(!vecLeftStepUp.empty())
            {
                moveTo = vecLeftStepUp[0];
            }
            else if (!vecRightStepDown.empty())
            {
                moveTo=vecRightStepDown[0];
            }
        }
        else if(!vecFlatThree.empty())
        {
            moveTo =vecFlatThree[0] +1 ;
        }

        break;

    case S_Block_e:

        if(!vecLeftStepTwo.empty())
        {
            tryMove(curPiece.rotatedRight(), iCurX, iCurY);
            moveTo = vecLeftStepTwo[0];
        }

        else   if(!vecLeftStepUp.empty() || !vecRightStepDown.empty())
        {
            if(!vecLeftStepUp.empty() && !vecRightStepDown.empty())
            {

                int ret = iShortestColumn;
                if(ColumnHeights[vecLeftStepUp[0]] <= ColumnHeights[vecRightStepDown[0]])
                {
                    ret = vecLeftStepUp[0]-1;
                } else {ret = vecRightStepDown[0]; }

                moveTo = ret;
            }
            else if(!vecLeftStepUp.empty())
            {

                moveTo =vecLeftStepUp[0]>0 ? vecLeftStepUp[0] -1 : vecLeftStepUp[0];
            }
            else if(!vecRightStepDown.empty())
            {moveTo = vecRightStepDown[0] ;}
        }

        break;
    case Z_Block_e:

        if(!vecRightStepTwo.empty())
        {
            tryMove(curPiece.rotatedRight(), iCurX, iCurY);
            moveTo = vecRightStepTwo[0]+1;
        }
        else if(!vecLeftStepDown.empty() || !vecRightStepUp.empty())
        {
            if(!vecLeftStepDown.empty() && !vecRightStepUp.empty())
            {

                int ret = qrand() % 8;

                if(ColumnHeights[vecLeftStepDown[0]] <= ColumnHeights[vecRightStepUp[0]])
                {
                    ret = vecLeftStepDown[0];
                } else {ret = vecRightStepUp[0]+1; }

                moveTo = ret;
            }
            else if(!vecLeftStepDown.empty())
            {
                moveTo =vecLeftStepDown[0] ;
            }
            else if(!vecRightStepUp.empty())
            {
                moveTo = vecRightStepUp[0]+1;
            }
        }

        break;
    }
    return moveTo;
}

void BlockTrixBoard::clearVectors()
{
    //reset all the placement vectors for recalculation
    vecFlatBaseLeft.clear();
    vecFlatBaseRight.clear();
    vecFlatThree.clear();

    vecLeftStepDown.clear();
    vecLeftStepUp.clear();
    vecLeftWellUp.clear();
    vecLeftWellDown.clear();

    vecRightStepDown.clear();
    vecRightStepUp.clear();
    vecRightWellUp.clear();
    vecRightWellDown.clear();
    vecLineLeft.clear();
    vecLineRight.clear();
    vecLeftStepTwo.clear();
    vecRightStepTwo.clear();
}

void BlockTrixBoard::BestFitColumn()
{
    // find the shortest column for each of the placement vectors
    ShortestColumn(vecFlatBaseLeft);
    ShortestColumn(vecFlatBaseRight);
    ShortestColumn(vecFlatThree);

    ShortestColumn(vecLeftStepDown);
    ShortestColumn(vecLeftStepUp);
    ShortestColumn(vecLeftWellUp);
    ShortestColumn(vecLeftWellDown);

    ShortestColumn(vecRightStepDown);
    ShortestColumn(vecRightStepUp);
    ShortestColumn(vecRightWellUp);
    ShortestColumn(vecRightWellDown);
    ShortestColumn(vecLineLeft);
    ShortestColumn(vecLineRight);
    ShortestColumn(vecLeftStepTwo);
    ShortestColumn(vecRightStepTwo);
}

// we compare and choose the best fit column from the left and right sides of the column
int BlockTrixBoard::MoveToDouble(vector<int> &vecLeftDouble, vector<int> &vecRightDouble)
{
    // hold the 2 columns from left and right adjacent columns
    // we will pick a random one afterwards

    //  int arrayMove[2] = {0,0};
    int ret = iShortestColumn;

    if(!vecLeftDouble.empty() || !vecRightDouble.empty())
    {
        if (!vecLeftDouble.empty() && !vecRightDouble.empty())
        {
            ret =ColumnHeights[vecLeftDouble[0]] <= ColumnHeights[vecRightDouble[0]] ? vecLeftDouble[0]-1:vecRightDouble[0];
        }
        else if(!vecLeftDouble.empty())
        {  ret = vecLeftDouble[0]-1;}
        else {ret = vecRightDouble[0];}
    }

    return ret;
}


int BlockTrixBoard::GetMoveTo(vector<int> &vecLeft, vector<int> &vecRight)
{
    // hold the 2 columns from left and right adjacent columns
    // we will pick a random one afterwards

    int ret = iShortestColumn;  // default is the shortest column

    if(!vecLeft.empty() || !vecRight.empty())
    {
        if (!vecLeft.empty() && !vecRight.empty())
        {
            // we pick the shortest column from the two options
            ret =ColumnHeights[vecLeft[0]] <= ColumnHeights[vecRight[0]] ? vecLeft[0]:vecRight[0];
        }
        else if(!vecLeft.empty())
        {  ret = vecLeft[0];}
        else {ret = vecRight[0];}

    }

    return ret;
}

void BlockTrixBoard::ShortestColumn(vector<int> &colVector)
{
    int iIndex;

    if(!colVector.empty())
    {
        iIndex= colVector[0];  // set the iIndex to the first value in colVector

        // loop through colVector and find the shortest column
        // we compare the column heights and store only the shortest one in the vector
        for( int it=0; it< static_cast<int>(colVector.size()); it++)
        {

            int iCurColumn=static_cast<int>(colVector[it]);

            if(it>0)
            {

                if (ColumnHeights[iCurColumn] <= ColumnHeights[iIndex])
                {
                    if(ColumnHeights[iCurColumn] == ColumnHeights[iIndex])
                    {
                        int iRandTwo[2]={iCurColumn,iIndex}; // if both columns are the same, take one randomly
                        int iRandOne = qrand() % 1;
                        iIndex = iRandTwo[iRandOne];
                    }
                    else{
                        iIndex = iCurColumn;  }

                }
            }
            else {
                iIndex = iCurColumn;
            }
        }
        // we need only one column so clear the vector and store only the one chosen from above
        colVector.clear();
        colVector.push_back(iIndex);

    }

}
