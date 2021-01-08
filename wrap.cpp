#include "wrap.h"
#include <climits>
#include <map>
#include "play.h"
#include "thc.h"

/* Global Declarations. */
game::GameFns game::ChessFns;
std::map<play::GameType, game::GameFns *> play::GameMap;

game::Wrapper::Wrapper(Wrapper *copy)
{
    this->wrapped = copy->gameFns->copyGameFn(copy->wrapped);
    this->gameFns = copy->gameFns;
    this->movesCalculated = false;
}

game::Wrapper::Wrapper(Wrapper *prev, void *wrapped)
{
    this->wrapped = wrapped;
    this->gameFns = prev->gameFns;
    this->movesCalculated = false;
}

game::Wrapper::Wrapper(GameFns *gameFns)
{
    this->wrapped = gameFns->initGameFn();
    this->gameFns = gameFns;
    this->movesCalculated = false;
}

game::Wrapper::~Wrapper(void)
{
    if (movesCalculated)
        clearMoveMap();
    free(wrapped);
}

int game::Wrapper::evalWrap()
{
    return gameFns->evaluateFn(wrapped);
}

std::vector<game::Wrapper *> game::Wrapper::nextWrap()
{
    std::vector<void *> games = gameFns->nextGamesFn(wrapped);
    std::vector<game::Wrapper *> ret;

    int count = games.size();
    int i;
    for (i = 0; i < count; i++) {
        game::Wrapper *wrap = new game::Wrapper(this, games[i]);
        ret.push_back(wrap);
    }

    return ret;
}

game::Player game::Wrapper::nextPlayer()
{
    return gameFns->nextPlayerFn(wrapped);
}

game::State game::Wrapper::gameState()
{
    return gameFns->gameStateFn(wrapped);
}

std::string game::Wrapper::gameString()
{
    return gameFns->gameStringFn(wrapped);
}

std::unordered_map<std::string, game::Move *> game::Wrapper::gameValidMoves()
{
    if (!movesCalculated) {
        movesCalculated = true;
        gameFns->gameMovesFn(wrapped, moveMap);
    }

    return moveMap;
}

void game::Wrapper::gameApplyMove(Move &move, bool clear)
{
    gameFns->applyMoveFn(wrapped, move);

    if (clear) {
        movesCalculated = false;
        clearMoveMap();
    }
}

bool game::Wrapper::gameTryStrMove(std::string &move)
{
    if (!movesCalculated) {
        movesCalculated = true;
        gameFns->gameMovesFn(wrapped, moveMap);
    }

    std::optional<std::string> optStr = gameFns->convertStrFn(wrapped, move);
    if (!optStr.has_value())
        return false;
    std::string str = optStr.value();

    auto search = moveMap.find(str);
    if (search == moveMap.end())
        return false;
    else {
        gameApplyMove(*search->second, true);
        return true;
    }
}

void game::Wrapper::gameDisplay()
{
    if (gameFns->gameDisplayFn)
        gameFns->gameDisplayFn(wrapped);
}

void game::Wrapper::clearMoveMap()
{
    for (auto i = moveMap.begin(); i != moveMap.end(); i++) {
        game::Move *mv = i->second;
        free(mv->input);
        delete mv;
    }
    moveMap.clear();
}

game::Move::Move(void *input, std::string text)
{
    this->input = input;
    this->text = text;
}


/* CHESS WRAPPERS */

void *chessInit(void)
{
    thc::ChessRules *cr = new thc::ChessRules();

    return (void *)cr;
}

int chessEval(void *chess)
{
    int material, position, ret;
    game::State state = chessState(chess);

    switch (state)
    {
        case game::ONE_WON:
            ret = INT_MAX;
            break;
        case game::TWO_WON:
            ret = INT_MIN;
            break;
        case game::DRAW:
            ret = 0;
            break;
        default:
            thc::ChessRules *cr = (thc::ChessRules *) chess;
            thc::ChessEvaluation *ce = new thc::ChessEvaluation(*cr);

            ce->EvaluateLeaf(material, position);
            free(ce);
            ret = material + position;
            break;
    }

    return ret;
}

