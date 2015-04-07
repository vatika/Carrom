#include <iostream>
#include <cmath>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include<string.h>
#include<sys/time.h>
using namespace std;
#define eCS 1
#define PI 3.141592653589
#define DEG2RAD(deg) (deg * PI / 180)
#define MAX_SPEED 0.5
#define BOARD_LEN 5.0
#define EPSILON 0.03
#define NCOINS 7
#define COIN_RADIUS 0.08f
#define STRIKER_RADIUS 0.10f
#define STRIKER_SPEED_DEC 0.1f
#define BUTTON_CLICKED 1
#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>


// Class Declarations
int game_over = 0;
int mouse_flag = 0;
int mouse_speed_flag = 0;
int left_mouse = 0;
int right_mouse = 0;
void mouseMotion(int x,int y);
float distance(float x1, float y1, float x2, float y2)
{
	return sqrt(((x1-x2)*(x1-x2)) + ((y1-y2)*(y1-y2)));

}

float magnitude(float x,float y){
	float mag = sqrt(pow(x,2) + pow(y,2));
	return mag;
}
float dotProduct(float x1,float y1,float x2,float y2){
	return (x1*x2 + y1*y2);
}
class Circle{
	private:
		float rad;
	protected:
		float x;
		float y;
		float speed;

	public:
		Circle(){
			x = 0;
			y = 0;
			rad = 0;
			speed = 0.0;
		}
		Circle(float x1,float rad1,float y1){
			x = x1;
			y = y1;
			rad = rad1;
			speed= 0.0;
		}
		void set_x(float x1){
			x = x1;
		}
		void set_y(float y1){
			y=y1;
		}
		void set_rad(float r1){
			rad = r1;
		}
		void set_speed(float v){
			speed = v;
		}

		float get_x(){
			return x;
		}
		float get_y(){
			return y;
		}
		float get_rad(){
			return rad;
		}
		float get_speed(){
			return speed;
		}
		void drawCoin(float rad) {

			glBegin(GL_TRIANGLE_FAN);
			for(int i=0 ; i<360 ; i++) {
				glVertex2f(rad * cos(DEG2RAD(i)), rad * sin(DEG2RAD(i)));
			}

			glEnd();
		}
};
class Striker : public Circle {
	private:

		int rotate_flag;
		int up_flag;
		int right_flag;
		int speed_flag;
		int mass;
	public:
		float theta;
		Striker(): Circle(){
			mass = 2.0;
			theta= PI/2;
			up_flag = 0;
			right_flag = 0;

		}

