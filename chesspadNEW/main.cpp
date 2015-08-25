
#include "chesspad.h"
#include "openglSupport.h"

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(720, 720);
	// glutInitWindowPosition(100, 150);
	glutCreateWindow("chesspadNEW");
	glutDisplayFunc(displayPad);
	glutPassiveMotionFunc(myMovedMouse);
	glutMouseFunc(myMouse);
	glutReshapeFunc(myReshape);
	myInit();

	glutMainLoop();
	return 0;
}