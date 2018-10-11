#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include "objects.h"
#include <iostream>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>
#include <math.h>

using namespace std;

const int width = 1200;
const int height = 600;

const int NUMBEROFBULLETS = 5;
const int NUMBEROFENEMIES = 10; 

int enemySpeed = 5;
int numberOfBullets = NUMBEROFBULLETS;
int numberOfEnemies = NUMBEROFENEMIES;
int pointsPerKill = 10;
int bonusScore = 50;

int bulletSize = 10;
int enemySize = 10;
int r = 150;
int g= 150;
int b = 150;
int c1 = 0;
int c2 = 0;
int c3 = 0;

int bonusCount = 0;
int bonus[] = {10,25,50,100,150,200,300,500};
long int score = 0;
enum KEYS{UP,DOWN,LEFT,RIGHT,SPACE};
bool keys[] = {false,false,false,false,false};
bool r1 = true;
bool r2 = true;

void InitShip(SpaceShip &);
void DrawShip(SpaceShip &);
void effects(int);
void MoveShipUp(SpaceShip &);
void MoveShipDown(SpaceShip &);
void MoveShipLeft(SpaceShip &);
void MoveShipRight(SpaceShip &);
void CollideShip(SpaceShip &, Enemy [], int);

void InitBullet(Bullet bullets[], int size);
void DrawBullet1(Bullet bullets[], int size);
void DrawBullet2(Bullet bullets[], int size);
void FireBullet(Bullet bullets[], int size, SpaceShip &);
void UpdateBullet(Bullet bullets[], int size);
void CollideBullet(Bullet [], int, Enemy [], int);
	
void InitEnemy(Enemy [],int);
void DrawEnemy(Enemy [],int,int,int,int);
void CallEnemy(Enemy [],int);
void UpdateEnemy(Enemy [],int);
void CollideEnemy(Enemy [], int,SpaceShip &,bool &, bool &); 

int max(int a, int b);

