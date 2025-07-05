import argparse
import numpy as np

# klasa koja modelira sloj neurona neuronske mreže
class Neuron_Layer:
    def __init__(self, inputs, number, final):
        self.inputs = inputs    # broj ulaza, tj. broj varijabli u ulaznim podatcima
        self.number = number    # broj neurona u sloju

        # generiraj matricu težina za trenutni sloj, vrijednosti uzorkovane iz normalne razdiobe s disperzijom 0.01
        # dimenzije matrice: broj ulaza x broj neurona u sloju
        self.weights = np.random.normal(loc=0.0, scale=0.01, size=(inputs, number))

        # vektor pragova (biases), uzorkovanih iz normalne razdiobe s disperzijom 0.01
        self.bias = np.random.normal(loc=0.0, scale=0.01, size=(1, number))

        # final je zastavica koja označava radi li se o zadnjem neuronu
        self.final = final

# klasa koja modelira neuronsku mrežu
class Network:
    def __init__(self, neurons, p, K):
        self.neurons = neurons  # lista slojeva neurona, jedan element = jedan sloj
        self.goodness = 0       # faktor dobrote mreže, računa se pomoću formule kvadratne pogreške
        self.p = p              # vjerojatnost mutacije elemenata kromosoma, dana iz komandne linije
        self.K = K              # disperzija Gaussove razdiobe koju koristimo u mutaciji, dana iz komandne linije

    # funkcija za treniranje neuronske mreže
    # pomoć oko programiranja algoritma treniranja: https://short-link.me/101Sk
    def train(self, data, results):

        # predictions je zasad prazan, u njega će se spremiti vrijednosti koje predvidi neuronska mreža
        predictions = None

        # početna vrijednost values su vrijednosti iz datoteke
        values = data

        # provuci podatke kroz sve slojeve neuronske mreže
        for neuron in self.neurons:
            new_values = np.dot(values, neuron.weights)     # množenje s težinama
            new_values = np.add(new_values, neuron.bias)    # pribrajanje pragova

            # ako trenutni sloj nije zadnji, provuci podatke kroz prijenosnu funkciju
            if(not neuron.final):
                new_values = self.transition_function(new_values)
                values = new_values     # postavi nove vrijednosti za sljedeći sloj neurona
            else:
                # ako je trenutni sloj zadnji, postavi trenutne nove vrijednosti kao predictions
                predictions = new_values
       
       # dobrota se računa preko funkcije kvadratne pogreške
        self.goodness = np.mean((results - predictions) ** 2)

        return self.goodness
    
    def transition_function(self, data):
        # prijenosna funkcija -> logistička sigmoida
        return 1 / (1 + np.exp(-data))
    
    # funkcija mutacije kromosoma    
    def mutate(self):
        for layer in self.neurons:
            # za svaki sloj neurona stvori masku za matricu težina, s obzirom na vjerojatnos p
            mask = np.random.rand(*layer.weights.shape) < self.p

            # matrica mutacija se dobiva uzorkovanjem iz normalne razdiobe sa zadanom disperzijom K
            noise = np.random.normal(loc=0.0, scale=self.K, size=layer.weights.shape)
            # dobivena ,,mutacija" (noise) se pribraja elementima koji su označeni sa True
            layer.weights[mask] += noise[mask]
            
            # ponovi postupak za pragove
            mask = np.random.rand(*layer.bias.shape) < self.p
            noise = np.random.normal(loc=0.0, scale=self.K, size=layer.bias.shape)
            layer.bias[mask] += noise[mask]

    # funkcija za testiranje neuronske mreže
    # funkcionira na potpuno isti način kao i funkcija za treniranje
    def test(self, test_data, test_results):
        predictions = None
        values = test_data
        for neuron in self.neurons:
            new_values = np.dot(values, neuron.weights)
            new_values = np.add(new_values, neuron.bias)
            if(not neuron.final):
                new_values = self.transition_function(new_values)
                values = new_values
            else:
                predictions = new_values
        
        self.goodness = np.mean((test_results - predictions) ** 2)

        return self.goodness

# funkcija za križanje kromosoma
def crossover(mother, father):

    # stvori listu novih slojeva mreže
    # iz roditelja izvuci njihove težine i pragove
    # izračunaj aritmetičku sredinu i stvori novi objekt sloja
    # vrati novu mrežu sa stvorenim slojevima

    new_layers = []
    for m_layer, f_layer in zip(mother.neurons, father.neurons):
        weights = (m_layer.weights + f_layer.weights) / 2
        bias = (m_layer.bias + f_layer.bias) / 2
        layer = Neuron_Layer(m_layer.inputs, m_layer.number, m_layer.final)
        layer.weights = weights.copy()
        layer.bias = bias.copy()
        new_layers.append(layer)
    return Network(new_layers, mother.p, mother.K)

