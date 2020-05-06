// Creatures.c
// Runs on any computer
// Sound assets based off the original Space Invaders 
// Import these constants into your SpaceInvaders.c for sounds!
// Jonathan Valvano
// 1/17/2020
#include <stdint.h>
#include "Random.h"
#include "Timer2.h"
#include "SysTick.h"

#define human 0				//defining some values for ease
#define lander 1
#define mutant 2

#define humanh 12
#define humanw 7

#define landerh 12
#define landerw 13

#define mutanth 12
#define mutantw 13

#define playerRh 8
#define playerRw 15

#define screenH 128
#define screenW 128


uint32_t Score;

//this structure keeps track of all non-player creature properties. if a creature does not use a certain value, it wont be checked but its best to leave it at zero to avoid confusion
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
uint8_t fullEnemy = 0;

//this structure keeps track of player information
typedef struct{
	uint8_t xpos;
	uint8_t ypos;
	uint8_t newxpos;
	uint8_t newypos;
	uint8_t xvel;
	uint8_t width;
	uint8_t height;
	uint8_t lives;
	uint8_t movingFlag;
	uint8_t facingLeft; //0 is right, 1 is left
	uint8_t thrust;
} player_t;
uint8_t gameDone = 0;			//this flag stores whether the player is dead

//this structure keeps track of projectile information
typedef struct{
	int16_t xpos;
	int16_t ypos;
	uint8_t xvel;			//1 is left, 2 is right
	uint8_t yvel;			//1 is down, 2 is up
	uint8_t override;	//1 means it can be overwritten, 0 means it still exists
	uint8_t enemyOrPlayer; //1 means its player 0 means its enemy
} ball_t;
uint8_t fullShot = 0;
uint8_t fullLaser = 0;

player_t player[1] ={
	{10, 10, 0, 0, 5, playerRw, playerRh, 3, 0, 1, 0}
};

uint8_t enemySize = 3;
creature_t enemies[5] = {
	{lander, 20, 20, landerw, landerh, 0, 0, 0, 0},
	{lander, 80, 80, landerw, landerh, 0, 0, 0, 0},
	{lander, 110, 100, landerw, landerh, 0, 0, 0, 0},
	{lander, 60, 60, landerw, landerh, 0, 0, 0, 0},
	{lander, 20, 40, landerw, landerh, 0, 0, 0, 0}
};

uint8_t humanSize = 1;
creature_t humans[1] = {
	{human, 41, 110, humanw, humanh, 0, 0, 0, 0}
};

uint8_t shotSize = 5;
ball_t shots[5] = {												//a maximum of five balls on screen at once
	{0, 0, 0, 0, 1, 0},											//every value is initialized with 1 in the override field to show that the balls dont exist
	{0, 0, 0, 0, 1, 0},
	{0, 0, 0, 0, 1, 0},
	{0, 0, 0, 0, 1, 0},
	{0, 0, 0, 0, 1, 0}
};

uint8_t laserSize = 10;
ball_t lasers[10] = {											//a maximum of 10 player lasers on screen at once
	{0, 0, 0, 0, 1, 1},											//every value is initialized with 1 in the override field to show that the lasers dont exist
	{0, 0, 0, 0, 1, 1},
	{0, 0, 0, 0, 1, 1},
	{0, 0, 0, 0, 1, 1},
	{0, 0, 0, 0, 1, 1},
  {0, 0, 0, 0, 1, 1},
	{0, 0, 0, 0, 1, 1},
	{0, 0, 0, 0, 1, 1},
	{0, 0, 0, 0, 1, 1},
	{0, 0, 0, 0, 1, 1}
};

void spawnLander(void);
void landerMove(uint8_t);
void humanMove(void);
void spawnShot(uint8_t, uint8_t, uint8_t);
void spawnPlayerShot(void);
void moveShot(uint8_t);
void movePlayerShot(uint8_t);
void checkHit(uint8_t);
void checkPlayerHit(uint8_t);
void enemyMove(void);