int main(void)
{
	bool done = false;
	bool redraw = true;
	bool gameOver = false;
	int check = 0;
	int highScore = 0;
	
	const int FPS = 60;
	const int efSpeed = 20;

	SpaceShip ship;
	Bullet bullets[NUMBEROFBULLETS+5];
	Enemy enemies[NUMBEROFENEMIES+5];


	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;

	if(!al_init())
		return -1;

	display = al_create_display(1200,600);

	if(!display)
		return -1;

	al_init_primitives_addon();
	al_install_keyboard();
	al_install_mouse();
	al_init_font_addon();
	al_init_ttf_addon();

	ALLEGRO_FONT *font30 = al_load_font("Raleway-Black.ttf", 30, 0);

	
	srand(time(NULL));
	InitShip(ship);
	InitBullet(bullets, numberOfBullets);
	InitEnemy(enemies, numberOfEnemies);

	event_queue = al_create_event_queue();
	timer = al_create_timer(1.0/FPS);

	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_mouse_event_source());

	al_start_timer(timer);

	while(!done)
	{
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);
		
		if(ev.type == ALLEGRO_EVENT_TIMER)
		{
			if(!ship.lives)
			gameOver = true;

			ship.score = score;
			redraw = true;

			al_clear_to_color(al_map_rgb(0,0,0));

			if(keys[UP])
				MoveShipUp(ship);
			else if(keys[DOWN])
				MoveShipDown(ship);
			else if(keys[LEFT])
				MoveShipLeft(ship);
			else if(keys[RIGHT])
					MoveShipRight(ship);

			if(!gameOver)
			{
				UpdateBullet(bullets, numberOfBullets);
				CallEnemy(enemies, numberOfEnemies);
				UpdateEnemy(enemies, numberOfEnemies);
				CollideBullet(bullets, numberOfBullets, enemies, numberOfEnemies);
				CollideEnemy(enemies, numberOfEnemies, ship, r1, r2);
				CollideShip(ship, enemies, numberOfEnemies);
			}
		}
	
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			done = true;
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			switch(ev.keyboard.keycode)
			{
				case ALLEGRO_KEY_UP: keys[UP] = true;
				break;
				case ALLEGRO_KEY_DOWN: keys[DOWN] = true;
				break;
				case ALLEGRO_KEY_LEFT: keys[LEFT] = true;
				break;
				case ALLEGRO_KEY_RIGHT: keys[RIGHT] = true;
				break;
				case ALLEGRO_KEY_SPACE: keys[SPACE] = true;
					FireBullet(bullets, numberOfBullets, ship);
				break;
				case ALLEGRO_KEY_R: if(gameOver)
				{
					gameOver = false;

					if(highScore>score)
					score = 0;
					else
					highScore = score;

					ship.score = 0;
					score = 0;
					bonusCount = 0;
					check = 0;
					srand(time(NULL));
					InitShip(ship);
					InitBullet(bullets, NUMBEROFBULLETS);
					InitEnemy(enemies, NUMBEROFENEMIES);
					numberOfBullets = NUMBEROFBULLETS;
					numberOfEnemies = NUMBEROFENEMIES;
					pointsPerKill = 10;
					bonusScore = 50;
					enemySpeed = 5;
					r1 = true;
					r2 = true;
				}
				break;
				case ALLEGRO_KEY_ESCAPE: done = true;
				break;
			}
		}
		else if(ev.type == ALLEGRO_EVENT_KEY_UP)
		{
			switch(ev.keyboard.keycode)
			{
				case ALLEGRO_KEY_UP: keys[UP] = false;
				break;
				case ALLEGRO_KEY_DOWN: keys[DOWN] = false;
				break;
				case ALLEGRO_KEY_LEFT: keys[LEFT] = false;
				break;
				case ALLEGRO_KEY_RIGHT: keys[RIGHT] = false;
				break;
				case ALLEGRO_KEY_SPACE: keys[SPACE] = false;
				break;
			}
		}

		else if(ev.type == ALLEGRO_EVENT_MOUSE_AXES)
		{
			ship.x = ev.mouse.x;
			ship.y = ev.mouse.y;
			if(ev.mouse.x > width/3)
			ship.x = width/3;
			if(ev.mouse.x < width/3 + 20)
			al_hide_mouse_cursor(display);
			else
			al_show_mouse_cursor(display);
			
		}

		else if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
			if(ev.mouse.button & 1)
			FireBullet(bullets, numberOfBullets, ship);

		if(!gameOver)
		if(bonusCount > bonus[check])
		{
			ship.lives++;
			if(!r1)
			r1 = true;
			else if(!r2)
			r2 = true;
			if(!(check > 7)) 
			check++;
			numberOfEnemies++;
			numberOfBullets++;
			pointsPerKill += 10;
			bonusScore += 50;
			enemySpeed++;

		}
		if(!gameOver)
		switch(check)
		{
			case 0:c1 = 0;c2 = 255;c3 = 0;break;
			case 1:c1 = 0;c2 = 0;c3 = 255;break;
			case 2:c1 = 255;c2 = 255;c3 = 255;break;
			case 3:c1 = 255;c2 = 0;c3 = 0;break;
			case 4:c1 = 255;c2 = 255;c3 = 0;break;
			case 5:c1 = 160;c2 = 40;c3 = 50;break;
			case 6:c1 = 10;c2 = 10;c3 = 120;break;

		}

		if(redraw && al_is_event_queue_empty(event_queue))
		{
			redraw = false;
			if(!gameOver)
			{

				effects(efSpeed);
				DrawShip(ship);
				DrawEnemy(enemies, numberOfEnemies,c1,c2,c3);
				DrawBullet1(bullets, numberOfBullets);
				DrawBullet2(bullets, numberOfBullets);
				al_draw_textf(font30, al_map_rgba(255,255,0,0),width/2,10,ALLEGRO_ALIGN_CENTRE, "SCORE:  %i", ship.score);
			}
			else
			{
				al_clear_to_color(al_map_rgb(0,0,0));
				al_draw_textf(font30,al_map_rgba(rand()%255,rand()%60*3,rand()%60*4,rand()%60*4),width/2,height/3,ALLEGRO_ALIGN_CENTER," SCORE  :  %i", ship.score);
				al_draw_textf(font30,al_map_rgba(rand()%80*3,rand()%80*3,rand()%80*3,rand()%80*3),width/2,height/2,ALLEGRO_ALIGN_CENTER," |* GAME * OVER *| !");
				al_draw_textf(font30,al_map_rgba(rand()%60*4,rand()%60*4,rand()%60*4,rand()%60*4),width/2,height*2/3,ALLEGRO_ALIGN_CENTER," Press ' R ' to Restart");
				al_draw_textf(font30,al_map_rgba(rand()%60*4,rand()%60*4,rand()%60*4,rand()%60*4),width/2,height-30,ALLEGRO_ALIGN_CENTER," HighScore  :  %i", max(highScore,score));

			}
			

			al_flip_display();
		
		}
	}

	al_destroy_event_queue(event_queue);
	al_destroy_display(display);

	
	return 0;
}

int max(int a, int b)
{
	if (a > b)
		return a;
	else
		return b;
}

