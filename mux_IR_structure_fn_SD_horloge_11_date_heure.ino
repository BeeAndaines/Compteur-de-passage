//variable pour compter les entrées et sorties d'abeilles 
int entrees =0; //pour mettre les compteurs à 0 au démarrage
int sorties =0;

//variable pour compter les erreurs abeilles qui stationnent ou qui se suivent
int erreur =0;

/**
 * Tiny RTC module
 * module horloge
 *
 * DS1307 pour bus I2C
 * avec batterie au lithium CR1225
 *
 * Le port I2C de l'Arduino est situé
 * sur les pin A4 et A5
 *
 * Analog pin A5 <-> SCL
 * Analog pin A4 <-> SDA
 */

#include <Wire.h>
/**
 * Nous utilisons la librairie RTClib
 * cf: https://github.com/adafruit/RTClib/archive/master.zip
 * à installer dans le répertoire libraries
 * du répertoire de sketchs
 */
#include "RTClib.h"
RTC_DS1307 RTC;
// pour ajuster l'heure changer false en : true
bool definirHeure = true;

/**
 * Écriture sur une carte SD
 *
 * SD card reliée au bus SPI :
 * MOSI       - pin 11
 * MISO       - pin 12
 * CLK ou SCK - pin 13
 * CS         - pin 4
 *
 * SPI pour Serial Peripheral Interface
 *
 * created  24 Nov 2010
 * modified 9 Apr 2012
 * by Tom Igoe
 * cf: https://www.arduino.cc/en/Tutorial/Datalogger
 */
#include <SPI.h>
#include <SD.h>

// Arduino Uno pin 4
// cf: https://www.arduino.cc/en/Reference/SPI
const int chipSelect = 4;

//bool sDisReady = false;

//branchement des sorties du multiplexeur qui permettent de récupérer les données des 16 entrées (4X4)
int s0 = 6;
int s1 = 7;
int s2 = 8;
int s3 = 9;

int SIG_pin = A0; //connexion du multiplexeur à l'arduino

#define NBPORTES 3  // à adapter au nombre de portes connectées, jusqu'à 8
#define NBCAPTEURS 2 // 2 capteurs par porte

#define nbPortes 8
/**
 * structure Porte
 * permet d'organiser les infos par porte
 */
struct Porte {
    int  pinsortie; //c'est pour indiquer le N°d'entrée sur le mux (en référence tout à la fin, 
    //et tu lui affectes le channelEntree mais je ne vois pas où il est défini celui-là ???
    int  pinentree;
    long tempsLectureEntree;//enregistre l'heure de passage (equivalent du chronoA)
    long tempsLectureSortie;//chronoB
    int  lectureEntree;//lecture de la valeur sur le pinentrée = capteur coté entrée
    int  lectureSortie;//lecture de la valeur sur le pinsortie = capteur coté sortie
    int  compteurEntree;//c'est l'équivalent du count A qui se déclenche quand on coupe le faisceau, qui se remet à 0 quand le faisceau n'est plus coupé et qui compte 1 passage, peut-êtr qu'il faudrait aussi introduire le passage A et B dans la structure, sinon ces 2 variables ne seront pas affectées à une porte donnée???
    int  compteurSortie; //countB
   
} portes[nbPortes];




void setup() {
// initialisation porte 0
portes[0].pinentree = 0;
portes[0].pinsortie = 1;
portes[0].tempsLectureEntree = 0;
portes[0].tempsLectureSortie = 0;
portes[0].lectureEntree = 1; // 1 quand rien ne coupe le faisceau
portes[0].lectureSortie = 1;
portes[0].compteurEntree = 0;
portes[0].compteurSortie = 0;

// initialisation porte 1
portes[1].pinentree = 2;
portes[1].pinsortie = 3;
portes[1].tempsLectureEntree = 0;
portes[1].tempsLectureSortie = 0;
portes[1].lectureEntree = 1;
portes[1].lectureSortie = 1;
portes[1].compteurEntree = 0;
portes[1].compteurSortie = 0;

// initialisation porte 2
portes[2].pinentree = 4;
portes[2].pinsortie = 5;
portes[2].tempsLectureEntree = 0;
portes[2].tempsLectureSortie = 0;
portes[2].lectureEntree = 1;
portes[2].lectureSortie = 1;
portes[2].compteurEntree = 0;
portes[2].compteurSortie = 0;


  // put your setup code here, to run once:
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);

  digitalWrite(s0, LOW);
  digitalWrite(s1, LOW);
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);

  Serial.begin(9600);
  Serial.println("Start");
  Serial.println("Date horaire sorties entrées erreurs");

   //zone horloge
    Wire.begin();
    RTC.begin();
  if (! RTC.isrunning())
  {
    Serial.println("RTC ne fonctionne pas!");
  }

  if(definirHeure)
  {
      // la ligne suivante permet d'ajuster la date & l'heure
      // à la date de compilation du sketc
      RTC.adjust(DateTime(__DATE__, __TIME__));
  }

