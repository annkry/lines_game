# lines_game
The segments have centers on an NxN grid (and 10x10 grid as a default), with coordinates ranging from [-0.9,0.9] and randomly rotated. The player is a segment of a different color with distinguishable ends in different colors. The player can move using arrow keys (UP - forward, DOWN - backward, LEFT/RIGHT - rotation). Movement should be limited by collisions detected between segments, and collision with the segment in the upper right corner is a success and the end of the game, at which point the transition time should be displayed on the terminal. The game is called from the command line with the first optional parameter being a number as a seed for the pseudorandom number generator, which ensures that the same segments are rotated identically on the board with the same seed. The second optional argument is a number N, the size of the grid. Additionally, at the end of the game, a special effect is implemented where the board shrinks. Furthermore, a changing background scenario is implemented using functions that return a color value based on sine/cosine values.

To compile: make  
To run: ./main <seed> <N>  

Where seed and N are optional
