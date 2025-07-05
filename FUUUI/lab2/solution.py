import argparse
import os
from collections import deque
import copy

# -----------------------------------------------
# credit za pomoć oko skupova: Patricia Asanović
# -----------------------------------------------

clauses = set()     # skup svih klauzula
clauses_dict = {}   # dictionary svih klauzula; ključ je redni broj klauzule, a vrijednost je skup literala klauzule
resolved = set()    # skup parova koji su već provjereni
new = set()         # skup literala nove klauzule, resetira se na prazno nakon učitavanja svake nove klauzule
conclusions = {}    # dictionary za izvedene klauzule, sprema se redni broj nove klauzule kao ključ i par rednih brojeva klauzula iz koje je izvedena
absolution = 0      # ,,kazaljka" na ciljnu klauzulu u dictionaryju clauses_dict
final_absolution = 0    # ,,kazaljka" na klauzulu poslije zadnje klauzule negiranog cilja u clauses_dict
new_clauses = 0     # ,,kazaljka" na prazno mjesto nakon zadnje izvedene klauzule
teto = None         # ciljna klauzula učitana direktno iz datoteke
check = False       # zastavica koja prati postoji li tautologija u učitanim klauzulama

def read_file(path):

    # funkcija za učitavanje klauzula iz datoteke

    global clauses
    global clauses_dict
    global absolution
    global new_clauses

    if not(os.path.exists(path)):
        print("File does not exist, read file")
        return
    
    # u lines se učita cijela datoteka pa se iterira po njima, liniju po liniju
    lines = None
    with open(path, "r", encoding='utf-8') as file:
        lines = file.readlines()

    # offset broji koliko smo nepotrebnih klauzula i komentara izbacili
    # ovo je potrebno radi rednih brojeva klauzula u clauses_dict
    offset = 0

    # izbacujemo zakomentirane linije
    # petlja iterira po svim linijama, osim po zadnjoj koja sadrži ciljnu klauzulu
    for i in range(1, len(lines)):
        if(lines[i - 1].startswith("#")):
            offset += 1
            continue
        
        # kad se izbace svi komentari, iteriramo po klauzulama
        # i ide od 1 pa moramo pisati i - 1 za indekse u listi
        absolution = i

        # literali klauzule se spreme u parts
        # velika slova se pretvaraju u mala
        parts = [line.strip().lower() for line in lines[i - 1].split()]

        # klauzula se spremi u clauses
        clause = lines[i - 1].strip().lower()
        clauses.add(clause)

        # iz parts se mora ukloniti znak "v" prije nego se literali dodaju u new
        new = set()
        for element in parts:
            if(element == "v"):
                continue
            new.add(element.strip())

        # new se negira te se provjeri presjek njega s new
        set_neg = set()
        for element in new:
            if(element.startswith("~")):
                element = element.lstrip("~")
                set_neg.add(element)
            else:
                element = "~" + element
                set_neg.add(element)

        # ako postoji presjek između new i new_neg, znači da on u sebi sadrži literal i negaciju njega samog
        # to znači da će klauzula uvijek biti istinita, te se zbog toga izbacuje
        if(new & set_neg):
            offset += 1
            continue
        
        # prva učitana klauzula se sprema u clauses_dict
        if(i - offset == 1):
            clauses_dict[i - offset] = new
            continue

        # iteriramo po clauses_dict
        replaced = False
        for key, value in clauses_dict.items():

            #ako nađemo klauzulu koja je sadržana u novoj, preskačemo ju
            if(value <= new):
                offset += 1
                replaced = True
                break

            # ako nađemo klauzulu u kojoj je već sadržana nova klauzula, 
            # zamijenimo staru klauzulu novom i postavljamo zastavicu replaced
            elif(new <= value):
                clauses_dict[key] = new
                offset += 1
                replaced = True
                break
        
        # ako nismo nigdje našli ili zamijenili klauzulu, spremamo ju u dictionary
        if(not replaced):
            clauses_dict[i - offset] = new

    # kazaljku cilja umanjujemo za offset i uvećavamo za 1 kako bi pokazivala na mjesto gdje će se spremati ciljna klauzula
    absolution -= offset
    absolution += 1

    # zadnja linija (ciljna kluzula) šalje se na negiranje 
    negate_conclusion(lines[-1])

