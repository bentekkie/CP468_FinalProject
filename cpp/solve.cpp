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
    int *state, *conflicts, *allConflicts;
    size_t *candidates;
    int totalConflicts = 0;
    int n;
    int checkConflicts(int row, int col) {
        int count = 0;
        for (size_t val = 0; val < n; val++) {
            if(val != col) {
                int next_queen_row = state[val];
                if(next_queen_row == row || abs(next_queen_row - row) == abs((int)val - col)){
                    count += 1;
                }
            }
        }
        return count;
    }
    void checkAllConflicts(int col) {
        return checkAllConflicts(col, n);
    }
    void checkAllConflicts(int col, int max) {
        int count = 0;
        for(int i=0; i<n; i++)allConflicts[i] = 0;
        for (int val = 0; val < max; val++) {
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
    int checkConflicts(int col) {
        return checkConflicts(state[col],col);
    }
    void moveQueen(size_t queen) {
        int num = 0;
        int min_conflict = n;
        checkAllConflicts(queen);
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
        if(num > 0){
            int old = state[queen];
            state[queen] = random_element(candidates, num);
            updateConflicts(old,state[queen],queen);
        }
    }

    void updateConflicts(int oldPos, int newPos, int queen){
        if(oldPos != newPos){
            for(size_t val = 0; val < n; val++){
                if(val != queen){
                    if(state[val] == oldPos || abs((int)state[val] - oldPos) == abs((int)val - queen)) {
                        conflicts[val]--;
                        totalConflicts--;
                    }
                    if(state[val] == newPos || abs((int)state[val] - newPos) == abs((int)val - queen)) {
                        conflicts[val]++;
                        totalConflicts++;
                    }
                } else {
                    totalConflicts -= conflicts[val];
                    conflicts[val] = checkConflicts(val);
                    totalConflicts += conflicts[val];
                }
            }
        }
    }
public:
    Queens(int queens) {
        n = queens; 
        state = (int*)calloc(n,sizeof(int));
        allConflicts = (int*)calloc(n,sizeof(int));
        candidates = (size_t*)calloc(n,sizeof(size_t));
        int num = 0;
        for (size_t c = 0; c < n; c++) {
            state[c] = random_int(0,c);
        }
        conflicts = (int*) calloc(n, sizeof(int));
        for (size_t val = 0; val < n; val++) {
            conflicts[val] = checkConflicts(val);
            totalConflicts += conflicts[val];
        }
    }
    int solveBoard(int max){
        int moves = 0;
        int prev_num_conflicts = -1;
        int itt = 0;
        for(int i = 0; i < max; i++){
            int num_conflicts = totalConflicts;
            int max = *max_element(conflicts, conflicts + n);
            vector<int> maxs;
            for(int i = 0; i < n; i++){
                if(conflicts[i] == max) maxs.push_back(i);
            }
            if(num_conflicts == 0){
                return moves;
            }
            size_t q = random_element(maxs);
            moveQueen(q);
            if(num_conflicts != prev_num_conflicts){
                cout << "|" << num_conflicts << " " << itt << " "  << n << " " <<  q << endl;
                //for (int i = 0; i < n; i++) printf ("%d ",conflicts[i]);
                itt = 0;
            }
            itt++;
            prev_num_conflicts = num_conflicts;
            moves += 1;
        }
    }
    bool checkBoard(){
        for(int i = 0; i < n; i++){
            if(checkConflicts(i) != 0){
                return false;
            }
        }
        return true;
    }
    void printBoard(){
    }
    ~Queens() {
        free(state);
        free(conflicts);
        free(allConflicts);
        free(candidates);
    }
};

int main(int a, char** argv)
{
    int n = atoi(argv[1]);
    int max = atoi(argv[2]);
    cout << "n=" << n << ",max=" << max << endl;
    //for(int i = 100; i < n; i += 100){
        using clock = std::chrono::steady_clock;
        clock::time_point start = clock::now();
        Queens *q = new Queens(n);
        q->solveBoard(max);
        clock::time_point end = clock::now();
        std::chrono::duration<double> execution_time =(end - start);
        cout << n << "," << execution_time.count() << "," << q->checkBoard() << endl; 
        delete q;
   // }
}