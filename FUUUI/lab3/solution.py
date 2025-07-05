import argparse
from collections import deque
import math


# klasa koja modelira čvorove stabla
class TreeNode:
    def __init__(self, value, prob):
        self.value = value
        self.prob = prob
        self.children = []

    def add_child(self, child_node):
        self.children.append(child_node)

# klasa za algoritam strojnog učenja
class ID3:
    def __init__(self, hyperparameter = 0):

        # hyperparameter postaje ograničenje dubine stabla, ako se ne preda postavlja se na nulu
        # u self.values spremaju se obilježja i njihove značajke
        # u self.categories sprema se lista obilježja
        # u self.final spremaju se vrijednosti ciljnog obilježja
        # u self.lines spremaju se linije iz datoteke za treniranje
        # u self.tree sprema se ID3 stablo nakon izgradnje
        self.depth = hyperparameter
        self.categories = []
        self.values = dict()
        self.final = set()
        self.tree = None
        self.lines = []
        self.tree = None

    def fit(self, training_data):

        # funkcija za treniranje algoritma i izgradnju stabla

        # učitaj datoteku
        lines = None
        with open(training_data, "r") as file:
            lines = file.readlines()

        # spremi prvi red u self.categories kao listu
        self.categories = [category.strip() for category in lines[0].split(",")]

        # inicijaliziraj rječnik za vrijednosti obilježja
        for category in self.categories:
            self.values[category.strip()] = set()

        # iteriraj po linijama i učitaj podatke
        # prva linija se preskače jer sadržava samo imena obilježja
        for line in lines[1:]:
            i = 0
            elements = line.strip().split(",")

            # u self.lines se dodaje lista vrijednosti obilježja
            self.lines.append(elements)

            # u self.final se dodaje vrijednost ciljnog obilježja
            self.final.add(elements[-1].strip())
            for element in elements[:-1]:
                
                # za svaki element linije (osim zadnjeg) spremi u vrijednosti odgovarajućeg obilježja
                self.values[self.categories[i]].add(element)
                i += 1

        # ako dubina nije zadana, pokreni osnovnu inačicu algoritma
        if(self.depth == 0):
            self.tree = self.build_tree(self.lines, self.lines, self.categories[:-1])
        # ako je zadana, pokreni inačicu algoritma s ograničenjem dubine
        else:
            self.tree = self.build_tree_restricted(self.lines, self.lines, self.categories[:-1], self.depth)

    def build_tree(self, d, dparent, categories):

        # rekurzivna funkcija za izgradnju stabla

        # ako je d prazan (ne postoje linije iz datoteke koje zadovoljavaju odabir obilježja), 
        # vrati najčešću vrijednost ciljnog obilježja iz roditeljskog čvora
        if not d:
            v = self.argmax(dparent)
            return TreeNode(v, None)
        
        # pronađi najčešću vrijednost ciljnog obilježja za trenutni čvor
        # provjeri jesu li možda vrijednosti ciljnog obilježja za sve značajke u čvoru iste
        v = self.argmax(d)
        check = self.check(d)

        # ako je lista obilježja prazna (već smo obradili sva obilježja)
        # ili ako smo otkrili da su sve vrijednosti ciljnog obilježja u čvoru iste
        # vrati najčešću vrijednost ciljnog obilježja u trenutnom čvoru
        if not categories or check:
            return TreeNode(v, None)
        
        # pronađi koje obilježje ima najveći information gain
        returned = self.argmax_IG(d, categories)

        # to obilježje spremi u x
        x = returned[0]
        
        # iz liste obilježja makni x
        new_cat = categories.copy()
        new_cat.remove(x)
        subtrees = []
        
        # iteriraj po vrijednostima obilježja x
        for v in sorted(self.values[x]):
            
            # prilagodi d tako da sadržava linije koje zadovoljavaju vrijednosti obilježja
            d_new = self.initiate_D(d, v, x)
            
            # pozovi rekurzivnu funkciju s novim parametrima
            t = self.build_tree(d_new, dparent, new_cat)

            # stvori čvor koji sadrži vrijednost obilježja
            node = TreeNode(v, None)
            node.add_child(t)

            # spremi čvor u listu podstabala
            subtrees.append(node)
        
        # stvori čvor koji sadrži samo obilježje i dodaj mu djecu
        root = TreeNode(x, returned[1])
        for child in subtrees:
            root.add_child(child)

        # vrati stvoreni čvor zajedno s njegovom djecom
        return root

    def build_tree_restricted(self, d, dparent, categories, depth):
        
        # rekurzivna funkcija za algoritam s ograničavanjem dubine
        # vrijedi sve kao i u osnovnoj inačici algoritma
        # osim što se u svakom rekurzivnom pozivu dubina smanjuje za jedan
        # kada dubina dođe do nule, vrati najčešću vrijednost ciljnog obilježja u trenutnom čvoru

        if not d:
            # print("NOT D")
            v = self.argmax(dparent)
            return TreeNode(v, None)
        
        v = self.argmax(d)
        check = self.check(d)
        if (not categories) or (depth == 0) or check:
            return TreeNode(v, None)
        
        returned = self.argmax_IG(d, categories)
        x = returned[0]
        new_cat = categories.copy()
        new_cat.remove(x)
        subtrees = []
        for v in sorted(self.values[x]):
            d_new = self.initiate_D(d, v, x)
            t = self.build_tree_restricted(d_new, dparent, new_cat, depth - 1)
            node = TreeNode(v, None)
            node.add_child(t)
            subtrees.append(node)
        
        root = TreeNode(x, returned[1])
        for child in subtrees:
            root.add_child(child)

        return root

    def initiate_D(self, d, v, x):
        
        # funkcija koja prilagođava skup d po novoj vrijednosti obilježja v

        d_new = []

        # index u listi obilježja označava o kojem se obilježju radi
        index = self.categories.index(x)
        for entry in d:
            if(v == entry[index]):
                d_new.append(entry)
        
        return d_new

    def check(self, d):

        # funkcija koja provjerava nalazi li se u skupu d samo jedna vrijednost ciljnog obilježja

        old = None
        for entry in d:
            checking = entry[-1]
            if(checking != old and old != None):
                return False
            old = checking
        
        # ako nismo našli dvije različite vrijednosti, vrati True
        return True

    def information_gain(self, total, d, category):
        
        # pomoćna vrijednost za izračun informacijske dobiti

        ig = 0
        values = self.values[category]
        index = self.categories.index(category)
        help = dict()
        entropy = 0

        # izračunaj entropiju samog obilježja
        for entry in d:
            checking = entry[-1]
            help[checking] = help.get(checking, 0) + 1

        # nađi i koja je vrijednost ciljnog obilježja najčešća
        old_value = 0
        old_key = ""
        for key, value in help.items():
            if(value > old_value):
                old_value = value
                old_key = key

            # ako su vrijednosti iste, vrati onu koja je po abecednom redu prije druge
            elif(value == old_value):
                if(key < old_key):
                    old_key = key

            # izračun entropije za samo obilježje
            prob = value/total
            entropy -= prob*math.log2(prob)

        # očisti dictionary u koji smo spremali broj pojedinih vrijednosti ciljnog obilježja u skupu d
        help.clear()

        # za svaku vrijednost obilježja izračunaj entropiju
        count = 0
        for entry in values:
            for element in d:
                if(entry == element[index]):
                    checking = element[-1]
                    help[checking] = help.get(checking, 0) + 1

                    count += 1
                else:
                    continue

            # pribroji entropiju obilježja ukupnoj informacijskog dobiti, otežanu sa value/count
            # value/count -> broj pojedine vrijednosti ciljnog obilježja podijeljen s ukupnim brojem linija koje odgovaraju danim vrijednostima obilježja
            holp = 0
            for key, value in help.items():
                if(value == 0):
                    continue
                prob = value/count
                holp -= prob*math.log2(prob)
            ig += count/total * holp

            # očisti help, resetiraj count, i idi dalje
            help.clear()
            count = 0

        # izračunaj konačnu informacijsku dobit        
        entropy -= ig

        # vrati informacijsku dobit i vrijednost najčešćeg ciljnog obilježja u trenutnom čvoru
        return entropy, old_key
    
    def argmax_IG(self, d, categories):

        # funkcija za izračun argmaxa na temelju informacijske dobiti

        total = len(d)
        gainz = []
        
        # izračunaj informacijsku dobit za svako preostalo obilježje
        for category in categories:
            gain = self.information_gain(total, d, category)
            gainz.append([category, float(gain[0]), gain[1]])

        # ispiši pojedine informacijske dobiti
        for goon in gainz:
            print(f"IG({goon[0]}): {goon[1]:.4f}")
        print("\n")

        # pronađi najveću informacijsku dobit
        # ako su dvije dobiti jednake, uzmi ono obilježje koje dolazi prije drugoga po abecedi
        old_value = -5
        old_category = ""
        old_label = ""
        for entry in gainz:
            if(entry[1] > old_value):
                old_value = entry[1]
                old_category = entry[0]
                old_label = entry[2]
            elif(entry[1] == old_value):
                if(entry[0] < old_category):
                    old_category = entry[0]
                    old_label = entry[2]
        
        return old_category, old_label

    def argmax(self, d):

        # funkcija argmax za ciljna obilježja i dani skup d

        help = dict()

        for entry in d:
            checking = entry[-1]
            help[checking] = help.get(checking, 0) + 1

        old_key = ""
        old_value = 0

        # probađi najveću vrijednost
        # ako su dvije vrijednsti iste, vrati ono obilježje koje prvo dolazi po abecedi
        for key, value in help.items():
            if(value > old_value):
                old_value = value
                old_key = key
            elif(value == old_value):
                if(key < old_key):
                    old_key = key

        return old_key
 
    def predict(self, testing_data):

        # funkcija za testiranje modela
        # prima putanju do datoteke s testnim primjerima

        # u predictions se spremaju rezultati algoritma, a u real_results stvarne vrijenosti
        predictions = []
        real_results = [] 

        # učitaj sadržaj datoteke
        lines = None
        with open(testing_data, "r") as file:
            lines = file.readlines()

        # za svaku liniju prođi izgrađenim stablom
        for line in lines[1:]:
            loine = line.strip().split(",")

            # u real_results spremi stvarnu vrijednost (koja će se uvijek nalaziti na kraju linije)
            real_results.append(loine[-1])

            # obilazak stabla počinjemo stavljanjem korijena u red
            queue = deque()
            queue.append(self.tree)
            while queue:
                node = queue.pop()
                name = node.value

                # ako čvor nema djece, znači da smo došli do vrijednosti ciljnog obilježja
                # spremi vrijednost tog čvora u predictions
                if(not node.children):
                    predictions.append(name)
                    break

                # index u self.cateogries označava o kojem se obilježju radi
                index = self.categories.index(name)
                next = loine[index]
                kid = None

                # provjeri nalazi li se vrijednost u vrijednostima danog obilježja
                for child in node.children:
                    if(child.value == next):
                        kid = child.children[0]
                
                # ako vrijednost postoji, u red stavi njegovo dijete (koje će biti novo obilježje)
                if(kid):
                    queue.append(kid)
                # ako ne postoji, vrati najčešću vrijednost ciljnog obilježja u trenutnom čvoru
                else:
                    predictions.append(node.prob)
                    break

        # za lakše vraćanje vrijednosti, spremi predictions i stvarne rezultate u dictionary
        help = dict()
        help["predictions"] = predictions
        help["real"] = real_results

        return help

    def print_tree(self):

        # funkcija za ispis stabla

        print("[BRANCHES]:")

    	# ispis je zapravo DFS obilazak stabla
        # deuque koristimo kao stog
        stacc = deque()
        for child in self.tree.children:
            message = f"1:{self.tree.value}={child.value}"

            # elementi stoga su trojke <trenutna dubina>, <sljedeći čvor>, <poruka za ispis>
            element = [1, child, message]
            stacc.append(element)

        while stacc:
            element = stacc.pop()
            message = element[2]
            chile = element[1]
            level = element[0]

    	    # ako dijete nema djece, znači da smo došli do lista
            # stavi vrijednost lista u message i ispiši ga
            if not chile.children[0].children:
                message += f" {chile.children[0].value}"
                print(message)
                continue
            
            # dijete vrijednosti će biti novo obilježje
            # svako dijete novog obilježja stavi na stog, uz prilagođeni message
            category = chile.children[0]
            children = category.children
            for child in children:
                slur = message + f" {level + 1}:{category.value}={child.value}"
                word = [level + 1, child, slur]
                stacc.append(word)

