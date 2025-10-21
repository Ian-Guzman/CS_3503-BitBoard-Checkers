# CS_3503-BitBoard-Checkers
## Author 
Ian Guzman

## Description
This project utilizes the game of Chekers and implements it in C via a bitboard representation. It combines functions using bit manipulation and  game logic to create a fully playable game that includes:

- An 8×8 checkers board composed of 64 individual bits in bitboards

- Bitwise operations to move and reposition pieces

- Red and Black play turns

- Move validation based on valid game rules

- multi-jump capturing

- King transformation and backward movement

- Win validation and game termination

- Piece count output after every move

## Build Instructions
Open the terminal and make sure it is in the project folder. Then compile the program by running :
‘‘ bash
make
./checkers

## Gameplay Instructions
The game will then show the gameboard and ask for the player's move:
Enter move (fromRow fromCol toRow toCol): 2 1 3 2 
// Moves piece in row 2 column 1 to row 3 column 2.
It then switches turn between black and red with lowercase pieces only being able to move forward while uppercase (Kings) can move backwards as well. Pieces become kings when they reach opponent's back row. Pieces are taken off the board when jumped over by opposing colors piece and the team that loses all of it's pieces on the board or no longer has any legal moves first loses.
