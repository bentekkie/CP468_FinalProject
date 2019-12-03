#include <iostream>
#include <vector>
#include <iterator>
#include <chrono> 
#include <string>
#include <algorithm>

using namespace std;
using namespace std::chrono; 


int random_int(int min, int max)
{
   return rand()%(max-min+1) + min;
}

template<class T>
T random_element(std::vector<T> &elements)
{
   return elements[random_int(0, elements.size()-1)];
}

template<class T>
T random_element(T* &elements, size_t size)
{
   return elements[random_int(0, size-1)];
}

class Queens
{
private:

    const int n; // The number of queens
    size_t *candidates; // Used to store candidate rows at various points in the algoritm;
    int totalConflicts = 0; // Stores the sum of all values in conflicts
    int* state; // state[i]=j represents a queen at column i and row j 
    int* conflicts; // conflicts[i] stores the number of conflicts that the queen in column i has at it current row stored in state[i]
    int* allConflicts; // Used to store the results from the checkAllConflicts method
    
    
    constexpr bool isConflicted(int rowA, int colA, int rowB, int colB){
        return rowA == rowB || abs(rowA - rowB) == abs(colA - colB);
    }

    // Calculate the conflicts for the given queen
    int checkConflicts(int queen) {
        return checkConflicts(state[queen],queen);
    }

    // Calculate the conflicts at the given row and column
    int checkConflicts(int row, int col) {
        int count = 0;
        for (size_t queen = 0; queen < n; queen++) {
            if(queen != col) {
                int next_queen_row = state[queen];
                if(next_queen_row == row || abs(next_queen_row - row) == abs((int)queen - col)){
                    count += 1;
                }
            }
        }
        return count;
    }

    // Update allConflicts to represent the number of conflicts for each row at the given column
    void checkAllConflicts(int col) {
        int count = 0;
        for(int i=0; i<n; i++)allConflicts[i] = 0;
        for (int val = 0; val < n; val++) {
            if(val != col) {
                int next_queen_row = state[val];
                int diff = abs(val-col);
                allConflicts[next_queen_row]++;
                int up = next_queen_row + diff;
                if(up < n){
                    allConflicts[up]++;
                }
                int down = next_queen_row - diff;
                if(down > -1){
                    allConflicts[down]++;
                }
            }
        }
        return;
    }
    

    // Moves given queen to the row with the minimum conflicts, ties broken randomly
    void moveQueen(size_t queen) {

        // num keeps track of how many candidates we have
        int num = 0;
        // min_conflict is the number of conflicts that all of the candidates we have have 
        int min_conflict = n;

        // Update the allConflicts array to represent the conflicts at each row for the given queen
        checkAllConflicts(queen);

        // Fill candidates with all rows that have the minimum number of conflicts
        for(int val = 0; val < n; val++){
            int conflict_num = allConflicts[val];
            if(conflict_num < min_conflict){
                num = 0;
                candidates[0] = val;
                min_conflict = conflict_num;
                num++;
            }else if(conflict_num == min_conflict){
                candidates[num] = val;
                num++;
            }
        }

        //If there is at least one candidate then move the queen to a random row in the list of candidates
        if(num > 0){
            int old = state[queen];
            state[queen] = random_element(candidates, num);
            
            // Update the conflicts and totalConflicts after the move of the queen
            updateConflicts(old,state[queen],queen);
        }
    }


