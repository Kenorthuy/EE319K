// Sound.h
// Runs on TM4C123 or LM4F120
// Prototypes for basic functions to play sounds from the
// original Space Invaders.
// Jonathan Valvano
// 1/17/2020

#ifndef __sound_h
#define __sound_h
#include <stdint.h>
void Sound_Init(void);
void Sound_Play(const uint8_t *pt, uint32_t count);
void Sound_Shoot(void);
void Sound_Explosion(void);


typedef enum {shoot, playerDie, thrust, scoreTable, laser, defenderStart} soundeffect;
void playsound(soundeffect);

#endif // __sound_h
