// %%writefile p1_par.c
// VERSÃO PARALELA — parta do código sequencial abaixo.
// Comente cada diretiva e cláusula: o que faz e por que é necessária.

#include <stdio.h>
#include <omp.h>

#define N 80000        // classifica todos os números de 2 a N

int main() {
    int n, d;
    long s;                    // soma dos divisores próprios de n
    long perfeitos = 0;        // s == n
    long abundantes = 0;       // s > n
    long deficientes = 0;      // s < n
    long maior_exc = 0;        // maior excesso (s - n) entre os abundantes
    int  n_maior = 0;          // número que tem o maior excesso (empate: o menor n)
    double t0 = omp_get_wtime();



      
      #pragma omp parallel for schedule(runtime) private(s, d) reduction(+:perfeitos, abundantes, deficientes)
      for (n = 2; n <= N; n++) {
            long maior_exc_t = 0;        
      int  n_maior_t = 0;    
          s = 0;
          for (d = 1; d <= n / 2; d++)
              if (n % d == 0)
                  s += d;

          if (s == n)
              perfeitos++;
          else if (s > n) {
              abundantes++;
              if (s - n > maior_exc_t) {
                  maior_exc_t = s - n;
                  n_maior_t = n;
              }
          } else
              deficientes++;

        #pragma omp critical
        {
            if (maior_exc_t > maior_exc) {
                maior_exc = maior_exc_t;
                n_maior = n_maior_t;
            }
        }



    }
    // Ao terminar suas iterações, cada thread entra uma única vez em uma região critical para comparar seu resultado com o global.

    double t1 = omp_get_wtime();
    printf("Perfeitos   = %ld\n", perfeitos);
    printf("Abundantes  = %ld\n", abundantes);
    printf("Deficientes = %ld\n", deficientes);
    printf("Maior excesso = %ld (n = %d)\n", maior_exc, n_maior);
    printf("Tempo = %.2f s\n", t1 - t0);
    return 0;
}