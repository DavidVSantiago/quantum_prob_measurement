#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/** Tipo Abstrato de Dados que representa um valor numerico componente da amplitude */
class Value{
public:
    int num;
    int rootIndex;
};

/** Tipo Abstrato de Dados que representa a fracao de cada um dos termos da amplitude*/
typedef struct{
    Value num;
    Value den;
} Fraction;

/** Tipo Abstrato de Dados que representa a amplitude de probabilidade */
typedef struct {
    Fraction real;
    Fraction imag;
} Amplitude;

/** Aloca e retorna um complexo */
Complex* makeComplex() {
    return (Complex*) malloc(sizeof(Complex));
}

float calcModSquared(Complex *value){
    return (value->real*value->real)+(value->imag*value->imag);
}

float* measureQuantumState(Complex *alpha, Complex *beta){
    float alphaModSquared = calcModSquared(alpha); // calculo de |alpha|²
    float betaModSquared = calcModSquared(beta); // calculo de |beta|²

    // verifica a necessidade de normalização e realiza, caso necessário
    if(alphaModSquared+betaModSquared!=1){ // teste de normalização
        float normalizationFactor = sqrt(alphaModSquared+betaModSquared); // calculo do fator de normalização
        alpha->real = (alpha->real)/(normalizationFactor);// recalculando a parte real do alpha
        alpha->imag = (alpha->imag)/(normalizationFactor);// recalculando a parte imaginária do alpha
        beta->real = (beta->real)/(normalizationFactor);// recalculando a parte real do beta
        beta->imag = (beta->imag)/(normalizationFactor);// recalculando a parte imaginária do beta
        alphaModSquared = calcModSquared(alpha); // recalcula |alpha|²
        betaModSquared = calcModSquared(beta); // recalcula |beta|²
    }

    float* results = (float*) malloc(sizeof(float)*2); // vetor para os dois resultados
    results[0]=alphaModSquared; // probabilidade de medir |0⟩
    results[1]=betaModSquared; // probabilidade de medir |1⟩
    return results; // retorna as probabilidades calculadas
}

void main(){
    Complex *alpha = makeComplex();
    Complex *beta = makeComplex();

    printf("Digite a parte real de alpha: ");
    scanf("%f",&alpha->real);
    printf("\nDigite a parte imaginaria de alpha: ");
    scanf("%f",&alpha->imag);
    printf("\nDigite a parte real de beta: ");
    scanf("%f",&beta->real);
    printf("\nDigite a parte imaginaria de beta: ");
    scanf("%f",&beta->imag);

    float *results =  measureQuantumState(alpha,beta);

    printf("Probabilidade de medir |0> = %f%%\n",results[0]);
    printf("Probabilidade de medir |1> = %f%%\n",results[1]);
}