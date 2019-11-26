#!/usr/bin/python3.8
# %%
from typing import List, Set, Dict, Tuple, TypeVar, Generic, Callable, Any, KeysView, Deque
from queue import Queue
from copy import deepcopy
from collections import namedtuple, deque
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


class CSP(Generic[T, S]):
    domains: Dict[T, Set[S]]
    constraint: FunctionProperty[Callable[[S, S], bool]]

    def __init__(
            self,
            domains: Dict[T, Set[S]],
            constraint: Callable[[S, S], bool]):
        self.domains = domains
        self.constraint = constraint
    
    # Perform the revise operation on a pair of variables to enforce arc consistency
    def revise(self, x_var: T, y_var: T) -> bool:
        new_domain = {vx for vx in self.domains[x_var] if any(
            True for vy in self.domains[y_var] if self.constraint(vx, vy))}
        change = self.domains[x_var] != new_domain
        self.domains[x_var] = new_domain
        return change

    # Check if all variables have domains with exactly 1 value
    def solved(self) -> bool:
        return all(len(values) == 1 for values in self.domains.values())

    # Perform the arc reduction ac3 algorithm on this CSP
    def ac3(self) -> bool:
        v = self.domains.keys()
        # Initialize work queue with all arcs
        q: Deque[Tuple[T,T]] = deque((x,y) for x in v for y in v if x != y)
        # Keep processing till queue is empty
        while len(q) > 0:
            i, j = q.popleft()
            # Perform the revise operation for an arc. If the domain changes, add all incoming arcs to queue
            if self.revise(i, j):
                # If a variable has no valid values, then there is no solution
                if len(self.domains[i]) == 0:
                    return False
                q.extend((k,i) for k in v if k != i and k != j and len(self.domains[k]) > 1)
        return True

    # Perform the backtracking operation on this CSP
    def backtrack(self) -> bool:
        # Ensure that the csp is arc consistent before backtracking begins
        if not self.ac3():
            return False
        if self.solved():  # Short circuit if the puzzle is already solved
            return True
        # minimum remaining values heuristic
        mrv_var = min(
            ((k, len(v)) for k, v in self.domains.items() if len(v) > 1),
            key=lambda k: k[1])[0]
        # least constraining value heuristic
        for value in sorted(
                self.domains[mrv_var],
                key=lambda x: sum(
                    1 for n in self.domains.keys() if x in self.domains[n])):
            new_csp = deepcopy(self)
            new_csp.domains[mrv_var] = {value}  # Assign the value to the variable
            # Check if this is consistent using the backtracking alogorithm
            if new_csp.backtrack():
                self.domains = new_csp.domains  # If it is we are done
                return True
        return False


def solve_nqueens(n: int) -> Tuple[CSP,bool]:
    domains: Dict[int,Set[P]] = {
        i:{P(i,y) for y in range(n)} for i in range(n)
    }
    csp = CSP(
        domains,
        lambda va, vb: va.x != vb.x and va.y != vb.y and abs(va.x - vb.x) != abs(va.y - vb.y)
    )

    return csp, csp.backtrack()

#This part is for benchmarking run time
for n in range(5,100,2):
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
print(n,str(end-start))
grid = [[0]*n for _ in range(n)]
if valid:
    for var in res.domains.values():
        p = var.pop()
        grid[p.x][p.y] = 1
    with open("result.txt","w") as f:
        print(
                '\n'.join(
                    ' '.join(
                        str(grid[x][y]) for x in range(n))
                    for y in range(n)), file=f)
    print(str(end-start) + " seconds", valid)
'''