		Striker(float x,float rad,float y): Circle(){
			theta= PI/2;
			rotate_flag = 0;
			up_flag = 0;
			right_flag = 0;
		}
		int get_rotate_flag(){
			return rotate_flag;
		}
		void set_rotate_flag(int f){
			rotate_flag = f;
		}
		int get_mass(){
			return mass;
		}
		void set_mass(int f){
			mass = f;
		}
		int get_speed_flag(){
			return rotate_flag;
		}
		void set_speed_flag(int f){
			speed_flag = f;
		}
		int get_up_flag(){
			return up_flag;
		}
		void set_up_flag(int f){
			up_flag = f;
		}
		int get_right_flag(){
			return right_flag;
		}
		void set_right_flag(int f){
			right_flag = f;
		}
		void make_strike_line(){
			glPushMatrix();
			glTranslatef(get_x(),get_y(),0.0f);
			glLineWidth(3.5);
			glColor3f(1.0, 0.0, 0.0);
			glBegin(GL_LINES);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(0.5*cos(theta), 0.5*sin(theta), 0);
			glEnd();
			glPopMatrix();


		}
		void rotate(int rotate_right){
			make_strike_line();
			glPushMatrix();
			glTranslatef(get_x(),get_y(),0.0f);
			if ( rotate_right == 1 ){
				theta += 0.05;
				if (theta > 2*PI) 
					theta -= 2*PI; 
			}
			else if ( rotate_right ==  -1 ){
				theta -= 0.1;
				if (theta < 0.0) 
					theta += 2*PI; 

			}
			glRotatef(theta,0.0,0.0,1.0);
			glTranslatef(-(get_x()),-(get_y()),0.0f);
			glPopMatrix();
			set_rotate_flag(0);

		}
		void make_speed_line(){
			glPushMatrix();
			glTranslatef(4.0f,-1.0f,0.0f);
			glLineWidth(5.5);
			glColor3f(2.0, 2.0, 0.0);
			glBegin(GL_LINES);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(0.0, speed*3, 0.0);
			glEnd();
			glPopMatrix();
		
		}
		void check_keys(){

			if ( right_flag  == 1){
				if (x <= 1.76f ){
					x = Circle::get_x() + 0.1;
					right_flag = 0;
				}
			}
			else if ( right_flag == -1 ){
				if (x >= -1.76f ){
					x = Circle::get_x() - 0.1;
					right_flag = 0;
				}
			}
			make_speed_line();
			if ( speed_flag == 1 ){
				speed  += STRIKER_SPEED_DEC;
				if ( speed > MAX_SPEED )speed = MAX_SPEED;

			}
			else if ( speed_flag == -1 ){
				speed -= STRIKER_SPEED_DEC;
				if ( speed < 0 )speed = 0;
			}
			speed_flag = 0;
		}	
};
class Coin : public Circle{
	public:
		int mass;
		float theta;
		int visible;

		Coin(): Circle(){
			mass = 1.0;
			theta = 0.0;
			visible = 1;
		}
		Coin(float x,float rad,float y,float color): Circle(){
			mass = 1.0;
		}
		int get_mass(){
			return mass;
		}
		void set_mass(int f){
			mass = f;
		}

};
class Board {
	private :
		Circle Pocket[4];
		Circle sideC[4];
		float len;
		float fric;	
		int score;
		int coins_w;
		int coins_b;
		int coins_q;
		float collected_x;
		float collected_y;
		int positioning_striker_flag;
	public:
		Striker S;
		int move_flag;
		Coin C[7];
		struct timeval tim;
		double t1,t2;
		Board(){
			Pocket[0].set_x(-2.2f);Pocket[0].set_rad(0.30f);Pocket[0].set_y(-2.23f);
			Pocket[1].set_x(2.2f);Pocket[1].set_rad(0.30f);Pocket[1].set_y(-2.23f);
			Pocket[2].set_x(2.2f);Pocket[2].set_rad(0.30f);Pocket[2].set_y(2.23f);
			Pocket[3].set_x(-2.2f);Pocket[3].set_rad(0.30f);Pocket[3].set_y(2.23f);
			C[6].set_x(0.0f);C[6].set_rad(COIN_RADIUS);C[6].set_y(0.0f);
			sideC[0].set_x(1.6f);sideC[0].set_rad(0.28f);sideC[0].set_y(1.47f);
			sideC[1].set_x(-1.6f);sideC[1].set_rad(0.28f);sideC[1].set_y(1.47f);
			sideC[2].set_x(-1.6f);sideC[2].set_rad(0.28f);sideC[2].set_y(-1.47f);
			sideC[3].set_x(1.6f);sideC[2].set_rad(0.28f);sideC[3].set_y(-1.47f);		
			S.set_x(-0.7f);S.set_rad(STRIKER_RADIUS);S.set_y(-1.7f);

			gettimeofday(&tim,NULL);
			t1=tim.tv_sec+(tim.tv_usec/1000000.0);  
			score = 30;	//initially start game with 30
			coins_w = 0;	//coins are 0 in the starting of the game
			coins_b = 0;
			coins_q = 0;
			collected_x = 0.0f;
			collected_y = -3.0f;
			len = BOARD_LEN;
			fric = 0.01;
			move_flag=0;
			int flag =0 ;
			for(int j=0 ; j<6 ;j++){
				if(flag == 0){
					flag=1;
				}
				else{
					flag=0;
				}
				float c_x = 0.28*cos((PI/3)*j);
				float c_y = 0.28*sin((PI/3)*j  );
				C[j].set_x(c_x);C[j].set_rad(COIN_RADIUS);C[j].set_y(c_y);

			}	
		}

