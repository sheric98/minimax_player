#include <chrono>
#include <iostream>
#include <stdio.h>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>
#include "thc.h"
#include "wrap.h"
#include "minimax.h"
#include "play.h"


int main(int argc, char **argv)
{
    if (argc < 4) {
        printf("Need at least 3 arguments.\nFirst argument is game type: "
               "0 for Chess.\nNext two arguments are player types: "
               "0 for computer, 1 for human.\n");
        exit(-1);
    }
    
    std::string gameString = std::string(argv[1]);
    std::string playerOneStr = std::string(argv[2]);
    std::string playerTwoStr = std::string(argv[3]);
    unsigned long playerOneLong, playerTwoLong, gameLong;

    try {
        gameLong = std::stoul(gameString, nullptr);
    }
    catch (...) {
        printf("First argument must be 0 (Chess).\n");
        exit(-1);
    }
    if (gameLong != 0) {
        printf("First argument must be 0 (Chess).\n");
        exit(-1);
    }

    try {
        playerOneLong = std::stoul(playerOneStr, nullptr);
    }
    catch (...) {
        printf("Second argument must be 0 (CPU) or 1 (Human)\n");
        exit(-1);
    }
    if (playerOneLong != 0 && playerOneLong != 1) {
        printf("Second argument must be 0 (CPU) or 1 (Human)\n");
        exit(-1);
    }

    try {
        playerTwoLong = std::stoul(playerTwoStr, nullptr);
    }
    catch (...) {
        printf("Third argument must be 0 (CPU) or 1 (Human)\n");
        exit(-1);
    }
    if (playerTwoLong != 0 && playerTwoLong != 1) {
        printf("Third argument must be 0 (CPU) or 1 (Human)\n");
        exit(-1);
    }

    play::PlayerType playerOne, playerTwo;
    playerOne = static_cast<play::PlayerType>(playerOneLong);
    playerTwo = static_cast<play::PlayerType>(playerTwoLong);

    play::GameType gameType = static_cast<play::GameType>(gameLong);
    
    argv[3] = argv[0];

    int c;
    unsigned long a;
    unsigned long b;
    bool aFlag = false;
    bool bFlag = false;
    bool aErr = false;
    bool bErr = false;
    bool quiet = false;
    std::string aString;
    std::string bString;

    while ((c = getopt(argc - 3, argv + 3, "a:b:q")) != -1) {
        switch (c)
        {
            case 'a':
                try {
                    aString = std::string(optarg);
                    a = std::stoul(aString, nullptr);
                    aFlag = true;
                }
                catch (...) {
                    aErr = true;
                }
                break;
            case 'b':
                try {
                    bString = std::string(optarg);
                    b = std::stoul(bString, nullptr);
                    bFlag = true;
                }
                catch (...) {
                    bErr = true;
                }
                break;
            case 'q':
                quiet = true;
                break;
            case '?':
                if (optopt == 'a') {
                    break;
                }
                if (optopt == 'b') {
                    break;
                }
                printf("Unknown option character `\\x%x'.\n", optopt);
            default:
                abort();
        }
    }

    if (playerOne == play::CPU) {
        if (aErr) {
            printf("-a is CPU 1 depth. Needs a nonnegative integer argument.\n");
            exit(-1);
        }
        if (!aFlag) {
            printf("Player one is a CPU. Must enter -a argument for CPU 1 depth.\n");
            exit(-1);
        }
    }
    if (playerTwo == play::CPU) {
        if (bErr) {
            printf("-b is CPU 2 depth. Needs a nonnegative integer argument.\n");
            exit(-1);
        }
        if (!bFlag) {
            printf("Player two is a CPU. Must enter -b argument for CPU 2 depth.\n");
            exit(-1);
        }
    }

    initGameFns();

    play::Play *arena = new play::Play(gameType, playerOne, playerTwo, a, b);

    // play::Play *arena = new play::Play(play::CHESS, play::CPU, play::HUMAN, 4, 4);
    // arena->playGame(true);
    // std::string inp = std::string("Hi");
    // printf("Turns taken %u\n", arena->turns);
    // for (int i = 0; i < 3; i++) {
    //     std::cout << inp << " " << i << std::endl << std::flush;
    //     std::getline(std::cin, inp);
    // }

    arena->playGame(!quiet);

    if (quiet)
        std::cout << "finished" << std::endl << std::flush;
    return 0;
}