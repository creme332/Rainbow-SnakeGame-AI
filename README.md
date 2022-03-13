# LGBT Snake

![](Media/ezgif.com-gif-maker.gif)

The LGBT (Lavender-Green-Blue-Teal) snake changes color repeatedly.
# Features #
1. Smooth gameplay (No flickering of screen/cursor which is common when refreshing terminal without a graphics library).
2. Colored terminal.
- [ ] Food must be generated at free spot
- [ ] End game when size of snake ==  total cells in grid
- [ ] Output score at the end in the center of grid
- [ ] Vary difficulty
- [ ] Add AI snake (A* path finding algorithm)

# How to play #
Use W-A-S-D keys to move snake while avoiding walls and collecting food. (Caps lock should be OFF) 
Recommended : maximise size of terminal

# Limitations # 
- Snake appears thinner when moving vertically. This is because in the terminal there is small separation between 2 horizontal lines right on top on each other. Unfortunately, nothing can be done to get rid of this line height. 
- For the same reason as above, there is an illusion that the snake moves faster vertically. However, this speed difference was "fixed" by making snake more slower vertically using the sleep function. 


