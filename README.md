# LGBT Snake

![](Media/ezgif.com-gif-maker.gif)

# Features #
1. Smooth gameplay (No flickering of screen/cursor which is a common problem when refreshing the terminal without a graphics library).
2. Colored terminal.

# How to play #
Use W-A-S-D keys to move snake while avoiding walls and collecting food. (Caps lock of keyboard should be OFF)

If game is over, close all previous terminals before starting a new one.

# Limitations # 
- Snake appears thinner when moving vertically. This is because in the terminal there is small separation between 2 horizontal lines right on top on each other. Unfortunately, nothing can be done to get rid of this line height. 
- For the same reason as above, there is an illusion that the snake moves faster vertically. However, this speed difference was "fixed" by making snake more slower vertically using the sleep function. 

# Future work #
- [x] Food must be generated at free spot
- [x] End game when size of snake ==  total cells in grid
- [x] Output score at the end in the center of grid
- [ ] Improve AI algorithm (use an approximation of the longest path from snake head to food)
- [ ] Add a few seconds at end of gif so that there's time to see game over.
- [ ] Rename GIF

# Resources used to build the game #
https://youtu.be/AxrQje7V65o 
