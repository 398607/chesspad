#include "chesspad.h"

chesspad::chesspad()
{
    init();
}

chesspad::~chesspad()
{
	// if(frompad != nullptr) delete frompad;
}

// init the pad, including the current version
bool chesspad::init()
{  
    // current version
    ver = 2.10f;

    // do not make any change!!!
    pad[8] = "rnbqkbnr";
    pad[7] = "pppppppp";
    pad[6] = pad[5] = pad[4] = pad[3] = "........";
    pad[2] = "PPPPPPPP";
    pad[1] = "RNBQKBNR";
    stat = 0;
    memset(pois, 0, sizeof pois);
    movedk = movedK = movedR[0] = movedR[1] = movedr[0] = movedr[1] = false;
    frompad = NULL;
    poisdebug = true;
    if(poisdebug) {
        calcpois();
    }
    console = true;
    return true;
}

// if the poi xy is a X
inline bool chesspad::isX(char x, int y)
{
    char w = pad[y][x - 'a'];
    return w >= 'A' && w <= 'Z';
}

// if the poi xy is a x
inline bool chesspad::isx(char x, int y)
{
    char w = pad[y][x - 'a'];
    return w >= 'a' && w <= 'z';
}

// get the owner, return char 'x' or 'X'
inline char chesspad::getowner(char w)
{
    if(w >= 'A' && w <= 'Z') return 'X';
    if(w >= 'a' && w <= 'z') return 'x';
    return '.';
}

// check if the poi is mine
inline bool chesspad::isme(char x, int y, char who)
{
    if(who == 'X') return isX(x, y);
    else if(who == 'x') return isx(x, y);
    else return false;
}

// try to insert a poi into the avalible string
inline bool chesspad::tryinsert(std::string &loli, char x, int y, char who)
{
// do not make any change!!

    // out of pad
    if(x < 'a' || x > 'h') return false;
    if(y < 1 || y > 8) return false;

    // can`t move into friends
    if(isme(x, y, who)) return false;

    // can arrive, push to loli
    loli = loli + x + char(y + '0');

    // can`t continue, return false
    if(pad[y][x - 'a'] != '.') return false;

    // can continue, return ture
    return true;
}

// try to insert a line until cannot reach
inline bool chesspad::trygoin(std::string &loli, char x, int y, char who, int dx, int dy)
{
// do not make any change!!

    // move a step
    char nowx = char(x + dx), nowy = y + dy;

    // insert until can`t continue, and keep going aside the direction
    while(tryinsert(loli, nowx, nowy, who)) nowx = char(nowx + dx), nowy = nowy + dy;
    return true;
}

// return the avalible string of the poi
std::string chesspad::getmove(char x, int y)
{
// do not make any change!!
// change the parameters carefully if necessary

    std::string loli = "";
    char w = pad[y][x - 'a'];
    if(w == '.') {
        return loli;
    } else if(w == 'P') { // Pawn is special: it moves and kills in different ways
        if(y == 2 && pad[4][x - 'a'] == '.' && pad[3][x - 'a'] == '.') tryinsert(loli, x, 4, 'X');
        if(y < 8 && pad[y + 1][x - 'a'] == '.') tryinsert(loli, x, y + 1, 'X');

		// eat
        if(y < 8 && x > 'a' && isme(x - 1, y + 1, 'x')) tryinsert(loli, x - 1, y + 1, 'X');
        if(y < 8 && x < 'h' && isme(x + 1, y + 1, 'x')) tryinsert(loli, x + 1, y + 1, 'X');
    } else if(w == 'p') {
        if(y == 7 && pad[5][x - 'a'] == '.' && pad[6][x - 'a'] == '.') tryinsert(loli, x, 5, 'x');
        if(y > 1 && pad[y - 1][x - 'a'] == '.')tryinsert(loli, x, y - 1, 'x');

		// eat
        if(y > 1 && x > 'a' && isme(x - 1, y - 1, 'X')) tryinsert(loli, x - 1, y - 1, 'x');
        if(y > 1 && x < 'h' && isme(x + 1, y - 1, 'X')) tryinsert(loli, x + 1, y - 1, 'x');

    // and others, just use the dx[] and dy[] to determine, with the help of trygoin() or just tryinsert()
    } else if(w == 'R' || w == 'r') { // trygoin
        const int dx[4] = {1, -1, 0, 0}, dy[4] = {0, 0, 1, -1};
        for(int i = 0; i < 4; i++) trygoin(loli, x, y, getowner(w), dx[i], dy[i]);
    } else if(w == 'N' || w == 'n') { // tryinsert
        const int dx1[8] = {1, 1, -1, -1, 2, 2, -2, -2}, dy1[8] = {2, -2, 2, -2, 1, -1, 1, -1};
        for(int i = 0; i < 8; i++) tryinsert(loli, x + dx1[i], y + dy1[i], getowner(w));
    } else if(w == 'B' || w == 'b') { // trygoin
        const int dx[4] = {1, -1, 1, -1}, dy[4] = {1, 1, -1, -1};
        for(int i = 0; i < 4; i++) trygoin(loli, x, y, getowner(w), dx[i], dy[i]);
    } else if(w == 'K' || w == 'k') { // tryinsert
        const int dx2[8] = {1, 1, 1, -1, -1, -1, 0, 0}, dy2[8] = {1, 0, -1, 1, 0, -1, 1, -1};
        for(int i = 0; i < 8; i++) tryinsert(loli, x + dx2[i], y + dy2[i], getowner(w));
		// 0-0, 0-0-0

		if(w == 'K') {
			if(canChange('X', 3)) loli = loli + "a1";
			if(canChange('X', 2)) loli = loli + "h1";
		}
		if(w == 'k') {
			if(canChange('x', 3)) loli = loli + "a8";
			if(canChange('x', 2)) loli = loli + "h8";
		}

    } else if (w == 'Q' || w == 'q') { // trygoin
        const int dx[8] = {1, -1, 0, 0, 1, -1, 1, -1}, dy[8] = {0, 0, 1, -1, 1, 1, -1, -1};
        for(int i = 0; i < 8; i++) trygoin(loli, x, y, getowner(w), dx[i], dy[i]);
    }
    return loli;
}