void InitShip(SpaceShip &ship)
{
	ship.id = PLAYER;
	ship.x = 40;
	ship.y = height/2;
	ship.lives = 3;
	ship.speed1 = 10;
	ship.speed2 = 30;
	ship.score = 0;
	ship.boundx = 10;
	ship.boundy = 35;
	return;
}


void DrawShip(SpaceShip &ship)
{
	r = (rand()%1000)/4;
	g = (rand()%1000)/4;
	b = (rand()%1000)/4;
		
	al_draw_filled_rectangle(ship.x,ship.y+40,ship.x+30,ship.y-40,al_map_rgb(r,g,b));
	al_draw_filled_rectangle(ship.x+5,ship.y+35,ship.x+25,ship.y-35,al_map_rgb(r/2,g/2,b/2));
	al_draw_filled_circle(ship.x+15,ship.y+15,5,al_map_rgb(255,0,0));
	al_draw_filled_circle(ship.x+15,ship.y-15,5,al_map_rgb(255,0,0));
	
	al_draw_filled_rectangle(ship.x+20,ship.y-20,ship.x+30,ship.y-10,al_map_rgb(0,0,255));
	al_draw_filled_rectangle(ship.x+20,ship.y+20,ship.x+30,ship.y+10,al_map_rgb(0,0,255));
	al_draw_line(ship.x+15,ship.y-15,ship.x+30,ship.y-15,al_map_rgb(255,255,0),2);
	al_draw_line(ship.x+15,ship.y+15,ship.x+30,ship.y+15,al_map_rgb(255,255,0),2);

	al_draw_filled_triangle(ship.x+25,ship.y+10,ship.x+25,ship.y+20,ship.x+35,ship.y+15,al_map_rgb(255,0,0));
	al_draw_filled_triangle(ship.x+25,ship.y-10,ship.x+25,ship.y-20,ship.x+35,ship.y-15,al_map_rgb(255,0,0));

	al_draw_line(ship.x,ship.y,20,ship.y,al_map_rgba(255,255,255,10),2);
	al_draw_line(20,0,20,height,al_map_rgba(r*3/4,g/3/4,b*3/4,85),5);
	al_draw_filled_circle(20,ship.y,10,al_map_rgba(r*2/3,g*2/3,b*2/3,90));
	
	return;
}

void MoveShipUp(SpaceShip &ship)
{
	ship.y -= ship.speed1;
	if(ship.y < 0)
		ship.y = 0;
}

void MoveShipDown(SpaceShip &ship)
{
	ship.y += ship.speed2;
	if(ship.y > height)
		ship.y = height;
}

void MoveShipLeft(SpaceShip &ship)
{
	ship.x -= ship.speed2;
	if(ship.x < 0)
		ship.x = 0;
}

void MoveShipRight(SpaceShip &ship)
{
	ship.x += ship.speed1;
	if(ship.x > width/3)
		ship.x = width/3;
}


void CollideShip(SpaceShip &ship, Enemy enemies[], int size)
{
	for(int i=0; i<size; i++)
	if(enemies[i].life)
	if((enemies[i].x - enemySize) < (ship.x + ship.boundx)
	&& (enemies[i].x + enemySize) > (ship.x - ship.boundx)
	&& (enemies[i].y - enemySize) < (ship.y + ship.boundy)
	&& (enemies[i].y + enemySize) > (ship.y - ship.boundy))
	{
		enemies[i].life = false;
		score += (pointsPerKill + bonusScore);
		bonusCount++;
	}
}

void effects(int efSpeed)
{
	static int ed = 0;
	if(ed > height)
		ed = 0;
	static int eu = 0;
	if(eu < 0)
		eu = height;
	r = rand()%255;
	g = rand()%255;
	b = rand()%255;
	if(r1)
	al_draw_filled_rectangle(10,ed,30,ed + 50,al_map_rgb(r,g,b));
	if(r2)
	al_draw_filled_rectangle(10,eu,30,eu - 50,al_map_rgb(r/2,g/2,b/2));
	ed += efSpeed;
	eu -= efSpeed;

}

