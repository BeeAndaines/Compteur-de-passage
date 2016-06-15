Le but est de comptabiliser le nombre d'entrées et de sorties des abeilles de la ruche afin d'être prévenu d'un essaimage.
Pour cela : deux capteurs IR sont placés à chaque porte de la ruche, ils permettent à l'aide d'arduino de compter les passages et de distinguer les entrées des sorties. 

# Compteur-de-passage, detecteur IR Infrared reflective Switch IR Barrier Line Track sensor TCRT5000 
on observe un changement de tension à chaque fois que quelque chose passe devant. 
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
