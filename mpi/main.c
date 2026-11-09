// O vetor deve ser inicializado da mesma forma:

// v[i] = i + 1;

// Cada processo MPI deve:

//todo Descobrir seu rank e o número total de processos.
//todo Determinar quais posições do vetor estão sob sua responsabilidade.
//todo Calcular a soma parcial dos elementos atribuídos.
//todo  Se for worker (rank > 0): enviar a soma parcial para o processo 0 usando MPI_Send.
//todo  Se for coordenador (rank == 0): receber as somas parciais de todos os workers usando MPI_Recv, acumulá-las e calcular a soma global.
// O processo 0 exibe os resultados.

// Para mpirun -np 4 soma e N=16, a saída esperada é:


#include <stdio.h>
#include <mpi.h>

#define N 16

int work (int inicio, int fim, int* vetor) {

	int soma = 0;

	int i = inicio;
	for (i; i < fim; i++) {
		soma += vetor[i];
	}

	return soma;
} 


int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, num_processos;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processos);

    int v[N];

	for (int i = 0; i < N; i++) {
        v[i] = i + 1;
    }

    int elementos_por_processo = N / num_processos;
    int inicio = rank * elementos_por_processo;
    int fim = inicio + elementos_por_processo;
    int soma_parcial = work(inicio, fim, v);


    // Como você usaria MPI_ANY_SOURCE no lugar do rank específico na chamada MPI_Recv? Qual a vantagem e a desvantagem?

    if (rank == 0) {
        int soma_global = 0; 
        printf("Processo %d (Coordenador) calculou intervalo [%d, %d): soma parcial = %d\n", rank, inicio, fim, soma_parcial);

        for (int i = 1; i < num_processos; i++) {
            int soma_recebida;
            MPI_Status status;

            // Payload, count, MPI_TIPO, destino, tag, Comunicador, Status 
            
            // MPI_Recv(&soma_recebida, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            MPI_Recv(&soma_recebida, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            soma_global += soma_recebida;

            int actual_sender = status.MPI_SOURCE;

            printf("Sender: %d \n", actual_sender);

        }

        printf("\nSoma Global Final = %d\n", soma_global);

    } else {
        printf("Processo %d (Worker) do intervalo [%d, %d): soma parcial = %d\n", rank, inicio, fim, soma_parcial);
        MPI_Send(&soma_parcial, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }


    MPI_Finalize();
    return 0;
}


// mpicc main.c -o main; mpirun --oversubscribe -np 3 ./main