enum IDS{PLAYER,BULLET,ENEMY};

struct SpaceShip
{
	int id;
	int y;
	int x;
	int lives;
	int speed1;
	int speed2;
	int boundx;
	int boundy;
	int score;
};
struct Bullet
{
	int id;
	int x;
	int y;
	bool life1;
	bool life2;
	int speed;
};
struct Enemy
{
	int id;
	int x;
	int y;
	bool life;
	int boundx;
	int boundy;
	int speed;
};