def main(args):

    # main funkcija

    # izvuci argumente predane u komandnoj liniji
    training_data = args.training_data
    testing_data = args.testing_data
    hyperparameter = args.hyperparameter

    # stvori model
    model = ID3(hyperparameter)

    # istreniraj model
    model.fit(training_data)

    # testiraj model
    values = model.predict(testing_data)

    # iz vraćenog dictionaryja izvuci podatke
    predictions = values["predictions"]
    real_values = values["real"]

    total = len(real_values)
    correct = 0

    # prebroji koliko točnih rezultata ima
    for i in range(0, total):
        if(predictions[i] == real_values[i]):
            correct += 1

    # izračunaj točnost
    accuracy = correct/total

    # stvori matricu zbunjenosti
    final = sorted(model.final)
    size = len(final)
    confusion_matrix = [[0 for _ in range(size)] for _ in range(size)]

    # popuni matricu zbunjenosti
    for i in range(0, len(real_values)):
        joestar = predictions[i]
        kars = real_values[i]

        # nađi indekse vrijednosti iz predictions i real_values
        index = final.index(joestar)
        hr = final.index(kars)

        # na odgovarajućem mjestu povećaj vrijednost 
        confusion_matrix[hr][index] += 1        

    # ispiši stablo
    model.print_tree()

    # ispiši rezultate algoritma, točnost i matricu zbunjenosti
    print("[PREDICTIONS]: ", " ".join(predictions))
    print(f"[ACCURACY]: {accuracy:.5f}")
    print("[CONFUSION_MATRIX]:")
    for row in confusion_matrix:
        print(" ".join(str(x) for x in row))

if __name__ == "__main__":

    parser = argparse.ArgumentParser(description="Ulazni parametri za program")
    
    # parsiraj argumente iz komandne linije
    # training_data -> datoteka s podatcima za treniranje
    # testing_data -> datoteka s podatcima za testiranje
    parser.add_argument("training_data", type=str, nargs = "?", default = None)
    parser.add_argument("testing_data", type=str, nargs = "?", default = None)
    parser.add_argument("hyperparameter", type=int, nargs = "?", default = 0)

    args = parser.parse_args()
    main(args)