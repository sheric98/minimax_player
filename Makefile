playChess: playChess.cpp thc.cpp thc.h wrap.cpp wrap.h minimax.cpp minimax.h play.cpp play.h
	g++ -std=c++17 -pthread -o playChess playChess.cpp thc.cpp wrap.cpp minimax.cpp play.cpp