# funkcija main
def main(args):

    # izvuci podatke koji su predani preko komandne linije
    train = args.train
    test = args.test
    nn = args.nn.split("s")
    popsize = args.popsize
    elitism = args.elitism
    p = args.p
    K = args.K
    iterations = args.iter

    # parsiraj liniju s veličinama slojeva mreže
    network_size = []
    for number in nn:
        if number == "":
            continue
        network_size.append(int(number))

    # parsiraj datoteku za treniranje
    lines = np.loadtxt(train, delimiter=",", skiprows=1)
    
    # u data spremi podatke, a u results zadnji stupac (vrijednosti cilja)
    data = lines[:, :-1]
    results = lines[:, -1].reshape(-1, 1)   # reshape je potreban kako bi results bili matrica, a ne vektor
    
    # stvori onoliko mreža koliko smo zadali preko popsize
    networks = []
    for i in range(0, popsize):
        layers = []
        inputs = len(data[0])

        # stvori slojeve neurona
        for number in network_size:
            layer = Neuron_Layer(inputs, number, False)
            inputs = number
            layers.append(layer)
        
        # stvori zadnji neuron i spremi ga u listu slojeva
        layers.append(Neuron_Layer(number, 1, True))
        web = Network(layers, p, K)

        # novostvorenu mrežu dodaj u listu mreža
        networks.append(web)

    networks = np.array(networks)
    # provodi algoritam onoliko puta koliko smo zadali
    for i in range(1, iterations + 1):

        # istreniraj svaku mrežu jednom i spremi dobrote u listu
        lovely_goodness = []
        for network in networks:
            goodness = network.train(data, results)
            lovely_goodness.append((goodness, network))
        
        # vrijednosti dobrota su vrijednosti pogreške
        # manja pogreška -> veća dobrota
        # sortiraj uzlazno listu dobrota
        sorted_goodness = sorted(lovely_goodness, key=lambda x: x[0])

        # u new_networks spremi one mreže koje nepromijenjene idu u sljedeću iteraciju
        new_networks = [item[1] for item in sorted_goodness[:elitism]]

        # u fitness spremi sortirane dobrote
        fitness = np.array([item[0] for item in sorted_goodness])

        # za točan izračun vjerojatnosti, potrebno je pretvoriti dobrote u padajuće vrijednosti
        # te im pribrojiti epsilon u slučaju da je koja od njih jednaka nuli
        epsilon = 1e-8
        inv_fitness = 1 / (fitness + epsilon)

        # ukupna dobrota je zbroj svih korigiranih dobrota
        total_inv = np.sum(inv_fitness)

        # ako je zbroj svih dobrota jednak nuli, stvori novu listu popunjenu istim vrijednostima
        if total_inv == 0:
            probabilities = np.full_like(inv_fitness, 1 / len(inv_fitness))
        else:
            # ako nije, vjerojatnost je količnik korigirane dobrote i ukupne korigirane dobrote
            probabilities = inv_fitness / total_inv

        # svakih 2000 iteracija ispiši pogrešku
        if(i % 2000 == 0):
            print(f"[Train error @{i}]: {sorted_goodness[0][0]:.6f}")

        used = set()
        # stvori ostatak populacije tako da križaš roditelje
        for j in range(elitism, len(networks)):
            parents = False
            mother = None
            father = None

            # biraj roditelje dok ne nađeš dvoje koji još nisu bili zajedno
            while not parents:
                selected_index1 = np.random.choice(len(fitness), p=probabilities)
                parent1 = networks[selected_index1]

                selected_index2 = np.random.choice(len(fitness), p=probabilities)
                parent2 = networks[selected_index2]

                if((selected_index1, selected_index2) not in used):
                    mother = parent1
                    father = parent2
                    parents = True

                    # dodaj par indexa u skup kako bismo znali da smo ih već odabrali
                    used.add((selected_index1, selected_index2))
                    break                
            
            # dijete nastaje križanjem roditelja te se sprema u listu novih mreža
            child = crossover(mother, father)
            new_networks.append(child)

        new_networks = np.array(new_networks)

        # za svaku mrežu, osim elite, provedi mutaciju kromosoma
        for network in new_networks[elitism:]:
            network.mutate()

        # postavi nove mreže za novu iteraciju
        networks = new_networks
    
    # učitavanje datoteke za testiranje na isti način kao i za treniranje
    lines = np.loadtxt(test, delimiter=",", skiprows=1)
    data = lines[:, :-1]
    results = lines[:, -1].reshape(-1, 1)

    # odaberi najbolju mrežu i testiraj nju
    favorite_child = networks[0]
    expectation = favorite_child.test(data, results)

    # ispiši pogrešku testiranja
    print(f"[Test error]: {expectation:.6f}")
 
if __name__ == "__main__":

    # parsiranje argumenata iz komandne linije
    parser = argparse.ArgumentParser(description="Ulazni parametri za program")
    
    # datoteka za treniranje
    parser.add_argument("--train", type=str, required=True)

    # datoteka za testiranje
    parser.add_argument("--test", type=str, required=True)

    # konfiguracija mreže
    parser.add_argument("--nn", type=str, required=True)

    # broj mreža koje trebamo trenirati i testirati
    parser.add_argument("--popsize", type=int, required=True)

    # broj mreža koje nepromijenjene predajemo u daljnje iteracije
    parser.add_argument("--elitism", type=int, required=True)

    # vjerojatnost mutacija
    parser.add_argument("--p", type=float, required=True)

    # disperzija normalne razdiobe
    parser.add_argument("--K", type=float, required=True)

    # broj iteracija
    parser.add_argument("--iter", type=int, required=True)

    args = parser.parse_args()
    main(args)