#include "minimax.h"
#include <climits>
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <vector>


MiniMax::MiniMax(unsigned depth)
{
    this->depth = depth;
}

std::optional<int> MiniMax::findGame(MMKey &key)
{
    this->tableLock.lock();
    auto search = this->transTable.find(key);
    if (search != this->transTable.end()) {
        this->tableLock.unlock();
        return std::optional<int>{search->second};
    }
    else {
        this->tableLock.unlock();
        return std::nullopt;
    }
}

void MiniMax::insertScore(MMKey &key, int score)
{
    this->tableLock.lock();
    this->transTable[key] = score;
    this->tableLock.unlock();
}

static int minimaxEval(MiniMax *mm, game::Wrapper *wrap, unsigned depth, int alpha, int beta, game::Player player)
{
    MMKey key = std::make_tuple(wrap->gameString(), depth);
    std::optional<int> val = mm->findGame(key);
    if (val.has_value())
        return val.value();
    
    int ret;
    if (depth == 0) {
        ret = wrap->evalWrap();
        mm->insertScore(key, ret);
        return ret;
    }

    unsigned len, i;
    std::vector<game::Wrapper *> children = wrap->nextWrap();
    len = children.size();

    if (len == 0) {
        ret = wrap->evalWrap();
        mm->insertScore(key, ret);
        return ret;
    }

    game::Player otherPlayer;
    if (player == game::MAX_PLAYER)
        otherPlayer = game::MIN_PLAYER;
    else
        otherPlayer = game::MAX_PLAYER;
        
    int best = player == game::MAX_PLAYER ? INT_MIN : INT_MAX;
    int score;
    game::Wrapper *child;
    for (i = 0; i < len; i++) {
        score = minimaxEval(mm, children[i], depth - 1, alpha, beta, otherPlayer);
        if (player == game::MAX_PLAYER) {
            if (score > best)
                best = score;
            if (score > alpha)
                alpha = score;
            if (alpha >= beta) {
                break;
            }
        }
        else {
            if (score < best)
                best = score;
            if (score < beta)
                beta = score;
            if (beta <= alpha) {
                break;
            }
        }
        delete children[i];
    }
    while (i < len) {
        delete children[i];
        i++;
    }

    mm->insertScore(key, best);

    return best;
}

void threadEval(MiniMax *mm, game::Wrapper *wrap, unsigned depth, game::Player player, std::promise<int> && p)
{
    p.set_value(minimaxEval(mm, wrap, depth, INT_MIN, INT_MAX, player));
}

game::Move *MiniMax::nextMove(game::Wrapper *wrap)
{
    std::unordered_map<std::string, game::Move *> childMap = wrap->gameValidMoves();
    std::vector<game::Move *> best;
    game::Move *ret;

    unsigned len = childMap.size();
    if (len == 0)
        return NULL;

    game::Player player = wrap->nextPlayer();
    game::Player otherPlayer;
    if (player == game::MAX_PLAYER)
        otherPlayer = game::MIN_PLAYER;
    else
        otherPlayer = game::MAX_PLAYER; 

    std::promise<int> ps[len];
    std::future<int> fs[len];
    game::Move *mvs[len];
    game::Wrapper *children[len];
    std::vector<std::thread> ts;

    game::Wrapper *child;
    game::Move *mv;
    unsigned idx = 0;

    for (auto i = childMap.begin(); i != childMap.end(); i++) {
        mv = i->second;
        child = new game::Wrapper(wrap);
        child->gameApplyMove(*mv, false);
        mvs[idx] = mv;
        children[idx] = child;

        fs[idx] = ps[idx].get_future();
        ts.emplace_back(std::thread(&threadEval, this, child, depth, otherPlayer, std::move(ps[idx])));
        idx++;
    }

    int best_score = player == game::MAX_PLAYER ? INT_MIN : INT_MAX;
    int score;
    for (idx = 0; idx < len; idx++)
    {
        mv = mvs[idx];
        child = children[idx];
        ts[idx].join();
        score = fs[idx].get();

        if (score == best_score) {
            best.push_back(mv);
            delete child;
            continue;
        }
        if (player == game::MAX_PLAYER) {
            if (score > best_score) {
                best.clear();
                best_score = score;
                best.push_back(mv);
            }
        }
        else {
            if (score < best_score) {
                best.clear();
                best_score = score;
                best.push_back(mv);
            }
        }
        delete child;
    }

    unsigned best_len = best.size();
    unsigned best_idx = rand() % best_len;
    ret = best[best_idx];

    this->transTable.clear();

    return ret;
}