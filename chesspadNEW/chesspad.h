//RIAFNU SI EFIL
#ifndef CHESSPAD_H
#define CHESSPAD_H

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <stack>
#include <fstream>

class chesspad {
public:
	std::string pad[9];
private:
	float ver;
	int stat;
	int pois[9][9];
	bool movedk, movedK, movedR[2], movedr[2];
	chesspad* frompad;
	bool poisdebug;
	bool console;
	int cal;
public:
	chesspad();
	~chesspad();
public:
	bool init();
	bool isX(char, int);
	bool isx(char, int);
	char getowner(char);
	bool isme(char, int, char);
	bool tryinsert(std::string&, char, int, char);
	bool trygoin(std::string&, char, int, char, int, int);
	std::string getmove(char, int);
	bool calcpois();
	bool checkking(char);
	int checkstat();
	int checkatk(char, int);
	bool oppoatk(char, int, char);
	int claimmove(std::string, char);
	int claimmovefullpath(std::string, char, bool = false);
	int givescore();
	int calcscore(int, char, char, int, int);
	bool givebestplay(int, char);
	bool givebestplay(int, int, char);
	int AIclaim(int, char);
	bool save();
	bool p2game();
	bool kakera();
	bool p1game();
	bool sermode(); 
	// stdio
	bool print(std::ostream&);
	bool print();
	bool read(std::istream&);
	bool read();
	int playerclaim(char);
	// experimental
	bool throwmsg(std::string, int);
	bool throwmsg(std::string);
	// experimetal*
	bool canChange(char, int);
public:
	bool panel();
};

#endif // CHESSPAD_H