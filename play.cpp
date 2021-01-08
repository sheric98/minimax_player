#include "play.h"
#include <iostream>
#include <string>


play::Play::Play(GameType gameType, PlayerType oneType, PlayerType twoType, unsigned depth1, unsigned depth2)
{
    this->gameType = gameType;
    this->playerOneType = oneType;
    this->playerTwoType = twoType;
    if (oneType == CPU)
        this->cpuOne = new MiniMax(depth1);
    else
        this->cpuOne = NULL;
    if (twoType == CPU)
        this->cpuTwo = new MiniMax(depth2);
    else
        this->cpuTwo = NULL;
    this->game = NULL;
}

play::Play::~Play()
{
    if (playerOneType == CPU)
        delete cpuOne;
    if (playerTwoType == CPU)
        delete cpuTwo;
    if (game)
        delete game;
}

std::string play::Play::humanMove(bool display)
{
    std::string inp;
    bool done = false;

    if (display)
        std::cout << "Make a move." << std::endl;

    while (!done) {
        std::getline(std::cin, inp);
        done = game->gameTryStrMove(inp);
        if (!done && display)
            std::cout << inp << " is an invalid move. Make a valid move." << std::endl;
    }
    return inp;
}

std::string play::Play::cpuMove(MiniMax *mm)
{
    std::string ret;
    game::Move *move = mm->nextMove(game);
    ret = move->text;
    game->gameApplyMove(*move, true);

    return ret;
}

game::State play::Play::playGame(bool display)
{
    game::State term = game::NOT_ENDED;
    game::GameFns *gameFns = GameMap[gameType];

    this->game = new game::Wrapper(gameFns);
    currPlayer = PLAYER_ONE;
    turns = 0;
    std::string playerOneStr, playerTwoStr;

    while (term == game::NOT_ENDED) {
        if (display)
            this->game->gameDisplay();

        if (currPlayer == PLAYER_ONE) {
            if (playerOneType == HUMAN)
                playerOneStr = humanMove(display);
            else
                playerOneStr = cpuMove(cpuOne);
            if (display)
                std::cout << "Player One Makes Move " << playerOneStr << std::endl;
            else if (playerOneType == CPU)
                std::cout << playerOneStr << std::endl << std::flush;
            currPlayer = PLAYER_TWO;
        }
        else {
            if (playerTwoType == HUMAN)
                playerTwoStr = humanMove(display);
            else
                playerTwoStr = cpuMove(cpuTwo);
            if (display)
                std::cout << "Player Two Makes Move " << playerTwoStr << std::endl;
            else if (playerTwoType == CPU)
                std::cout << playerTwoStr << std::endl << std::flush;
            currPlayer = PLAYER_ONE;
        }
        turns++;
        term = this->game->gameState();
    }
    if (display) {
        this->game->gameDisplay();

        switch(term)
        {
            case game::ONE_WON:
                printf("Player One Won!\n");
                break;
            case game::TWO_WON:
                printf("Player Two Won!\n");
                break;
            default:
                printf("Draw.\n");
                break;
        }
    }

    delete this->game;
    return term;
}
