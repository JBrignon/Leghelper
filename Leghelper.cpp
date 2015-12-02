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
#include "Brain.h"
//Definition de la matrice de position
int main()
{
	//Initialisation de WiringPi
	if (wiringPiSetupGpio() == -1)
	{
		exit (1);
	}
	//Initialistaion du mcp23017
	mcp23017Setup(pinBase,i2cAddress);
	//Suite du programme
	int i,j,h,k;
	int  Pmatrix[m][n][p]={{{2,0,0,0,0,0,0,1},
				{0,0,0,0,0,1,0,0},
				{0,0,1,0,0,0,0,0},
				{0,0,0,0,0,0,0,0}}
			      ,{{0,0,0,0,0,0,0,0},
				{0,0,1,0,0,0,0,0},
				{0,0,0,0,1,0,0,0},
				{0,0,0,0,0,1,0,0}}};
	Brain commande;
	Brain::posMatrix object;
	//Execution de toute les positions succesives
	int g = commande.getNbIteration(Pmatrix);
	for (k=0; k<g; k++)
	{
		std::cout << '\n' << "DEBUT BOUCLE N°" << k+1 << '\n';
		object = commande.goNextPosition(Pmatrix);
		for (j=0; j<m; j++)
		{
			for (i=0; i<n; i++)
			{
				for (h=0; h<p; h++)
				{
					Pmatrix[j][i][h] = object.matrix[j][i][h];
				}
			}
		}
		std::cout << "FIN BOUCLE N°" << k+1 << '\n';
	}
	//Retour à l'origine
	std::cout << '\n' << "RETOUR A L'ORIGINE" << '\n';
	Pmatrix[0][0][0] = 1;
	commande.goNextPosition(Pmatrix);
	std::cout << "PROGRAMME FINI" << '\n';
	return 1;
}
////////////////////////////////////////////////////////////////---RevB---//