void chessValidMoves(void *chess, std::unordered_map<std::string, game::Move *> &moveMap)
{
    std::vector<thc::Move> moves;
    thc::ChessRules *cr = (thc::ChessRules *) chess;

    cr->GenLegalMoveList(moves);
    int count = moves.size();
    int i;
    for (i = 0; i < count; i++) {
        thc::Move mv = moves[i];
        std::string mv_text = mv.NaturalOut(cr);
        thc::Move *copy = new thc::Move(mv);
        game::Move *move = new game::Move((void *) copy, mv_text);
        moveMap[mv_text] = move;
    }
}

void chessApplyMove(void *chess, game::Move &move)
{
    thc::ChessRules *cr = (thc::ChessRules *) chess;
    thc::Move *chessMove = (thc::Move *) move.input;

    cr->PlayMove(*chessMove);
}

std::vector<void *> chessNextGames(void *chess)
{
    std::vector<thc::Move> moves;
    std::vector<void *> ret;
    
    thc::ChessRules *cr = (thc::ChessRules *) chess;
    cr->GenLegalMoveList(moves);
    int count = moves.size();
    int i;
    for (i = 0; i < count; i++) {
        thc::ChessRules *cr_next = new thc::ChessRules(*cr);
        cr_next->PlayMove(moves[i]);
        ret.push_back((void *)cr_next);
    }

    return ret;
}

game::Player chessPlayer(void *chess)
{
    thc::ChessRules *cr = (thc::ChessRules *) chess;
    if (cr->WhiteToPlay())
        return game::MAX_PLAYER;
    else
        return game::MIN_PLAYER;
}

game::State chessState(void *chess)
{
    thc::ChessRules *cr = (thc::ChessRules *) chess;
    thc::TERMINAL term;
    cr->Evaluate(term);
    game::State ret;

    switch (term)
    {
        case thc::NOT_TERMINAL:
            ret = game::NOT_ENDED;
            break;
        case thc::TERMINAL_WCHECKMATE:
            ret = game::TWO_WON;
            break;
        case thc::TERMINAL_BCHECKMATE:
            ret = game::ONE_WON;
            break;
        default:
            ret = game::DRAW;
            break;
    }
    return ret;
}

std::string chessString(void *chess)
{
    thc::ChessRules *cr = (thc::ChessRules *) chess;
    std::string fen = cr->ForsythPublish();
    unsigned chars_remove = 4;
    while (chars_remove > 0) {
        fen.pop_back();
        chars_remove--;
    }        

    return fen;
}

std::optional<std::string> chessConvStr(void *chess, std::string &str)
{
    thc::Move mv;
    thc::ChessRules *cr = (thc::ChessRules *)chess;
    bool okay = mv.NaturalIn(cr, str.c_str());
    if (okay)
        return std::optional<std::string>{mv.NaturalOut(cr)};
    else
        return std::nullopt;
}

void *chessCopy(void *chess)
{
    thc::ChessRules *cr = (thc::ChessRules *) chess;
    return (void *) new thc::ChessRules(*cr);
}

void chessDisplay(void *chess)
{
    thc::ChessRules *cr = (thc::ChessRules *) chess;
    std::string fen = cr->ForsythPublish();
    std::string s = cr->ToDebugStr();
    printf( "FEN (Forsyth Edwards Notation) = %s\n", fen.c_str() );
    printf( "Position = %s\n", s.c_str() );
}

static void initChessFns()
{
    game::ChessFns.initGameFn = &chessInit;
    game::ChessFns.evaluateFn = &chessEval;
    game::ChessFns.gameMovesFn = &chessValidMoves;
    game::ChessFns.applyMoveFn = &chessApplyMove;
    game::ChessFns.nextGamesFn = &chessNextGames;
    game::ChessFns.nextPlayerFn = &chessPlayer;
    game::ChessFns.gameStateFn = &chessState;
    game::ChessFns.gameStringFn = &chessString;
    game::ChessFns.convertStrFn = &chessConvStr;
    game::ChessFns.copyGameFn = &chessCopy;
    game::ChessFns.gameDisplayFn = &chessDisplay;
}

/* Initialize GameFns */
static void initGameMap()
{
    play::GameMap[play::CHESS] = &game::ChessFns;
}

void initGameFns()
{
    initChessFns();

    initGameMap();
}