// a method to let down the complexity of calculating the score of a status
inline bool chesspad::calcpois()
{
    memset(pois, 0, sizeof(pois));
    for(int i = 1; i <= 8; i++) for(int j = 0; j < 8; j++) {
        if(pad[i][j] >= 'a' && pad[i][j] <= 'z') {
            std::string loli = getmove(j + 'a', i);
            for(int k = 0; k < (int)loli.length() - 1; k += 2) {
                int newx = loli[k + 1] - '0', newy = loli[k] - 'a';
                if(pois[newx][newy] == 1 || pois[newx][newy] == 3) continue;

                // include the attacker x
                pois[newx][newy] += 1;
            }
        }
        if(pad[i][j] >= 'A' && pad[i][j] <= 'Z') {
            std::string loli = getmove(j + 'a', i);
            for(int k = 0; k < (int)loli.length() - 1; k += 2) {
                int newx = loli[k + 1] - '0', newy = loli[k] - 'a';
                if(pois[newx][newy] == 2 || pois[newx][newy] == 3) continue;

                // include the attacker X
                pois[newx][newy] += 2;
            }
        }
    }
    return true;
}

// check whether the king is under attack
inline bool chesspad::checkking(char who)
{
    for(int i = 1; i <= 8; i++) for(int j = 0; j < 8; j++) {
        if(who == 'x' && pad[i][j] == 'k') {
            if(pois[i][j] == 2 || pois[i][j] == 3) return true;
            else return false;
        }
        if(who == 'X' && pad[i][j] == 'K') {
            if(pois[i][j] == 1 || pois[i][j] == 3) return true;
            else return false;
        }
    }
    return false;
}

// check the status of a pad
inline int chesspad::checkstat()
{
    int danK = 0, dank = 0;
    int danger = 0;
    if(poisdebug) {

        // find the king and its pois
        for(int i = 1; i <= 8; i++) for(int j = 0; j < 8; j++) {
            if(pad[i][j] == 'K') danK = pois[i][j];
            if(pad[i][j] == 'k') dank = pois[i][j];
        }
        if(danK == 1 || danK == 3) danger = 2; // K is under attack
        if(dank == 2 || dank == 3) danger = 1; // k is under attack

        // if kings are safe, no need to check the checkmate
        if(danger == 0) return stat = 0;

        // if stat is check, find who is in danger of checkmate
        char loser = 'X';
        if(danger == 1) loser = 'x';
        for(int i = 0; i < 8; i++) for(int j = 1; j <= 8; j++) if(getowner(pad[j][i]) == loser) {
            std::string loli = getmove('a' + i, j);
            if(loli != "") for(int k = 0; k < (int)loli.length() - 1; k += 2) {

                // for any move the loser can do
                chesspad *tmppad = new chesspad;
                *tmppad = *this;
                int newy = loli[k + 1] - '0';
                char newx = loli[k];

                // try the move
                tmppad->pad[newy][newx - 'a'] = pad[j][i];
                tmppad->pad[j][i] = '.';
                if(poisdebug) tmppad->calcpois();

                // check whether the king is under attack
                int king = tmppad->checkking(loser);
                delete tmppad;

                // a way to return safety exists, return "check"
                if(!king) return stat = danger;
            }
        }

        // no way to return safety, that`s a checkmate!
        return stat = danger + 2;
    }

    // poisdebug end
    // no comment for codes outside the poisdebug
    std::string K = "", k = "";
    for(int i = 1; i <= 8; i++) for(int j = 0; j < 8; j++) {
        if(pad[i][j] == 'K') K = K + char(j + 'a') + char(i + '0');
        if(pad[i][j] == 'k') k = k + char(j + 'a') + char(i + '0');
    }
    if(K != "" && checkatk(K[0], K[1] - '0') % 2) danger = 2;
    if(k != "" && checkatk(k[0], k[1] - '0') >= 2) danger = 1;
    if(danger == 0) return stat = 0;

    //check checkmate
    char loser = 'X';
    if(danger == 1) loser = 'x';
    for(int i = 0; i < 8; i++) for(int j = 1; j <= 8; j++) if(getowner(pad[j][i]) == loser) {
        std::string loli = getmove('a' + i, j);
        if(loli != "") for(int k = 0; k < (int)loli.length() - 1; k += 2) {
            chesspad *tmppad = new chesspad;
            *tmppad = *this;
            int newy = loli[k + 1] - '0';
            char newx = loli[k];
            tmppad->pad[newy][newx - 'a'] = pad[j][i];
            tmppad->pad[j][i] = '.';
            tmppad->calcpois();
            int king = tmppad->checkking(loser);
            delete tmppad;
            if(!king) return stat = danger;
        }
    }
    return stat = danger + 2;
}

