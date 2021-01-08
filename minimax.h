#ifndef MINIMAX_H
#define MINIMAX_H

#include "wrap.h"
#include <future>
#include <mutex>
#include <optional>
#include <string>
#include <tuple>
#include <unordered_map>


typedef std::tuple<std::string, unsigned> MMKey;

struct MMKeyHash : public std::unary_function<MMKey, std::size_t>
{
    std::size_t operator()(const MMKey &k) const
    {
        return std::hash<std::string>{}(std::get<0>(k)) ^ std::get<1>(k);
    }
};

struct MMKeyEqual : public std::binary_function<MMKey, MMKey, bool>
{
    bool operator()(const MMKey &v0, const MMKey &v1) const
    {
        return (std::get<0>(v0) == std::get<0>(v1) && std::get<1>(v0) == std::get<1>(v1));
    }
};

class MiniMax
{

public:
    unsigned depth;

    game::Move *nextMove(game::Wrapper *wrap);
    std::optional<int> findGame(MMKey &key);
    void insertScore(MMKey &key, int score);
    MiniMax(unsigned depth);

private:
    std::mutex tableLock;
    std::unordered_map<MMKey, int, MMKeyHash, MMKeyEqual> transTable;
};

#endif