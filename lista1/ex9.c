// Exercício 9 - Acumulados antes e depois
// Para cada posição i de um vetor a, queremos duas somas: ant[i], a soma dos elementos antes de i, e pos[i], a soma dos elementos depois de i. De propósito, o programa usa a versão ingênua (quadrática): cada posição soma os elementos um a um.

// As duas etapas escrevem em vetores diferentes e nenhuma usa o resultado da outra. 
// No final, o programa confere que ant[i] + a[i] + pos[i] é igual à soma total para todo i.

// Requisitos:

// Paralelizar as duas etapas com for, dentro de uma mesma região parallel, mantendo Verificacao: OK;
// Medir o tempo com 1 thread e com todas as threads; e
// Repare que o custo da etapa 1 cresce com i e o da etapa 2 diminui. Além disso, todo for termina com uma espera: nenhuma thread passa para o laço seguinte enquanto todas não terminarem. Essa espera é necessária aqui? Se não for, descubra como eliminá-la e compare o tempo.
// Saída esperada (o tempo varia):

// Soma total = 330000
// Verificacao: OK


#include <stdio.h>
#include <omp.h>

#define N 30000

double a[N];           // dados
double ant[N];         // ant[i] = soma dos elementos antes de i   (a[0] + ... + a[i-1])
double pos[N];         // pos[i] = soma dos elementos depois de i  (a[i+1] + ... + a[N-1])

int main() {
    double soma_total = 0.0;

    for (int i = 0; i < N; i++) {
        a[i] = (i * 7) % 10 + 1;
        soma_total += a[i];
    }

    double t0 = omp_get_wtime();
    double t1;
    int ok = 1;

    #pragma omp parallel
    {   
        #pragma omp for
            // Etapa 1: acumulado antes de cada posição
            for (int i = 0; i < N; i++) {
                double s = 0.0;
                for (int j = 0; j < i; j++)
                    s += a[j];
                ant[i] = s;
            }

            // Etapa 2: acumulado depois de cada posição
            #pragma omp for
            for (int i = 0; i < N; i++) {
                double s = 0.0;
                for (int j = i + 1; j < N; j++)
                    s += a[j];
                pos[i] = s;
            }

            // Uma thread registra o tempo
            #pragma omp single
            {
                t1 = omp_get_wtime();
            }

            #pragma omp for reduction(&&:ok)
            for (int i = 0; i < N; i++)
                if (ant[i] + a[i] + pos[i] != soma_total)
                    ok = 0;

            
    }

    printf("Soma total = %.0f\n", soma_total);
    printf("Verificacao: %s\n", ok ? "OK" : "FALHOU");
    printf("Tempo = %.2f s\n", t1 - t0);
    
    return 0;
}


// gcc -o ex10 ex9.c -O2 -fopenmp
// for t in 1 2 3; do echo "== $t thread(s)"; OMP_NUM_THREADS=$t ./ex9; done

//* Versao do exercicio
// == 1 thread(s)
// Soma total = 165000
// Verificacao: OK
// Tempo = 0.69 s

// == 2 thread(s)
// Soma total = 165000
// Verificacao: OK
// Tempo = 0.69 s

// == 3 thread(s)
// Soma total = 165000
// Verificacao: OK
// Tempo = 0.69 s

//* Minha versão
// == 1 thread(s)
// Soma total = 165000
// Verificacao: OK
// Tempo = 0.73 s
// == 2 thread(s)
// Soma total = 165000
// Verificacao: OK
// Tempo = 0.52 s
// == 3 thread(s)
// Soma total = 165000
// Verificacao: OK
// Tempo = 0.40 s
