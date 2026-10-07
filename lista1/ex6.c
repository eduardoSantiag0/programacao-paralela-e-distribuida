// Exercício 6 - Primos em uma janela de números grandes
// O programa abaixo conta quantos números primos existem no intervalo [INICIO, INICIO + TAM) e encontra o maior deles. Para cada candidato n, ele testa os divisores d de 2 até √n; a variável primo guarda o resultado do teste (1 = primo, 0 = composto).

// Paralelize o laço em n. Comece apenas com #pragma omp parallel for, sem cláusulas, e rode algumas vezes: o resultado bate com o esperado? É o mesmo em todas as execuções? Descubra o que está errado e corrija usando cláusulas de compartilhamento de dados.

// Requisitos:

// Paralelizar o laço em n com OpenMP;
// Classificar cada variável (n, d, primo, qtd, maior) como shared, private ou reduction;
// Obter resultado idêntico ao sequencial, com qualquer número de threads e em todas as execuções;
// Refletir: rode a versão só com parallel for várias vezes. O resultado é sempre o mesmo? Se maior saiu certo, isso quer dizer que estava correto?
// Comparar o tempo com 1 thread e com todas as threads.
// Saída esperada:

// Primos em [1000000000, 1000400000) = 19259
// Maior primo encontrado = 1000399997

#include <stdio.h>
#include <omp.h>

#define INICIO  1000000000     // procura primos em [INICIO, INICIO + TAM)
#define TAM     400000

int main() {
    int n;                     // candidato
    int primo;                 // 1 se o candidato n é primo, 0 caso contrário
    int qtd = 0;               // quantidade de primos encontrados
    int maior = 0;             // maior primo encontrado

    double t0 = omp_get_wtime();

    for (n = INICIO; n < INICIO + TAM; n++) {
        primo = 1;
        for (int d = 2; d * d <= n; d++) {
            if (n % d == 0) {
                primo = 0;
                break;
            }
        }
        if (primo) {
            qtd++;
            if (n > maior)
                maior = n;
        }
    }

    double t1 = omp_get_wtime();
    printf("Primos em [%d, %d) = %d\n", INICIO, INICIO + TAM, qtd);
    printf("Maior primo encontrado = %d\n", maior);
    printf("Tempo = %.2f s\n", t1 - t0);
    return 0;
}