// check the status of a poi, 0: neither; 1: atked by x; 2: by X; 3: both
inline int chesspad::checkatk(char x, int y)
{
    if(poisdebug) {
        return pois[y][x - 'a'];
    }

    // poisdebug end
    std::string at = "";
    at = at + x + char(y + '0');
    int atk1 = 0, atk2 = 0; // 2: X, 1: x;
    for(int i = 1; i <= 8; i++) for(int j = 0; j < 8; j++) {
        if(pad[i][j] >= 'a' && pad[i][j] <= 'z') {
            std::string loli = getmove(j + 'a', i);
            if(loli != "" && loli.find(at) != std::string::npos) atk1 = 1;
        }
        if (pad[i][j] >= 'A' && pad[i][j] <= 'Z') {
            std::string loli = getmove(j + 'a', i);
            if(loli != "" && loli.find(at) != std::string::npos) atk2 = 1;
        }
    }
    if(!atk1 && !atk2) return 0;
    if(atk1 && !atk2) return 1;
    if(atk2 && !atk1) return 2;
    return 3;
}

// check whether the poi is being atked by aite
inline bool chesspad::oppoatk(char x, int y, char who)
{
    if(checkatk(x, y) == 3) return true;
    if(who == 'x') {
        if(checkatk(x, y) == 2) return true;
        else return false;
    } else {
        if(checkatk(x, y) == 1) return true;
        else return false;
    }
}