		int get_move_flag(){
			return move_flag;
		}
		void set_move_flag(int f){
			move_flag = f;
		}
		void drawCoins(){
			for(int j=0 ; j< 6 ;j++){

				if(j%2 == 0){
					glColor3f(0.2f,0.2f,0.2f);
				
				}
				else{
					glColor3f(1.0f,1.0f,1.0f);
				}
				glPushMatrix();
				glTranslatef(0.0f,0.0f,0.0f);
				glTranslatef(C[j].get_x(),C[j].get_y(),0.0f);
				S.drawCoin(COIN_RADIUS);
				glPopMatrix();
			}
			//Draw Queen
			glTranslatef(C[6].get_x(), C[6].get_y(), 0.0f);
			glColor3f(1.0f, 0.0f, 0.0f);
			C[6].drawCoin(COIN_RADIUS);
			glPopMatrix(); 


		}
		void drawBox(){
			glTranslatef(0.0f, 0.0f, -8.0f);
			glColor3f(1.0f, 0.0f, 0.0f);

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			glBegin(GL_QUADS);
			glVertex2f(-len / 2, -len / 2);
			glVertex2f(len / 2, -len / 2);
			glVertex2f(len / 2, len / 2);
			glVertex2f(-len / 2, len / 2);
			glEnd();

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		void innerBoxCorner(float x1,float y1,float x2,float y2){
			glPushMatrix();
			glColor3f(0.0f, 0.0f, 1.0f);
			glRectf(x1,x2,y1,y2);
			glPopMatrix();
		}

		void innerBox(){
			innerBoxCorner(-1.6f,1.1f,-1.8f,-1.1f); 
			innerBoxCorner(1.6f,-1.1f,1.8f,1.1f); 
			innerBoxCorner(-1.3f,1.85f,1.3f,1.65f);
			innerBoxCorner(-1.3f,-1.85f,1.3f,-1.65f);
		}
		void innerCircle(float x,float y){
			glPushMatrix();
			glTranslatef(x,y, 0.0f);
			glColor3f(0.0f, 1.0f, 1.0f);
			S.drawCoin(0.28f);
			glPopMatrix(); 
		}
		void drawPocket(float x,float y){
			glPushMatrix();
			glTranslatef(x,y, 0.0f);
			glColor3f(1.0f, 1.0f, 0.0f);
			S.drawCoin(0.30f);
			glPopMatrix(); 
		}

		void checkCollisionWithBoard(){
			if ( S.get_x() + S.get_rad() >= BOARD_LEN/2 ){
				S.theta = (PI - S.theta);
				S.set_x(BOARD_LEN/2 - S.get_rad());
			}
			if ( S.get_x() - S.get_rad() <= -BOARD_LEN/2  ){
				S.theta = (PI - S.theta);
				S.set_x(-BOARD_LEN/2 + S.get_rad());
			}
			if (  S.get_y() + S.get_rad() >= BOARD_LEN/2  ){
				S.theta = (2*PI - S.theta);
				S.set_y(BOARD_LEN/2 - S.get_rad() );
			}
			if (  S.get_y() - S.get_rad() <= -BOARD_LEN/2 ){
				S.theta = (2*PI - S.theta);
				S.set_y(-BOARD_LEN/2 + S.get_rad() );
			}
			for ( int i = 0 ; i < 7 ; i ++ ){

				if ( C[i].visible == 1 ){
					if ( C[i].get_x() + C[i].get_rad() >= BOARD_LEN/2 ){
						C[i].theta = (PI - C[i].theta);
						C[i].set_x(BOARD_LEN/2 - C[i].get_rad());
					}
					if ( C[i].get_x() - C[i].get_rad() <= -BOARD_LEN/2  ){
						C[i].theta = (PI - C[i].theta);
						C[i].set_x(-BOARD_LEN/2 + C[i].get_rad());
					}
					if (  C[i].get_y() + C[i].get_rad() >= BOARD_LEN/2  ){
						C[i].theta = (2*PI - C[i].theta);
						C[i].set_y(BOARD_LEN/2 - C[i].get_rad() );
					}
					if (  C[i].get_y() - C[i].get_rad() <= -BOARD_LEN/2 ){
						C[i].theta = (2*PI - C[i].theta);
						C[i].set_y(-BOARD_LEN/2 + C[i].get_rad() );
					}
				}

			}

		}	
		float globaltolocal(float theta,float alpha){

			if ( (theta >= 0 && theta <= alpha) || ((2*PI - theta >= 0) && (2*PI - theta <= alpha)) ){
				if ( alpha-theta <= 2*PI ) return alpha-theta;
				else return 2*PI -alpha+theta;
			}
			else {
				if ( alpha+theta <= 2*PI ) return alpha+theta;
				else return 2*PI -alpha-theta;
			}

		}
		float localtoglobal(float theta, float alpha){
			if ( (theta >= 0 && theta <= alpha) || ((2*PI - theta >= 0) && (2*PI - theta <= alpha)) ){
				if ( alpha-theta <= 2*PI ) return alpha-theta;
				else return 2*PI -alpha+theta;
			}
			else 
				if ( theta-alpha <= 2*PI ) return theta-alpha;
				else return 2*PI +alpha-theta;


		}
		void checkCollision(){
			for (int i = 0 ; i < 7 ; i ++ ){
				float sep = sqrt(pow(C[i].get_x() - S.get_x(),2) + pow(S.get_y()-C[i].get_y(),2));
				if (C[i].visible && sep <= C[i].get_rad() + S.get_rad() + EPSILON ){
					float temp = C[i].get_speed();
					C[i].set_speed(S.get_speed()*4/3);
					S.set_speed(temp*3/4);
					temp = C[i].theta;
					C[i].theta = S.theta;
					S.theta = temp;

				}

			}
			for(int j=0;j < 7 ; j++)
			{
				for(int k=j+1;k < 7 ; k++)
				{
						float sep = sqrt(pow(C[j].get_x() - C[k].get_x(),2) + pow(C[j].get_y()-C[k].get_y(),2));
						if (sep <= C[j].get_rad() + C[k].get_rad() + EPSILON ){
							float temp = C[j].get_speed();
							
							C[j].set_speed(C[k].get_speed());
							
							C[k].set_speed(temp);
							
							temp = C[j].theta;
							
							C[j].theta = C[k].theta;
							
							C[k].theta = temp;


						}

				}
			}


		}
		void checkStrikerCollisionWithCoin(){

			for (int i = 0 ; i < 7 ; i ++ ){
				float sep = sqrt(pow(C[i].get_x() - S.get_x(),2) + pow(S.get_y()-C[i].get_y(),2));
				if (sep <= C[i].get_rad() + S.get_rad() )
				{
					float alpha = abs(asin(((S.get_y() - C[i].get_y()))/sep));
					float theta1 =  globaltolocal(C[i].theta,alpha);
					float theta2 = globaltolocal(S.theta,alpha);

					float a = C[i].get_speed()*cos(theta1) + 2*S.get_speed()*cos(theta2);

					float d = pow(C[i].get_speed()*cos(theta1),2) + 2*pow(S.get_speed(),2);

					float e = pow(S.get_speed()*sin(theta2),2);

					float v2 = sqrt(abs(pow(2,3/2)*sqrt(abs( -(a*a)*(a*a - 3*d + 6*e )))+ a*a + 3*d + 12*e  )  )/(3*sqrt(2));

					float theta1f=0,theta2f=0;
					if ( v2 == S.get_speed() ){
						cout <<"v2 0"<<endl;
						v2 = sqrt(abs(-(pow(2,3/2)*sqrt(abs( -(a*a)*(a*a - 3*d + 6*e ))))+ a*a + 3*d + 12*e  )  )/(3*sqrt(2));
						if ( v2 == S.get_speed ())
							cout<<"v3 0"<<endl;
					}
					cout << pow(C[i].get_speed(),2 ) <<" : "<<pow(S.get_speed(),2) <<" : "<< pow(v2,2) << endl;
					float v1 = sqrt(abs(pow(C[i].get_speed(),2 ) + 2*pow(S.get_speed(),2) - 2*pow(v2,2))  );
					//if ( v1 == 0 )
					if ( v1 != 0 )
						theta1f =asin((C[i].get_speed()*sin(C[i].theta)/v1));
					else
						theta1f = 0;
					if ( v2 != 0){
						theta2f = asin((S.get_speed()*sin(S.theta))/v2);
						if (!(theta2f == theta2f)){
							cout << "Nan! "<<S.get_speed() << " theta "<<S.theta<<" v2 "<<v2;

						}
					}
					else
						theta2f = 0;
					theta2f = localtoglobal(theta2f,alpha);
					theta1f = localtoglobal(theta1f,alpha);

					S.set_speed(v2);
					C[i].set_speed(v1);
					S.theta = theta2f;
					C[i].theta = theta1f;
					cout << "S speed : " << S.get_speed() << " S alpha : " << S.theta << " C[i] speed : "<<C[i].get_speed() << " C[i] theta : "<< C[i].theta <<"\n";
				} 


			} 
		}
		void checkCoinInPocket()
		{
			//check if either 
			// 1. coin falls in the pocket -> increment/decrement score and coins
			// 2. striker falls in the pocket -> put the striker back again
			
			for( int i=0; i<7; i++)
			{
				for(int j=0; j<4; j++){
					if(C[i].visible && distance(C[i].get_x(),C[i].get_y(),Pocket[j].get_x(),Pocket[j].get_y()) < abs(C[i].get_rad() - Pocket[j].get_rad()))
					{
						C[i].set_speed(0);
						C[i].theta = 0;
						C[i].visible = 0;
						if( i == 6)
						{
							coins_q = 1;
							score += 50;
						}

						else if(i % 2)
						{ 
							system("canberra-gtk-play -f correct_coin.ogg");
							coins_w += 1;
							score += 10;
						}
						else
						{
							system("canberra-gtk-play -f wrong_coin.ogg");
							coins_b += 1;
							score -= 5;
						}
						C[i].set_x(collected_x);
						C[i].set_y(collected_y);
						collected_x += 0.5f;
					}

				}
			}
				//Check striker in pocket
			for(int j=0; j<4; j++){
				if(distance(S.get_x(),S.get_y(),Pocket[j].get_x(),Pocket[j].get_y()) < abs(S.get_rad() - Pocket[j].get_rad()))
				{
					S.set_speed(0);
					S.theta = 0;
					system("canberra-gtk-play -f wrong_coin.ogg");
					score -= 5;
					S.set_x(0);
					S.set_y(-1.7);

				}

			}

			//END the game if all the white coins are collected

			if(coins_w == 3)
			{
			glPushMatrix();
			glRasterPos2f(-5.0f,-2.0f);
			const char *text = "Game over !";
			int length = strlen(text);
			for(int i=0;i<length;i++)
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,(int)text[i]);
			glPopMatrix();
			game_over = 1;
			
//			sleep(10);
//			exit(0);
			}


		}
		