def negate_conclusion(line):

    # funkcija za negiranje ciljne klauzule

    global clauses
    global clauses_dict
    global absolution
    global new_clauses
    global teto
    global check
    global final_absolution

    # u iterator se sprema vrijednost kazaljke cilja, što je potrebno jer se ,,absolution" ne smije mijenjati
    iterator = absolution

    # ciljna klauzula se učitava u globalnu varijablu
    teto = line.strip().lower()

    parts = teto.split()

    # iz parts se ukloni "v" te se literali dodaju u checkset
    checkset = set()
    for element in parts:
        if(element == "v"):
            continue
        checkset.add(element)
    
    for key, value in clauses_dict.items():
        
        # ako postoji klauzula koja je sadržana u ciljnoj klauzuli, pronašli smo tautologiju
        # postavlja se zastavica check te se vraća iz funkcije
        if(value <= checkset):
            check = True
            return

    # negiranje literala ciljne klauzule i spremanje u clauses_dict
    for part in checkset:
        part = part.strip()
        if(part.startswith("~")):
            part = part.lstrip("~")
            tempset = set()
            tempset.add(part)
            clauses_dict[iterator] = tempset
            iterator += 1
        else:
            part = "~" + part
            tempset = set()
            tempset.add(part)
            clauses_dict[iterator] = tempset
            iterator += 1

    # kazaljke new_clauses i final_absolution se postavljaju na vrijednost koja je nakon svih klauzula u clauses_dict
    new_clauses = iterator
    final_absolution = iterator

def printer(i, j):

    # funkcija za ispisivanje rezultata

    global clauses_dict
    global absolution
    global resolved
    global teto
    global final_absolution

    # ispisujemo dictionary s klauzulama
    # ispis staje kad se ispišu sve početne klauzule i cijeli negirani cilj
    for key, value in clauses_dict.items():
        if(key == final_absolution):
            break
        clause = ""
        if(key == absolution):
            print("===============")
        for element in value:
            clause += element
            clause += " v "

        print(f"{str(key)}. {clause[:-3]}")

    print("===============")

    # ako se funkciji pošalju indeksi i = 0 i j = 0, znači da NIL nije pronađen
    if(i == 0 and j == 0):
        print(f"[CONCLUSION]: {teto} is unknown")
        return
    
    # i = -1 i j = -1 označavaju da smo pronašli tautologiju
    elif(i == -1 and j == -1):
        print(f"[CONCLUSION]: {teto} is true")
        return
    
    # message i stacc su dva stoga koji nam pomažu u ispisu
    # u message se prvo stavlja ,,NIL", a u stacc se stavljaju redni brojevi klauzula iz kojih je NIL proizašao
    message = deque()
    stacc = deque()

    # na indeksu new_clauses se nalaze redni brojevi klauzula iz kojih je proizašao NIL
    numbers = conclusions[new_clauses]

    # ako je indeks manji od final_absolution, znači da se radi o izvornoj klauzuli ili o klauzuli negiranog cilja
    # njih ne treba provjeravati jer nisu proizašle ni iz čega
    message.appendleft(f"{new_clauses}. NIL {numbers}")
    if(numbers[1] >= final_absolution):
        stacc.appendleft(numbers[1])

    if(numbers[0] >= final_absolution):
        stacc.appendleft(numbers[0])

    # provjeravaj indekse na stogu dok ih ne prođeš sve
    while stacc:
        next = stacc.pop()

        # klauzula je spremljena u skup te ju moramo pretvoriti u smisleni string radi ispisa
        clause = ""
        for element in clauses_dict[next]:
            clause += element
            clause += " v "

        message.appendleft(f"{next}. {clause[:-3]} {conclusions[next]}")

        # numbers će dobiti par indeksa (i, j) iz kojih je proizašla trenutna klauzula
        numbers = conclusions[next]

        # ako je indeks manji od final_absolution, znači da se radi o izvornoj klauzuli ili o klauzuli negiranog cilja
        if(numbers[1] >= final_absolution):
            stacc.appendleft(numbers[1])

        if(numbers[0] >= final_absolution):
            stacc.appendleft(numbers[0])

    # kad se prođe cijeli stog, ispisuje se cijeli ispis liniju po liniju
    for element in message:
        print(element)

    print(f"[CONCLUSION]: {teto} is true")


