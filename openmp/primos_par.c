#include <omp.h>
#include <stdio.h>

#define N 2000001
// #define NTHREADS 7
#define RESPOSTA 148933

int eh_primo(int n)
{
    if (n < 2)
        return 0;

    int d;

    for (d = 2; d * d <= n; d++)
        if (n % d == 0)
            return 0;

    return 1;
}

int main(void)
{
    int NTHREADS = omp_get_max_threads();

    printf("Numero de threads: %d\n\n", NTHREADS);
    // omp_set_num_threads(NTHREADS);
    int parciais[NTHREADS];

    for (int i = 0; i < NTHREADS; i++) {
        parciais[i] = 0;
    }

    int sliceSize = N / NTHREADS;

    int resto = N%NTHREADS;

    #pragma omp parallel shared(sliceSize)
    {
        int id = omp_get_thread_num();

        int inicio = id * sliceSize;
        int fim = inicio + sliceSize;

        if (id == NTHREADS-1) 
            fim += resto;

        printf("Thread %d verificando de %d ate %d\n", id, inicio, fim);

        int soma = 0;
        for (int i=inicio; i<fim; i++)
            if (eh_primo(i))
                soma++;

        parciais[id] = soma;

    }


    int count = 0;
    for (int i=0; i < NTHREADS; i++) {
        count += parciais[i];
        // printf("\n\n%d\n", parciais[i]);
    } 

    if (count == RESPOSTA) printf("ESTÁ CORRETO: %d\n", count);

    else printf(":(( resultado: %d\n", count);

    return 0;

}