		void decrement_time(){
			gettimeofday(&tim, NULL);  
			t2=tim.tv_sec+(tim.tv_usec/1000000.0);  
			if((t2-t1)>10)
			{	if(score > 0)
					score= score-1;
				else
					score= 0;
				t1=tim.tv_sec+(tim.tv_usec/1000000.0);
			}
		}
		void show_rules()
		{
			glPushMatrix();
			glRasterPos2f(2.7f,2.40f);
			const char *text = "       Rules of the game";
			int length = strlen(text);
			for(int i=0;i<length;i++)
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,(int)text[i]);
			glPopMatrix();
			
			glColor3f(0.0f,0.0f,1.0f);
			glPushMatrix();
			glRasterPos2f(2.7f,1.9f);
			const char *text1 = "*Collect all white coins +10 ;)";
			length = strlen(text1);
			for(int i=0;i<length;i++)
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,(int)text1[i]);
			glPopMatrix();

			glPushMatrix();
			glRasterPos2f(2.7f,1.4f);
			const char *text2 = "*Black coin will cost you -5";
			length = strlen(text2);
			for(int i=0;i<length;i++)
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,(int)text2[i]);
			glPopMatrix();

			glPushMatrix();
			glRasterPos2f(2.7f,0.9f);
			const char *text3 = "*Queen increases score by +50";
			length = strlen(text3);
			for(int i=0;i<length;i++)
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,(int)text3[i]);
			glPopMatrix();

			glPushMatrix();
			glRasterPos2f(2.7f,0.4f);
			const char *text4 = "*Each 10th sec & foul costs you -1";
			length = strlen(text4);
			for(int i=0;i<length;i++)
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,(int)text4[i]);
			glPopMatrix();

		}

		void draw_scoreboard(){

			glPushMatrix();
			glRasterPos2f(-5.5f,0.0f);
			glColor3f(1.0f,0.0f,0.0f);
			const char *text="Score Board : ";

			int length = strlen(text),i,j;
			for(int i=0;i<length;i++)
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,(int)text[i]);
			char integer[15];
			for (i=0, j =score;j!=0;j/=10,i++)
			{
				integer[i] = (j%10) + '0';
			}
			if(score<0)
				integer[i++]='-';
			integer[i]='\0';
			const char *text1=integer;
			glColor3f(0.0f,1.0f,0.0f);
			for(i = strlen(text1); i>=0; i--)
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,(int)text1[i]);
			glPopMatrix();

			glPushMatrix();
			glColor3f(1.0f,0.0f,0.0f);
			const char *text2="White Coins Scored : ";
			length = strlen(text2);
			glRasterPos2f(-5.5f,-0.5f);
			for(i=0;i<length;i++)
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,(int)text2[i]);


			char integer1[15]="";
			if(coins_w == 0)
				strcpy(integer1,"0\0");
			else{
				for (i=0, j =coins_w;j!=0;j/=10,i++)
				{
					integer1[i] = (j%10) + '0';
				}

				integer1[i]='\0';
			}
			const char *text3=integer1;
			glColor3f(0.0f,1.0f,0.0f);

			for(i = strlen(text3); i>=0; i--)
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,(int)text3[i]);
			glPopMatrix();
			glPushMatrix();
			const char *text4="Black Coins Scored : ";
			length = strlen(text4);
			glRasterPos2f(-5.5f,-1.0f);
			glColor3f(1.0f,0.0f,0.0f);
			for(i=0;i<length;i++)
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,(int)text4[i]);

			char integer2[15] = "";
			if(coins_b == 0)
				strcpy(integer2,"0\0");
			else{
				for (i=0, j =coins_b;j!=0;j/=10,i++)
				{
					integer2[i] = (j%10) + '0';
				}

				integer2[i]='\0';
			}
			const char *text5=integer2;
			glColor3f(0.0f,1.0f,0.0f);

			for(i = strlen(text5); i>=0; i--)
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,(int)text5[i]);

			glPopMatrix();
		}
		void draw() {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear window 
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glPushMatrix(); 

			decrement_time();

			drawBox();
			show_rules();
			draw_scoreboard();

			checkCoinInPocket();
			//draw rectangle patterns
			glPushMatrix();
			glColor3f(0.0f, 0.0f, 1.0f);
			glRectf(-1.6f,1.1f,-1.8f, -1.1f);
			glPopMatrix();

			glPushMatrix();
			glColor3f(0.0f,0.0f,1.0f);
			glRectf(1.6f,-1.1f,1.8f,1.1f);
			glPopMatrix();

			glPushMatrix();
			glColor3f(0.0f,0.0f,1.0f);
			glRectf(-1.3f,1.85f,1.3f,1.65f);
			glPopMatrix();

			glPushMatrix();
			glColor3f(0.0f,0.0f,1.0f);
			glRectf(-1.3f,-1.85f,1.3f,-1.65f);
			glPopMatrix();
	
			
			//draw circle patterns
			for ( int i = 0 ; i < 4; i ++ ){
				innerCircle(sideC[i].get_x(),sideC[i].get_y());
			}
			//draw pockets
			for ( int i  = 0 ; i < 4 ; i++ ){
				drawPocket(Pocket[i].get_x(),Pocket[i].get_y());
			}

			// draw striker 
			glPushMatrix();
			//position striker
			if(!move_flag){

				S.rotate(S.get_rotate_flag());
				glTranslatef(0.0f,0.0f,0.0f);
				S.check_keys();
			}
			//move going on
			else{
				//checking if move is complete
				int move_complete_f = 1;
				for ( int i = 0 ; i < 7; i ++ ){
					if ( C[i].get_speed() > 0 ) {
						move_complete_f = 0;
						break;
					}
				}

				if (move_complete_f &&  S.get_speed() <= 0 ){
					move_flag = 0;

					S.theta = PI/2;
					S.set_x(0.0f);S.set_rad(0.15f);S.set_y(-1.7f);
				}

				checkCollisionWithBoard();
				checkCollision();

				S.set_x(S.get_x() + S.get_speed()*cos(S.theta));
				S.set_y(S.get_y() + S.get_speed()*sin(S.theta));

				for ( int i = 0 ; i < 7; i ++ ){
					C[i].set_x(C[i].get_x() + C[i].get_speed()*cos(C[i].theta));
					C[i].set_y(C[i].get_y() + C[i].get_speed()*sin(C[i].theta));	
					if ( C[i].get_speed() > 0.0f)
						C[i].set_speed(C[i].get_speed() - fric);
					if ( C[i].get_speed() < 0.0f)
						C[i].set_speed(0.0f);
				}

				S.set_speed(S.get_speed() - fric);
				if ( S.get_speed() < 0 ){
					S.set_speed(0.0f);
				}
			}
			glTranslatef(S.get_x(),S.get_y(),0.0f);   
			glColor3f(1.0f,0.0f,1.0f); 
			S.drawCoin(STRIKER_RADIUS);
			glPopMatrix();
			drawCoins();
			glPopMatrix();
		}

};
Board B;
void initRendering();
void handleResize(int w, int h);
void handleKeypress1(unsigned char key, int x, int y);
void handleKeypress2(int key, int x, int y);
void handleMouseclick(int button, int state, int x, int y);
void mouse(int,int,int,int);
void drawscene(){
	B.draw();
	glutSwapBuffers();
}
void initRendering() {

	glEnable(GL_DEPTH_TEST);        // Enable objects to be drawn ahead/behind one another
	glEnable(GL_COLOR_MATERIAL);    // Enable coloring
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);   // Setting a background color
}

