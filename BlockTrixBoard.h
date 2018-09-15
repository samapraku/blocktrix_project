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


#ifndef BLOCKTRIXBOARD_H
#define BLOCKTRIXBOARD_H


#include <QWidget>
#include <QFrame>
#include <QBasicTimer>
#include <QPointer>
#include <QObject>


#include "Interfaces.h"
#include "BlockTrixPiece.h"
#include <vector>
#include <algorithm>

using std::vector;

class QLabel;
class QPushButton;

class BlockTrixBoard : public QFrame, public IBoard
{
    Q_OBJECT

public:
    BlockTrixBoard(QWidget *parent = 0);

    void setNextPieceLabel(QLabel *label);  // method to show the next block type
    QSize sizeHint() const Q_DECL_OVERRIDE;   // overide them to specify the size
    QSize minimumSizeHint() const Q_DECL_OVERRIDE;


    // INTERFACE METHODS

    // Initializes the board with a random current and next block .
    virtual void InitBoard() ;

    // Returns the field at the given position .
    // A value of 0 means empty , all other values represent
    // occupied by an element of a block .
    virtual unsigned int GetField( unsigned int PosX , unsigned int PosY ) const ;

    // Moves the current block .
    virtual void Move ( Movement_E Movement ) ;

    // Returns true , if the game is finished and false otherwise .
    virtual bool IsFinished() const;

    // Returns the next block type .
    virtual BlockType_E GetNextBlockType() const ;

    // Returns the current amount of points .
    virtual unsigned int GetPoints() const ;

    // Returns the current amount of removed rows .
    virtual unsigned int GetRemovedRows() const ;



    void SetPoints(unsigned int pt); // method to update game points


    BlockType_E Empty_Block_e;  // used to indicate no shape block

    void SetAutoPlay(bool autoplay) {bAutoPlay = autoplay;} // sets the value for autoplay

    void RemoveRows();  //

public slots:
    void start(int player); // start button slot
    void pause();   // pause button slot
    void autoPlayer();  // auto player button slot

signals:
    void scoreChanged(int iScore);   // used to update score text on game window
    void linesRemovedChanged(int numLines); // used to update lines removed text on game window

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;   //method to paint in the window
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;   // keypress event method
    void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;       // timer event used for autoplayer

    bool bAutoPlay; // tells the game if autoplayer is active

private:
    enum { BoardWidth = 10, BoardHeight = 16 };  // enum to hold the board size

    BlockType_E &shapeAt(int x, int y) { return board[(y * BoardWidth) + x]; }   // to locate a shape on the board given x and y.

    int iTimerInterval() { return 10;  }  // the speed of the autoplayer game (ms)

    int iSquareWidth() { return contentsRect().width() / BoardWidth; }   // THE WIDTH OF 1 BLOCK
    int iSquareHeight() { return contentsRect().height() / BoardHeight; }  // THE HEIGHT OF 1 ONE BLOCK

    void releaseBlock();  // method declaration for release
    void oneLineDown();
    void pieceDropped();
    void newPiece();

    void showNextPiece();
    bool tryMove(const BlockTrixPiece &newPiece, int newX, int newY);
    void drawSquare(QPainter &painter, int x, int y, BlockType_E shape);


    QBasicTimer timer;      // variable for timer
    QPointer<QLabel> nextPieceLabel;  // lable for next
    bool bIsStarted;
    bool bIsPaused;
    bool bIsWaitingAfterLine;   // tells the game a block has been dropped
    BlockTrixPiece curPiece;
    BlockTrixPiece nextPiece;
    int iCurX;      // holds current x position of piece
    int iCurY;       // holds current y position of piece
    int iRowsRemoved;
    int iTotalRowsRemoved;
    int iScore;
    unsigned int iEmptyBoardHeight; // to keep track of the vertical empty spaces on the board

    // declaring board to be an instance of BlockTrixShape.
    // meaning that the board can take any or all the shapes.
    BlockType_E board[BoardWidth * BoardHeight];


     int ColumnHeights[10]; // holds the heights of the columns at any moment


    // Auto player methods and field declarations
    void resetColumnHeight(); // method declaration
    int GetColumnHeights();
    void HeightDifferences(int *array);
    int SortColumns(BlockType_E blk);

    // these vectors are used to hold the stack surfaces
    // of the blocks. They are used to determine how a block will be positioned
    vector<int> vecFlatBaseLeft;
    vector<int> vecFlatBaseRight;

    vector<int> vecLeftStepDown;
    vector<int> vecLeftStepUp;
    vector<int> vecLeftWellUp;
    vector<int> vecLeftWellDown;

    vector<int> vecRightStepDown;
    vector<int> vecRightStepUp;
    vector<int> vecRightWellUp;
    vector<int> vecRightWellDown;
    vector<int> vecLineLeft;
    vector<int> vecLineRight;
    vector<int> vecFlatThree;
    vector<int> vecLeftStepTwo;
    vector<int> vecRightStepTwo;

    void clearVectors();    // resets block placement vectors
    // calculates the column a new block should be moved to
    int GetMoveTo(vector<int> &vecLeft, vector<int> &vecRight);
    int iShortestColumn;    // shortest column
    int iHighestColumn;     // highest column
    void ShortestColumn(vector<int> &colVector);    // calculates the shortest column
    void BestFitColumn();   // calculates the best fit column for a block
    //chooses one column from the two arguments to move a block to
    int MoveToDouble(vector<int> &vecLeftDouble,vector<int> &vecRightDouble);
};

#endif
