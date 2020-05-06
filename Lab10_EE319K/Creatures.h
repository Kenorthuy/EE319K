#include <stdint.h>

void spawnLander(uint8_t);
void landerMove(uint8_t);
void humanMove(void);
void spawnShot(uint8_t, uint8_t, uint8_t);
void spawnPlayerShot(void);
void moveShot(uint8_t);
void movePlayerShot(uint8_t);
void checkHit(uint8_t);
void checkPlayerHit(uint8_t);
void enemyMove(void);

extern uint32_t Score;
extern uint8_t spawnMutants;

typedef struct { 
  uint8_t type;				//the definitions above explain how types work. this informs the system which sprite to print and which behavior to adapt
  int16_t xpos;
	int16_t ypos;
	uint8_t width;
	uint8_t height;
	uint8_t dead;				//if an enemy is dead or if a lander leaves with a human, this will allow them to be overwritten
											//0 = alive; 1 = just died; 2 = died a while back (important for sprite display and respawn information
	uint8_t deadimpact;	//property for humans which die if they fall from too high up. 
											//0 = wont die when hitting the ground; 1 = will die when hitting the ground
	uint8_t pickedup;		//property unique to humans so that they can decrement their y when being carried
											//0 = not picked up; 1 = picked up
	uint8_t carrying;		//property unique to landers for moving humans out
											//0 = not carrying the human; 1 = carrying the human
} creature_t;

typedef struct{
	uint8_t xpos, ypos;
	uint8_t newxpos, newypos;
	uint8_t xvel;
	uint8_t width;
	uint8_t height;
	uint8_t lives;
	uint8_t movingFlag;
	uint8_t facingLeft; //0 is right, 1 is left
	uint8_t thrust;
} player_t;

typedef struct{
	int16_t xpos;
	int16_t ypos;
	uint8_t xvel;
	uint8_t yvel;
	uint8_t override;
} ball_t;

typedef struct{
	int16_t xpos;
	int16_t ypos;
	uint8_t override;	//1 means it can be overwritten, 0 means it still exists
} laser_t;

extern uint8_t gameDone;

extern uint8_t enemySize;
extern creature_t enemies[5];

extern uint8_t humanSize;
extern creature_t humans[1];

extern player_t player[1];

extern uint8_t shotSize;
extern ball_t shots[5];

extern uint8_t laserSize;
extern ball_t lasers[10];
