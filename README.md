# MyTouchControls
Touch screen used as controller with push on push off action.
Playing with an inexpensive touch screen I decided to make a full set control system
That would be flexible yet stable. Breaking up the screen into a grid allows 
integer math to map exactly on touch which grid block has been touched. Also each block 
activates or deactivates depending on which state it was in. Each block has 4 states
recorded in an array to keep track of stylus touch release order and debounce.
I used a arduino mega from GeekCreit but the code should work fine on a 328 if you adjust the 
control lines.
