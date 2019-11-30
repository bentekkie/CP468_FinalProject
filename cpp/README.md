## Compiling
Complile program using `g++ solve.cpp`

## Running
Run the program using `a.out <n> <option>`
### n: the number of queens
### option: an integer for different options of output
0. Solve n-queens problem for n queens, output will be `n,<execution time(sec)>,<did it solve the board(bool)>` 
1. Solve n-queens problem for i queens where i increments from 100 to n by 100 each step, output will be same as `0` with a line for each iteration `i`
2. Same as `0` but also will print the board state as `n` comma separated integers where the `i`th integer `j` represents a queen at column `i` and row `j`
3. Same as `2` but only print the board state