// claim a move: loli by player "who", and react to it. ONLY USED IN CONSOLE
int chesspad::claimmove(std::string loli, char who)
{
    // check the no-move draw
    if(stat == 0) {
        int flag = 0;
        for(int i = 0; i < 8; i++) {
            for(int j = 1; j <= 8; j++)
                if(getowner(pad[j][i]) == who) {
                    std::string loli = getmove('a' + i, j);
                    if(loli != "") for(int k = 0; k < (int)loli.length() - 1; k += 2) {
                        chesspad *tmppad = new chesspad;
                        *tmppad = *this;
                        int newy = loli[k + 1] - '0';
                        char newx = loli[k];

                        // try the move
                        tmppad->pad[newy][newx - 'a'] = pad[j][i];
                        tmppad->pad[j][i] = '.';
                        tmppad->calcpois();

                        // check wehether the king is under attack
                        int king = tmppad->checkking(who);
                        delete tmppad;

                        // find any lawful move, break and continue
                        if(!king) {
                            flag = 1;
                            break; //-
                        } //          |
                    } //              |
                } //                  |
            // <----------------------/
            if(flag) break;
        }
        if(flag == 0) {
            throwmsg("no lawful move to be executed, tie game\n");
            return 2;
        }
    }

    // analyse the move
    std::string move;

    // exit
    if(loli == "exit") {
        throwmsg("exit game\n");
        return 2;
    }

    // undo
    if(loli == "undo") {
        if(this->frompad != NULL && this->frompad->frompad != NULL) {
            if(console) std::cout << "undo\n";
            return 3;
        } else {
            throwmsg("undo failure due to data lose, try again\n");
            return 0;
        }
    }

    // make the move string
    if((int)loli.length() < 2) {
        throwmsg("ERROR: found no lawful moves (maybe pattern error), try again\n");
        return 0;
    }
    if(loli[(int)loli.length() - 1] > '0' && loli[(int)loli.length() - 1] < '9') move = loli.substr((int)loli.length() - 2, 2);
    else if((int)loli.length() >= 3) move = loli.substr((int)loli.length() - 3, 2);
    else {
        throwmsg("ERROR: found no lawful moves (maybe pattern error), try again\n");
        return 0;
    }
    std::string oklist = ""; // to store the lawful moves(as the original place)
    char w = 'p'; // the kind

    // normal move
    if('A' <= loli[0] && 'Z' >= loli[0]) {
        if(who == 'x') w = loli[0] - 'A' + 'a';
        else w = loli[0];
    }
    if(loli[0] != '0' && loli[0] >= 'a' && loli[0] <= 'z') {
        if(who == 'x') w = 'p';
        else w = 'P';
    }

    // 0-0, 0-0-0
    int succ = 0;
    if(loli[0] == '0') {
        if(loli == "0-0-0" && who == 'X' && !movedK && !movedR[0] && stat != 2 && pad[1][1] == '.' && pad[1][2] == '.' && pad[1][3] == '.' && !oppoatk('b', 1, 'X') && !oppoatk('c', 1, 'X') && !oppoatk('d', 1, 'X')) {
                pad[1][2] = 'K'; pad[1][3] = 'R';
                pad[1][0] = pad[1][4] = '.';
                succ = 1;
            }
        if(loli == "0-0-0" && who == 'x' && !movedk && !movedr[0] && stat != 1 && pad[8][1] == '.' && pad[8][2] == '.' && pad[8][3] == '.' && !oppoatk('b', 8, 'x') && !oppoatk('c', 8, 'x') && !oppoatk('d', 8, 'x')) {
                pad[8][2] = 'k'; pad[8][3] = 'r';
                pad[8][0] = pad[8][4] = '.';
                succ = 1;
            }
        if(loli == "0-0" && who == 'X' && !movedK && !movedR[1] && stat != 2 && pad[1][5] == '.' && pad[1][6] == '.' && !oppoatk('f', 1, 'X') && !oppoatk('g', 1, 'X')) {
                pad[1][6] = 'K'; pad[1][5] = 'R';
                pad[1][7] = pad[1][4] = '.';
                succ = 1;
            }
        if(loli == "0-0" && who == 'x' && !movedk && !movedr[1] && stat != 1 && pad[8][5] == '.' && pad[8][6] == '.' && !oppoatk('f', 8, 'x') && !oppoatk('g', 8, 'x')) {
                pad[8][6] = 'k'; pad[8][5] = 'r';
                pad[8][7] = pad[8][4] = '.';
                succ = 1;
            }
        if(succ) {

            // frompad
            chesspad* tpad = new chesspad;
            *tpad = *this;
            frompad = tpad;

            return 1;
        } else {
            throwmsg("ERROR: benign transposition is not lawful, try again\n");
            return 0;
        }
    }

    // make the oklist of the normal move
    for(int i = 1; i <= 8; i++) for(int j = 0; j < 8; j++) {
        if(pad[i][j] == w) {
            std::string p = getmove(j + 'a', i); // std::cout << p << " " << p.find(move) << endl;
            if(p.find(move) != std::string::npos) {

                // unambigious

                // Bbe7
                if((int)loli.length() == 4 && loli[1] != 'x' && loli[1] != char(j + 'a')) continue;

                // de7 (for safety)
                if((int)loli.length() == 3 && (w == 'p' || w == 'P') && (loli[0] >= 'a' && loli[0] <= 'z') && loli[0] != char(j + 'a')) continue;

                // dxe7
                if((int)loli.length() == 4 && loli[0] >= 'a' && loli[0] <= 'z' && loli[0] != char(j + 'a')) continue;

                // Bbxe7
                if((int)loli.length() == 5 && loli[2] == 'x' && loli[1] != char(j + 'a')) continue;

                // Bb4e7
                if((int)loli.length() == 5 && loli[2] != 'x' && (loli[1] != char(j + 'a') || loli[2] != char(i + '0'))) continue;

                // push it into the oklist
                oklist = oklist + char(j + 'a') + char(i + '0');
            }
        }
    }
    if(oklist.length() > 2) { // more than one moves found
        throwmsg("ERROR: multiple moves (maybe ambigious), try again\n");
        return 0;
    } else if(oklist.length() == 0) { // no lawful move
        throwmsg("ERROR: no lawful move exists, try again\n");
        return 0;
    } else { // try to do the only move found
        int y = oklist[1] - '0', newy = move[1] - '0';
        char x = oklist[0], newx = move[0];

        // king can`t (attempt to) suicide
        if(stat == 0) { // at time normal
            chesspad *tmppad = new chesspad;
            *tmppad = *this;

            // try the move
            tmppad->pad[newy][newx - 'a'] = pad[y][x - 'a'];
            tmppad->pad[y][x - 'a'] = '.';
            tmppad->calcpois();

            // check whether the king is under attack
            int king = tmppad->checkking(who);
            delete tmppad;

            // king is suiciding
            if(king) {
                throwmsg("ERROR: king can`t go under attack\n");
                return 0;
            }
        }

        // check should be avoided
        if((stat == 1 && who == 'x') || (stat == 2 && who == 'X')) {
            chesspad *tmppad = new chesspad;
            *tmppad = *this;

            //try the move
            tmppad->pad[newy][newx - 'a'] = pad[y][x - 'a'];
            tmppad->pad[y][x - 'a'] = '.';
            tmppad->calcpois();

            // check whether the king is under attack
            int king = tmppad->checkking(who);
            delete tmppad;

            // check not avoided
            if(king) {
                throwmsg("ERROR: still in check, try again\n");
                return 0;
            }
        }

        // frompad
        chesspad* tpad = new chesspad;
        *tpad = *this;
        frompad = tpad;

        // movedK / k, movedR / r
        if(pad[y][x - 'a'] == 'k') movedk = 1;
        if(pad[y][x - 'a'] == 'K') movedK = 1;
        if(x == 'a' && y == 1 && pad[y][x - 'a'] == 'R') movedR[0] = 1;
        if(x == 'h' && y == 1 && pad[y][x - 'a'] == 'R') movedR[1] = 1;
        if(x == 'a' && y == 8 && pad[y][x - 'a'] == 'r') movedr[0] = 1;
        if(x == 'h' && y == 8 && pad[y][x - 'a'] == 'r') movedr[1] = 1;

        // execute the move
        pad[newy][newx - 'a'] = pad[y][x - 'a'];
        pad[y][x - 'a'] = '.';

        // pawn update for P and p
        if(w == 'p' && newy == 1 && y == 2) {
            throwmsg("p can be promoted, please type the result...[q/b/r/m/p]p\n");
            char sb = 'k';
            std::string luke = "qbrnp";
            while(luke.find(sb) == std::string::npos) {
                std::cin >> sb;
            }
            pad[newy][newx - 'a'] = sb;
        }
        if(w == 'P' && newy == 8 && y == 7) {
            throwmsg("P can be promoted, please type the result...[Q/B/R/M/P]\n");
            char sb = 'K';
            std::string luke = "QBRNP";
            while(luke.find(sb) == std::string::npos) {
                std::cin >> sb;
            }
            pad[newy][newx - 'a'] = sb;
        }

        //update pois for sure
        calcpois();
        checkstat();

        return 1;
    }
}

