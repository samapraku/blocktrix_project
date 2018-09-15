#ifndef PLAYER_H
#define PLAYER_H

#include "BlockTrixBoard.h"

class Player: public IPlayer
{
public:
    Player(){}
    virtual ~Player(){}

    // IPlayer interface
    void Play(IBoard *pBoard);
private:
    BlockTrixBoard *playerBoard;
};

#endif // PLAYER_H
