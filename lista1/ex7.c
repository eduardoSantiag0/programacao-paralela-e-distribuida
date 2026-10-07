// Exercício 7 - Três tarefas independentes
// O programa abaixo executa, uma depois da outra, três tarefas: contar primos, somar os passos da sequência de Collatz e contar números perfeitos. Uma não usa o resultado da outra.

// Cada tarefa imprime qual thread a executou e quanto tempo levou. Paralelize o programa usando sections.

// Requisitos:

// Obter os mesmos resultados da versão sequencial; e
// Comparar o tempo total com 1, 2 e 3 threads e explicar o que aconteceu com 2 threads.
// Saída esperada (a ordem das linhas e os tempos variam):

// Relatorio final sequencial:
//   primos    <= 3300000 : 236900
//   collatz   <= 2000000 : 277182223 passos
//   perfeitos <= 43000 : 4
// Tempo total = 3.98 s


#include <stdio.h>
#include <omp.h>

#define LIM_PRIMOS     3300000     // tarefa A
#define LIM_COLLATZ    2000000     // tarefa B
#define LIM_PERFEITOS    43000     // tarefa C

// Tarefa A: quantidade de primos <= limite (divisão por tentativa)
long tarefa_primos(int limite) {
    double ini = omp_get_wtime();
    long qtd = 0;
    for (int n = 2; n <= limite; n++) {
        int primo = 1;
        for (int d = 2; (long)d * d <= n; d++)
            if (n % d == 0) { primo = 0; break; }
        qtd += primo;
    }
    printf("  [thread %d] primos    terminou em %.2f s\n", omp_get_thread_num(), omp_get_wtime() - ini);
    return qtd;
}

// Tarefa B: soma dos passos da sequência de Collatz de todos os n <= limite
long tarefa_collatz(int limite) {
    double ini = omp_get_wtime();
    long total = 0;
    for (int n = 1; n <= limite; n++) {
        long x = n;
        while (x != 1) {
            x = (x % 2 == 0) ? x / 2 : 3 * x + 1;
            total++;
        }
    }
    printf("  [thread %d] collatz   terminou em %.2f s\n", omp_get_thread_num(), omp_get_wtime() - ini);
    return total;
}

// Tarefa C: quantidade de números perfeitos <= limite
long tarefa_perfeitos(int limite) {
    double ini = omp_get_wtime();
    long qtd = 0;
    for (int n = 2; n <= limite; n++) {
        int soma = 0;
        for (int d = 1; d <= n / 2; d++)
            if (n % d == 0) soma += d;
        if (soma == n) qtd++;
    }
    printf("  [thread %d] perfeitos terminou em %.2f s\n", omp_get_thread_num(), omp_get_wtime() - ini);
    return qtd;
}

int main() {
    long r_primos = 0, r_collatz = 0, r_perfeitos = 0;

    double t0 = omp_get_wtime();

    #pragma omp parallel sections
    {
        #pragma omp section
        { r_primos    = tarefa_primos(LIM_PRIMOS); }
        #pragma omp section
        { r_collatz   = tarefa_collatz(LIM_COLLATZ); }
        #pragma omp section
        { r_perfeitos = tarefa_perfeitos(LIM_PERFEITOS); }

    }


    printf("\nRelatorio final:\n");
    printf("  primos    <= %d : %ld\n", LIM_PRIMOS, r_primos);
    printf("  collatz   <= %d : %ld passos\n", LIM_COLLATZ, r_collatz);
    printf("  perfeitos <= %d : %ld\n", LIM_PERFEITOS, r_perfeitos);
    printf("Tempo total = %.2f s\n", omp_get_wtime() - t0);
    return 0;
}

// gcc -o ex7 ex7.c -O3 -fopenmp
// for t in 1 2 3; do echo "== $t thread(s)"; OMP_NUM_THREADS=$t ./ex7; done