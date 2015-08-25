#include "openglSupport.h"

// the main pad
chesspad* myPad = new chesspad;;

// display stuff
int canMove1[9][9];

// whether the poi is on mouse
int onMouse[9][9];

// constLamp: the move hint; claimLamp: the poi you want to move
std::string constLamp = "", claimLamp = "";

// current player
char who = 'X';

// current state of user cmd
int statex = 0;

// scr W & H and poiRange, for display and mouseEvent
int scrWidth = 720, scrHeight = 720, poiRange = 72;

// depth of AI thought
int thinkDepth = 2;

// current mouse i j
int curX, curY;

// if the game is end
bool endGame = false;

// get the correct i for pad[i][j] from (x, y) in mouse event
int geti(int x, int y) {
		y = scrHeight - y;
		return (y + poiRange / 2) / poiRange;
}

// get the correct j for pad[i][j] from (x, y) in mouse event
int getj(int x, int y) {
	return (x - poiRange / 2) / poiRange;
}

// my Init()
void myInit() {
	glClearColor(0.8, 0.8, 0.8, 0.0);
	glColor3f(0.0f, 0.0f, 0.0f);
	glPointSize(4.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, scrWidth, 0.0, scrHeight);
}

inline void vertex(int i, int j, float range) {
	glBegin(GL_QUADS);
	glVertex2f(poiRange * (j + 1) + range, poiRange * i + range);
	glVertex2f(poiRange * (j + 1) + range, poiRange * i - range);
	glVertex2f(poiRange * (j + 1) - range, poiRange * i - range);
	glVertex2f(poiRange * (j + 1) - range, poiRange * i + range);
	glEnd();
}

inline void vertex(int i, int j, int range) {
	vertex(i, j, (float)range);
}

inline void fream(int i, int j, int range) {
	glLineWidth(3.0);
	glBegin(GL_LINE_LOOP);
	glVertex2d(poiRange * (j + 1) + range, poiRange * i + range);
	glVertex2d(poiRange * (j + 1) + range, poiRange * i - range);
	glVertex2d(poiRange * (j + 1) - range, poiRange * i - range);
	glVertex2d(poiRange * (j + 1) - range, poiRange * i + range);
	glVertex2d(poiRange * (j + 1) + range, poiRange * i + range);
	glEnd();
}

inline void vertexPoiExact(char w, int x, int y) {

	int atx = x, aty = y;
	int i = 9 - geti(x, y), j = getj(x, y);

	if(w == 'r' || w == 'R') {
		vertex(i, j, poiRange / 5);
	} else if(w == 'n' || w == 'N') {
		glBegin(GL_POLYGON);
		glVertex2d(atx, aty);
		double rad = poiRange / 5;
		for(double k = -60; k <= 240; k += 1) {
			glVertex2f(atx + cosf(3.1415 * k / 180) * rad, aty + sin(3.1415 * k / 180.0) * rad);
		}
		glVertex2d(atx, aty);
		glEnd();
	} else if(w == 'b' || w == 'B') {
		glBegin(GL_POLYGON);
		double rad = poiRange / 6;
		glVertex2f(atx, aty);
		glVertex2f(atx + rad / 2, aty + rad / 2 * 1.73);
		glVertex2f(atx, aty + rad * 1.73);
		glVertex2f(atx - rad, aty);
		glVertex2f(atx, aty - rad * 1.73);
		glVertex2f(atx + rad, aty);
		glVertex2f(atx, aty);
		glEnd();
	} else if(w == 'q' || w == 'Q') {
		glBegin(GL_POLYGON);
		double rad = poiRange / 4;
		for(int k = 0; k < 8; k++) {
			glVertex2f(atx + cosf((22.75 + 45 * k) * 3.14 / 180) * rad / 2.5, aty + sinf((22.75 + 45 * k) * 3.14 / 180) * rad / 2.5);
			glVertex2f(atx + cosf((45 + 45 * k) * 3.14 / 180) * rad, aty + sinf((45 + 45 * k) * 3.14 / 180) * rad);
		}
		glVertex2f(atx + cos(22.75 * 3.14 / 180) * rad / 2, aty + sin(22.75 * 3.14 / 180) * rad / 2);
		glEnd();
	} else if(w == 'k' || w == 'K') {
		glBegin(GL_QUADS);
		double rad = poiRange / 5;
		glVertex2f(atx + rad, aty + rad / 4);
		glVertex2f(atx + rad, aty - rad / 4);
		glVertex2f(atx - rad, aty - rad / 4);
		glVertex2f(atx - rad, aty + rad / 4);

		glVertex2f(atx + rad / 4, aty + rad);
		glVertex2f(atx + rad / 4, aty - rad);
		glVertex2f(atx - rad / 4, aty - rad);
		glVertex2f(atx - rad / 4, aty + rad);
		glEnd();
	} else if(w == 'p' || w == 'P') {
		glBegin(GL_POLYGON);
		double rad = poiRange / 7;
		for(double k = 360; k >= 0; k -= 1) {
			glVertex2f(atx + cosf(3.1415 * k / 180) * rad, aty + sin(3.1415 * k / 180.0) * rad);
		}
		glEnd();
	}
}