uint8_t spawnMutants;

// *************** spawnLander ******************************************
// Adds a lander to the array of enemies to replace a "stale" dead enemy
// Input: None
// Output: None
void spawnLander() {
	if(fullEnemy == 0 && changeFrame) {										//changeFrame exists so that enemies are not constantly generated
		for(int i = 0 ; i < enemySize; i++) {								//index through the enemies array
			if(enemies[i].dead == 2) {												//if there exists an enemy that has been dead long enough for the sprites to be "old," theyre free to be replaced
				creature_t enemy = {lander, Random()%110+1, Random()%20+10, landerw, landerh, 0, 0, 0, 0};
				enemies[i] = enemy;
			}
		}
	}
}

// *************** spawnMutant ******************************************
// Adds a mutant to the array of enemies
// Input: None
// Output: None
void spawnMutant() {
	if(fullEnemy == 0 && changeFrame) {
		for(int i = 0 ; i < enemySize; i++) {
			if(enemies[i].dead == 2) {												//if there exists an enemy that has been dead long enough for the sprites to be "old," theyre free to be replaced
				creature_t enemy = {mutant, Random()%110+1, Random()%20+10, landerw, landerh, 0, 0, 0, 0};
				enemies[i] = enemy;
				//break;																					//break to not overwhelm the player with mutants unless theyre asking for it
			}
		}
	}
}

// *************** landerMove *******************************************
// If a human exists on the screen and isn't picked up, the lander should try to reach and pick it up in a predictable manner
// If a lander is carrying a human to the top of the screen, that should be their sole focus
// If another lander is carrying the human or if there is no human this round, landers should move away and attack from a distance
// Landers can fire in 8 directions depending on what is closest to the player
// Input: Index Position of the Enemy
// Output: None
void landerMove(uint8_t index) {
	if(humans[0].pickedup != 1 && humans[0].dead == 0) {																						//if the human can be picked up
		if(humans[0].xpos + humans[0].width/2 < enemies[index].xpos + enemies[index].width/2) {				//try to get the center of the lander to align with the center of the human
			(enemies[index].xpos)--;
		}
		if(humans[0].xpos + humans[0].width/2 > enemies[index].xpos + enemies[index].width/2) {
			(enemies[index].xpos)++;;
		}
		if(humans[0].xpos + humans[0].width/2 == enemies[index].xpos + enemies[index].width/2 && humans[0].ypos > enemies[index].ypos+humanh-3) {	//if theyre aligned, start going down
			(enemies[index].ypos)++;
		}
	}																																																//if the human is picked up by the lander OR if the human isnt picked up and the lander can get him
	if(((humans[0].pickedup == 0 && enemies[index].carrying == 0) || (humans[0].pickedup == 1 && enemies[index].carrying == 1)) 
		&& humans[0].xpos + humans[0].width/2 == enemies[index].xpos + enemies[index].width/2 && humans[0].ypos <= enemies[index].ypos+humanh-3) {	//if theyre aligned, start going down
		(humans[0].pickedup) = 1;
		(enemies[index].carrying) = 1;									//if the ship with a carrying flag is destroyed, the human falls (ypos increases by 2) and should die if he hits the ground
		(enemies[index].ypos)--;
	}
	if(humans[0].dead == 1 || 
		(humans[0].pickedup == 1 && enemies[index].carrying == 0)) {				//this is for the landers that are not or cannot carry a human
		int8_t xMover = Random()%15 + 1;																		//randomly generated movement with small additions or subtractions to "nudge" landers towards the player
		int8_t yMover = Random()%15 + 1;
		if(player[0].xpos > enemies[index].xpos) {													//if the player is to the right, give the rolled value a small nudge up. for example
			xMover+=5;
		}
		if(player[0].xpos < enemies[index].xpos) {
			xMover-=5;
		}
		if(player[0].ypos > enemies[index].ypos) {
			yMover+=5;
		}
		if(player[0].ypos < enemies[index].ypos) {
			yMover-=5;
		}
		if(xMover>=12) {																										//now each random value is compared to be higher or lower than a certain threshold. the modifiers come into effect here
			enemies[index].xpos+=1;
		}
		else if(xMover<=8) {
			enemies[index].xpos-=1;
		}
		if(yMover>=12) {
			enemies[index].ypos+=1;
		}
		else if(yMover<=8){
			enemies[index].ypos-=1;
		}
		if(enemies[index].xpos < 0) {
			enemies[index].xpos = 0;
		}
		if(enemies[index].xpos > 128-landerw) {
			enemies[index].xpos = 128-landerw;
		}
		if(enemies[index].ypos < 20) {
			enemies[index].ypos = 20;
		}
		if(enemies[index].ypos > 110-landerh) {
			enemies[index].ypos = 110-landerh;
		}
	}
	
	if(enemies[index].carrying == 1 && enemies[index].ypos + humanh - 3 <= 0) {									//if the human is carried off it and the lander are technically dead
		humans[0].dead = 1;
		enemies[index].dead = 1;
	}
	if(((Random())%20+1) == 20 && fullShot == 0) {																						//the lander can decide randomly to shoot in the players general direction
		uint8_t xway = 0;
		uint8_t yway = 0;
		if(enemies[index].xpos > player[0].xpos) {																								//the lander picks a direction for projectile velocity here
			xway = 1;
		}
		if(enemies[index].xpos < player[0].xpos) {
			xway = 2;
		}
		if(enemies[index].ypos > player[0].ypos) {
			yway = 2;
		}
		if(enemies[index].ypos < player[0].ypos) {
			yway = 1;
		}
		spawnShot(xway, yway, index);				
	}
}