def resolution_process():

    # funkcija za sami algoritam procesa zaključivanja

    global clauses_dict
    global clauses
    global absolution
    global resolved
    global new

    # beskonačna petlja provjerava jesmo li provjerom klauzula dobili novu klauzulu
    # ako nismo, zaključivanje završava i nismo uspjeli izvesti cilj
    # ako nađemo NIL, uspjeli smo izvesti i dokazati cilj
    while True:
        kagaminelen = len(clauses_dict)

        # iteriramo po parovima klauzula u clauses_dict
        # prvi indeks ide po svim klauzulama, a drugi ide po klauzulama negiranog cilja i po novoizvedenim klauzulama (Set of Support)
        for i in range(1, kagaminelen + 1):
            for j in range(absolution, kagaminelen + 1):

                # ako je par klauzula već provjeren, preskače se 
                if((i, j) in resolved):
                    continue

                # pošalji indekse na provjeru i dobij natrag novi skup
                resolution = resolve(i, j)
                if("NIL" in resolution):
                    return (i, j)
                
                # pretvori novi skup u klauzulu i dodaj ju u skup klauzula
                new_clause = ""
                for element in resolution:
                    new_clause += element
                    new_clause += " v "
                
                new.add(new_clause[:-3])
        
        # ako je nova klauzula podskup svih klauzula, nismo dobili ništa novo te se ne može izvući NIL
        if(new <= clauses):
            return False

        # dodaj new u skup svih klauzula
        clauses = clauses | new

def resolve(i, j):

    # funkcija za provjeru klauzula i izvođenje novih

    global clauses_dict
    global clauses
    global resolved
    global new_clauses
    global conclusions
    global new

    # dodaj par indeksa u resolved da znamo da smo ga provjerili
    resolved.add((i, j))

    # set1 je jedna klauzula, set2 je druga
    set1 = clauses_dict[i].copy()
    set2 = clauses_dict[j].copy()

    # negiraj oba skupa; ovo nam omogućuje pronalazak parova literal - negirani literal
    set1_neg = set()
    for element in set1:
        if(element.startswith("~")):
            element = element.lstrip("~")
            set1_neg.add(element)
        else:
            element = "~" + element
            set1_neg.add(element)

    set2_neg = set()
    for element in set2:
        if(element.startswith("~")):
            element = element.lstrip("~")
            set2_neg.add(element)
        else:
            element = "~" + element
            set2_neg.add(element)

    presjek = set1_neg & set2
    presjek2 = set1 & set2_neg

    # ako ne postoje presjeci, nema literala koji se mogu poništiti te ne postoji novo izvedena klauzula
    if(not presjek and not presjek2):
        return set()

    # ako je jedan skup jednak negiranom drugom, znači da će nova klauzula biti NIL
    if(set1 == set2_neg or set2 == set1_neg):
        set2.add("NIL")
        tempset = set()
        tempset.add("NIL")
        clauses_dict[new_clauses] = tempset
        conclusions[new_clauses] = (i, j)
        return set2

    # ako je drugi skup podskup negiranog prvog, izbaci taj presjek iz prvog skupa i vrati taj novi skup
    elif(set2 <= set1_neg):
        set1 = set1 - presjek2

        new_clause = ""
        for element in set1:
            new_clause += element
            new_clause += " v "

        if(new_clause[:-3] in new):
            return set()

        clauses_dict[new_clauses] = set1
        conclusions[new_clauses] = (i, j)
        new_clauses += 1
        return set1
    
    # ako je prvi skup podskup negiranog drugog, izbaci taj presjek iz drugog skupa i vrati taj novi skup
    elif(set1 <= set2_neg):
        set2 = set2 - presjek

        new_clause = ""
        for element in set2:
            new_clause += element
            new_clause += " v "

        if(new_clause[:-3] in new):
            return set()

        clauses_dict[new_clauses] = set2
        conclusions[new_clauses] = (i, j)
        new_clauses += 1
        return set2
    
    # ako nisu podskupovi, a postoji presjek, napravi uniju skupova, izbaci presjek te vrati taj skup
    else:
        tempset = set1 - presjek2
        tempset2 = set2 - presjek
        final_set = tempset | tempset2

        new_clause = ""
        for element in final_set:
            new_clause += element
            new_clause += " v "

        if(new_clause[:-3] in new):
            return set()

        clauses_dict[new_clauses] = final_set
        conclusions[new_clauses] = (i, j)
        new_clauses += 1
        return final_set


