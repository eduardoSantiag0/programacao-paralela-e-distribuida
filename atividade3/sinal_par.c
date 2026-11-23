#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef N
#define N       1200000
#endif
#ifndef TOL
#define TOL     0.01
#endif
#ifndef MAX_IT
#define MAX_IT  100000
#endif
#define B_ESQ   50.0
#define B_DIR  -50.0

int main(int argc, char *argv[]) {
    int rank, nprocs;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);


    int n = N / nprocs;
    double *s = malloc((n + 4) * sizeof(double));
    double *novo = malloc((n + 4) * sizeof(double));
    double *dados = NULL;

    if (s == NULL || novo == NULL) {
        fprintf(stderr, "Rank %d: erro de alocacao de memoria.\n", rank);
        // Aborta todos os processos se um deles nao conseguiu alocar memoria.
        MPI_Abort(MPI_COMM_WORLD, 1);
        return 1;
    }

    if (rank == 0) {
        dados = malloc(N * sizeof(double));
        if (dados == NULL) {
            fprintf(stderr, "Erro de alocacao do sinal global.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
            return 1;
        }
        srand(7);
        for (int i = 0; i < N; i++) {
            dados[i] = rand() % 200 - 100;
        }
    }

    // Distribui N amostras geradas pelo rank 0 em blocos de n amostras.
    MPI_Scatter(dados, n, MPI_DOUBLE,
                &s[2], n, MPI_DOUBLE,
                0, MPI_COMM_WORLD);

    int esquerda;
    int direita;
    if (rank == 0) {
        esquerda = MPI_PROC_NULL;
    } else {
        esquerda = rank - 1;
    }
    if (rank == nprocs - 1) {
        direita = MPI_PROC_NULL;
    } else {
        direita = rank + 1;
    }

    // Todos devem estar na mesma linha antes de começar o while loop
    MPI_Barrier(MPI_COMM_WORLD);
    double t0 = MPI_Wtime();
    int it = 0;
    int mudaram_global;

    do {
        if (rank == 0) {
            s[0] = B_ESQ;
            s[1] = B_ESQ;
        }
        if (rank == nprocs - 1) {
            s[n + 2] = B_DIR;
            s[n + 3] = B_DIR;
        }

        // Envia as duas primeiras amostras para a esquerda; ao mesmo tempo,
        // recebe as duas primeiras amostras do vizinho da direita em seu halo direito.
        // MPI_PROC_NULL faz a operacao correspondente ser ignorada, sem travar.
        MPI_Sendrecv(&s[2], 2, MPI_DOUBLE, esquerda, 0,
                     &s[n + 2], 2, MPI_DOUBLE, direita, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Envia as duas ultimas amostras para a direita; ao mesmo tempo,
        // recebe as duas ultimas amostras do vizinho da esquerda em seu halo esquerdo.
        // A troca casada de envio/recebimento evita deadlock.
        MPI_Sendrecv(&s[n], 2, MPI_DOUBLE, direita, 1,
                     &s[0], 2, MPI_DOUBLE, esquerda, 1,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int mudaram_local = 0;
        for (int i = 2; i <= n + 1; i++) {
            novo[i] = (s[i - 2] + s[i - 1] + s[i] + s[i + 1] + s[i + 2]) / 5.0;
            if (fabs(novo[i] - s[i]) >= TOL) {
                mudaram_local++;
            }
        }

        double *tmp = s;
        s = novo;
        novo = tmp;

        // O divulga o resultado o resultado para TODOS.
        // Assim, nenhum processo para enquanto ainda ha mudancas em outro bloco.
        MPI_Allreduce(&mudaram_local, &mudaram_global, 1,
                      MPI_INT, MPI_SUM, MPI_COMM_WORLD);
        it++;
    } while (mudaram_global > 0 && it < MAX_IT);

    double tempo_local = MPI_Wtime() - t0;
    double tempo_max = 0.0;

    // Coleta o maior tempo entre processos: a execucao paralela so termina
    // quando o ultimo processo conclui seu trabalho.
    MPI_Reduce(&tempo_local, &tempo_max, 1,
               MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    // Reunir os dados de todos os processos em um único processo.
    // preservando a ordem original para somar como na versao sequencial.
    MPI_Gather(&s[2], n, MPI_DOUBLE,
               dados, n, MPI_DOUBLE,
               0, MPI_COMM_WORLD);

    if (rank == 0) {
        double soma = 0.0;
        for (int i = 0; i < N; i++) {
            soma += dados[i];
        }
        printf("Iteracoes = %d\n", it);
        printf("Soma      = %.6f\n", soma);

        printf("s[2] = %.6f   s[N/2] = %.6f   s[N+1] = %.6f\n",
               dados[0], dados[N / 2 - 2], dados[N - 1]);
        printf("Tempo = %.2f s\n", tempo_max);
    }

    free(dados);
    free(s);
    free(novo);

    MPI_Finalize();
    return 0;
}