// *************** mutantMove *******************************************
// Mutants should move randomly and quickly
// Input: Index Position of the Enemy
// Output: None
void mutantMove(uint8_t index) {
	int8_t xMover = Random()%15 + 1;
	int8_t yMover = Random()%15 + 1;
	if(player[0].xpos > enemies[index].xpos) {
		xMover+=5;
	}
	if(player[0].xpos < enemies[index].xpos) {
		xMover-=5;
	}
	if(player[0].ypos > enemies[index].ypos) {
		yMover+=5;
	}
	if(player[0].ypos < enemies[index].ypos) {
		yMover-=5;
	}
	if(xMover>=12) {
		enemies[index].xpos+=2;
	}
	else if(xMover<=8) {
		enemies[index].xpos-=2;
	}
	if(yMover>=12) {
		enemies[index].ypos+=2;
	}
	else if(yMover<=8){
		enemies[index].ypos-=2;
	}
	if(enemies[index].xpos < 0) {
		enemies[index].xpos = 0;
	}
	if(enemies[index].xpos > 128-landerw) {
		enemies[index].xpos = 128-landerw;
	}
	if(enemies[index].ypos < 20) {
		enemies[index].ypos = 20;
	}
	if(enemies[index].ypos > 110-landerh) {
		enemies[index].ypos = 110-landerh;
	}
	
	if(((Random())%20+1) == 20 && fullShot == 0) {																									//the lander can decide randomly to shoot in the players general direction
		uint8_t xway = 0;
		uint8_t yway = 0;
		if(enemies[index].xpos > player[0].xpos) {																										//the lander picks a direction for projectile velocity here
			xway = 1;
		}
		if(enemies[index].xpos < player[0].xpos) {
			xway = 2;
		}
		if(enemies[index].ypos > player[0].ypos) {
			yway = 2;
		}
		if(enemies[index].ypos < player[0].ypos) {
			yway = 1;
		}
		spawnShot(xway, yway, index);				
	}
}

