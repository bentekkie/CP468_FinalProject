#include <iostream>
#include <vector>
#include <iterator>
#include <chrono> 
#include <string>
#include <algorithm>
#include <memory>
#include <random>

using namespace std;
using namespace std::chrono; 

uint32_t xor128(void) {
  static uint32_t x = 123456789;
  static uint32_t y = 362436069;
  static uint32_t z = 521288629;
  static uint32_t w = 88675123;
  uint32_t t;
  t = x ^ (x << 11);   
  x = y; y = z; z = w;   
  return w = w ^ (w >> 19) ^ (t ^ (t >> 8));
}

int random_int(int min, int max)
{
    return xor128() % (max - min + 1) + min;
}

template<class T>
T random_element(T* &elements, size_t size)
{
   return elements[random_int(0, size-1)];
}

class Queens
{
private:

    const unsigned int n; // The number of queens
    const unsigned int diags;
    //int totalConflicts = 0; // Stores the sum of all values in conflicts
    unsigned int totalConflictsFast = 0; // Stores the sum of all values in conflicts
    unsigned int* candidates; // Used to store candidate rows at various points in the algoritm;
    unsigned int* state; // state[i]=j represents a queen at column i and row j 
    //int* conflicts; // conflicts[i] stores the number of conflicts that the queen in column i has at it current row stored in state[i]
    //int* allConflicts; // Used to store the results from the checkAllConflicts method
    unsigned int* rowCounts;
    unsigned int* pDiagCounts;
    unsigned int* nDiagCounts;

    // Calculate the conflicts for the given queen
    int checkConflicts(int queen) const{
        return checkConflicts(state[queen],queen);
    }