    //Updates conflicts and totalConflicts after a given queen moves from row oldPos to row newPos
    void updateConflicts(int oldPos, int newPos, int queen){
        if(oldPos != newPos){
            for(size_t val = 0; val < n; val++){
                if(val != queen){
                    // Remove conflicts that were a result of the queen being at the row oldPos
                    if(state[val] == oldPos || abs((int)state[val] - oldPos) == abs((int)val - queen)) {
                        conflicts[val]--;
                        conflicts[queen]--;
                        totalConflicts -= 2;
                    }

                    // Add conflicts that were a result of the queen being at the row newPos
                    if(state[val] == newPos || abs((int)state[val] - newPos) == abs((int)val - queen)) {
                        conflicts[val]++;
                        conflicts[queen]++;
                        totalConflicts += 2;
                    }
                }
            }
        }
    }
public:
    Queens(int queens): n(queens){

        // Initialize arrays needed for algorithm
        state = new int[n];
        allConflicts = new int[n];
        candidates = new size_t[n];
        conflicts = new int[n];

        // Initialize all queens at random rows on the board
        for (size_t c = 0; c < n; c++) {
            state[c] = random_int(0,n-1);
        }

        // Initialize conflicts array and totalConflicts to reflect the starting state of the board
        for (size_t val = 0; val < n; val++) {
            conflicts[val] = checkConflicts(val);
            totalConflicts += conflicts[val];
        }
    }
    int solveBoard(){
        // moves keeps track of how many moves were required to solve the board
        int moves = 0;

        // itt keeps track of how many moves it has been since the number of conflicts changed
        int itt = 0;
        int prev_num_conflicts = -1;

        while(true){
            // If there are no conflicts left then we are done, we return the number of moves it took
            if(totalConflicts == 0){
                return moves;
            }
            
            // Pick a random queen from the queens that have the max amount of conflicts
            int max = *max_element(conflicts, conflicts + n);
            vector<int> maxs;
            for(int i = 0; i < n; i++){
                if(conflicts[i] == max) maxs.push_back(i);
            }
            size_t q = random_element(maxs);

            // Move that queen to the row with the min conflicts
            moveQueen(q);
            moves += 1;
            /*
            // This code is for debugging purposes
            if(num_conflicts != prev_num_conflicts){
                //cout << "|" << num_conflicts << " " << itt << " "  << n << " " <<  q << endl;
                //for (int i = 0; i < n; i++) printf ("%d ",conflicts[i]);
                itt = 0;
            }
            itt++;
            prev_num_conflicts = num_conflicts;
            */
        }
    }

    // Verify that the state represents a solved board
    bool checkBoard(){
        for(int i = 0; i < n; i++){
            if(checkConflicts(i) != 0){
                return false;
            }
        }
        return true;
    }
    void printBoard(){
    	for (int i = 0; i < n; i++) printf ("%d,",state[i]);
    }
    ~Queens() {
        delete[] state;
        delete[] conflicts;
        delete[] allConflicts;
        delete[] candidates;
    }
};

int main(int a, char** argv)
{
    int n = atoi(argv[1]);
    int option = atoi(argv[2]);
    switch (option){
    case 0: {
        using clock = std::chrono::steady_clock;
        clock::time_point start = clock::now();
        Queens *q = new Queens(n);
        q->solveBoard();
        clock::time_point end = clock::now();
        std::chrono::duration<double> execution_time =(end - start);
        cout << n << "," << execution_time.count() << "," << q->checkBoard() << endl; 
        break;
    }
    case 1: {
        for(int i = 100; i < n; i += 100){
            using clock = std::chrono::steady_clock;
            clock::time_point start = clock::now();
            Queens *q = new Queens(i);
            q->solveBoard();
            clock::time_point end = clock::now();
            std::chrono::duration<double> execution_time =(end - start);
            cout << i << "," << execution_time.count() << "," << q->checkBoard() << endl; 
            
        delete q;
        }
        break;
    }
    case 2: {
        using clock = std::chrono::steady_clock;
        clock::time_point start = clock::now();
        Queens *q = new Queens(n);
        q->solveBoard();
        clock::time_point end = clock::now();
        std::chrono::duration<double> execution_time =(end - start);
        cout << n << "," << execution_time.count() << "," << q->checkBoard() << endl;
        q->printBoard(); 
        break;
    }
    case 3: {
        Queens *q = new Queens(n);
        q->solveBoard();
        q->printBoard();
        break;
    }
    default: {
        break;
    }
    }
    
}
