#!/snap/bin/pypy3
# %%
from typing import List, Set, Dict, Tuple, TypeVar, Generic, Callable, Any, KeysView, Deque
from queue import Queue
from copy import deepcopy
from collections import namedtuple, deque
from itertools import permutations, combinations
import time

P = namedtuple('Point', ['x', 'y'])

T = TypeVar('T')
S = TypeVar('S')

#Workaround for typing
class FunctionProperty(Generic[T]):
    def __get__(self, oself: Any, owner: Any) -> T:
        ...

    def __set__(self, oself: Any, value: T) -> None:
        ...


class CSP():
    domains: List[Set[int]]
    constraint: FunctionProperty[Callable[[int, int], bool]]

    def __init__(
            self,
            domains: List[Set[int]]):
        self.domains = domains
        self.q: Deque[Tuple[int,...]]  = deque()
    
    # Perform the revise operation on a pair of variables to enforce arc consistency
    def revise(self, x_var: int, y_var: int) -> bool:
        diff = (x_var - y_var)
        changed = False
        for vx in self.domains[x_var].copy():
            hits = (vx in self.domains[y_var]) + (vx + diff in self.domains[y_var]) + (vx - diff in self.domains[y_var])
            if len(self.domains[y_var]) <= hits:
                self.domains[x_var].remove(vx)
                changed = True
        return changed

    # Check if all variables have domains with exactly 1 value
    def solved(self) -> bool:
        return all(len(values) == 1 for values in self.domains)

    # Perform the arc reduction ac3 algorithm on this CSP
    def ac3(self) -> bool:
        v = range(len(self.domains))
        # Initialize work queue with all arcs

        self.q.extend(combinations(v,2))
        # Keep processing till queue is empty
        while len(self.q) > 0:
            i, j = self.q.popleft()
            # Perform the revise operation for an arc. If the domain changes, add all incoming arcs to queue
            diff = (i - j)
            changed = False
            jlen = len(self.domains[j])
            for vx in self.domains[i].copy():
                if jlen <= (vx in self.domains[j]) + (vx + diff in self.domains[j]) + (vx - diff in self.domains[j]):
                    self.domains[i].remove(vx)
                    changed = True
            if changed:
                # If a variable has no valid values, then there is no solution
                if len(self.domains[i]) == 0:
                    return False
                self.q.extend((k,i) for k in v if k != i and k != j and len(self.domains[k]) > 1)
            temp = j
            j = i
            i  = temp
            if self.revise(i, j):
                # If a variable has no valid values, then there is no solution
                if len(self.domains[i]) == 0:
                    return False
                self.q.extend((k,i) for k in v if k != i and k != j and len(self.domains[k]) > 1)
        return True
        
    # Perform the backtracking operation on this CSP
    def backtrack(self) -> bool:
        # Ensure that the csp is arc consistent before backtracking begins
        if not self.ac3():
            return False
        if self.solved():  # Short circuit if the puzzle is already solved
            return True
        # minimum remaining values heuristic
        mrv_var = min((k for k in range(len(self.domains)) if len(self.domains[k]) > 1), key=lambda k: len(self.domains[k]))
        # least constraining value heuristic
        for value in sorted(
                self.domains[mrv_var],
                key=lambda x: sum(
                    1 for n in range(len(self.domains)) if n != mrv_var and x in self.domains[n])):
            new_csp = deepcopy(self)
            new_csp.domains[mrv_var] = {value}  # Assign the value to the variable
            # Check if this is consistent using the backtracking alogorithm
            if new_csp.backtrack():
                self.domains = new_csp.domains  # If it is we are done
                return True
        return False


def solve_nqueens(n: int) -> Tuple[CSP,bool]:
    domains: List[Set[int]] = [{y for y in range(n)} for i in range(n)]
    csp = CSP(domains)

    return csp, csp.backtrack()

#This part is for benchmarking run time
for n in range(6,100,2):
    start = time.perf_counter()
    res, valid = solve_nqueens(n)
    end = time.perf_counter()
    print(n,str(end-start), valid)
'''
#This part is to output a given solution for n
n = 200
start = time.perf_counter()
res, valid = solve_nqueens(n)
end = time.perf_counter()
print(n,str(end-start),res.solved())


grid = [[0]*n for _ in range(n)]

if valid:
    print(res.domains)
    for key, var in enumerate(res.domains):
        grid[key][var.pop()] = 1
    with open("result.txt","w") as f:
        print(
                '\n'.join(
                    ' '.join(
                        str(grid[x][y]) for x in range(n))
                    for y in range(n)), file=f)
    print(str(end-start) + " seconds", valid)
'''