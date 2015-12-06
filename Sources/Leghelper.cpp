////////////////////////////////////////////////////////////////////////////
//-------2015 - Interface de commande du Leghelper - Julien BRIGNON-------//
/////////////////////////////////////////////////////////////////---GPL---//
//Using C++11, Compile with "g++ Leghelper.cpp -std=c++11  -lwiringPi" /////
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <wiringPi.h>
#include <mcp23017.h>
#include <thread>
#include <array>
#include "Brain.h"
//Definition de la matrice de position
int main()
{
	//Initialisation de WiringPi
	if (wiringPiSetup() == -1)
	{
		exit (1);
	}
	//Initialistaion du mcp23017
	mcp23017Setup(pinBase,i2cAddress);
	//Suite du programme
	int i,j,h,k;
	//Inscription de la matrice dans le tas
	std::array <std::array<std::array <int, p>, n>, m> pMatrix =  {{{{{{2,0,0,0,0,0,0,0}},
									  {{0,0,0,0,0,0,0,0}},
									  {{0,0,0,0,0,0,0,0}},
									  {{0,0,0,0,0,0,0,1}}}}
								       ,{{{{0,0,0,0,0,0,0,0}},
									  {{0,0,0,0,0,0,0,0}},
									  {{0,0,0,0,0,0,0,0}},
									  {{0,0,0,0,0,0,0,0}}}}}};
	//Declaration des classes
	Brain commande;
	//Execution de toute les positions succesives
	int g = commande.getNbIteration(pMatrix);
	for (k=0; k<g; k++)
	{
		std::cout << '\n' << "DEBUT BOUCLE N°" << k+1 << '\n';
		pMatrix = commande.goNextPosition(pMatrix);
		std::cout << "FIN BOUCLE N°" << k+1 << '\n';
	}
	//Retour à l'origine
	std::cout << '\n' << "RETOUR A L'ORIGINE" << '\n';
	pMatrix[0][0][0] = 1;
	commande.goNextPosition(pMatrix);
	std::cout << "PROGRAMME FINI" << '\n';
	return 1;
}
////////////////////////////////////////////////////////////////---RevB---//