// Function called when the window is resized

void handleKeypress1(unsigned char key, int x, int y) {

	if (key == 27) {
		exit(0);     // escape key is pressed
	}
	if(!game_over){
	if ( key == 'a')
		B.S.set_rotate_flag(1);
	if ( key == 'c')
		B.S.set_rotate_flag(-1);
	if ( key == 32){
		B.set_move_flag(1);
	}
	}

}
void handleResize(int w, int h) {

	glViewport(0, 0, w, h); 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (float)w / (float)h, 0.1f, 200.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}  
int main(int argc, char **argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB );
	int w = glutGet(GLUT_SCREEN_WIDTH);
	int h = glutGet(GLUT_SCREEN_HEIGHT);
	int windowWidth = w * 2 / 3;
	int windowHeight = h * 2 / 3;

	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition((w - windowWidth) / 2, (h - windowHeight) / 2);
	glutCreateWindow("Carom");
	initRendering();
	glutDisplayFunc(drawscene);
	glutIdleFunc(drawscene);
	glutKeyboardFunc(handleKeypress1);
	glutSpecialFunc(handleKeypress2);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);
	glutPassiveMotionFunc(mouseMotion);
	glutReshapeFunc(handleResize);
	glutMainLoop();
	return 0;
}
void leftMouseMove(float world_x,float world_y){
	B.S.set_x(world_x);
	B.S.set_y(-1.7);
	B.S.drawCoin(B.S.get_rad());

}
void rightMouseMove(float world_x,float world_y){
	float relY = world_y-B.S.get_y();
	float relX = world_x - B.S.get_x();
	float theta = atan(relY/relX);
	float d = distance(world_x,world_y,B.S.get_x(),B.S.get_y());
	B.S.set_speed((d/BOARD_LEN)*MAX_SPEED);
	if ( (relY >= 0 && relX >= 0)  || (relY <= 0 && relX >= 0) ){
		B.S.theta = theta;
	}
	else if ( relX <= 0 && relY >= 0){
		B.S.theta = PI + theta;
	}
	else if ( (relY <= 0 && relX <= 0)){
		B.S.theta = theta - PI;
	}	    	

}

