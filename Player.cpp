/*
 * Institute of Automation - University of Bremen
 *BLOCKTRIX GAME AUTO PLAYER
 * GROUP MEMBERS:
 *  ERIC GAMOR
 * JIBIN JOHN
 * SAMUEL  APRAKU
 *
*/

#include "Player.h"

void Player::Play(IBoard *pBoard)
{
    playerBoard = dynamic_cast<BlockTrixBoard *>(pBoard);  // cast pBoard to BlockTrixBoard
    playerBoard->SetAutoPlay(true);  // tell game to start autoplay
}
