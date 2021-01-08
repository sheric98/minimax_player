#ifndef WRAP_H
#define WRAP_H

#include <stddef.h>
#include <string>
#include <vector>
#include <cstring>
#include <optional>
#include <unordered_map>


namespace game {

    enum Player
    {
        MAX_PLAYER,
        MIN_PLAYER
    };

    enum State
    {
        NOT_ENDED,
        ONE_WON,
        TWO_WON,
        DRAW
    };

    struct Move
    {
        Move(void *input, std::string text);

        void *input;
        std::string text;
    };

    typedef int EvaluateFn(void *orig);
    typedef std::vector<void *> NextGamesFn(void *orig);
    typedef void *InitGameFn(void);
    typedef Player NextPlayerFn(void *orig);
    typedef State GameStateFn(void *orig);
    typedef std::string GameStringFn(void *orig);
    typedef void GameDisplayFn(void *orig);
    typedef void GameMovesFn(void *orig, std::unordered_map<std::string, game::Move *> &moveMap);
    typedef void ApplyMoveFn(void* orig, Move &move);
    typedef void *CopyGameFn(void* orig);
    typedef std::optional<std::string> ConvertStrFn(void* orig, std::string &str);

    struct GameFns
    {
        InitGameFn *initGameFn;
        EvaluateFn *evaluateFn;
        NextGamesFn *nextGamesFn;
        NextPlayerFn *nextPlayerFn;
        GameStateFn *gameStateFn;
        GameStringFn *gameStringFn;
        ConvertStrFn *convertStrFn;
        GameMovesFn *gameMovesFn;
        ApplyMoveFn *applyMoveFn;
        CopyGameFn *copyGameFn;
        GameDisplayFn *gameDisplayFn;
    };

    class Wrapper
    {
    public:
        int evalWrap(void);
        std::vector<Wrapper *> nextWrap(void);
        Player nextPlayer(void);
        State gameState(void);
        std::string gameString(void);
        std::unordered_map<std::string, Move *> gameValidMoves(void);
        void gameApplyMove(Move &move, bool clear);
        bool gameTryStrMove(std::string &move);
        void gameDisplay(void);
        void clearMoveMap(void);

        Wrapper(GameFns *gameFns);
        Wrapper(Wrapper *copy);
        Wrapper(Wrapper *prev, void *wrapped);
        ~Wrapper(void);

    private:
        void *wrapped;
        GameFns *gameFns;
        std::unordered_map<std::string, Move *> moveMap;
        bool movesCalculated;
    };

    /* Static Structs for GameFns */
    extern struct GameFns ChessFns;
}


/* INITIALIZE GAMEFNS */
void initGameFns(void);


/* CHESS WRAPPERS */

static void *chessInit(void);
int chessEval(void *chess);
void chessValidMoves(void *chess, std::unordered_map<std::string, game::Move *> &moveMap);
void chessApplyMove(void *chess, game::Move &move);
std::vector<void *> chessNextGames(void *chess);
game::Player chessPlayer(void *chess);
game::State chessState(void *chess);
std::string chessString(void *chess);
std::optional<std::string> chessConvStr(void *chess, std::string &str);
void *chessCopy(void *chess);
void chessDisplay(void *chess);

#endif