// *************** humanMove ********************************************
// The human runs around the x direction in a panic unless hes picked up or dropped
// Input: None
// Output: None
void humanMove(void) {
	if(humans[0].pickedup == 0 && humans[0].ypos < 110) {
		if(humans[0].ypos < 90) {
			humans[0].deadimpact = 1;
		}
		humans[0].ypos++;
	}
	if(humans[0].pickedup == 1) {				//if the human is picked up, decrease his y position to show he's going up
		humans[0].ypos--;
	}
	if(humans[0].ypos == 110) {
		if(humans[0].deadimpact == 1) {
			humans[0].dead = 1;
		}
		else{
			uint8_t randommove = Random()%20+1;
			if(randommove >= 11) {
				humans[0].xpos++;
			}
			else{
				humans[0].xpos--;
			}
			if(humans[0].xpos < 0) {
				humans[0].xpos = 0;
			}
			if(humans[0].xpos > 128) {
				humans[0].xpos = 0;
			}
		}
	}
	if(humans[0].deadimpact == 1 && humans[0].xpos + humans[0].width >= player[0].xpos && humans[0].xpos <= player[0].xpos + player[0].width && 
		 humans[0].ypos + humans[0].height>= player[0].ypos && humans[0].ypos <= player[0].ypos + player[0].height) {
			 Score += 100; 						// doesn't really work the way it should but when you catch the human mid-air you get some points
			 humans[0].deadimpact = 0;
	}
}

// *************** spawnShot ********************************************
// This creates a shot object that is given the position of the shooter and the appropriate x and y velocities. it also clears the override flag, which makes it drawable
// Input: x = x velocity; y = y velocity; index = index position of the enemy in enemies[]
// Output: None
void spawnShot(uint8_t x, uint8_t y, uint8_t index) {
	uint8_t spot;
	for(int i = 0; i < shotSize; i++) {								//check which spot can be overwritten
		if(shots[i].override == 1) {
			spot = i;
		}
	}
	shots[spot].xpos = enemies[index].xpos;						//copy positional information from the enemy shooting
	shots[spot].ypos = enemies[index].ypos;
	shots[spot].xvel = x;															//copy values that dictate direction of the shot
	shots[spot].yvel = y;
	shots[spot].override = 0;													//make it exist
}

// *************** spawnPlayerShot **************************************
// This creates a shot object that is given the position of the player and the appropriate x velocity. it also clears the override flag, which makes it drawable
// Input: None
// Output: None
void spawnPlayerShot() {
	if(makeShot == 1) {																//check makeShot to not be able to spam lasers
		uint8_t spot;
		for(int i = 0; i < laserSize; i++) {						//check what can be overwritten
			if(lasers[i].override == 1) {
				spot = i;
			}
		}
		lasers[spot].xpos = player[0].xpos;
		lasers[spot].ypos = player[0].ypos;
		if(player[0].facingLeft) {											//calculate directional data here
			lasers[spot].xvel = 0;
		}
		else {
			lasers[spot].xvel = 1;
		}
		lasers[spot].yvel = 0;													//lasers cannot be shot up or down
		lasers[spot].override = 0;
		
		makeShot = 0;
	}
}


// *************** moveShot *********************************************
// Updates shot position according to velocity information. If the shot hits the edge of the screen, it is no longer drawn
// Input: Index position of the shot within shots[]
// Output: None
void moveShot(uint8_t index) {
	checkHit(index);												//first check if the shot hit anything
	if(shots[index].xvel == 2) {						//use velocity information to change the position of the shot
		shots[index].xpos += 2;
	}
	if(shots[index].xvel == 1) {
		shots[index].xpos -= 2;
	}
	if(shots[index].yvel == 2) {
		shots[index].ypos -= 2;
	}
	if(shots[index].yvel == 1) {
		shots[index].ypos += 2;
	}
	if(shots[index].xpos >= screenW || shots[index].ypos >= screenH || shots[index].xpos <= -7 || shots[index].ypos <= -7) {
		shots[index].override = 1;						//delete the shot if the projectile hits any screen edges
	}
}

