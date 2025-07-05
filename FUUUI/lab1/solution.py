import argparse
import os
import heapq
from collections import deque
import time

start_state = None  #početno stanje
target = []     #lista krajnjih stanja
states = {}     #funkcija prijelaza
visited = set() #skup posjećenih stanja
heuristika = {} #dictionary za heuristiku

class State:

    #klasa koja modelira čvor u grafu
    #sadrži naziv čvora, cijenu prijelaza do njega, listu čvorova (putanju) do prvog roditelja
    #i heuristiku kad se provodi A* algoritam

    def __init__(self, name, cost, parent=[], heuristic=0.0):
        self.name = name
        self.cost = cost
        self.parent = parent 
        self.heuristic = heuristic

    def __eq__(self, other):

        #dvije klase su iste ako su im ista imena
        #ovo omogućava lakšu provjeru u skupu visited
        #i kod A* algoritma zamjenu čvora sa čvorom s istim imenom, a manjom cijenom

        return self.name == other.name
    
    def __lt__(self, other):

        #usporedba čvorova po cijeni (+ heuristika)
        #ako su jednaki, uspoređuje se po imenu, po abecedi

        if (self.cost + self.heuristic) != (other.cost + other.heuristic):
            return (self.cost + self.heuristic) < (other.cost + other.heuristic)
        
        return self.name < other.name

    def __hash__(self):
        return hash(self.name)
    
def binary_search(open_set, low, high, state):
    if high >= low:
        mid = low + (low + high)/2
        if(open_set[mid].name == state.name):
            return state
        
        elif(open_set[mid].cost > state.cost):
            return binary_search(open_set, low, mid-1, state)
        
        else:
            return binary_search(open_set, mid + 1, high, state)
        
    else:
        return -1


def printer(alg, path, solution):

    #funkcija za ispis

    print("# " + alg + " " + path)
    if(solution):
        print("[FOUND_SOLUTION]: yes")
    else:
        print("[FOUND_SOLUTION]: no")
        print("[STATES_VISITED]: " + str(len(visited)))
        print("[PATH_LENGTH]: " + 0)
        print("[TOTAL_COST]: " + 0)
        print("[PATH]: " + str(start_state.name))
        return

    print("[STATES_VISITED]: " + str(len(visited)))

    kagaminelen = solution.parent
    message = " "
    length = 0
    for i in range(len(kagaminelen)):
        message += kagaminelen[i]
        length += 1
        message += " => "

    message += str(solution.name)

    print("[PATH_LENGTH]: " + str(length + 1))
    print("[TOTAL_COST]: " + str(solution.cost))
    print("[PATH]: " + message)

def read_file(path):

    #funkcija za učitavanje stanja iz datoteke
    #u start_state se sprema početno stanje
    #u target se spremaju ciljna stanj
    #u states se sprema funkcija prijelaza

    global start_state
    global target
    global states

    if not(os.path.exists(path)):
        print("File does not exist")
        return
    
    with open(path, "r", encoding='utf-8') as file:
        line = file.readline()
        while(line.startswith("#")):
            line = file.readline()
        
        start_state = State(line.strip(), 0.0)

        line = file.readline()
        target = line.split()

        line = file.readline()
        while line:
            state_function = line.split(":")
            parent = state_function[0].strip()
            state_function = state_function[1].strip().split()
            children = []
            
            for state in state_function:
                state = state.split(",")
                child = State(state[0], float(state[1]))
                children.append(child)

            states[parent] = children

            line = file.readline()

def expand(n):

    #funkcija za proširivanje čvorova
    #u cost se upisuje ukupna cijena prijelaza do tog stanja
    #parent se kopira, u njega se upisuje roditeljski čvor, te se upisuje u novo stanje

    global states
 
    next = states[n.name]
    children = []
    parent = n.parent[:]
    parent.append(n.name)
    for succ in next:
        child = State(succ.name, n.cost + succ.cost, parent)
        children.append(child)

    return children

def expand_astar(n):

    #posebna funkcija proširivanja za algoritam A*
    #u čvor se dodatno upisuje heuristika

    global states

    next = states[n.name]
    children = []
    parent = n.parent[:]
    parent.append(n.name)

    for succ in next:
        child = State(succ.name, n.cost + succ.cost, parent, heuristika[succ.name])
        children.append(child)

    return children