// claim a move: loli. loli includes all infos of the move
int chesspad::claimmovefullpath(std::string loli, char who, bool debug)
{
    // exit
    if(loli == "exit") {
        if(debug) throwmsg("exit game\n");
        return 2;
    }

    // undo
    if(loli == "undo") {
        if(this->frompad != NULL && this->frompad->frompad != NULL) {
			if(debug) throwmsg("<---< undo >--->");
			*this = *(this->frompad->frompad);
            return 0;
        } else {
            if(debug) throwmsg("undo failure due to data lose, try again\n");
            return 0;
        }
    }

	// make place
	int newy = loli[3] - '0', newx = loli[2] - 'a', y = loli[1] - '0', x = loli[0] - 'a';

	// 0-0, 0-0-0
	int move = 0;
	if(pad[newy][newx] == 'R' && pad[y][x] == 'K' && newx == 0) {
		pad[1][2] = 'K'; pad[1][3] = 'R';
        pad[1][0] = pad[1][4] = '.';
		move = 1;
	} else if(pad[newy][newx] == 'R' && pad[y][x] == 'K' && newx == 7) {
		pad[1][6] = 'K'; pad[1][5] = 'R';
		pad[1][7] = pad[1][4] = '.';
		move = 1;
	} else if(pad[newy][newx] == 'r' && pad[y][x] == 'k' && newx == 0) {
		pad[8][2] = 'k'; pad[8][3] = 'r';
        pad[8][0] = pad[8][4] = '.';
		move = 1;
	} else if(pad[newy][newx] == 'r' && pad[y][x] == 'k' && newx == 7) {
		pad[8][6] = 'k'; pad[8][5] = 'r';
        pad[8][7] = pad[8][4] = '.';
		move = 1;
	}
	if(move) {
		// frompad
		chesspad* tpad = new chesspad;
		*tpad = *this;
		frompad = tpad;

		// movedK / k, movedR / r
		if(pad[y][x] == 'k') movedk = 1;
		if(pad[y][x] == 'K') movedK = 1;
		if(x == 0 && y == 1 && pad[y][x] == 'R') movedR[0] = 1;
		if(x == 7 && y == 1 && pad[y][x] == 'R') movedR[1] = 1;
		if(x == 0 && y == 8 && pad[y][x] == 'r') movedr[0] = 1;
		if(x == 7 && y == 8 && pad[y][x] == 'r') movedr[1] = 1;

		return 1;
	}

	// the move is ok?
	std::string oklist = getmove(x + 'a', y);
	if(oklist.find(loli.substr(2, 2)) == std::string::npos) return 0; // didn`t find the move

    // king can`t (attempt to) suicide
    if(stat == 0) { // at time normal
        chesspad *tmppad = new chesspad;
        *tmppad = *this;

        // try the move
        tmppad->pad[newy][newx] = pad[y][x];
        tmppad->pad[y][x] = '.';
        tmppad->calcpois();

        // check whether the king is under attack
        int king = tmppad->checkking(who);
        delete tmppad;

        // king is suiciding
        if(king) {
            if(debug) throwmsg("ERROR: king can`t go under attack\n");
            return 0;
        }
	}

    // check should be avoided
    if((stat == 1 && who == 'x') || (stat == 2 && who == 'X')) {
        chesspad *tmppad = new chesspad;
        *tmppad = *this;

        //try the move
        tmppad->pad[newy][newx] = pad[y][x];
        tmppad->pad[y][x] = '.';
        tmppad->calcpois();

        // check whether the king is under attack
        int king = tmppad->checkking(who);
        delete tmppad;

        // check not avoided
        if(king) {
            if(debug) throwmsg("ERROR: still in check, try again\n");
            return 0;
        }
	}

    // frompad
    chesspad* tpad = new chesspad;
    *tpad = *this;
    frompad = tpad;

    // movedK / k, movedR / r
    if(pad[y][x] == 'k') movedk = 1;
    if(pad[y][x] == 'K') movedK = 1;
    if(x == 0 && y == 1 && pad[y][x] == 'R') movedR[0] = 1;
    if(x == 7 && y == 1 && pad[y][x] == 'R') movedR[1] = 1;
    if(x == 0 && y == 8 && pad[y][x] == 'r') movedr[0] = 1;
    if(x == 7 && y == 8 && pad[y][x] == 'r') movedr[1] = 1;

    // execute the move
    pad[newy][newx] = pad[y][x];
    pad[y][x] = '.';

	char w = pad[newy][newx];

	/*
    // pawn update for P and p
    if(w == 'p' && newy == 1 && y == 2) {
        throwmsg("p can be promoted, please type the result...[q/b/r/m/p]p\n");
        char sb = 'k';
        std::string luke = "qbrnp";
        while(luke.find(sb) == std::string::npos) {
            std::cin >> sb;
        }
        pad[newy][newx] = sb;
    }
    if(w == 'P' && newy == 8 && y == 7) {
        throwmsg("P can be promoted, please type the result...[Q/B/R/M/P]\n");
        char sb = 'K';
        std::string luke = "QBRNP";
        while(luke.find(sb) == std::string::npos) {
            std::cin >> sb;
        }
        pad[newy][newx] = sb;
    }
	*/
	if(w == 'p' && newy == 1 && y == 2) {
        pad[newy][newx] = 'q';
    }
    if(w == 'P' && newy == 8 && y == 7) {
        pad[newy][newx] = 'Q';
    }

    //update pois for sure
    calcpois();
    checkstat();

    return 1;
}