// *************** movePlayerShot ***************************************
// Updates shot position according to velocity information. If the shot hits the edge of the screen, it is no longer drawn
// Input: Index position of the shot within shots[]
// Output: None
void movePlayerShot(uint8_t index) {
	checkPlayerHit(index);
	if(lasers[index].xvel == 1) {
		lasers[index].xpos += 2;
	}
	else {
		lasers[index].xpos -= 2;
	}
	if(lasers[index].xpos >= screenW || lasers[index].xpos <= -7) {
		lasers[index].override = 1;						//set override if the projectile hits any screen edges
	}
}

// *************** checkHit *********************************************
// compares shot position information to player position information. if the x and y information align, a life is taken and a small animation should play. 
// if the player has no lives,  raise the game over flag
// Input: Index position of the shot within shots[]
// Output: None
void checkHit(uint8_t thisShot) {
	if(shots[thisShot].xpos >= player[0].xpos && shots[thisShot].xpos <= player[0].xpos + playerRw - 4 && 
		shots[thisShot].ypos <= player[0].ypos && shots[thisShot].ypos >= player[0].ypos - playerRh) {
		player[0].lives--;								//the player does not exist
		shots[thisShot].override = 1;			//the shot does not exist
	}
	if(player[0].lives == 0) {
		gameDone = 1;
	}
}

void checkPlayerHit(uint8_t thisShot) {
	for(int i = 0; i < enemySize; i++) {
		if(lasers[thisShot].xpos + 12>= enemies[i].xpos && lasers[thisShot].xpos <= enemies[i].xpos + enemies[i].width&& 
			 lasers[thisShot].ypos - 2 <= enemies[i].ypos && lasers[thisShot].ypos >= enemies[i].ypos - enemies[i].height) {
			enemies[i].dead = 1;
			lasers[thisShot].override = 1;
			if(enemies[i].type == lander) {
				Score+=50;
			}
			if(enemies[i].type == mutant) {
				Score+=100;
			}
			break;
		}
	}
}

// *************** enemyMove ********************************************
// moves enemies, humans, and projectiles in their proper way
// Input: None
// Output: None
void enemyMove() {
	fullShot = 1;												//we also check if the arrays are full in this method. these values will be cleared when an open spot is found
	fullLaser = 1;											//in their respective arrays
	fullEnemy = 1;
	for(int i = 0; i < shotSize; i++) {
		if(shots[i].override == 1) {			//fullShot is how enemies are aware if there are too many projectiles on screen
			fullShot = 0;
		}
		if(shots[i].override == 0) {			//if a shot exists, it should be moved until it doesnt
			moveShot(i);
		}
	}
	spawnPlayerShot();
	for(int i = 0; i < laserSize; i++) {
		if(lasers[i].override == 1) {			//fullLaser and Timer2 generally ensure you cant shoot too much
			fullLaser = 0;
		}
		if(lasers[i].override == 0) {			//if a shot exists, it should be moved until it doesnt
			movePlayerShot(i);
		}
	}
	for(int i = 0; i < enemySize; i++) {
		if(enemies[i].type == lander && enemies[i].dead == 0) {
			landerMove(i);									//a non-dead enemy should move
		}
		if(enemies[i].type == mutant && enemies[i].dead == 0) {
			mutantMove(i);									//a non-dead enemy should move
		}
		if(enemies[i].dead == 1) {				//a dead enemy should drop the human they were carrying
			fullEnemy = 0;
			if(enemies[i].carrying == 1) {
				humans[0].pickedup = 0;
			}
			if(changeFrame) {								//use change frame to signify that the dead enemy is now "stale" and can be replaced
				enemies[i].dead = 2;					//now this should make them overwritable later
			}
		}
	}
	if(humans[0].dead == 0) {
		humanMove();											//a non-dead human should move
	}
	if(spawnMutants) {									//spawn some mutants if possible
		spawnMutant();
	}
	spawnLander();											//otherwise spawn landers
}
