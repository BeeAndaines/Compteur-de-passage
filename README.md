Le but est de comptabiliser le nombre d'entrées et de sorties des abeilles de la ruche afin d'être prévenu d'un essaimage.
Pour cela : deux capteurs IR sont placés à chaque porte de la ruche, ils permettent à l'aide d'arduino de compter les passages et de distinguer les entrées des sorties. 

# Compteur-de-passage, detecteur IR Infrared reflective Switch IR Barrier Line Track sensor TCRT5000.
On observe un changement de tension à chaque fois que quelque chose passe devant. 
On a la possibilité d’enregistrer une tension en analogique 
(1023 quand rien ne perturbe le faisceau, valeur inférieure quand le faisceau est coupé par un passage d’abeille)
ou en digital (mesure 1 quand rien ne coupe le faisceau, 0 quand le faisceau est coupé). 

Les mesures en analogique nécessitent de déterminer un seuil en dessous duquel on compte un passage, 
ce seuil peut varier d’un détecteur à l’autre. 
Les valeurs mesurées en digital sont plus faciles à utiliser car ne nécessitent pas de seuil : c’est la solution retenue. 

Ces détecteurs disposent d’un potentiomètre qui permet d’ajuster la détection en fonction de la distance à laquelle
l’objet est détecté.
Lors de nos essais, un problème est apparu :
quand l’abeille reste devant le détecteur, le faisceau est coupé (état 0)
et plusieurs passages sont comptabilisés. 
Pour y remédier, nous enregistrerons le passage seulement quand le détecteur ne détecte plus rien 
(retour à l’état 1).

Nous utilisons un multiplexeur pour compter les entrées et sorties sur plusieurs portes en même temps.
Analog/Digital MUX Breakout - CD74HC4067
Il permet de connecter 16 detecteurs IR, soit 8 portes. Il a 4 sorties digitales vers Arduino (pour repérer les 16 entrées detecteurs en base 4) et une sortie SIG (qui transmet la valeur du détecteur) à relier à un port analogique.

Nous utilisons une structure (struct)  à laquelle nous affectons des données qui seront utilisées pour détecter les entrées et sorties de chaque porte:
    pinsortie et pinentree pour indiquer le N°d'entrée des 2 détecteurs sur le mux,
    tempsLectureEntree et tempsLectureSortie pour enregistrer l'heure de passage,
    lectureEntree et lectureSortie pour lire la valeur sur le pinentrée = capteur coté entrée ou sur le capteur coté sortie,
    compteurEntree et compteurSortie qui se déclenchent quand on coupe le faisceau, qui se remettent à 0 quand le faisceau n'est plus coupé et qui comptent 1 passage.
En comparant les horaires de passage devant chacun des capteurs, on en déduit qu'il s'agit d'une entrée si tempsLectureEntree > tempsLectureSortie, ou d'une sortie si tempsLectureEntree < tempsLectureSortie.

Nous comptabilisons aussi des erreurs si un seul capteur est déclenché et pas l'autre : une abeille hésite à entrer ou sortir, ou si un capteur reste déclenché trop longtemps, ce qui peut indiquer une abeille stationnant dans la porte, ou une sortie d'abeilles à la queue leu-leu, ce qui peut se produire lors d'un essaimage.

Chaque entrée, sortie et erreur est enregistrée avec la date précise sur SD grace à une horloge RTC DS1307 AT24C32 et un lecteur de carte SD SPI Micro SD Storage Board TF Card Mémoire Shield Module pour Arduino.

La capteur fonctionne au laboratoire et compte le passage d'abeille épinglée sur une aiguille dans un sens comme dans l'autre, plusieur en même temps sur des portes différentes.
Mais la préparation pour l'extérieur rend le dispositif totalement obsolète, les capteurs infrarouge ne voient plus aucun passage ...
Affaire à suivre