// give the score of the pad, only act as x
inline int chesspad::givescore()
{
    // const int midpoi[8] = {1, 2, 3, 5, 5, 3, 2, 1};
    // const int outpoi[8] = {5, 3, 2, 1, 1, 2, 3, 5};
    const int pvalue[8] = {700, 350, 260, 240, 220, 210, 200, 200};
    const int Pvalue[8] = {200, 200, 220, 250, 255, 260, 350, 700};
    const int nvalue[8] = {295, 297, 300, 305, 305, 300, 297, 295};
    int sc = 0, c = 1;
    if(stat == 4) sc = 2000;
    if(stat == 3) sc = -2000;

    // calc each poi
    for(int i = 0; i < 8; i++) for(int j = 1; j <= 8; j++) {

        // c depend on the one on it
        c = 1;
		
        if(pad[j][i] != '.') {
            switch(pad[j][i]) {
                case 'p': c = pvalue[j - 1]; break;
                case 'P': c = -1 * Pvalue[j - 1]; break;
                case 'r': c = 900; break;
                case 'R': c = -900; break;
                case 'n': c = nvalue[i] + nvalue[j - 1]; break;
                case 'N': c = -1 * (nvalue[i] + nvalue[j - 1]); break;
                case 'b': c = 600; break;
                case 'B': c = -600; break;
                case 'q': c = 1000; break;
                case 'Q': c = -1000; break;
            }
		}
        int poi;
        if(poisdebug) poi = pois[j][i];
        else poi = checkatk(i + 'a', j);

        // c varys to the attackers
        // 0: normal; 1: atk by x only; 2: atk by X only; 3: atk by both.
        const double empty[4] = {1.0, 1.01, 0.99, 1.0};
        const double mypoi[4] = {1.0, 1.20, 0.20, 1.01};
        const double empoi[4] = {1.0, 0.95, 1.05, 1.01};
        if(getowner(pad[j][i]) == '.') c = (int)(c * empty[poi]);
        if(getowner(pad[j][i]) == 'x') c = (int)(c * mypoi[poi]);
        if(getowner(pad[j][i]) == 'X') c = (int)(c * empoi[poi]);

        // the middle area is more important (?)
        if(4 <= i && 5 >= i && 5 <= j && 6 >= j) c = c * 2;

        // add the poi to sc
        sc += c;
    }
    return sc;
}

// calc a pad`s score thinking ahead depth steps
int chesspad::calcscore(int depth, char who, char AIact, int alpha, int beta)
{
    // when it reaches the bottom, just return the score of the current pad
    if(depth == 0) {
		return givescore();	
	}

    int maxsc = -20000, minsc = 20000;
    std::string maxplay = "";
    for(int i = 0; i < 8; i++) for(int j = 1; j <= 8; j++) if(getowner(pad[j][i]) == who) {
        std::string loli = getmove(i + 'a', j);
        if(loli != "") for(int k = 0; k < (int)loli.length() - 1; k += 2) {

            // for any move
            char newx = loli[k]; int newy = loli[k + 1] - '0';
            chesspad* tmppad = new chesspad;
            *tmppad = *this;

            // try the move
			// old version:
            // tmppad->pad[newy][newx - 'a'] = pad[j][i];
            // tmppad->pad[j][i] = '.';
			std::string voi = "";
			int claim = tmppad->claimmovefullpath(voi + char(i + 'a') + char(j + '0') + loli.substr(k, 2), AIact);
			if(claim == 0) continue;

            // pawn update
            if(tmppad->pad[newy][newx - 'a'] == 'p' && newy == 1 && j == 2) tmppad->pad[newy][newx - 'a'] = 'q';
            if(tmppad->pad[newy][newx - 'a'] == 'P' && newy == 8 && j == 7) tmppad->pad[newy][newx - 'a'] = 'Q';

            // calc pois and check status
            tmppad->calcpois();
            int king = tmppad->checkking(who);

            // (dfs) to the next layer
            int w = tmppad->calcscore(depth - 1, 'X' + 'x' - who, AIact, alpha, beta);

            delete tmppad;

            // update the max or min sc
            if(who == 'x') if(w > maxsc) maxsc = w;
            if(who == 'X') if(w < minsc) minsc = w;

            // alpha-beta purning
            if(who == 'x') { // max
                if(alpha < maxsc) alpha = maxsc;
                if(beta <= alpha) return maxsc;
            }
            if(who == 'X') { // min
                if(beta > minsc) beta = minsc;
                if(beta <= alpha) return minsc;
            }
        }
    }
    if(who == 'x') return maxsc;
    else return minsc;
}

// find and do the play by AI!
bool chesspad::givebestplay(int debug, int depth, char AIact)
{
	printf("========================================\n");
    if(stat == 3) return true;
    int maxsc = -20000, minsc = 20000;
    std::string maxplay1 = "", maxplay0 = "", voi = "";
    for(int i = 0; i < 8; i++) for(int j = 1; j <= 8; j++) if(getowner(pad[j][i]) == AIact) {
        std::string loli = getmove(i + 'a', j);
        if(loli != "") for(int k = 0; k < (int)loli.length() - 1; k += 2) {

            // for any lawful move
            char newx = loli[k]; int newy = loli[k + 1] - '0';
            chesspad *tmppad = new chesspad;
            *tmppad = *this;

            // try the move
            // tmppad->pad[newy][newx - 'a'] = pad[j][i];
            // tmppad->pad[j][i] = '.';
			int claim = tmppad->claimmovefullpath(voi + char(i + 'a') + char(j + '0') + loli.substr(k, 2), AIact);
			if(claim == 0) continue;

            // pawn update auto
            if(tmppad->pad[newy][newx - 'a'] == 'p' && newy == 1 && j == 2) tmppad->pad[newy][newx - 'a'] = 'q';
            if(tmppad->pad[newy][newx - 'a'] == 'P' && newy == 8 && j == 7) tmppad->pad[newy][newx - 'a'] = 'Q';
            if(poisdebug) tmppad->calcpois();

			int w = tmppad->calcscore(depth, 'x' + 'X' - AIact, AIact, -20000, 20000);
            // debug log
			if(debug) {
				std::cout << time(NULL);
				std::cout << std::setw(8) << depth << "      " << char(i + 'a') << char(j + '0') << newx << char(newy + '0') << "     " << std::setw(8) << w;
				std::cout << "        " << time(NULL) << std::endl;
			}

            delete tmppad;

            // find the best move and store in the maxplay string
            if(AIact == 'x') {
                if(w > maxsc || (w == maxsc && time(NULL) / 2)) {
                    maxsc = w;
                    maxplay1 = voi + newx + char(newy + '0');
                    maxplay0 = voi + char(i + 'a') + char(j + '0');
                }
            }
            if(AIact == 'X') {
                if(w < minsc || ((w == minsc && time(NULL) / 2))) {
                    minsc = w;
                    maxplay1 = voi + newx + char(newy + '0');
                    maxplay0 = voi + char(i + 'a') + char(j + '0');
                }
            }
        }
    }
    if(maxplay1 == "" || maxplay0 == "") {
		std::cout << "=================RESIGN=================" << std::endl; 
		return false; // that seems impossible, just for sure
	}
    // frompad
    chesspad* tpad = new chesspad;
    *tpad = *this;
    frompad = tpad;

    // show and execute the move
    int newx = maxplay1[1] - '0', x = maxplay0[1] - '0', newy = maxplay1[0] - 'a', y = maxplay0[0] - 'a';
    if(debug) printf("\n%s(%c) -> %s(%c)\n\n", maxplay0.c_str(), pad[x][y], maxplay1.c_str(), pad[newx][newy]);

    pad[newx][newy] = pad[x][y];
    pad[x][y] = '.';

	claimmovefullpath(maxplay0 + maxplay1, AIact);

    // pawn update auto
    if(pad[newx][newy] == 'p' && newx == 1 && x == 2) pad[newx][newy] = 'q';
    if(pad[newx][newy] == 'P' && newx == 8 && x == 7) pad[newx][newy] = 'Q';

    // update pois for sure
    calcpois();
    checkstat();

	printf("========================================\n");
    return true;
}