//zone SD
  while (!Serial)
    {
    ; // wait for serial port to connect. Needed for native USB port only
 
    }
  
  Serial.println("Initialisation de la carte SD ...");
 
  // on verifie que la carte est présente et peut être initialisée
  if (!SD.begin(chipSelect))
    {
    Serial.println("Carte Sd inaccesible ou absente");
    // don't do anything more:
    return;
    }
  Serial.println("Carte OK");
}

void loop() {
  // programme qui tourne en boucle

    //temps est une variable qui enregitre le temps au moment où on l'appelle
    unsigned long temps =millis();
    for (int numPorte=0; numPorte<NBPORTES; numPorte++)
    {
        int channelEntree = portes[numPorte].pinentree;
        portes[numPorte].lectureEntree = readMux(channelEntree);

        int channelSortie = portes[numPorte].pinsortie;
        portes[numPorte].lectureSortie = readMux(channelSortie);

     
        //on compte le nombre de passage devant le capteur, passé à 0
        if(portes[numPorte].lectureEntree == 0)
        {
            portes[numPorte].compteurEntree++;
             //si le temps de séjour devant un capteur est trop long (valeur à ajuster), 
            //nous incrémentons le compteur d'erreur et remettons le compteur de temps à0
              if(portes[numPorte].compteurEntree>5000)
                {
                erreur++;
                portes[numPorte].compteurEntree=0;
             
        String dataString = "";
            
        dataString += lectureDate();
        dataString += ";"; 
        dataString += String(sorties);
        dataString +=  ";";
        dataString += String(entrees);
        dataString +=  ";";  
        dataString += String(erreur);
        dataString +=  ";";
      /**
       * nous ouvrons le fichier
       * Nb: un seul fichier peut être ouvert à la fois
       * le fichier se nomme : journal.csv
       */
      File dataFile = SD.open("journal.csv", FILE_WRITE);
      // si le fichier est disponible, nous écrivons dedans :
      if (dataFile) {
        dataFile.println(dataString);
        dataFile.close(); 
        Serial.println(dataString);
             }
              
                }
        }

        if(portes[numPorte].lectureSortie == 0)
        {
            portes[numPorte].compteurSortie++;
            //si le temps de séjour devant un capteur est trop long (valeur à ajuster), 
            //nous incrémentons le compteur d'erreur et remettons le compteur de temps à0
            if(portes[numPorte].compteurSortie>5000)
                {
                erreur++;
                portes[numPorte].compteurSortie=0;
                 
        String dataString = "";
            
        dataString += lectureDate();
        dataString += ";"; 
        dataString += String(sorties);
        dataString +=  ";";
        dataString += String(entrees);
        dataString +=  ";";  
        dataString += String(erreur);
        dataString +=  ";";
      /**
       * nous ouvrons le fichier
       * Nb: un seul fichier peut être ouvert à la fois
       * le fichier se nomme : journal.csv
       */
      File dataFile = SD.open("journal.csv", FILE_WRITE);
      // si le fichier est disponible, nous écrivons dedans :
      if (dataFile) {
        dataFile.println(dataString);
        dataFile.close(); 
        Serial.println(dataString);
             }    
                }
        }

        // si nous n'avons plus de passage (capteur à 1)
        if(portes[numPorte].lectureEntree == 1)
        {
            // et que nous avons eu plusieurs lecture positive
            if(portes[numPorte].compteurEntree > 1)
            {
                // alors nous remettons notre compteur à 0
                portes[numPorte].compteurEntree = 0;
                // nous pouvons stocker le temps au passage:
                portes[numPorte].tempsLectureEntree = temps;
            }
        }
        // idem pour la porte pres de la sortie
        // si nous n'avons plus de passage (capteur à 1)
        if(portes[numPorte].lectureSortie == 1)
        {
            // et que nousa avons eu plusieurs lecture positive
            if(portes[numPorte].compteurSortie > 1)
            {
                // alors remettons notre compteur à 0
                portes[numPorte].compteurSortie = 0;
                // nous pouvons stocker le temps au passage:
                portes[numPorte].tempsLectureSortie = temps;
            }
        }

        // verification du temps de passage :

        //on ne compte que si les 2 capteurs sont déclenchés avec un écart proche 2s par ex, 
        // à adapter en fonction du temps que met une abeille à franchir la porte
        // si l'écart est plus grand, on remet à 0 le 1er compteur, celui qui a le plus petit chrono
 
 if (portes[numPorte].tempsLectureEntree > portes[numPorte].tempsLectureSortie 
     && portes[numPorte].tempsLectureEntree - portes[numPorte].tempsLectureSortie > 2000)
    {
      portes[numPorte].tempsLectureSortie=0;
    }
 
 if (portes[numPorte].tempsLectureEntree < portes[numPorte].tempsLectureSortie 
     && portes[numPorte].tempsLectureSortie - portes[numPorte].tempsLectureEntree > 2000)
    {
      portes[numPorte].tempsLectureEntree=0;
    }
 
  else if (portes[numPorte].tempsLectureEntree - portes[numPorte].tempsLectureSortie <2000 
           || portes[numPorte].tempsLectureSortie - portes[numPorte].tempsLectureEntree <2000)
       {
        
        if(portes[numPorte].tempsLectureEntree > portes[numPorte].tempsLectureSortie && portes[numPorte].tempsLectureSortie!=0)
        {
            // passée devant capteur entree avant celui de sortie: elle est sortie
            sorties++;
                
        String dataString = "";
            
        dataString += lectureDate();
        dataString += ";"; 
        dataString += String(sorties);
        dataString +=  ";";
        dataString += String(entrees);
        dataString +=  ";";  
        dataString += String(erreur);
        dataString +=  ";";
      /**
       * nous ouvrons le fichier
       * Nb: un seul fichier peut être ouvert à la fois
       * le fichier se nomme : journal.csv
       */
      File dataFile = SD.open("journal.csv", FILE_WRITE);
      // si le fichier est disponible, nous écrivons dedans :
      if (dataFile) {
        dataFile.println(dataString);
        dataFile.close(); 
        Serial.println(dataString);
             } 
 
              //et on remet les chrono à 0 pour ne pas utiliser 2 fois ce passage
              portes[numPorte].tempsLectureEntree =0;
              portes[numPorte].tempsLectureSortie =0;        
        }
              
        else if(portes[numPorte].tempsLectureEntree < portes[numPorte].tempsLectureSortie && portes[numPorte].tempsLectureEntree!=0)
        {
            entrees++;
                
        String dataString = "";
            
        dataString += lectureDate();
        dataString += ";"; 
        dataString += String(sorties);
        dataString +=  ";";
        dataString += String(entrees);
        dataString +=  ";";  
        dataString += String(erreur);
        dataString +=  ";";
      /**
       * nous ouvrons le fichier
       * Nb: un seul fichier peut être ouvert à la fois
       * le fichier se nomme : journal.csv
       */
      File dataFile = SD.open("journal.csv", FILE_WRITE);
      // si le fichier est disponible, nous écrivons dedans :
      if (dataFile) {
        dataFile.println(dataString);
        dataFile.close(); 
        Serial.println(dataString);
             }
              //et on remet les chrono à 0
              portes[numPorte].tempsLectureEntree =0;
              portes[numPorte].tempsLectureSortie =0; 
        }
    }
  }
}