void mouse(int button, int state, int x, int y) {
	if ( !B.move_flag ){
		GLdouble objx, objy, objz;
		GLint viewport[16];
		GLdouble modelMatrix[16];
		GLdouble projMatrix[16];

		glGetIntegerv(GL_VIEWPORT, viewport);
		glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
		glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
		float viewportHeight = viewport[3]; // = 600.0f
		y = viewportHeight - y;

		gluUnProject((GLdouble)x, (GLdouble)y, 0, modelMatrix, projMatrix, viewport, &objx, &objy, &objz);
		float world_x = objx*80;
		float world_y = objy*80;
		if ( world_x >= 1.83f)
			world_x = 1.83f;
		if (world_x <= -1.83f)
			world_x = -1.83f;
		if(mouse_flag || (button == GLUT_LEFT_BUTTON &&state == GLUT_DOWN)) {
			left_mouse = 1;
			mouse_flag = 1;
			leftMouseMove(world_x,world_y);

		}
		if ( left_mouse && state == GLUT_UP){
			left_mouse = 0;
			mouse_flag = 0;
		}
		if ( button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
			right_mouse = 1;
			rightMouseMove(world_x,world_y);

		}
		if (right_mouse && button == GLUT_RIGHT_BUTTON && state == GLUT_UP){
			right_mouse = 0;
			B.set_move_flag(1);
		}
	}
}
void mouseMotion(int x,int y){
	if ( !B.move_flag ){
		GLdouble objx, objy, objz;
		GLint viewport[16];
		GLdouble modelMatrix[16];
		GLdouble projMatrix[16];

		glGetIntegerv(GL_VIEWPORT, viewport);
		glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
		glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
		float viewportHeight = viewport[3]; // = 600.0f
		y = viewportHeight - y;

		gluUnProject((GLdouble)x, (GLdouble)y, 0, modelMatrix, projMatrix, viewport, &objx, &objy, &objz);
		float world_x = objx*80;
		float world_y = objy*80;

		if ( left_mouse ){
			leftMouseMove(world_x,world_y);
		}
		else if ( right_mouse ){
			rightMouseMove(world_x,world_y);
		}
	}
}

void handleKeypress2(int key, int x, int y) {
	if (key == GLUT_KEY_LEFT)
		B.S.set_right_flag( -1);
	if (key == GLUT_KEY_RIGHT)
		B.S.set_right_flag(1);
	if (key == GLUT_KEY_UP)
		B.S.set_speed_flag(1);
	if (key == GLUT_KEY_DOWN)
		B.S.set_speed_flag(-1);

}