inline void vertexPoi(char w, int i, int j) {
	vertexPoiExact(w, poiRange * (j + 1), poiRange * i);
	
	 /*
	 // centre of poi
	int atx = poiRange * (j + 1), aty = poiRange * i;

	if(w == 'r' || w == 'R') {
		vertex(i, j, poiRange / 5);
	} else if(w == 'n' || w == 'N') {
		glBegin(GL_POLYGON);
		glVertex2d(atx, aty);
		double rad = poiRange / 5;
		for(double k = -60; k <= 240; k += 1) {
			glVertex2f(atx + cosf(3.1415 * k / 180) * rad, aty + sin(3.1415 * k / 180.0) * rad);
		}
		glVertex2d(atx, aty);
		glEnd();
	} else if(w == 'b' || w == 'B') {
		glBegin(GL_POLYGON);
		double rad = poiRange / 5;
		glVertex2f(atx, aty);
		glVertex2f(atx + rad / 2, aty + rad / 2 * 1.73);
		glVertex2f(atx, aty + rad * 1.73);
		glVertex2f(atx - rad / 2, aty);
		glVertex2f(atx, aty - rad * 1.73);
		glVertex2f(atx + rad / 2, aty);
		glVertex2f(atx, aty);
		glEnd();
	} else if(w == 'q' || w == 'Q') {
		glBegin(GL_POLYGON);
		double rad = poiRange / 4;
		for(int k = 0; k < 8; k++) {
			glVertex2f(atx + cosf((22.75 + 45 * k) * 3.14 / 180) * rad / 2.5, aty + sinf((22.75 + 45 * k) * 3.14 / 180) * rad / 2.5);
			glVertex2f(atx + cosf((45 + 45 * k) * 3.14 / 180) * rad, aty + sinf((45 + 45 * k) * 3.14 / 180) * rad);
		}
		glVertex2f(atx + cos(22.75 * 3.14 / 180) * rad / 2, aty + sin(22.75 * 3.14 / 180) * rad / 2);
		glEnd();
	} else if(w == 'k' || w == 'K') {
		glBegin(GL_QUADS);
		double rad = poiRange / 5;
		glVertex2f(atx + rad, aty + rad / 4);
		glVertex2f(atx + rad, aty - rad / 4);
		glVertex2f(atx - rad, aty - rad / 4);
		glVertex2f(atx - rad, aty + rad / 4);

		glVertex2f(atx + rad / 4, aty + rad);
		glVertex2f(atx + rad / 4, aty - rad);
		glVertex2f(atx - rad / 4, aty - rad);
		glVertex2f(atx - rad / 4, aty + rad);
		glEnd();
	} else if(w == 'p' || w == 'P') {
		glBegin(GL_POLYGON);
		double rad = poiRange / 7;
		for(double k = 360; k >= 0; k -= 1) {
			glVertex2f(atx + cosf(3.1415 * k / 180) * rad, aty + sin(3.1415 * k / 180.0) * rad);
		}
		glEnd();
	}
	*/
}

// displayFunc.
void displayPad() {
	glClear(GL_COLOR_BUFFER_BIT);

	// chesspad Basic
	for(int j = 0; j < 8; j++) for(int i = 1; i <= 8; i++) {
		if((i + j) % 2 == 1) glColor3f(0.44f, 0.303f, 0.214f); // black
		else if((i + j) % 2 == 0) glColor3f(0.92f, 0.77f, 0.337f); // white
		vertex(i, j, poiRange / 2);
	}

	// constLamp
	for(int k = 0; k < (int)constLamp.length() - 1; k += 2) {
		int j = constLamp[k] - 'a', i = constLamp[k + 1] - '0';
		
		int curI = geti(curX, curY), curJ = getj(curX, curY);
		if(i == curI && j == curJ) glColor3f(0.05f, 1.0f, 0.1f);
		else glColor3f(0.05f, 0.5f, 0.1f);
		fream(i, j, poiRange / 3);
	}

	for(int j = 0; j < 8; j++) for(int i = 1; i <= 8; i++) {
		// std::cout << myPad->pad[j][i];

		// poi
		if(myPad->pad[i][j] != '.' && !onMouse[i][j]) {
			if(myPad->isx(j + 'a', i)) glColor3f(0.0f, 0.0f, 0.0f);
			if(myPad->isX(j + 'a', i)) glColor3f(1.0f, 1.0f, 1.0f);
			// vertex(i, j, 2);
			vertexPoi(myPad->pad[i][j], i, j);
		}

		// onMouse home hint
		if(onMouse[i][j]) {
			int curI = geti(curX, curY), curJ = getj(curX, curY);
			if(i == curI && j == curJ)
				glColor3f(0.05f, 0.3f, 0.1f);
			else
				glColor3f(0.05f, 0.1f, 0.1f);
			fream(i, j, poiRange / 3);
		}

		// onMouse poi
		if(myPad->pad[i][j] != '.' && onMouse[i][j]) {
			if(myPad->isx(j + 'a', i)) glColor3f(0.0f, 0.0f, 0.0f);
			if(myPad->isX(j + 'a', i)) glColor3f(1.0f, 1.0f, 1.0f);
			vertexPoiExact(myPad->pad[i][j], curX, scrHeight - curY);
		}
		
		// can move
		if(canMove1[i][j]) {
			glColor3f(0.15f, 0.6f, 0.2f);
			// fream(i, j, poiRange / 5);
			glLineWidth(3);
			glBegin(GL_POLYGON);
			double rad = poiRange / 10;
			for(double k = 360; k >= 0; k -= 1) {
				glVertex2f(poiRange * (j + 1) + cosf(3.1415 * k / 180) * rad, poiRange * i + sin(3.1415 * k / 180.0) * rad);
			}
			glEnd();
		}
	}
	glFlush();
	Sleep(20);
}