void InitBullet(Bullet bullets[], int size)
{
	for(int i=0;i<size;i++)
	{
		bullets[i].id = BULLET;
		bullets[i].life1 = false;
		bullets[i].life2 = false;
		bullets[i].speed = 15;
		
	}
}
void DrawBullet1(Bullet bullets[], int size)
{
	r = (rand()%255);
	g = (rand()%255);
	b = (rand()%255);
	for(int i = 0;i < size; i++)
	{
		if(bullets[i].life1)
		{
			al_draw_filled_circle(bullets[i].x, bullets[i].y, bulletSize, al_map_rgb(r,g,b));
		}
	}
}
void DrawBullet2(Bullet bullets[], int size)
{
	r = (rand()%255);
	g = (rand()%255);
	b = (rand()%255);
	for(int i = 0;i < size; i++)
	{
		if(bullets[i].life2)
		{
			al_draw_filled_circle(bullets[i].x, bullets[i].y + 30, bulletSize, al_map_rgb(r,g,b));
		}
	}
}
void FireBullet(Bullet bullets[], int size,SpaceShip &ship)
{
	for(int i = 0;i < size;i++)
	{
		if(!bullets[i].life1 && !bullets[i].life2)
		{
			bullets[i].x = ship.x + 25;
			bullets[i].y = ship.y - 15;
			bullets[i].life1 = true;
			bullets[i].life2 = true;
			break;
		}
	}
}
void UpdateBullet(Bullet bullets[], int size)
{
	for(int i=0;i<size;i++)
	{
		if(bullets[i].life1)
		{
			bullets[i].x += bullets[i].speed;
			if(bullets[i].x > width)
				bullets[i].life1 = false;
		}
		if(bullets[i].life2)
		{
			bullets[i].x += bullets[i].speed;
			if(bullets[i].x > width)
				bullets[i].life2 = false;
		}
	}
}

void CollideBullet(Bullet bullets[], int bSize, Enemy enemies[], int cSize)
{
	for(int i=0; i<bSize; i++)
	if(bullets[i].life1)
	for(int j=0; j<cSize; j++)
	if(enemies[j].life)
	{
		if((bullets[i].x + bulletSize/2) > (enemies[j].x - enemies[j].boundx)
		&& (bullets[i].x - bulletSize/2) < (enemies[j].x + enemies[j].boundx)
		&& (bullets[i].y + bulletSize/2) > (enemies[j].y - enemies[j].boundy) 
		&& (bullets[i].y - bulletSize/2) < (enemies[j].y + enemies[j].boundy))
		{
			enemies[j].life = false;
			bullets[i].life1 = false;
			score += pointsPerKill;
		}
		else if((bullets[i].x + bulletSize/2) > (enemies[j].x - enemies[j].boundx)
		&& (bullets[i].x - bulletSize/2) < (enemies[j].x + enemies[j].boundx)
		&& (bullets[i].y+30 + bulletSize/2) > (enemies[j].y - enemies[j].boundy) 
		&& (bullets[i].y+30 - bulletSize/2) < (enemies[j].y + enemies[j].boundy))
		{
			enemies[j].life = false;
			bullets[i].life2 = false;
			score += pointsPerKill;
		}
	}
}

void InitEnemy(Enemy enemies[],int size)
{
	for(int i = 0;i< size; i++)
	{
		enemies[i].id = ENEMY;
		enemies[i].life = false;
		enemies[i].boundx = enemySize*3;
		enemies[i].boundy = enemySize*3;
		enemies[i].speed = enemySpeed;
	}
	return;
}
void DrawEnemy(Enemy enemies[],int size,int c1,int c2,int c3)
{
	r = rand() % 255;
	g = rand() % 255;
	b = rand() % 255;
	
	for(int i=0;i<size;i++)
		if(enemies[i].life)
		{
			al_draw_filled_circle(enemies[i].x, enemies[i].y, enemySize, al_map_rgba(c1,c2,c3,rand() % 250));
			al_draw_filled_circle(enemies[i].x, enemies[i].y, enemySize/2, al_map_rgba(r,g,b,r));
			al_draw_circle(enemies[i].x, enemies[i].y, enemySize*2, al_map_rgba(r,g,b,g),enemySize);
		}
			return;
}

void CallEnemy(Enemy enemies[],int size)
{
	for(int i = 0; i<size; i++)
	if(!enemies[i].life)
	if(!(rand() % 500))
	{
		enemies[i].life = true;
		enemies[i].speed = enemySpeed;
		enemies[i].x = width + enemySize;
		enemies[i].y = rand()%height;

		break;
	}
}

void UpdateEnemy(Enemy enemies[],int size)
{
	for(int i=0; i<size; i++)
		if(enemies[i].life)
	{
		enemies[i].x -= enemies[i].speed;
		if(enemies[i].x < 20)
			enemies[i].life = false;
	}
}

void CollideEnemy(Enemy enemies[], int size, SpaceShip &ship,bool &r1, bool &r2)
{
	for(int i=0; i<size; i++)
	if(enemies[i].life)
	if((enemies[i].x - enemies[i].boundx) < 25)
	{
		enemies[i].life = false;
		ship.lives--;
		if(!r1)
		r2 = false;
		else
		r1 = false;
		
	}
}
