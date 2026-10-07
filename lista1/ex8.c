// Exercício 8 - Detecção de movimento entre dois quadros
// Uma câmera de segurança compara quadros consecutivos para detectar movimento. O programa abaixo faz isso em duas etapas: 
    // (1) converte os dois quadros para escala de cinza; 
    // (2) conta quantos pixels mudaram de brilho entre os dois quadros. Se algum pixel mudou, há movimento.

// Paralelize o programa usando sections e for.

// Requisitos:

// Indicar, em comentário, quais etapas podem rodar ao mesmo tempo e quais precisam esperar por outras;
// Obter o mesmo resultado da versão sequencial; e
// Comparar o tempo com 1, 2 e 3 threads.
// Saída esperada (o tempo varia):

// Pixels que mudaram = 110000 (1.17%)
// Movimento detectado: sim



#include <stdio.h>
#include <math.h>
#include <omp.h>

#define H 3072
#define W 3072
#define N (H * W)
#define LADO 400            // lado do objeto (quadrado branco)
#define LIMIAR 0.05f        // diferença mínima de brilho para o pixel contar como "mudou"

typedef struct {
    unsigned char r, g, b;
} Pixel;

Pixel quadro1[N], quadro2[N];       // dois quadros consecutivos da câmera
float cinza1[N], cinza2[N];         // versões em escala de cinza

// Fundo com um padrão fixo e um objeto branco com canto superior esquerdo em (ox, oy)
void gera_quadro(Pixel *q, int ox, int oy) {
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            Pixel p;
            p.r = (x * 3 + y) & 127;
            p.g = (x + y * 5) & 127;
            p.b = (x * y) & 127;
            if (x >= ox && x < ox + LADO && y >= oy && y < oy + LADO)
                p.r = p.g = p.b = 255;
            q[y * W + x] = p;
        }
    }
}

// Escala de cinza pela luminância em luz linear (fórmula Rec. 709)
void converte_cinza(const Pixel *q, float *cinza) {
    for (int i = 0; i < N; i++) {
        float r = powf(q[i].r / 255.0f, 2.2f);
        float g = powf(q[i].g / 255.0f, 2.2f);
        float b = powf(q[i].b / 255.0f, 2.2f);
        cinza[i] = 0.2126f * r + 0.7152f * g + 0.0722f * b;
    }
}

int main() {
    int mudou = 0;              // quantidade de pixels que mudaram entre os quadros
    
    gera_quadro(quadro1, 200, 200);  
    gera_quadro(quadro2, 300, 250);  
    

    double t0 = omp_get_wtime();

    #pragma omp parallel 
    {
        // ETAPA 1:
        // As conversões dos dois quadros são independentes e podem
        // executar simultaneamente em threads diferentes.
        #pragma omp parallel sections 
        {
            #pragma omp  section 
            {
                converte_cinza(quadro1, cinza1);
            } 
            
            #pragma omp  section 
            {
                converte_cinza(quadro2, cinza2);
            } 
        }


        
        // ETAPA 2:
        // A comparação dos pixels pode ser dividida entre várias threads,
        // pois cada índice i é independente.
        // "mudou" precisa de reduction para evitar race condition.
        #pragma omp parallel for reduction(+:mudou)
        for (int i = 0; i < N; i++) {
            if (fabsf(cinza1[i] - cinza2[i]) > LIMIAR)
                mudou++;
        }

    }


    double t1 = omp_get_wtime();
    printf("Pixels que mudaram = %d (%.2f%%)\n", mudou, 100.0 * mudou / N);
    printf("Movimento detectado: %s\n", mudou > 0 ? "sim" : "nao");
    printf("Tempo = %.2f s\n", t1 - t0);
    return 0;
}

// gcc -o ex8 ex8.c -O3 -fopenmp -lm
// for t in 1 2 3; do echo "== $t thread(s)"; OMP_NUM_THREADS=$t ./ex8; done