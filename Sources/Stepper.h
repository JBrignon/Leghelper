////////////////////////////////////////////////////////////////////////////
//---------2015 - Gestion des moteurs PAS à PAS - Julien BRIGNON----------//
/////////////////////////////////////////////////////////////////---GPL---//
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
		pinMode(pin.PULSE, OUTPUT);
		pinMode(pin.DIR, OUTPUT);
		pinMode(pin.AWO, OUTPUT);
		//Envoie du signal enable au driver
		digitalWrite(pin.AWO,LOW);
		//Gestion du sens de rotation
		if (valeur < 0)
		{
			digitalWrite(pin.DIR,HIGH);
			tours = -1*valeur;
		}
		if (valeur > 0)
		{
			digitalWrite(pin.DIR,LOW);
			tours = valeur;
		}
		if (valeur = 0)
		{
			return 1;
		}
		//Lancement de la boucle
		for (i=0; i<tours; i++)
		{
			digitalWrite(pin.PULSE,LOW);
			delayMicroseconds(vitesse/2);
			digitalWrite(pin.PULSE,HIGH);
			delayMicroseconds(vitesse/2);
		}
		//Remise à zero des pins
		digitalWrite(pin.AWO,HIGH);
		digitalWrite(pin.DIR,HIGH);
		return 1;
	}
	int init(GPIO_SS2000MD4 pin,int SF,int PAS)
	{
		//Declaration des variables pour la boucle
		int i;
		//Definition des pins de sortie
		pinMode(pin.PULSE, OUTPUT);
		pinMode(pin.DIR, OUTPUT);
		pinMode(pin.AWO, OUTPUT);
		//Envoie du signal enable au driver
		digitalWrite(pin.AWO,LOW);
		//Definition du sens de rotation
		digitalWrite(pin.DIR,HIGH);
		//Reglage du capteur et initialisation du PULL UP
		pinMode(SF,INPUT);
		pullUpDnControl(SF,PUD_UP);
		//Lancement de la boucle de mise a zero
		while (digitalRead(SF) == 1)
		{
			digitalWrite(pin.PULSE,LOW);
			delayMicroseconds(100);
			digitalWrite(pin.PULSE,HIGH);
			delayMicroseconds(100);
		}
		//Definition du sens de rotation
		digitalWrite(pin.DIR,LOW);
		//Lancement de la boucle de mise en position initiale
		for (i=0; i<PAS; i++)
		{
			digitalWrite(pin.PULSE,LOW);
			delayMicroseconds(100);
			digitalWrite(pin.PULSE,HIGH);
			delayMicroseconds(100);
		}
		//Remise à zero des pins
		digitalWrite(pin.AWO,HIGH);
		digitalWrite(pin.DIR,HIGH);
		return 1;
	}
};
////////////////////////////////////////////////////////////////---RevF---//
