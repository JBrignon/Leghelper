////////////////////////////////////////////////////////////////////////////
//---------2015 - Gestion des moteurs PAS à PAS - Julien BRIGNON----------//
/////////////////////////////////////////////////////////////////---GPL---//
#include <stdlib.h>
#include <wiringPi.h>
#include <iostream>
#include <mcp23017.h>
//Definition du mode de Fonctionnement
//PAS[6][6]+ARRETP; DEMIPAS[6][10]+ARRETD
#define u 4
#define v 6
#define START PAS
#define STOP ARRETP
class Stepper
{
	public:
	struct GPIO_L293D
	{
		int input1_bobine_1;
		int input2_bobine_1;
		int input3_bobine_2;
		int input4_bobine_2;
	};
	void Engine(int matrice[u][v],double temps)
	{
		int i,j,k,pin,value;
		for (j=1; j<v ; j++)
		{
			for (i=0; i<u ; i++)
			{
				pin = matrice[i][0];
				value = matrice[i][j];
				pinMode(pin, OUTPUT);
				if (value == 1)
				{
					digitalWrite(pin,HIGH);
				}
				else
				{
					digitalWrite(pin,LOW);
				}
			}
			delay(temps);
		}
		return;
	}
	int run(double vitesse, int valeur, GPIO_L293D in)
	{
		GPIO_L293D pin;
		int tours, i;
		if (valeur < 0)
		{
			pin.input1_bobine_1 = in.input3_bobine_2;
			pin.input2_bobine_1 = in.input4_bobine_2;
			pin.input4_bobine_2 = in.input2_bobine_1;
			pin.input3_bobine_2 = in.input1_bobine_1;
			tours = -1*valeur;
		}
		if (valeur > 0)
		{
			pin.input1_bobine_1 = in.input1_bobine_1;
			pin.input2_bobine_1 = in.input2_bobine_1;
			pin.input4_bobine_2 = in.input4_bobine_2;
			pin.input3_bobine_2 = in.input3_bobine_2;
			tours = valeur;
		}
		if (valeur = 0)
		{
			return 1;
		}
			int DEMIPAS[4][10] =    {{pin.input1_bobine_1,1,1,0,0,0,0,0,1,1}
						,{pin.input4_bobine_2,0,1,1,1,0,0,0,0,0}
						,{pin.input2_bobine_1,0,0,0,1,1,1,0,0,0}
						,{pin.input3_bobine_2,0,0,0,0,0,1,1,1,0}};
			int ARRETD[4][10] =     {{pin.input1_bobine_1,0,0,0,0,0,0,0,0,0}
						,{pin.input4_bobine_2,0,0,0,0,0,0,0,0,0}
						,{pin.input2_bobine_1,0,0,0,0,0,0,0,0,0}
						,{pin.input3_bobine_2,0,0,0,0,0,0,0,0,0}};
			int ARRETP[4][6] =      {{pin.input1_bobine_1,0,0,0,0,0}
						,{pin.input4_bobine_2,0,0,0,0,0}
						,{pin.input2_bobine_1,0,0,0,0,0}
						,{pin.input3_bobine_2,0,0,0,0,0}};
			int PAS[4][6] =         {{pin.input1_bobine_1,1,0,0,0,1}
						,{pin.input4_bobine_2,0,1,0,0,0}
						,{pin.input2_bobine_1,0,0,1,0,0}
						,{pin.input3_bobine_2,0,0,0,1,0}};
		//Lancement de la fonction d'avance de x pas
		for (i=0;i<tours;i++)
		{
			Engine(START,vitesse);
		}
		//Arrêt du moteur
		Engine(STOP,vitesse);
		return 1;
	}
};
class Driver
{
	public :
	struct GPIO_SS2000MD4
	{
		int PULSE;
		int DIR;
		int AWO;
	};
	int run(double vitesse, int valeur, GPIO_SS2000MD4 pin)
	{
		int tours, i;
		//Envoie du signal enable au driver
		digitalWrite(pin.AWO,HIGH);
		//Gestion du sens de rotation
		if (valeur < 0)
		{
			digitalWrite(pin.DIR,LOW);
			tours = -1*valeur;
		}
		if (valeur > 0)
		{
			digitalWrite(pin.DIR,HIGH);
			tours = valeur;
		}
		if (valeur = 0)
		{
			return 1;
		}
		//Lancement de la boucle
		for (i=0; i<tours; i++)
		{
			digitalWrite(pin.PULSE,HIGH);
			delay(vitesse/2);
			digitalWrite(pin.PULSE,LOW);
			delay(vitesse/2);
		}
		//Remise à zero des pins
		digitalWrite(pin.AWO,LOW);
		digitalWrite(pin.DIR,LOW);
		return 1;
	}
};
////////////////////////////////////////////////////////////////---RevE---//
