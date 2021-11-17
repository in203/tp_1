# include <iostream>

# include <mpi.h>

int main( int nargs, char* argv[] )
{
	// On initialise le contexte MPI qui va s'occuper :
	//    1. Créer un communicateur global, COMM_WORLD qui permet de gérer
	//       et assurer la cohésion de l'ensemble des processus créés par MPI;
	//    2. d'attribuer à chaque processus un identifiant ( entier ) unique pour
	//       le communicateur COMM_WORLD
	//    3. etc...

	MPI_Init( &nargs, &argv );
	
	
	// On interroge le communicateur global pour connaître le nombre de processus
	// qui ont été lancés par l'utilisateur :
	int nbp;
	MPI_Comm_size( MPI_COMM_WORLD,&nbp);
	//lire la tâche actuelle
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int token;
	token=1234;//initialisation du jeton
	MPI_Status bordereau;

	if(rank==0)//on traite la tâche de façon indépendante des autres 
	{// je suis de rank 0

	MPI_Request request;
		std:: cout << "jeton initial reçu par le processus [ "<<rank<<"] " << token << std::endl;

		MPI_Send(&token,1,MPI_INT,1,101,MPI_COMM_WORLD);// envoie la valeur au processus suivant 
		
		MPI_Recv(&token,1,MPI_INT,nbp-1,101,MPI_COMM_WORLD, &bordereau);// 'la étape finale de la programmation en parallèle

		std::cout <<"jeton final "<< token << std ::endl ;
	}
	else 
	{ // si on n'est pas le processus intial
		
		MPI_Request request;
		MPI_Recv(&token,1,MPI_INT,rank-1,101,MPI_COMM_WORLD  ,&bordereau);
		token=token+1;// l'incrémentation demandé
		std:: cout << "jeton processus [ "<<rank <<" ] est" << token << std::endl;
		int to=(rank+1)%nbp;// pour éviter le dépassemenet du nombre 
		MPI_Send(&token,1,MPI_INT,to,101,MPI_COMM_WORLD);

		}
		

	

	// A la fin du programme, on doit synchroniser une dernière fois tous les processus
	// afin qu'aucun processus ne se termine pendant que d'autres processus continue à
	// tourner. Si on oublie cet instruction, on aura une plantage assuré des processus
	// qui ne seront pas encore terminés.
	MPI_Finalize();
	return EXIT_SUCCESS;
}

