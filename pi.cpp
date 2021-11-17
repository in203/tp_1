# include <chrono>
# include <random>
# include <cstdlib>
# include <sstream>
# include <string>
# include <fstream>
# include <iostream>
# include <iomanip>
# include <mpi.h>

// Attention , ne marche qu'en C++ 11 ou supérieur :

double approximate_pi( unsigned long nbSamples ) 
{
    typedef std::chrono::high_resolution_clock myclock;
    myclock::time_point beginning = myclock::now();
    myclock::duration d = beginning.time_since_epoch();
    unsigned seed = d.count();
    std::default_random_engine generator(seed);
    std::uniform_real_distribution <double> distribution ( -1.0 ,1.0);
    unsigned long nbDarts = 0;
    // Throw nbSamples darts in the unit square [-1 :1] x [-1 :1]
    for ( unsigned sample = 0 ; sample < nbSamples ; ++ sample ) {
        double x = distribution(generator);
        double y = distribution(generator);
        // Test if the dart is in the unit disk
        if ( x*x+y*y<=1 ) nbDarts ++;
    }
    // Number of nbDarts throwed in the unit disk
    double ratio = double(nbDarts)/double(nbSamples);
    return 4*ratio;
}


// c'est la version 1 du calcul de pi 


int main( int nargs, char* argv[] )
{
	// On initialise l'environnement MPI qui va s'occuper :
	
	MPI_Init( &nargs, &argv );
	
	// MPI_COMM_WORLD qui gère l'ensemble des processus lancés par MPI.
	MPI_Comm globComm;

	MPI_Comm_dup(MPI_COMM_WORLD, &globComm);

	//lire le nombre de taches total

	int nbp;

	MPI_Comm_size(globComm, &nbp);


	//on lit la taches actuelle

	int rank;
	MPI_Comm_rank(globComm, &rank);


	// Création d'un fichier pour ma propre sortie en écriture :

	std::stringstream fileName;
	fileName << "Output" << std::setfill('0') << std::setw(5) << rank << ".txt";
	std::ofstream output( fileName.str().c_str() );
    int nbSamples = 1000000;//utilisation d'un nombre assez grand pour plus de precision

    double pi = approximate_pi(nbSamples);
    double result;



	// Rajout de code demandé

    if (rank == 0) {// si on est le processus 0 "maitre" on reçoit les resultat de calcul des autres processus pour calculer la moyenne à la fin
        MPI_Status status;
       

        double buffer;
        
        int i_source = 1;
        result = pi;
        std::cout << "reception de la source : " << 0 << " valeur " <<  result << std::endl;
        while (i_source < nbp) {
            MPI_Recv(&buffer, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status); //recevoir le resultat "buffer" du n'importe processis
            result +=  buffer;//incrementation du résultat
            std::cout << "reception de la source : " << status.MPI_SOURCE << " valeur " <<  buffer << std::endl;//affichage dans la console

            
            output << "la valeur reçue est : " << buffer << std::endl;//écriture dans le fichier souhaité


            i_source ++;
        }
        std::cout << "[" << nbp << "] " << "réponse finale : " << result / nbp << std::endl;//calcul  du resultat final;
        output<< " la valeur finale est :"<< result/nbp<< std::endl;
    }
    else {
        MPI_Request request;//si on n'est pas processus maitre , on envoie le résultat de pi grâce a la fonction donnée
        MPI_Send(&pi, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
       
           output << "le processus dont le rang est "<<rank<<" a fini son calcul de pi : "<< pi<<std::endl;
    
    }
	output.close();
	// A la fin du programme, on doit synchroniser une dernière fois tous les processus
	// afin qu'aucun processus ne se termine pendant que d'autres processus continue à
	// tourner. Si on oublie cet instruction, on aura une plantage assuré des processus
	// qui ne seront pas encore terminés.
	MPI_Finalize();
	return EXIT_SUCCESS;
}