def cookbook_process(cookbook):

    # glavna funkcija za kuharicu

    global absolution
    global final_absolution
    global new_clauses
    global clauses_dict
    global teto

    # izbriši negirani cilj i sve prije izvedene klauzule
    for i in range(absolution, len(clauses_dict) + 1):
        final_absolution -= 1
        new_clauses -= 1
        del clauses_dict[i]

    # učitaj nenegiranu ciljnu klauzulu i dodaj ju u skup osnovnih klauzula
    concl = teto.split()
    newset = set()
    for element in concl:
        newset.add(element)
    newset.discard("v")
    clauses_dict[absolution] = newset

    # otvori kuharicu i učitaj naredbe
    with open(cookbook, "r", encoding='utf-8') as file:
        lines = file.readlines()

        for line in lines:
            line = line.lower().strip()
            if(line.startswith("#")):
                continue
            if(line == ""):
                continue
            print(f"User's command: {line}")
            command = line.split()

            # ako je naredba za dodati klauzulu, pozovi funkciju za to
            if(command[-1] == "+"):
                add_clause(command)
            
            # ako je naredba za ukloniti klauzulu, pozovi funkciju za to
            elif(command[-1] == "-"):
                remove_clause(command)

            # ako je naredba za provjeriti klauzulu, pozovi funkciju za to
            elif(command[-1] == "?"):
                resolve_clause(command)

            # ako nije ništa od navedenog, vrati se
            else:
                print("Neispravna datoteka")
                return
            print("\n")

def add_clause(command):

    # funkcija za dodavanje nove klauzule

    global clauses_dict
    global clauses
    global new_clauses
    global absolution
    global final_absolution
    global new_clauses

    # pretvori klauzulu u skup
    new_set = set()
    for element in command[:-1]:
        element = element.strip()
        if(element != "v" or element != "+"):
            new_set.add(element)
        else:
            continue
    new_set.discard("v")

    # ako već klauzula postoji u clauses_dict, ne radi ništa
    for value in clauses_dict.values():
        if value == new_set:
            return

    # izvuci prvu ciljnu klauzulu, stavi ju na kraj dictionaryja, a novu klazulu stavi na njezino mjesto
    element = clauses_dict[absolution]
    clauses_dict[absolution] = new_set
    clauses_dict[len(clauses_dict) + 1] = element

    # ažuriraj kazaljke
    absolution += 1
    final_absolution += 1
    new_clauses += 1

def remove_clause(command):

    # funkcija za uklanjanje klauzule

    global clauses_dict
    global clauses
    global new_clauses
    global absolution
    global final_absolution
    
    # pretvori klauzulu u skup
    new_set = set()
    command = command[:-1]
    for element in command:
        if(element != "v" or element != "-"):
            new_set.add(element)
        else:
            continue
    new_set.discard("v")
    new_clause = ""

    # pretvori klauzulu u string i izbaci ju iz clauses
    for element in command:
        new_clause += element
        new_clause += " v "
    clauses.discard(new_clause[:-3])
    
    # delete je indeks na kojemu je smještena klauzula koju treba ukloniti
    delete = 0
    for key, value in clauses_dict.items():
        if(value == new_set):
            delete = key
            del clauses_dict[key]
            break
    
    # ako klauzula ne postoji, ne radi ništa
    if(delete == 0):
        return

    # iteriraj po dictionaryju i premjesti klauzule da se popuni izbrisani indeks
    length = len(clauses_dict)
    for i in range(delete + 1, length + 2):
        element = clauses_dict[i]
        del clauses_dict[i]
        clauses_dict[i - 1] = element

    # ažuriraj kazaljke
    absolution -= 1
    final_absolution -= 1
    new_clauses -= 1