def bfs():

    #funkcija za provođenje algoritma BFS
    #pseudokod je dan u prezentaciji 2. Pretraživanje prostora stanja, objavljena na web stranici predmeta
    #za open se koristi prioritetni red, a za skup visited se koristi set

    global start_state
    global target
    global visited

    open = deque([State(start_state.name, 0.0)])
    while open:
        n = open.popleft()
        if(n.name in target): 
            return n
        
        visited.add(n)

        m = expand(n)
        for yumm in m:
            if(yumm not in visited):
                open.append(yumm)
                

    return False

def ucs():

    #funkcija za provođenje algoritma UCS
    #pseudokod je dan u prezentaciji 2. Pretraživanje prostora stanja, objavljena na web stranici predmeta
    #za open se koristi prioritetni gomila, a za skup visited se koristi set
    #inspiracija za korištenje gomile: Dominik Mandić, Laura Lučin

    global start_state
    global target
    global visited

    open = [State(start_state.name, 0.0)]
    heapq.heapify(open)
    while open:
        n = heapq.heappop(open)
        if(n.name in target):
            return n
        
        visited.add(n.name)

        m = expand(n)
        for yumm in m:
            if(yumm.name not in visited):
                heapq.heappush(open, yumm)            

    return False


def astar(heur):

    #funkcija za provođenje algoritma A*
    #pseudokod je dan u prezentaciji 2. Pretraživanje prostora stanja, objavljena na web stranici predmeta
    #za open se koristi prioritetni gomila, a za skup visited se koristi set
    #inspiracija za korištenje gomile: Dominik Mandić, Laura Lučin
    #heuristika se učitava iz datoteke te se sprema u dictionary

    global start_state
    global target
    global visited
    global heuristika

    visited_dicc = {}

    if not(os.path.exists(heur)):
        print("File does not exist")
        return
    
    if(not heuristika):
        with open(heur, "r", encoding='utf-8') as file:
            line = file.readline()
            while line:
                linija = line.split(":")
                heuristika[linija[0]] = float(linija[1].strip())
                line = file.readline()

    open_set = [State(start_state.name, 0.0, [], heuristika[start_state.name])]
    heapq.heapify(open_set)

    while open_set:
        n = heapq.heappop(open_set)
        if(n.name in target):
            return n
        
        #ako čvor n postoji u visited, nađi ga, usporedi njegovu cijenu i heuristiku s novim čvorom
        #ako novi čvor ima veću vrijednost od postojećeg, preskoči ga i ne obrađuj
        if(n in visited):
            #discarded = next((node for node in visited if node == n), None)
            discarded = visited_dicc[n.name]
            if(n.cost + n.heuristic > discarded.cost + discarded.heuristic):
                continue
            else:
                #visited.discard(discarded)
                del visited_dicc[discarded.name]

        visited_dicc[n.name] = n
        m = expand_astar(n)

        for yumm in m:
            if(yumm.name not in visited_dicc):
                heapq.heappush(open_set, yumm)
            else:

                #ako čvor već postoji u visited, nađi ga i usporedi njegovu cijenu i heuristiku s novim čvorom
                #ako novi čvor ima manju vrijednost od prošloga, ubaci ga umjesto starog u visited i u open_set

                #removed = next((node for node in visited if node == yumm), None)
                removed = visited_dicc[yumm.name]
                if(removed.cost > yumm.cost):
                    del visited_dicc[removed.name]
                    visited_dicc[yumm.name] = yumm
                    # if removed in open_set:
                    #     open_set.remove(removed)
                    #     heapq.heapify(open_set)
                    #     heapq.heappush(open_set, yumm)

                    remove_heap = binary_search(open_set, 0, len(open_set), removed)
                    if remove_heap >= 0:
                        open_set.pop(remove_heap)
                        heapq.heapify(open_set)
                        heapq.heappush(open_set, yumm)

    return False

