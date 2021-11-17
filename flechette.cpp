# include <chrono>
# include <random>
# include <cstdlib>
# include <sstream>
# include <string>
# include <fstream>
# include <iostream>
# include <iomanip>
# include <mpi.h>


// c'est la version 2 de l'ex du calcul de pi :

double approximate_pi(unsigned long nbSamples) {
    typedef std::chrono::high_resolution_clock myclock;
    myclock::time_point beginning = myclock::now();

    myclock::duration d = myclock::now() - beginning;

    unsigned seed = d.count();
    std::default_random_engine generator(seed);
    std::uniform_real_distribution<double> distribution(-1.0,1.0);
    
    unsigned long nbDarts = 0;

    // on lance les flechettes
    for (unsigned sample = 0; sample < nbSamples ; sample++) {
        // démarrer de 0 ?
        double x = distribution(generator);
        double y = distribution(generator);
        // teste si dans le cercle unité
        if (x*x + y*y <= 1) {
            nbDarts++;
        }
    }
    double ratio = double(nbDarts)/double(nbSamples);
    return ratio;// on va multiplier par 4 dans le main
}

int main (int nargs, char** argv) {
    // 2 fois + rapide ? => non
    // 2 fois + précis ? => oui
    
    MPI_Init(&nargs, &argv); // initialisation
    int numero_du_processus, nombre_de_processus;
    
    MPI_Comm_rank(MPI_COMM_WORLD, &numero_du_processus); // donne le numéro du processus
    MPI_Comm_size(MPI_COMM_WORLD, &nombre_de_processus); // donne le nombre de processus

    int nbSamples = 1000000;

    double pi = approximate_pi(nbSamples);
    double result;
//creation du fichier necessaire pour l'ecriture du résultat
    
std::stringstream fileName;
	fileName << "Output" << std::setfill('0') << std::setw(5) << numero_du_processus<< ".txt";
	std::ofstream output( fileName.str().c_str() );
    
    double nbr;
   
    nbr = 4*pi;
    if (numero_du_processus == 0) {

        MPI_Status bordereau;
        

        double result;
        
        
        result = 4 * pi;
        
        std::cout << "calcul initial par le processus: " << 0 << " valeur " <<  result << std::endl;
        output<<"calcul initial par le processus : " << 0 << " valeur " <<  result << std::endl;
    
        MPI_Send(&result,1,MPI_DOUBLE,1,0,MPI_COMM_WORLD);
        MPI_Recv(&result, 1, MPI_DOUBLE,nombre_de_processus-1, 0, MPI_COMM_WORLD, &bordereau);
            
           
    
        std::cout << "[" << numero_du_processus << "] " << "réponse finale : " << result / nombre_de_processus << std::endl;
        output <<"[" << numero_du_processus << "] " << "réponse finale : " << result / nombre_de_processus << std::endl;
    }
    else {
        MPI_Status bordereau;
        MPI_Recv(&result, 1, MPI_DOUBLE,numero_du_processus-1, 0, MPI_COMM_WORLD, &bordereau);
         std::cout << "CALCUL EN COURS POUR LE PROCESSUS NUMERO : " << numero_du_processus<< " valeur " << nbr << std::endl;
         output <<"CALCUL EN COURS POUR LE PROCESSUS NUMERO : " << numero_du_processus<< " valeur " << nbr << std::endl;
         result=result+nbr;
        MPI_Send(&result, 1, MPI_DOUBLE, (numero_du_processus+1)%nombre_de_processus, 0, MPI_COMM_WORLD );
       

     
    }



    MPI_Finalize();
    return EXIT_SUCCESS;
}