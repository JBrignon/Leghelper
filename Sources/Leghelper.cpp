////////////////////////////////////////////////////////////////////////////
//-------2015 - Interface de commande du Leghelper - Julien BRIGNON-------//
/////////////////////////////////////////////////////////////////---GPL---//
//Using C++11, Compile with "g++ Leghelper.cpp -std=c++11  -lwiringPi" /////
//Inclusion des librairies
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <wiringPi.h>
#include <mcp23017.h>
#include <thread>
#include <array>
#include <fstream>
#include <string.h>
#include <vector>
//Inclusion des fichiers de communication niveau machine
#include "Stepper.h"
#include "Droper.h"
//Definition de la structure pour les pins
struct GPIO_ROOT
{
	Driver::GPIO_SS2000MD4 pin_axe_X;
	Driver::GPIO_SS2000MD4 pin_axe_Y;
	Droper::GPIO_VERIN pin_verin;
	int SFX;
	int SFY;
};
//Definition des constantes
//Declaration de k, conversion distance/pas
#define kx 600 //selon x
#define ky 600//selon y
#define kz 35 //selon z
//Declaration des constantes de taille de la matrice [z][y][x]
#define m 2 //selon z
#define n 8 //selon y
#define p 8 //selon x
//Declaration des constantes de position initiale
#define PX 7000
#define PY 3000
//Declaration des parametre du mcp23017
#define pinBase 100      //Definition des numeros de pin affectes au mcp23017
#define i2cAddress 0x20  //Definition de l'adresse du mcp23017
//Declaration de la constante de frequence
#define speed 200
//Inclusion du fichier de Fonction
#include "Brain.h"
//PROGRAMME PRINCIPAL :
int config()
{
	//Affichage
	std::cout << '\n' << "///////////////Leghelper V1.4-////////////////00   0000 00   0000////" << '\n';
	std::cout <<         "//Alors on se trompe de PINS (#On est nul) ?//00   000  0  0 0  0////" << '\n';
	std::cout <<         "///////////////////////////////////CONFIG/////0000 0000 0000 0000////" << '\n' << '\n';
	//Initialisation du buffer
	int buffer;
	//Ouverture du fichier de stream
	std::ofstream fichier("Config.txt",std::ios::trunc);
	//Ecriture des pins MOTEUR
	std::cout << "ETAPE 1 : Configuration des MOTEURS" << '\n'<< '\n';
	std::cout << "Axe X STEP : ";
	std::cin >> buffer;
	fichier << buffer << '\n';
	std::cout << "Axe X DIR  : ";
	std::cin >> buffer;
	fichier << buffer << '\n';
	std::cout << "Axe X AWO  : ";
	std::cin >> buffer;
	fichier << buffer << '\n';
	std::cout << '\n';
	std::cout << "Axe Y STEP : ";
	std::cin >> buffer;
	fichier << buffer << '\n';
	std::cout << "Axe Y DIR  : ";
	std::cin >> buffer;
	fichier << buffer << '\n';
	std::cout << "Axe Y AWO  : ";
	std::cin >> buffer;
	fichier << buffer << '\n';
	std::cout << '\n';
	std::cout << "Capteur Axe X  : ";
	std::cin >> buffer;
	fichier << buffer << '\n';
	std::cout << "Capteur Axe Y  : ";
	std::cin >> buffer;
	fichier << buffer << '\n';
	std::cout << '\n';
	//Ecriture des pins VERIN
	std::cout << "ETAPE 2 : Configuration de la PNEUMATIQUE" << '\n'<< '\n';
	std::cout << "Verin 1 (Avance)   : ";
	std::cin >> buffer;
	fichier << buffer << '\n';
	std::cout << "Verin 1 (Arriere)  : ";
	std::cin >> buffer;
	fichier << buffer << '\n';
	std::cout << "Capteur 1 (Avant)  : ";
	std::cin >> buffer;
	fichier << buffer << '\n';
	std::cout << '\n';
	std::cout << "Verin 2 (Avance)   : ";
	std::cin >> buffer;
	fichier << buffer << '\n';
	std::cout << "Verin 2 (Arriere)  : ";
	std::cin >> buffer;
	fichier << buffer << '\n';
	std::cout << "Capteur 2 (Bas)    : ";
	std::cin >> buffer;
	fichier << buffer << '\n';
	std::cout << "Capteur 2 (Milieu) : ";
	std::cin >> buffer;
	fichier << buffer << '\n';
	std::cout << '\n';
	std::cout << "Venturi            : ";
	std::cin >> buffer;
	fichier << buffer << '\n';
	std::cout << '\n';
	std::cout << "CONFIGURATION TERMINEE !";
	//Fermeture du fichier
	fichier.close();
}
GPIO_ROOT loadConfig()
{
	//Declaration des objets
	GPIO_ROOT pin;
	//Lecture du fichier 
	std::ifstream fichier("Config.txt",std::ios::in);
	if(fichier)
	{
		fichier >> pin.pin_axe_X.PULSE
			>> pin.pin_axe_X.DIR
			>> pin.pin_axe_X.AWO
			>> pin.pin_axe_Y.PULSE
			>> pin.pin_axe_Y.DIR
			>> pin.pin_axe_Y.AWO
			>> pin.SFX
			>> pin.SFY
			>> pin.pin_verin.verin1_e
			>> pin.pin_verin.verin1_r
			>> pin.pin_verin.sensor1_e
			>> pin.pin_verin.verin2_e
			>> pin.pin_verin.verin2_r
			>> pin.pin_verin.sensor2_e
			>> pin.pin_verin.sensor2_m
			>> pin.pin_verin.vacuum;
		fichier.close();
		return pin;
	}
	else
	{
		std::cout << '\n' << "Configuration introuvable ......";
		std::cout << '\n' << "Lancement de la configuration....";
		config();
	}
}
int run()
{
	//Affichage
	std::cout << '\n' << "///////////////Leghelper V1.4-//////////////00   0000 00   0000////" << '\n';
	std::cout <<         "//Attachez vos ceintures ca va decoiffer !//00   000  0  0 0  0////" << '\n';
	std::cout <<         "////////////////////////////////////RUN/////0000 0000 0000 0000////" << '\n' << '\n';
	//Initialisation de WiringPi
	if (wiringPiSetup() == -1)
	{
		exit (1);
	}
	//Initialistaion du mcp23017
	mcp23017Setup(pinBase,i2cAddress);
	//Suite du programme
	int i,j,h,k,x,y;
	//Inscription de la matrice dans le tas
	std::array <std::array<std::array <int, p>, n>, m> pMatrix =  {{{{{{2,0,0,0,0,0,0,0}},
									  {{0,0,0,0,0,0,0,0}},
									  {{0,0,0,0,0,0,0,0}},
									  {{0,0,0,0,0,0,0,0}},
									  {{0,0,0,0,0,0,0,0}},
									  {{0,0,0,0,0,0,0,0}},
									  {{0,0,0,0,0,0,0,0}},
									  {{0,0,0,0,0,0,0,0}}}}
								       ,{{{{0,0,0,0,0,0,0,0}},
								          {{0,0,0,0,0,0,0,0}},
									  {{0,0,0,0,0,0,0,0}},
									  {{0,0,0,0,0,0,0,0}},
									  {{0,0,0,0,0,0,0,0}},
									  {{0,0,0,0,0,0,0,0}},
									  {{0,0,0,0,0,0,0,0}},
									  {{0,0,0,0,0,0,0,0}}}}}};
	//Ouverture du fichier et remplissage de la Matrice
	std::fstream fichier("/var/www/grille.txt", std::ios::in); 
	for (x=0; x<p; x++)
	{
		for (y=0; y<n; y++)
		{
			fichier>>pMatrix[0][y][x];
		}
	}
	fichier.close();
	pMatrix[0][0][0] = 2;
	//Declaration des classes
	Brain commande;
	//Recuperation des pins dans la configuration
	GPIO_ROOT pin;
	//Execution de toute les positions succesives
	int g = commande.getNbIteration(pMatrix);
	for (k=0; k<g; k++)
	{
		std::cout << '\n' << "DEBUT BOUCLE N°" << k+1 << '\n' << '\n';
		//Deplacement a la position suivante
		pMatrix = commande.goNextPosition(pMatrix,pin);
		//Depose d'un Lego
		commande.dropLego(pin);
		std::cout << "FIN BOUCLE N°" << k+1 << '\n';
	}
	//Retour à l'origine
	std::cout << '\n' << "RETOUR A L'ORIGINE" << '\n';
	pMatrix[0][0][0] = 1;
	commande.goNextPosition(pMatrix,pin);
	//Affichage
	std::cout << '\n' << "////////////////////////////////////////////////////////////////////" << '\n';
	std::cout         << "//////////////////A Bientôt//////////////////00   0000 00   0000////" << '\n';
	std::cout <<         "//Pneumatosaure        : Thomas  ARNETTE   //00   000  0  0 0  0////" << '\n';
	std::cout <<         "//Webmaster en carton  : Bastien DELACROIX //0000 0000 0000 0000////" << '\n';
	std::cout <<         "//Component Killer     : Julien  BRIGNON   /////////////////////////" << '\n';
	std::cout <<         "/////////////////////////////////////////////////////////2016///////" << '\n';
	return 1;
}
int check()
{
	//Lecture du fichier de config
	GPIO_ROOT pin = loadConfig();
	//Affichage du contenu du fichier de config
	std::cout << "Lecture du fichier de Configuration ...";
	std::cout << '\n' << "AXE X : - PULSE : " << pin.pin_axe_X.PULSE << '\n'
                          << "      : - DIR   : " << pin.pin_axe_X.DIR   << '\n'
                          << "      : - AWO   : " << pin.pin_axe_X.AWO   << '\n'
		  << '\n' << "AXE Y : - PULSE : " << pin.pin_axe_Y.PULSE << '\n'
			  << "      : - DIR   : " << pin.pin_axe_Y.DIR   << '\n'
			  << "      : - AWO   : " << pin.pin_axe_Y.AWO   << '\n'
		  << '\n' << "Capteur fin de course : - Axe X : " << pin.SFX << '\n'
			  << "                      : - Axe Y : " << pin.SFY << '\n'
		  << '\n' << "Pneumatique : - Verin 1 (Avancer) : " << pin.pin_verin.verin1_e << '\n'
		          << "            : - Verin 1 (Reculer) : " << pin.pin_verin.verin1_r << '\n'
		          << "            : - Verin 2 (Avancer) : " << pin.pin_verin.verin2_e << '\n'
		          << "            : - Verin 2 (Reculer) : " << pin.pin_verin.verin2_r << '\n'
		          << "            : - Venturi           : " << pin.pin_verin.vacuum   << '\n'
		          << "            : - Capteur 1 (Sortie): " << pin.pin_verin.sensor1_e<< '\n'
		          << "            : - Capteur 2 (Bas)   : " << pin.pin_verin.sensor2_e<< '\n'
		          << "            : - Capteur 2 (Milieu): " << pin.pin_verin.sensor2_m<< '\n';
	
}
int help()
{
	//Affichage
	std::cout << '\n' << "///////////////Leghelper V1.4-//////////////00   0000 00   0000////" << '\n';
	std::cout <<         "// Un peu d'aide ne fait jamais de mal... //00   000  0  0 0  0////" << '\n';
	std::cout <<         "///////////////////////////////////HELP/////0000 0000 0000 0000////" << '\n' << '\n';
	//Affichage menu aide
	std::cout << " --run       -> Lance le Leghelper en mode manuel" << '\n'
	          << " --init      -> Initialise le systeme en position de démarrage" << '\n'
		  << " --configure -> Permet de reconfigurer l'attribution des pins" << '\n'
		  << " --check     -> Affiche la configuration courante" << '\n';
	//Affichage
	std::cout << '\n' << "////////////////////////////////////////////////////////////////////" << '\n';
	std::cout         << "//////////////////A Bientôt//////////////////00   0000 00   0000////" << '\n';
	std::cout <<         "//Pneumatosaure        : Thomas  ARNETTE   //00   000  0  0 0  0////" << '\n';
	std::cout <<         "//Webmaster en carton  : Bastien DELACROIX //0000 0000 0000 0000////" << '\n';
	std::cout <<         "//Component Killer     : Julien  BRIGNON   /////////////////////////" << '\n';
	std::cout <<         "/////////////////////////////////////////////////////////2016///////" << '\n';
}
int init()
{
	//Affichage
	std::cout << '\n' << "///////////////Leghelper V1.4-//////////////00   0000 00   0000////" << '\n';
	std::cout <<         "// Je suis perdu quelqu'un peut m'aider ? //00   000  0  0 0  0////" << '\n';
	std::cout <<         "///////////////////////////////////INIT/////0000 0000 0000 0000////" << '\n' << '\n';
	//Declaration des classes
	Brain commande;
	//Lecture du fichier de config
	GPIO_ROOT pin = loadConfig();
	//Recherche de la position initiale
	while (commande.Initialize(pin) != 1)
	{
		continue;
	}
	//Affichage
	std::cout << '\n' << "////////////////////////////////////////////////////////////////////" << '\n';
	std::cout         << "//////////////////A Bientôt//////////////////00   0000 00   0000////" << '\n';
	std::cout <<         "//Pneumatosaure        : Thomas  ARNETTE   //00   000  0  0 0  0////" << '\n';
	std::cout <<         "//Webmaster en carton  : Bastien DELACROIX //0000 0000 0000 0000////" << '\n';
	std::cout <<         "//Component Killer     : Julien  BRIGNON   /////////////////////////" << '\n';
	std::cout <<         "/////////////////////////////////////////////////////////2016///////" << '\n';
}
int main(int argc, char *argv[])
{
	//Nettoyage de l'affichage utilisateur
	system("clear");
	//Lecture et traitement des arguments
	int i;
	for(i=0;i<argc;i++)
	{
		if(strcmp(argv[i],"--configure")==0)
		{
			config();
			return EXIT_SUCCESS;
		}
		if(strcmp(argv[i],"--run")==0)
		{
			run();
			return EXIT_SUCCESS;
		}
		if(strcmp(argv[i],"--init")==0)
		{
			init();
			return EXIT_SUCCESS;
		}
		if(strcmp(argv[i],"--check")==0)
		{
			check();
			return EXIT_SUCCESS;
		}
		if(strcmp(argv[i],"--help")==0)
		{
			help();
			return EXIT_SUCCESS;
		}
	}
}
////////////////////////////////////////////////////////////////---RevD---//