// givebestplay(1, depth, AIact)
bool chesspad::givebestplay(int depth, char AIact)
{
	return givebestplay(1, depth, AIact);
}

// let AI claim a move and execute it, as x or X
int chesspad::AIclaim(int d, char AIact)
{
    // update stat and print
    if(poisdebug) calcpois();
    checkstat();
    print();

    // AI thinking
    throwmsg("\n------------------chesspad thinking\n");
    if(!givebestplay(d, AIact)) { // give and execute the move AI thought out
        if(AIact == 'x') stat = 3; // if AI didn`t make move, regard as checkmate
        else stat = 4;
        return 2;
    }

    throwmsg("------------------chesspad made move\n\n");
    std::cout << cal << std::endl;
    return 0;
}

// save the game to a .txt file
bool chesspad::save()
{
    // make name and file
    char sname[50];
    sprintf(sname, "chesspadsave_%d.txt", (int)time(NULL));
    std::ofstream fout;
    fout.open(sname);

    // make stack
    std::stack<chesspad> s;
    chesspad tmppad = *this;
    while(1) {
        s.push(tmppad);
        if(tmppad.frompad != NULL) tmppad = *(tmppad.frompad);
        else break;
    }

    // calculator
    int i = 0;

    // pop stack
    while(!s.empty()) {
        fout << "round " << i++ << std::endl;
        chesspad tpad = s.top();
        tpad.print(fout);
        fout << std::endl;
        s.pop();
    }
    fout << "end game\n";

    // close file
    fout.close();

    system(sname);

    return true;
}

// start a P2 game until the game ends
bool chesspad::p2game()
{
    init();
    char who = 'X';
    int end = 0;
    while(stat < 3) {
        
        // player claim
        int claim = playerclaim(who);
        
        // exit
        if(claim == 2) {
            end = 1;
            break;
        }

        // undo
        if(claim == 3) {
            *this = *(this->frompad);
            *this = *(this->frompad);
            continue;
        }

        // change player
        who = char('x' + 'X' - who);
    }

    // end game display
    std::cout << "endgame" << std::endl;
    if(end == 1) std::cout << "1/2 - 1/2" << std::endl;
    if(stat == 3) std::cout << "1 - 0\n" << std::endl;
    if(stat == 4) std::cout << "0 - 1\n" << std::endl;
    print();

    // save game
    std::cout << "save game? ...[Y/N]";
    char saveyn = 'W';
    while(saveyn != 'Y' && saveyn != 'N') std::cin >> saveyn;
    if(saveyn == 'Y') save();

    // pause
    system("pause");

    return true;
}

// input a kakera and continue the game until it ends
bool chesspad::kakera()
{
    throwmsg("type S to use stdin, F to input from file\n");
    char w = '\0';
    while(w != 'F' && w != 'S') std::cin >> w;
    if(w == 'S') {
        for(int i = 8; i >= 1; i--) std::cin >> pad[i];
    }
    if(w == 'F') {
        char name[50];
        std::cin >> name;
        std::ifstream fin(name);
        read(fin);
        fin.close();
    }
    return p2game();
}

// let`s play chess!
bool chesspad::p1game()
{
    init();
    throwmsg("-type the depth of AI`s thought-\n");
    int d = 1000;
    while(d < 0 || d > 4) std::cin >> d;
    std::cout << "-type your side...[X/x]-\n";
    char w = '\0', who = 'X';
    while(w != 'x' && w != 'X') std::cin >> w;
    int end = 0;
    while(stat < 3) {

        if(who == w) {

            // playerclaim
            // std::cout << "-type your move-\n";
            int claim = playerclaim(who);

            // player checkmate
            if((w == 'x' && stat == 3) || (w == 'X' && stat == 4)) break;

            // exit
            if(claim == 2) {
                end = 1;
                break;
            }

            // undo
            if(claim == 3) {
                *this = *(this->frompad);
                *this = *(this->frompad);
                continue;
            }
        }
        else {

            // AIclaim
            AIclaim(d, who);
        }
        who = 'x' + 'X' - who;
    }

    // end game display
    std::cout << "end game" << std::endl;
    if(end == 1) std::cout << "1/2 - 1/2" << std::endl;
    if(stat == 3) std::cout << "1 - 0\n" << std::endl;
    if(stat == 4) std::cout << "0 - 1\n" << std::endl;
    print();

    // save game
    std::cout << "-save game? ...[Y/N]-";
    char saveyn = 'W';
    while(saveyn != 'Y' && saveyn != 'N') std::cin >> saveyn;
    if(saveyn == 'Y') save();

    // pause
    system("pause");

    return true;
}

