
Guide to Pandemonic: an escape the virus game.
----------------------------------------------

This repository contains C++ code for Pandemonic, a newly created terminal game.
1) Model.cpp - models the spread of a virus through a grid, with parameter R0 (which tells us how infectious it is). 
2) Pandemonic.cpp - player has to move through the grid to reach a portal in a corner, while not getting infected by the virus. 

The player can choose the value of R0 (suggested value 2). They can also choose how many infected people are in the grid at the start of the game. These parameters set the difficulty of the game. The size of the grid is customisable. 

The player can collect health supplies so it can recover if infected. Certain grid cells are in self-isolation so the virus can't spread to them.

In each step, the game takes an input from the player. Then, we check if they've moved into an infected cell/reached the portal. 

If they're infected, we check if they have collected health supplies they can use to recover. If not, then it's game over. 

We let the virus spread. In each step, each infected cell infects R0 neighbouring cells. Note that a cell may infect an already infected cell. By setting R0 to a low number like 1, we can see the virus quickly dies out. 



