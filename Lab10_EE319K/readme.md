# EE 319K Final Lab: Defender
This was the final project @evargh and I created together. It is a recreation of the popular arcade game "Defender" put into a microcontroller.
The project was very fun, yet challenging and required the both of us to work collaboratively to research, plan, and develop the hardware and software
to build a finished product.

# Here is a video showcasing the project:

[![](https://yt-embed.herokuapp.com/embed?v=fDDiu07ZUBU)](https://www.youtube.com/watch?v=fDDiu07ZUBU "")


# Detailed information of the hardware/software components
Inputs:
  1 slide pot (PD2):
		position of the slide pot directly maps to the y level of the player
  2 directional buttons (PE0 and PE1)
		will change the player's x coordinate
  1 shoot button (PE2)
		moves along the same y level and for a certain distance in the x level
  
Outputs:
  Sound
    - done using a resistor DAC
  LCD
    - outputs the graphics for the game
  
Timers:
  Systick checks player input
  Timer0 tracks sound output
  Timer1 tracks creature interactions
  Timer2 will be for animation and keeping track of game time
  Busy-Wait LCD
  
 Gameplay:
  static background
  1 human. panics and runs around
  3 enemies for level 1 and increases each level
  
  Lander:
    goes to a humans x coordinate, picks them up
    
  Mutant:
    moves randomly, tries to reach players y level and shoot them
  