def heuristic_check_opt(heur):

    #funkcija za provjeru optimističnosti heuristike
    #heuristika se učitava iz datoteke, ako već prije nije bila učitana
    #provodi se UCS algoritam, svaki put s drugim početnim stanjem, zatim se dobivena vrijednost puta
    #uspoređuje s vrijenošću heuristike te se ispisuje odgovarajuća poruka

    global start_state
    global states
    global heuristika
    global visited

    optimizam = 1
    paths = {}

    if(not heuristika):
        with open(heur, "r", encoding='utf-8') as file:
            line = file.readline()
            while line:
                linija = line.split(":")
                heuristika[linija[0]] = float(linija[1].strip())
                line = file.readline()

    print("# HEURISTIC-OPTIMISTIC " + heur)
    for state in states:
        visited = set()
        start_state = State(state, 0.0)
        n = ucs()
        paths[state] = float(n.cost)
        if(paths[state] < heuristika[state]):
            print("[CONDITION]: [ERR] h(" + state + ") <= h*: " + str(heuristika[state]) +" <= " + str(paths[state]))
            optimizam = 0
        else:
            print("[CONDITION]: [OK] h(" + state + ") <= h*: " + str(heuristika[state]) +" <= " + str(paths[state]))

    if(optimizam):
        print("[CONCLUSION]: Heuristic is optimistic.")
    else:
        print("[CONCLUSION]: Heuristic is not optimistic.")

def heuristic_check_cons(heur):

    #funkcija za provjeru konzistentnosti heuristike
    #heuristika se učitava iz datoteke, ako već nije bila učitana
    #iterira se po dictionaryju states koji sadržava sva stanja i prijelaze
    #uspoređuju se heuristika roditelja, heuristika djeteta i cijena prijelaza do djeteta
    #ispisuje se odgovarajuća poruka

    global states

    heuristika = {}
    konzistentnost = 1

    if not(os.path.exists(heur)):
        print("File does not exist")
        return
    
    with open(heur, "r", encoding='utf-8') as file:
        line = file.readline()
        while line:
            linija = line.split(":")
            heuristika[linija[0]] = float(linija[1].strip())
            line = file.readline()

    print("# HEURISTIC-CONSISTENT " + heur)

    for state in states:
        for child in states[state]:
            if(heuristika[state] > heuristika[child.name] + child.cost):
                print("[CONDITION]: [ERR] h(" + state + ") <= h(" + child.name + ") + c: " + str(heuristika[state]) + " <= " + str(heuristika[child.name]) + " + " + str(child.cost))
                konzistentnost = 0
            else:
                print("[CONDITION]: [OK] h(" + state + ") <= h(" + child.name + ") + c: " + str(heuristika[state]) + " <= " + str(heuristika[child.name]) + " + " + str(child.cost))

    if(konzistentnost):
        print("[CONCLUSION]: Heuristic is consistent.")
    else:
        print("[CONCLUSION]: Heuristic is not consistent.")


def main(args):

    #main funkcija
    #na temelju podataka dobivenih preko command linea, zove se odgovarajući algoritam ili provjera heuristike

    path = args.ss
    read_file(path)
    result = None
    alg = args.alg

    if(alg == "BFS" or alg == "bfs"):
        result = bfs()
    elif(alg == "UCS" or alg == "ucs"):
        result = ucs()
    elif(alg == "ASTAR" or alg == "astar"):
        if(not args.h):
            print("Nedostaje putanja do heuristike")
            return
        
        heur = args.h
        
        result = astar(heur)
    else:
        heur = args.h
        opt = args.check_optimistic
        cons = args.check_consistent

        if(opt):
            heuristic_check_opt(heur)
            return
        if(cons):
            heuristic_check_cons(heur)
            return

        print("Krivi unos")
        return

    printer(alg, path, result)

if __name__ == "__main__":

    start_time = time.time()

    parser = argparse.ArgumentParser(description="Ulazni parametri za program")

    #iz command linea se argparseom izvlače argumenti
    #--alg -> algoritam
    #--ss -> putanja do datoteke sa stanjima
    #--h -> putanja do heuristike
    #--check-optimistic -> zastavica za provjeru optimističnosti heuristike
    #--check-consistent -> zastavica za provjeru konzistentnosti heuristike

    parser.add_argument("--alg", type=str)
    parser.add_argument("--ss", type=str)
    parser.add_argument("--h", type=str)
    parser.add_argument("--check-optimistic", action="store_true")
    parser.add_argument("--check-consistent", action="store_true")

    args = parser.parse_args()
    main(args)

    end_time = time.time()
    elapsed_time = end_time - start_time
    print(f"Elapsed time: {elapsed_time:.4f} seconds")