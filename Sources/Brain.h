////////////////////////////////////////////////////////////////////////////
//-------------2015 - Commande Imprimante 3D - Julien BRIGNON-------------//
/////////////////////////////////////////////////////////////////---GPL---//
//Creation de la classe principale
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
	//Declaration de la structure de controle des thread
	struct state
	{
		bool x;
		bool y;
		bool z;
	};
	//Definition de la Fonction de commande des moteurs
	void driverThread(int pas, Driver::GPIO_SS2000MD4 pin, int ID,state *etatThread)
	{
		//Lancement du moteur
		Driver motor;
		//Affichage por le log
		switch (ID)
		{
			case 0x200 :
				std::cout << "Moteur X avance de : " << pas << " pas" << '\n';
				break;
			case 0x276 : 
				std::cout << "Moteur Y avance de : " << pas << " pas" << '\n';
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
				etatThread->x = true;
				break;
			case 0x276 : 
				etatThread->y = true;
				break;
		}
	}
	//Definition de la fonction d'initialisation
	void initThread(Driver::GPIO_SS2000MD4 pin, int SF,int PAS,int ID ,state *etatThread)
	{
		//Lancement du moteur
		Driver motor;
		//Affichage por le log
		switch (ID)
		{
			case 0x200 :
				std::cout << "Moteur X Cherche Origine ..." << '\n';
				break;
			case 0x276 : 
				std::cout << "Moteur Y Cherche Origine ..." << '\n';
				break;
		}
		//Attente du moteur
		while (motor.init(pin,SF,PAS) != 1)
		{
			continue;
		}
		//Envoi du signal d'arret du thread
		switch (ID)
		{
			case 0x200 :
				etatThread->x = true;
				break;
			case 0x276 : 
				etatThread->y = true;
				break;
		}
	}
	//Definition de la fonction d'initialisation de la pneumatique
	int initPneumatique(Droper::GPIO_VERIN pin)
	{
		//Appel de la classe externe
		Droper pose;
		//mise a zero des pins
		std::cout << '\n' << "Mise a zero des pins ..." << '\n';
		pose.pinInit(pin);
		std::cout << '\n' << "Rentree du verin 1 ..." << '\n';
		while (pose.tick(pin.verin1_r) != 1)
		{
			continue;
		}
		std::cout << "Rentree du verin 2 ..." << '\n' << '\n';
		while (pose.tick(pin.verin2_r) != 1)
		{
			continue;
		}
		return 1;
	}
	//Definition de la fonction principale
	std::array <std::array<std::array <int, p>, n>, m> getGrid(std::array <std::array<std::array <int, p>, n>, m> pMatrix,position *grid)
	{
		std::vector <position> coordonees;
		position origin;
		position point;
		int j, i, h;
		for (j=0; j<m; j++)
		{
			for (i=0; i<n; i++)
			{
				for (h=0; h<p; h++)
				{
					switch (pMatrix[j][i][h])
					{
						case 0 :
							pMatrix[j][i][h] = 0;
							break;
						case 1 : 
							point.x = h;
							point.y = i;
							point.z = j;
							coordonees.push_back(point);
							pMatrix[j][i][h] = 1;
							break;
						case 2 : 
							origin.x = h;
							origin.y = i;
							origin.z = j;
							pMatrix[j][i][h] = 0;
							break;
					}
				}
			}
		}
		//Le -1 inverse le fonctionnement des moteur car la grille se deplace et non le systeme de pose
		grid->x = -1*(coordonees[0].x-origin.x)*kx;
		grid->y = -1*(coordonees[0].y-origin.y)*ky;
		grid->z = -1*(coordonees[0].z-origin.z)*kz;
		//Changement d'origine
		pMatrix[coordonees[0].z][coordonees[0].y][coordonees[0].x] = 2;
		//Renvoie de la position voulue
		return pMatrix;
	}
	int getNbIteration(std::array <std::array<std::array <int, p>, n>, m> pMatrix)
	{
		int g = 0;
		int j, i, h;
		for (j=0; j<m; j++)
		{
			for (i=0; i<n; i++)
			{
				for (h=0; h<p; h++)
				{
					if (pMatrix[j][i][h] == 1)
					{
						g = g + 1;
					}
				}
			}
		}
		return g;
	}
	int Initialize(GPIO_ROOT pin)
	{
		//Declaration de la variable de test des threads
		state *etatThread = new state;
		*etatThread = {false,false,false};
		//Initialisation de la pneumatique
		initPneumatique(pin.pin_verin);
		//Demarrage des threads
		std::thread xInit(&Brain::initThread,this,pin.pin_axe_X,pin.SFX,PX,0x200,etatThread);
		std::thread yInit(&Brain::initThread,this,pin.pin_axe_Y,pin.SFY,PY,0x276,etatThread);
		//Separation des threads du main
		xInit.detach();
		yInit.detach();
		//Attente des trois threads
		while (etatThread->x == false or etatThread->y == false)
		{
			continue;
		}
		return 1;
	}
	int dropLego(GPIO_ROOT pin)
	{
		//Appel de la fonction
		Droper pose;
		while (pose.run(pin.pin_verin) != 1)
		{
			continue;
		}
		return 1;
	}
	std::array <std::array<std::array <int, p>, n>, m> goNextPosition(std::array <std::array<std::array <int, p>, n>, m> pMatrix,GPIO_ROOT pin)
	{
		//Declaration de la variable de test des threads
		state *etatThread = new state;
		*etatThread = {false,false,false};
		//Declaration de la grille
		position *grid = new position;
		//Calcul des vecteurs
		pMatrix = getGrid(pMatrix,grid);
		//Demarrage des threads
		std::thread xEngine(&Brain::driverThread,this,grid->x,pin.pin_axe_X,0x200,etatThread);
		std::thread yEngine(&Brain::driverThread,this,grid->y,pin.pin_axe_Y,0x276,etatThread);
		//Separation des threads du main
		xEngine.detach();
		yEngine.detach();
		//Attente des trois threads
		while (etatThread->x == false or etatThread->y == false)
		{
			continue;
		}
		return pMatrix;
	}
};
////////////////////////////////////////////////////////////////---RevC---//