    // Calculate the conflicts at the given row and column
    int checkConflicts(int row, int col) const{
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
    

    // Moves given queen to the row with the minimum conflicts, ties broken randomly
    void moveQueen(const unsigned int queen) {

        // num keeps track of how many candidates we have
        auto num = 0;
        // min_conflict is the number of conflicts that all of the candidates we have have
        const auto prev_conflict = rowCounts[state[queen]] + pDiagCounts[state[queen]+queen] + nDiagCounts[(n-1)-state[queen]+queen] - 3;
        auto min_conflict = prev_conflict;

        // Fill candidates with all rows that have the minimum number of conflicts
        for(auto val = 0; val < n; val++){
            const auto conflict_num = rowCounts[val] + pDiagCounts[val+queen] + nDiagCounts[(n-1)-val+queen] - ((val == state[queen])? 3 : 0);
            if(conflict_num < min_conflict){
                num = 1;
                candidates[0] = val;
                min_conflict = conflict_num;
            }else if(conflict_num == min_conflict){
                candidates[num++] = val;
            }
        }

        //If there is at least one candidate then move the queen to a random row in the list of candidates
        if(num > 1 || (prev_conflict != min_conflict && num > 0)){
            totalConflictsFast -= --(rowCounts[state[queen]]);
            totalConflictsFast -= --(pDiagCounts[state[queen]+queen]);
            totalConflictsFast -= --(nDiagCounts[(n-1)-state[queen]+queen]);
            state[queen] = random_element(candidates, num);
            totalConflictsFast += rowCounts[state[queen]]++;
            totalConflictsFast += pDiagCounts[state[queen]+queen]++;
            totalConflictsFast += nDiagCounts[(n-1)-state[queen]+queen]++;
        }
    }
public:
    Queens(int queens): 
            n(queens),
            diags(2*queens-1),
            state{new unsigned int[n]},
            candidates{new unsigned int[n]{0}},
            rowCounts{new unsigned int[n]{0}},
            pDiagCounts{new unsigned int[diags]{0}},
            nDiagCounts{new unsigned int[diags]{0}}{
        
    }
    void initBoard(){
        // Initialize all queens at random rows on the board
        
        for (auto queen = 0; queen < n; queen++) {
            /*
            // num keeps track of how many candidates we have
            auto num = 0;
            // min_conflict is the number of conflicts that all of the candidates we have have 
            auto min_conflict = queen/2;
            auto min_conflict_row = -1;
            
            // Fill candidates with all rows that have the minimum number of conflicts
            for(auto row = 0; row < n; row++){
                const auto conflict_num = rowCounts[row] + pDiagCounts[row+queen] + nDiagCounts[(n-1)-row+queen];
                if(conflict_num < min_conflict){
                    num = 1;
                    min_conflict = conflict_num;
                    min_conflict_row = row;
                    candidates[0] = row;
                }else if(conflict_num == min_conflict){
                    candidates[num++] = row;
                }
            }
            state[queen] = random_element(candidates,num);
            */
            state[queen] = random_int(0,n-1);
            totalConflictsFast += rowCounts[state[queen]]++;
            totalConflictsFast += pDiagCounts[state[queen]+queen]++;
            totalConflictsFast += nDiagCounts[(n-1)-state[queen]+queen]++;
        }
    }
    int solveBoard(){
        // moves keeps track of how many moves were required to solve the board
        unsigned int moves = 0;

        // itt keeps track of how many moves it has been since the number of conflicts changed
        unsigned int  itt = 0;
        auto prev_num_conflicts = -1;

        while(totalConflictsFast > 0){
            // Pick a random queen from the queens that have the max amount of conflicts
            auto maxQueen = -1;
            unsigned int maxQueenConflicts = 0;
            unsigned long num = 0;
            for(auto queen = 0; queen < n; queen++){
                const auto prev = state[queen];
                const auto conflicts = rowCounts[prev] + pDiagCounts[prev+queen] + nDiagCounts[(n-1)-prev+queen] - 3;
                if(conflicts > maxQueenConflicts){
                    num = 1;
                    maxQueen = queen;
                    maxQueenConflicts = conflicts;
                    candidates[0] = queen;
                } else if(conflicts == maxQueenConflicts){
                    candidates[num++] = queen;
                }
            }
            // Move that queen to the row with the min conflicts
            moveQueen(random_element(candidates, num));
            moves += 1;
        }
        return moves;
    }

    // Verify that the state represents a solved board
    bool checkBoard() const{
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
    void prettyPrintBoard(){
        for(int row = 0; row < n; row++){
            printf("%d |",rowCounts[row]);
            for(int col = 0; col < n; col++){
                printf(" %d",state[col] == row);
            }
            printf("\n");
        }
        printf("\n");
    }
    ~Queens(){
        delete[] candidates; // Used to store candidate rows at various points in the algoritm;
        delete[] state; // state[i]=j represents a queen at column i and row j 
        //delete[] conflicts; // conflicts[i] stores the number of conflicts that the queen in column i has at it current row stored in state[i]
        //delete[] allConflicts; // Used to store the results from the checkAllConflicts method
        delete[] rowCounts;
        delete[] pDiagCounts;
        delete[] nDiagCounts;
    };
};

int main(int a, char** argv)
{
    srand (time(NULL));
    int n = atoi(argv[1]);
    int option = atoi(argv[2]);
    switch (option){
    case 0: {
        using clock = std::chrono::steady_clock;
        Queens *q = new Queens(n);
        clock::time_point start = clock::now();
        q->initBoard();
        clock::time_point done_init = clock::now();
        int moves = q->solveBoard();
        clock::time_point end = clock::now();
        std::chrono::duration<double> init_time =(done_init - start);
        std::chrono::duration<double> execution_time =(end - done_init);
        cout << n << "," << moves << "," << init_time.count() << "," << execution_time.count() << "," << q->checkBoard() << endl; 
        break;
    }
    case 1: {
        for(int i = 100; i < n; i += 100){
            using clock = std::chrono::steady_clock;
            Queens *q = new Queens(i);
            clock::time_point start = clock::now();
            q->initBoard();
            clock::time_point done_init = clock::now();
            q->solveBoard();
            clock::time_point end = clock::now();
            std::chrono::duration<double> init_time =(done_init - start);
            std::chrono::duration<double> execution_time =(end - done_init);
            cout << i << "," << init_time.count() << "," << execution_time.count() << "," << q->checkBoard() << endl;
            delete q;
        }
        break;
    }
    case 2: {
        using clock = std::chrono::steady_clock;
        Queens *q = new Queens(n);
        clock::time_point start = clock::now();
        q->initBoard();
        q->solveBoard();
        clock::time_point end = clock::now();
        std::chrono::duration<double> execution_time =(end - start);
        cout << n << "," << execution_time.count() << "," << q->checkBoard() << endl;
        q->printBoard(); 
        break;
    }
    case 3: {
        Queens *q = new Queens(n);
        q->initBoard();
        q->solveBoard();
        q->printBoard();
        break;
    }
    default: {
        break;
    }
    }
    
}