// glutPassiveMotionFunc
void myMovedMouse(int x, int y) {
	// get current mouse i j
	curX = x, curY = y;

	// clear the canMove1 list
	memset(canMove1, 0, sizeof canMove1);

	// if it`s AI`s turn, do the best move
	if(statex == 0 && who == 'x') {
		bool claim = myPad->givebestplay(thinkDepth, 'x');
		who = 'X';
		if(!claim) endGame = true;
	}
	glutPostRedisplay();
		if(statex == 0) {
		// get i and j
		// std::cout << x << " " << y << std::endl;
		int i = geti(x, y), j = getj(x, y);
		//std::cout << char(j + 'a') << i << std::endl;

		if(0 > j || 7 < j || 1 > i || 8 < i) return;

		// get move list and put in canMove list
		std::string loli = myPad->getmove(j + 'a', i);
		//std::cout << loli << std::endl;
		for(int k = 0; k < (int)loli.length() - 1; k += 2)
			canMove1[loli[k + 1] - '0'][loli[k] - 'a'] = 1;
	}
}

// glutMouseFunc
void myMouse(int button, int state, int x, int y) {
	if(endGame) return;
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		int i = geti(x, y), j = getj(x, y);
		if(0 > j || 7 < j || 1 > i || 8 < i) return;

		// claimLamp
		if(statex == 0 && myPad->getowner(myPad->pad[i][j]) == who) {
			if(statex == 0) {
				constLamp = myPad->getmove(j + 'a', i);
				if(constLamp.length() > 0) {
					statex = 1;
					claimLamp = claimLamp + char(j + 'a') + char(i + '0');
					onMouse[i][j] = 1;
				}
			}
			glutPostRedisplay();
			return;
		}

		// (try to) execute move
		if(statex == 1) {
			std::string voi = "";
			std::string destination = voi + char(j + 'a') + char(i + '0');
			std::string move = claimLamp + destination;
			// std::cout << move << std::endl;
			if(move == claimLamp) {
				statex = 0;
				onMouse[claimLamp[1] - '0'][claimLamp[0] - 'a'] = 0;
				constLamp = claimLamp = "";
				glutPostRedisplay();
				return;
			}
			int claim = myPad->claimmovefullpath(move, who, true);
			if(claim == 1) {
				// ready for next move
				memset(canMove1, 0, sizeof canMove1);
				glutPostRedisplay();
				who = 'x';
			}
			onMouse[claimLamp[1] - '0'][claimLamp[0] - 'a'] = 0;
			constLamp = claimLamp = "";
			statex = 0;
			glutPostRedisplay();
			return;
		}
	} // button == ... && state == ...
	if(statex != 1 && who == 'X' && button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		int i = geti(x, y), j = getj(x, y);
		if(0 > j || 7 < j || 1 > i || 8 < i) return;
		myPad->claimmovefullpath("undo", 'X');
		return;
	}
	if(statex == 1 && who == 'X' && button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		onMouse[claimLamp[1] - '0'][claimLamp[0] - 'a'] = 0;
		constLamp = claimLamp = "";
		statex = 0;
		glutPostRedisplay();
		return;
	}
	// std::cout << constLamp << " " << claimLamp << " " << who << " " << statex << std::endl;
	glutPostRedisplay();
}

// glutReshapeFunc
void myReshape(int width, int height) {
	scrWidth = width;
	scrHeight = height;
	poiRange = min(scrHeight / 9, scrWidth / 9);
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)width, 0.0, (GLdouble)height);
}