def resolve_clause(command):

    # funkcija za provjeru novih klauzula

    global clauses
    global clauses_dict
    global resolved
    global new
    global conclusions
    global absolution
    global final_absolution
    global new_clauses
    global teto
    global check

    # spremi stare podatke koji će se poslije obnoviti
    old_values = [clauses.copy(), copy.deepcopy(clauses_dict), resolved.copy(), new.copy(), copy.deepcopy(conclusions), absolution, final_absolution, new_clauses, teto, check]

    # ažuriraj kazaljku, pretvori novu klauzulu u string i spremi ju
    absolution += 1
    new_clause = ""
    for element in command:
        if(element == "?"):
            continue
        new_clause += element
        new_clause += " v "
    teto = new_clause[:-3]

    # negiraj novi cilj
    iterator = absolution
    for part in command:
        tempset = set()
        if(part == "v" or part == "?"):
            continue
        if(part.startswith("~")):
            part = part.lstrip("~")
            tempset.add(part)
            clauses_dict[iterator] = tempset
            iterator += 1
        else:
            part = "~" + part
            tempset.add(part)
            clauses_dict[iterator] = tempset
            iterator += 1

    # ažuriraj kazaljke i pokreni proces zaključivanja
    final_absolution = iterator
    new_clauses = iterator
    resolution = resolution_process()

    # ako NIL nije pronađen, pošalji 0, 0 u funkciju za ispis
    # otherwise, pošalji prave indekse
    if(not resolution):
        printer(0, 0)
    else:
        printer(resolution[0], resolution[1])

    # obnovi stare podatke
    clauses = old_values[0].copy()
    clauses_dict = copy.deepcopy(old_values[1])
    resolved = old_values[2].copy()
    new = old_values[3].copy()
    conclusions = copy.deepcopy(old_values[4])
    absolution = old_values[5]
    final_absolution = old_values[6]
    new_clauses = old_values[7]
    teto = old_values[8]
    check = old_values[9]
            
def main(args):

    # main funkcija

    global check

    # izvuci command line argumente
    command = args.command
    path_clauses = args.path_clauses
    path_cookbook = args.path_cookbook

    # za naredbu resolution i datoteku s klauzulama pokreni resolution_processs
    if(command == "resolution" and path_clauses):
        read_file(path_clauses)
        if(check):
            printer(-1, -1)
            return
        resolution = resolution_process()
        if(not resolution):
            printer(0, 0)
        else:
            printer(resolution[0], resolution[1])

    # za naredbu cooking i datoteku s klauzulama pokreni cooking process
    elif(command == "cooking" and path_clauses and path_cookbook):
        read_file(path_clauses)
        cookbook_process(path_cookbook)
    else:
        print("Neispravan unos")
        return

if __name__ == "__main__":

    parser = argparse.ArgumentParser(description="Ulazni parametri za program")

    # parsiraj argumente s komandne linije
    # path_clauses i path_cookbook se postavljaju u None ako se ne predaju
    # to se mora tako napraviti jer su svi argumenti obavezni
    # omogućuje pozive poput: python solution.py resolution <file path>
    parser.add_argument("command", type=str, nargs = "?", default = None, help="Varijanta programa koja se koristi (resolution ili cooking)")
    parser.add_argument("path_clauses", type=str, nargs = "?", default = None)
    parser.add_argument("path_cookbook", type=str, nargs = "?", default = None)

    args = parser.parse_args()
    main(args)