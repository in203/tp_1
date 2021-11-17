
# include <chrono>
# include <random>
# include <cstdlib>
# include <sstream>
# include <string>
# include <fstream>
# include <iostream>
# include <iomanip>
# include <mpi.h>
#include<cmath>
int main( int nargs, char* argv[] )



{



    int rank, nbp, jeton;
    MPI_Status status;


    MPI_Init(&nargs, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nbp);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    //creation de fichier pour l'ecriture
    std::stringstream fileName;
    fileName << "Output" << std::setfill('0') << std::setw(5) << rank << ".txt";
	std::ofstream output( fileName.str().c_str() );


    // Calcul de la dimension du cube :
    int dim = 0;
    
    while ( pow(2,dim) < nbp ) 
    
    {dim =dim+1;}//le nombre d étapes de calcul de jeton = 2 ^d devrait etre egale a la dimension d hypercube et on atteint le max de dimension possible qu'on peut travailler avec
    jeton = 10;

    

    for ( int d = 0; d <= dim; ++d )
    {
        int max = pow(2,d);
        std::cout<<"on est a la dimension"<<d<<std::endl;


        if (rank == 0) {
            MPI_Request request;
        MPI_Isend(&jeton, 1, MPI_INT, 1 ,10, MPI_COMM_WORLD,&request);
        std::cout<<"le jeton est lancé du processus"<<rank<<std::endl;
        output<<"le jeton est lancé du processus"<<rank<< std::endl;
        }



        else if ( rank < max )
       {
            MPI_Request request;
            MPI_Recv(&jeton, 1, MPI_INT, rank-1, 10, MPI_COMM_WORLD, &status );
            std::cout << "jeton reçu et traité en cours  du processus : [ " <<rank<<" ]" <<jeton<< std::endl;
            output<<  "jeton reçu et traité en cours  du processus : [ " <<rank<<" ]" <<jeton<< std::endl;
            
            MPI_Isend(&jeton, 1, MPI_INT, (rank+1)%(max) ,10, MPI_COMM_WORLD,&request);
       }
    }
    output.close();
    MPI_Finalize();
    return EXIT_SUCCESS;
}