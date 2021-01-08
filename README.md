# minimax_player

## Installation

To install the minimax chess algorithm, simply run `make`

If you want to use the auto player, there are two python dependencies. Run:  
`pip install undetected-chromedriver`  
`pip install chess`

## Minimax algorithm

After running `make`, you will should see a binary called `playChess`.  
The formatting to use this file is:  
`./playChess [gameType] [playerOneType] [playerTwoType] [-a cpuOneDepth] [-b cpuTwoDepth] [-q]`  

The only gameType allowed right now is 0, for Chess.  
playerOneType and playerTwoType is 0 for CPU player (using minimax) and 1 for human.  
If player one is a CPU, use `-a [depth]` to specify the depth of the minimax search algorithm you wish to use. I usually use around 3-5.  
Similarly if player two is a CPU, use `-b [depth]` to specify the depth of CPU two.  
The `-q` flag is used only if you want the game output to be quiet. This is primarily used to feed into the python autoplayer.  

## Autoplayer

The autoplayer uses selenium to play against the default bot in chess.com.  
To use the bot, run `python runChess.py [depth]`.  
Depth is the depth to use in the minimax algorithm.

## Possible Improvements

Some changes to make in the future:  
 - Improve the scoring of a board. Perhaps use heuristics. Definitely add a penalty for greater depth moves.
 - Allow the autoplayer to play starting black. As of now always starts white.
 - For cpu vs human mode, let cpu continue to think while waiting for human move input.
