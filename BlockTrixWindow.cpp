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
#include "BlockTrixWindow.h"
//#include <QTextEdit>

BlockTrixWindow::BlockTrixWindow()
{
    board = new BlockTrixBoard(); // new Board

    // on the formating of the display
    nextPieceLabel = new QLabel;
    nextPieceLabel->setFrameStyle(QFrame::Box | QFrame::NoFrame); // frame style for game area area
    nextPieceLabel->setAlignment(Qt::AlignCenter);
    board->setNextPieceLabel(nextPieceLabel);

    scoreLcd = new QLCDNumber(7);       // lcd display
    scoreLcd->setSegmentStyle(QLCDNumber::Flat);
    linesLcd = new QLCDNumber(5);
    linesLcd->setSegmentStyle(QLCDNumber::Flat);

    startButton = new QPushButton(tr("&Start"));  // set the text for start button
    startButton->setFocusPolicy(Qt::NoFocus);    // we do not want the buttons to be focused because the board takes key presses
    quitButton = new QPushButton(tr("&Quit"));  // text for quit button
    quitButton->setFocusPolicy(Qt::NoFocus);
    pauseButton = new QPushButton(tr("&Pause")); // text for pause
    pauseButton->setFocusPolicy(Qt::NoFocus);
    autoPlayButton = new QPushButton(tr("&Auto Player"));   // text for auto player button
    autoPlayButton->setFocusPolicy(Qt::NoFocus);


    signalMapper = new QSignalMapper(this);
    signalMapper->setMapping(startButton, 0);

    connect(startButton,SIGNAL(clicked()),signalMapper, SLOT(map())); // connect signal mapper to button slot

    connect(signalMapper, SIGNAL(mapped(int)),board, SLOT(start(int)));

    connect(quitButton , SIGNAL(clicked()), qApp, SLOT(quit()));        // connect quit button to slot quit
    connect(pauseButton, SIGNAL(clicked()), board, SLOT(pause()));      //  connect pause button to slot pause
    connect(autoPlayButton,SIGNAL(clicked()),board,SLOT(autoPlayer())); // connect autoplaybutton to slot autoplayer
    connect(board, SIGNAL(scoreChanged(int)), scoreLcd, SLOT(display(int)));    //
    connect(board, SIGNAL(linesRemovedChanged(int)), linesLcd, SLOT(display(int)));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(createLabel(tr("Press ↑ Rotate Left")), 6, 0,1,3,Qt::AlignLeft);
    layout->addWidget(createLabel(tr("Press ↓ Rotate Right")), 7, 0,1,3,Qt::AlignLeft);
    layout->addWidget(createLabel(tr("Press ← Move Left")), 8, 0,1,3,Qt::AlignLeft);
    layout->addWidget(createLabel(tr("Press → Move Right")), 9, 0,1,3,Qt::AlignLeft);
    layout->addWidget(createLabel(tr("Press space to release block")), 10, 0,1,3,Qt::AlignLeft);
    layout->addWidget(startButton, 14, 0,1,3);
    layout->addWidget(autoPlayButton, 15, 0,1,3);

    // add a vertical layout to hold the left border
    QVBoxLayout *leftBorderLayout	= new QVBoxLayout();
    leftBorderLayout->setSpacing(0);        // remove spacing in layout
    leftBorderLayout->setMargin(0);
    leftBorderLayout->setContentsMargins(0,0,0,0);


    for(int i=0;i<17;++i)
    {
        if(i<4)
        {
            // generate new pound label for the border
            QLabel  *poundLabel;
            poundLabel= new QLabel("#");
            poundLabel->setFrameStyle(QFrame::Box | QFrame::NoFrame);
            leftBorderLayout->addWidget(poundLabel);
        }
        else

        {
            // generate new x label for border
            QLabel  *xLabel;
            xLabel= new QLabel("X");
            xLabel->setFrameStyle(QFrame::Box | QFrame::NoFrame);
            leftBorderLayout->addWidget(xLabel);  // add to layout
        }
    }

    // add left border layout to gridlayout
    layout->addLayout(leftBorderLayout, 0,4,17,1);


    layout->addWidget(board, 0, 5, 16, 10);   // add board to window

    // add a horizontal layout to hold the down border
    QHBoxLayout *downBorderLayout	= new QHBoxLayout();

    downBorderLayout->setSpacing(0);  // remove spacing in layout
    downBorderLayout->setMargin(0);
    downBorderLayout->setContentsMargins(0,0,0,0);
    QVBoxLayout *rightBorderLayout	= new QVBoxLayout();
    rightBorderLayout->setSpacing(0);
    rightBorderLayout->setMargin(0);
    rightBorderLayout->setContentsMargins(0,0,0,0);


    for(int i=5;i<15;++i)
    {
        QLabel  *xLabel;
        xLabel= new QLabel("X");
        xLabel->setFrameStyle(QFrame::Box | QFrame::NoFrame);
        downBorderLayout->addWidget(xLabel);
    }

    //  downBorderLayout->addWidget(xLabel,16,i,1,1,Qt::AlignRight);
    layout->addLayout(downBorderLayout,16,5,1,10);

    for(int i=0;i<17;++i)
    {
        if(i<4)
        {
            QLabel  *poundLabel;
            poundLabel= new QLabel("#");
            poundLabel->setFrameStyle(QFrame::Box | QFrame::NoFrame);
            rightBorderLayout->addWidget(poundLabel);
        }

        else
        {
            QLabel  *xLabel;
            xLabel= new QLabel("X");
            xLabel->setFrameStyle(QFrame::Box | QFrame::NoFrame);
            rightBorderLayout->addWidget(xLabel);
        }
    }

    layout->addLayout(rightBorderLayout,0,16,17,1);


    layout->addWidget(nextPieceLabel, 1, 17,6,8);
    layout->addWidget(createLabel(tr("Points:")), 8, 17,1,8,Qt::AlignRight);
    layout->addWidget(scoreLcd, 9, 17,1,8,Qt::AlignRight);
    layout->addWidget(createLabel(tr("Removed Rows:")), 10,17,1,8,Qt::AlignRight);
    layout->addWidget(linesLcd, 11, 17,1,8,Qt::AlignRight);
    layout->addWidget(quitButton, 14, 17,1,8);
    layout->addWidget(pauseButton, 15, 17,1,8);

    setLayout(layout);

    setWindowTitle(tr("BlockTrix Game"));
    resize(550, 370);

}

QLabel *BlockTrixWindow::createLabel(const QString &text)
{
    QLabel *lbl = new QLabel(text);
    return lbl;
}