// secret mode, two AI(s) play the game, may take a long period of time even infinte-loop
bool chesspad::sermode()
{
    init();
    throwmsg("-type the depth of AI`s thought-\n");
    int d; std::cin >> d;
    while(stat < 3) {

        // AIclaim X
        AIclaim(d, 'X');
        // AIclaim x
        AIclaim(d, 'x');
    }

    // end game display
    std::cout << "end game" << std::endl;
    if(stat == 3) std::cout << "1 - 0\n" << std::endl;
    if(stat == 4) std::cout << "0 - 1\n" << std::endl;
    print();

    // save game
    std::cout << "-save game? ...[Y/N]-";
    char saveyn = 'W';
    while(saveyn != 'Y' && saveyn != 'N') std::cin >> saveyn;
    if(saveyn == 'Y') save();

    // pause
    system("pause");

    return true;
}

// print the status and the pad through (ostream) fout
bool chesspad::print(std::ostream &out)
{
    const std::string status[5] = {"-NORMAL-", "-CHECK-", "-CHECK-", "-CHECKMATE-", "-CHECKMATE-"};
    out << status[stat] << std::endl;
    // std::cout << "\\-a-b-c-d-e-f-g-h----a-b-c-d-e-f-g-h\n";
    out << "\\-a-b-c-d-e-f-g-h\n";
    for(int i = 8; i >= 1; i--) {
        out << i;
        for(int j = 0; j < 8; j++) out << " " << pad[i][j];
        // std::cout << "   ";
        // for(int j = 0; j < 8; j++) std::cout << " " << pois[i][j];
        out << std::endl;
    }
    return true;
}

// print(std::cout)
bool chesspad::print()
{
    print(std::cout);
    return true;
}

// read the pad through (istream) fin
bool chesspad::read(std::istream &fin)
{
    if(!fin) {
        std::cout << "istream failed\n";
        return false;
    }
    for(int i = 8; i >= 1; i--) fin >> pad[i];
    return true;
}

// read(std::cin)
bool chesspad::read()
{
    read(std::cin);
    return true;
}

// let the player claim a move and execute it, as x or X
int chesspad::playerclaim(char who)
{

    // update stat and print
    if(poisdebug) calcpois();
    checkstat();
    print();
    while(1) {
        std::string loli;

        std::cout << "-type your move-\n";
        std::cin >> loli;

        // make move
        int claim = claimmove(loli, who);

        if(claim == 0) continue; // failed, try again

        if(claim == 1) return 1; // success, change player
        if(claim == 2) return 2; // exit
        if(claim == 3) return 3; // undo
    }
    return 0;
}

// control panel of chesspad
bool chesspad::panel()
{
    std::cout << "chesspad\nMade By NAGI\n";
    printf("current version: %.2f  poisdebug: %d\n", ver, poisdebug);
    throwmsg("--type P to start a P1 game, W to start a P2 game, Q to enter kakera mode, E to exit-\n");
    char w = 'X';
    std::string likt = "PWQSE";
    while(likt.find(w) == std::string::npos) std::cin >> w;
    if(w == 'P') p1game();
    if(w == 'W') p2game();
    if(w == 'Q') kakera();
    if(w == 'S') sermode();
    return false;
}

// throw message through console(std::cout) (or...?)
bool chesspad::throwmsg(std::string msg, int mode)
{
    if(mode == 0) { // console
        if(!std::cout) return false;
        std::cout << msg;
        return true;
    }
    return false;
}

// throwmsg(msg, 0)
bool chesspad::throwmsg(std::string msg)
{
    return throwmsg(msg, 0);
}

// see if the king could do the change by length = 2 | 3
bool chesspad::canChange(char who, int length)
{
	if(who == 'X' && length == 3) {
		if(!movedK && !movedR[0] 
			&& stat != 2
			&& pad[1][1] == '.' && pad[1][2] == '.' && pad[1][3] == '.'
			&& !oppoatk('b', 1, 'X') && !oppoatk('c', 1, 'X') && !oppoatk('d', 1, 'X')) {
				return true;
		} else {
			return false;
		}
	}
	if(who == 'x' && length == 3) {
		if(!movedk && !movedr[0]
			&& stat != 1 
			&& pad[8][1] == '.' && pad[8][2] == '.' && pad[8][3] == '.' 
			&& !oppoatk('b', 8, 'x') && !oppoatk('c', 8, 'x') && !oppoatk('d', 8, 'x')) {
				return true;
		} else {
			return false;
		}
	}
	if(who == 'X' && length == 2) {
		if(!movedK && !movedR[1] 
			&& stat != 2 
			&& pad[1][5] == '.' && pad[1][6] == '.' 
			&& !oppoatk('f', 1, 'X') && !oppoatk('g', 1, 'X')) {
				return true;
		} else {
			return false;
		}
	}
	if(who == 'x' && length == 2) {
		if(!movedk && !movedr[1] 
			&& stat != 1
			&& pad[8][5] == '.' && pad[8][6] == '.' 
			&& !oppoatk('f', 8, 'x') && !oppoatk('g', 8, 'x')) {
			return true;
		} else {
			return false;
		}
    }
	return false;
}