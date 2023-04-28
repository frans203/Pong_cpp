#include <string>
#include <windows.h>
#include <iostream>
#include <conio.h>
#include <sstream>
#include <math.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <GL/glut.h>
#include <sstream>;
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "winmm.lib")
//window size and update rate
int width = 500;
int height = 550;
int interval = 1000 / 60;

//score
int score_left = 0;
int score_right = 0;

//rackets in general
int racket_width = 10;
int racket_height = 80;
int racket_speed = 3;

//left racket position
float racket_left_x = 10.0f;
float racket_left_y = height/2;

//right racket position 
float racket_right_x = width - racket_width - 10;
float racket_right_y = height/2;

//ball
float ball_pos_x = width / 2;
float ball_pos_y = height / 2;
float ball_dir_x = -1.0f;
float ball_dir_y = 0.0f;
int ball_size = 8;
int ball_speed = 2;

//game
bool paused = false;
bool endGame = false;
int lastKeyPressTime = 0;


std::string int2str(int x) {
	std::stringstream ss;
	ss << x;
	return ss.str();
}

void vec2_norm(float& x, float& y) {
	// sets a vectors length to 1 (which means that x + y == 1)
	float length = sqrt((x * x) + (y * y));
	if (length != 0.0f) {
		length = 1.0f / length;
		x *= length;
		y *= length;
	}
	ball_speed = 2;
}

void updateBall() {
	if (paused) {
		return;
	}
	ball_pos_x += ball_dir_x * ball_speed;
	ball_pos_y += ball_dir_y * ball_speed;

	//hit by left racket?
	if (ball_pos_x < racket_left_x + racket_width &&
		ball_pos_x > racket_left_x &&
		ball_pos_y < racket_left_y + racket_height &&
		ball_pos_y > racket_left_y) {
		float t = ((ball_pos_y - racket_left_y) / racket_height) - 0.5f;
		ball_dir_x = fabs(ball_dir_x); //force positive
		ball_dir_y = t;
		ball_speed += 0.6;
		PlaySound(TEXT("collision.wav"), NULL, SND_FILENAME | SND_ASYNC);
	}

	//hit by right racket?
	if (ball_pos_x > racket_right_x &&
		ball_pos_x < racket_right_x + racket_width &&
		ball_pos_y < racket_right_y + racket_height &&
		ball_pos_y > racket_right_y) {
		float t = ((ball_pos_y - racket_right_y) / racket_height) - 0.5f;
		ball_dir_x = -fabs(ball_dir_x);
		ball_dir_y = t;
		ball_speed += 0.6;
		PlaySound(TEXT("collision.wav"), NULL, SND_FILENAME | SND_ASYNC );
	}
	//hit left wall?
	if (ball_pos_x < 0) {
		++score_right;
		ball_pos_x = width / 2;
		ball_pos_y = height / 2;
		ball_dir_x = fabs(ball_dir_x);
		ball_dir_y = 0;
		vec2_norm(ball_dir_x, ball_dir_y); //sets the length of the vector to 1, maintaining speed
		PlaySound(TEXT("score.wav"), NULL, SND_FILENAME | SND_ASYNC);
	}

	//hit right wall?
	if (ball_pos_x > width) {
		++score_left;
		ball_pos_x = width / 2;
		ball_pos_y = height / 2;
		ball_dir_x = -fabs(ball_dir_x);
		ball_dir_y = 0;
		vec2_norm(ball_dir_x, ball_dir_y); //sets the length of the vector to 1, maintaining speed
		PlaySound(TEXT("score.wav"), NULL, SND_FILENAME | SND_ASYNC);
	}

	//hit top wall?
	if (ball_pos_y > height) {
		ball_dir_y = -fabs(ball_dir_y);
	}

	//hit bottom wall?
	if (ball_pos_y < 0) {
		ball_dir_y = fabs(ball_dir_y);
	}

}

void keyboard() {
		int currentTime = glutGet(GLUT_ELAPSED_TIME);
		//left racket
		if (GetAsyncKeyState(0x57) && !paused) racket_left_y += racket_speed; //verify if the user typed W key
		if (GetAsyncKeyState(0x53) && !paused) racket_left_y -= racket_speed; //verify if the user typed S key

		//right racket
		if (GetAsyncKeyState(VK_UP) && !paused) racket_right_y += racket_speed; //verify if the user typed up key 
		if (GetAsyncKeyState(VK_DOWN) && !paused) racket_right_y -= racket_speed; //verify if the user typed down key 
		
		if (GetAsyncKeyState(VK_SPACE) & 0x8000 && currentTime - lastKeyPressTime > 200) {
			paused = !paused;
			lastKeyPressTime = currentTime;
			if (endGame) {
				score_left = 0;
				score_right = 0;
				racket_left_x = 10.0f;
				racket_left_y = height / 2;
				racket_right_x = width - racket_width - 10;
				racket_right_y = height / 2;
			}
		} 

		
}

void drawText(float x, float y, const char* string) {
	// Set the position for the text using glRasterPos2f
	glRasterPos2f(x, y);

	// Loop through each character in the string
	for (const char* c = string; *c != '\0'; c++) {
		// Draw the character using glutBitmapCharacter
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
	}


}

void drawRect(float x, float y, float width, float height) {
	glBegin(GL_QUADS);
	glVertex2f(x, y);
	glVertex2f(x + width, y);
	glVertex2f(x + width, y + height);
	glVertex2f(x, y + height);
	glEnd();
}

void drawBall() {
	glColor3f(1.0f, 1.0f, 1.0f);
	glPushMatrix(); 
	glTranslatef(ball_pos_x, ball_pos_y, 0.0f); 
	gluDisk(gluNewQuadric(), 0.0, ball_size, 20, 1); 
	glPopMatrix(); 
}


void verifyWinner() {
	if (score_left == 15) {
		std::string winner_string = "Left Player is the winner!";
		int stringLength = glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)winner_string.c_str());
		drawText((width / 2) - (stringLength / 2), 30, winner_string.c_str());
		paused = true;
		endGame = true;
	}
	else if (score_right == 15) {
		std::string winner_string = "Right Player is the winner!";
		int stringLength = glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)winner_string.c_str());
		drawText((width / 2) - (stringLength / 2), 30, winner_string.c_str());
		paused = true;
		endGame = true;
	}
}

void draw() {
	//clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	std::string score_string = std::to_string(score_left) + ":" + std::to_string(score_right);

	//draw score
	drawText(width/2 - 10, height - 30, score_string.c_str());

	//draw rackets 
	drawRect(racket_left_x, racket_left_y, racket_width, racket_height);
	drawRect(racket_right_x, racket_right_y, racket_width, racket_height);
	drawBall();
	verifyWinner();
	glutSwapBuffers();

}




void update(int value) {
	keyboard();

	updateBall();

	glutTimerFunc(interval, update, 0);

	glutPostRedisplay();
}

void enable2D(int width, int height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, width, 0.0f, height, 0.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}



int main(int argc, char** argv)
{
	// Initialize glut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutCreateWindow("Pong");

	// Callback functions
	if (!paused || !endGame) {
		glutDisplayFunc(draw);
		glutTimerFunc(interval, update, 0);

	}
	
	
	//setup 2d scene
	enable2D(width, height);
	glColor3f(1.0, 1.0f, 1.0f);


	// Entra no loop principal do GLUT
	glutMainLoop();

	return 0;
}