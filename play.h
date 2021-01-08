#ifndef PLAY_H
#define PLAY_H

#include <map>
#include <string>
#include "minimax.h"
#include "wrap.h"

namespace play {
    enum PlayerType
    {
        HUMAN = 1,
        CPU = 0
    };

    enum CurrPlayer
    {
        PLAYER_ONE,
        PLAYER_TWO
    };

    enum GameType
    {
        CHESS
    };

    /* Dictionary of gameType to GameFns struct. */
    extern std::map<GameType, game::GameFns *> GameMap;

    class Play
    {
    public:
        unsigned turns;

        std::string humanMove(bool display);
        std::string cpuMove(MiniMax *mm);
        game::State playGame(bool display);

        Play(GameType gameType, PlayerType oneType, PlayerType twoType, unsigned depth1, unsigned depth2);
        ~Play(void);
    
    private:
        GameType gameType;
        CurrPlayer currPlayer;
        PlayerType playerOneType;
        PlayerType playerTwoType;
        MiniMax *cpuOne;
        MiniMax *cpuTwo;
        game::Wrapper *game;
    };
}


#endif