//readMux est la fonction qui lit chaque capteur appelé par readMux

int readMux(int channel){
  int controlPin[] = {s0, s1, s2, s3};

  int muxChannel[16][4]={
    {0,0,0,0}, //channel 0
    {1,0,0,0}, //channel 1
    {0,1,0,0}, //channel 2
    {1,1,0,0}, //channel 3
    {0,0,1,0}, //channel 4
    {1,0,1,0}, //channel 5
    {0,1,1,0}, //channel 6
    {1,1,1,0}, //channel 7
    {0,0,0,1}, //channel 8
    {1,0,0,1}, //channel 9
    {0,1,0,1}, //channel 10
    {1,1,0,1}, //channel 11
    {0,0,1,1}, //channel 12
    {1,0,1,1}, //channel 13
    {0,1,1,1}, //channel 14
    {1,1,1,1}, //channel 15
  };

   //loop through the 4 sig
  for(int i = 0; i < 4; i ++){
    digitalWrite(controlPin[i], muxChannel[channel][i]);
  }

  //read the value at the SIG pin
  int val = digitalRead(SIG_pin);

  //return the value
  return val;

}// fin de readmux


  
  /**
 * fonction qui construit une chaine=lecture de la date pour libreoffice 
 */
String lectureDate()
{
  DateTime now = RTC.now();
  String dateString = "";
  
  int day = now.day();
  if(day<10)
  {
    dateString += "0";
  }
  dateString += day;
  dateString += "/";
  
  int month = now.month();
  if(month<10)
  {
    dateString += "0";
  }
  dateString += month;

  dateString += "/";
  int year = now.year();
  dateString += year;

  dateString +=  ";";  
  
  int hour = now.hour();
  if(hour<10)
  {
    dateString += "0";
  }
  dateString += hour;

  dateString += ":";
  int minute = now.minute();
  if(minute<10)
  {
    dateString += "0";
  }
  dateString += minute;

  dateString += ":";
  int second = now.second();
  if(second<10)
  {
    dateString += "0";
  }
  dateString += second;

  return dateString;
} //fin de  lectureDate
