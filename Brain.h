////////////////////////////////////////////////////////////////////////////
//-------------2015 - Commande Imprimante 3D - Julien BRIGNON-------------//
/////////////////////////////////////////////////////////////////---GPL---//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <thread>
#include "Stepper.h"
#include <fstream>
#include <vector>
//Declaration de k, conversion distance/pas
#define kx 50 //selon x
#define ky 35 //selon y
#define kz 35 //selon z
//Declaration des constantes de taille de la matrice [z][y][x]
#define m 2 //selon z
#define n 4 //selon y
#define p 8 //selon x
//Declaration de la constante de frequence
#define speed 1
//Declaration des parametre du mcp23017
#define pinBase 100      //Definition des numeros de pin affecter au mcp23017
#define i2cAddress 0x20  //Definition de l'adresse du mcp23017
class Brain
{
	public:
	//Declaration de la structure contenant les coordonées d'un point
	struct position
	{
		int x;
		int y;
		int z;
	};
	//Declaration de la structure pour la fonction getGrid
	struct posMatrix
	{
		int x;
		int y;
		int z;
		int matrix[m][n][p];
	};
	//Declaration de la structure de controle des thread
	struct state
	{
		bool x;
		bool y;
		bool z;
	};
	state etatThread = {false,false,false};
	//Definition de la fonction pour les threads
	void motorThread(int pas, Stepper::GPIO pin, int ID)
	{
		//Lancement du moteur
		Stepper motor;
		//Affichage por le log
		switch (ID)
		{
			case 0x200 :
				std::cout << "Moteur x avance de : " << pas << " pas" << '\n';
				break;
			case 0x276 : 
				std::cout << "Moteur y avance de : " << pas << " pas" << '\n';
				break;
			case 0x398 :
				std::cout << "Moteur z avance de : " << pas << " pas" << '\n';
				break;
		}
		//Attente du moteur
		while (motor.run(speed,pas,pin) != 1)
		{
			continue;
		}
		//Envoi du signal d'arret du thread
		switch (ID)
		{
			case 0x200 :
				etatThread.x = true;
				break;
			case 0x276 : 
				etatThread.y = true;
				break;
			case 0x398 :
				etatThread.z = true;
				break;
		}
	}
	//Definition de la fonction principale
	posMatrix getGrid(int Pmatrix[m][n][p])
	{
		std::vector <position> coordonees;
		posMatrix object;
		position origin;
		position point;
		position grid;
		int j, i, h;
		for (j=0; j<m; j++)
		{
			for (i=0; i<n; i++)
			{
				for (h=0; h<p; h++)
				{
					switch (Pmatrix [j][i][h])
					{
						case 0 :
							object.matrix[j][i][h] = 0;
							break;
						case 1 : 
							point.x = h;
							point.y = i;
							point.z = j;
							coordonees.push_back(point);
							object.matrix[j][i][h] = 1;
							break;
						case 2 : 
							origin.x = h;
							origin.y = i;
							origin.z = j;
							object.matrix[j][i][h] = 0;
							break;
					}
				}
			}
		}
		object.x = (coordonees[0].x-origin.x)*kx;
		object.y = (coordonees[0].y-origin.y)*ky;
		object.z = (coordonees[0].z-origin.z)*kz;
		//Changement d'origine
		object.matrix[coordonees[0].z][coordonees[0].y][coordonees[0].x] = 2;
		//Mise en structure
		return object;
	}
	int getNbIteration(int Pmatrix[m][n][p])
	{
		int g = 0;
		int j, i, h;
		for (j=0; j<m; j++)
		{
			for (i=0; i<n; i++)
			{
				for (h=0; h<p; h++)
				{
					if (Pmatrix [j][i][h] == 1)
					{
						g = g + 1;
					}
				}
			}
		}
		return g;
	}
	posMatrix goNextPosition(int Pmatrix[m][n][p])
	{
		//Initialisation des pins axeX
		Stepper::GPIO pin_axe_X;
		pin_axe_X.input1_bobine_1 =  8;
		pin_axe_X.input2_bobine_1 =  9;
		pin_axe_X.input3_bobine_2 =  6;
		pin_axe_X.input4_bobine_2 =  7;
		//Initialisation des pins axeY
		Stepper::GPIO pin_axe_Y;
		pin_axe_Y.input1_bobine_1 =  10;
		pin_axe_Y.input2_bobine_1 =  11;
		pin_axe_Y.input3_bobine_2 =   4;
		pin_axe_Y.input4_bobine_2 =   5;
		//Initialisation des pins axeZ
		Stepper::GPIO pin_axe_Z;
		pin_axe_Z.input1_bobine_1 =  43;
		pin_axe_Z.input2_bobine_1 =  44;
		pin_axe_Z.input3_bobine_2 =  45;
		pin_axe_Z.input4_bobine_2 =  46;
		//Declaration de la grille
		position grid;
		//Declaration de l'objet de transfert
		posMatrix object;
		//Calcul des vecteurs
		object = getGrid(Pmatrix);
		//Demarrage des threads
		std::thread xEngine(&Brain::motorThread,this,object.x,pin_axe_X,0x200);
		std::thread yEngine(&Brain::motorThread,this,object.y,pin_axe_Y,0x276);
		std::thread zEngine(&Brain::motorThread,this,object.z,pin_axe_Z,0x398);
		//Attente des trois threads
		xEngine.detach();
		yEngine.detach();
		zEngine.detach();
		while (etatThread.x == false or etatThread.y == false or etatThread.z == false)
		{
			continue;
		}
		//Reinisialisation des variables d'etat des threads
		etatThread.x = false;
		etatThread.y = false;
		etatThread.z = false;
		//Renvoi de la matrice reactualiser
		return object;
	}
};
////////////////////////////////////////////////////////////////---RevC---//
