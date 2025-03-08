#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// ****************************************************************************************
// UTILS FUNCTIONS
// ****************************************************************************************
// Função para calcular o MDC usando o Algoritmo de Euclides
int MDC(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}
// Função para calcular o MMC usando a relação MMC(a, b) = (a * b) / MDC(a, b)
int MMC(int a, int b) {
    return (a / MDC(a, b)) * b; // Evita overflow ao dividir antes de multiplicar
}
// ****************************************************************************************

/** Tipo Abstrato de Dados que representa um valor numerico componente da amplitude */
class Value{
public:
    int val;
    int rootIndex; // (0 = simple number; 2 = squareroot)
    int factor; // fator multiplicativo
    
    Value():val(1),rootIndex(0),factor(1){} // default constructor
    Value(int val,int rootIndex,int factor):val(val),rootIndex(rootIndex),factor(factor){}

    bool equals(Value* other){ // verifica se dois objetos 'Value' são iguais
        return (factor==other->factor)&&(val==other->val)&&(rootIndex==other->rootIndex);
    }
};

/** Tipo Abstrato de Dados que representa a fracao de cada um dos termos da amplitude*/
class Fraction{
public:
    Value num;
    Value den;

    Fraction(){} // default constructor
    Fraction(Value num,Value den):num(num),den(den){}

    bool equals(Fraction* other){ // verifica se dois objetos 'Fraction' são iguais
        return (this->num.equals(&other->num))&&(this->den.equals(&other->den));
    }

    void simplifyFraction(){
        // verifica se já está simplificada
        if(MDC(num.val,den.val)==1) return; // caso esteja, não precisa fazer nada.
        // simplifica a fração
        int mdc = MDC(num.val,den.val);
        num.val /= mdc;
        den.val /= mdc;
    }

    /** Realiza a soma das frações. Pelo fato de que (|α|^2) = (x^2+y^2) a soma de frações nunca terão radicais! */
    Fraction* sum(Fraction *other){
        Fraction* result = (Fraction*) malloc(sizeof(Fraction));
        
        if(this->den.equals(&other->den)){ // denominadores iguais (não precisa fazer MDC)
            result->den = this->den; // mantém o denominador
            
            // Numerator SUM (they will never have radical)
            result->num.rootIndex=0; // pois não será raiz quadrada
            result->num.factor=1; // pois não será raiz quadrada
            result->num.val = this->num.val + other->num.val; // soma os valores dos numeradores

        }else{ // denominadores diferentes (precisa fazer MDC)
            int mmc = MMC(this->den.val, other->den.val); // find common denominator
            result->den.val=mmc;
            result->den.rootIndex=0; // pois não será raiz quadrada
            result->den.factor=1; // pois não será raiz quadrada
                
            // Numerator SUM //
            result->num.rootIndex=0; // pois não será raiz quadrada
            result->num.factor=1; // pois não será raiz quadrada
            result->num.val=((mmc/this->den.val)*this->num.val)+((mmc/other->den.val)*other->num.val); // soma os valores dos numeradores (considerando o MMC)
            
        }
        // TODO simplificar (o algoritmo pode somar frações em forma não-reduzida, gerando respostas em forma não-reduzidas)
        result->simplifyFraction();
        return result;
    }
};

/** Tipo Abstrato de Dados que representa a amplitude de probabilidade (um número complexo)*/
class Amplitude{
public:
    Fraction real;
    Fraction imag;

    Amplitude(Fraction real, Fraction imag):real(real),imag(imag){}

    /** Calcula |α|2 do amplitude*/
    Fraction* calcModSquared(){
        // PARTE REAL
        // quadrado da parte real (numerador)
        Fraction realSquared;
        if(this->real.num.rootIndex==0){ // o valor não possui raiz
            realSquared.num.val = this->real.num.val*this->real.num.val;
            realSquared.num.rootIndex = 0;
        }else if(this->real.num.rootIndex==2){ // o valor está como raiz quadrada
            realSquared.num.val = this->real.num.val;
            realSquared.num.rootIndex = 0;
        }
        // quadrado da parte real (denominador)
        if(this->real.den.rootIndex==0){ // o valor não possui raiz
            realSquared.den.val = this->real.den.val*this->real.den.val;
            realSquared.den.rootIndex = 0;
        }else if(this->real.den.rootIndex==2){ // o valor está como raiz quadrada
            realSquared.den.val = this->real.den.val;
            realSquared.den.rootIndex = 0;
        }
    
        // PARTE IMAGINÁRIA
        // (numerador)
        Fraction imagSquared;
        if(this->imag.num.rootIndex==0){ // o valor não possui raiz
            imagSquared.num.val = this->imag.num.val*this->imag.num.val;
            imagSquared.num.rootIndex = 0;
        }else if(this->imag.num.rootIndex==2){ // o valor está como raiz quadrada
            imagSquared.num.val = this->imag.num.val;
            imagSquared.num.rootIndex = 0;
        }
        // (denominador)
        if(this->imag.den.rootIndex==0){ // o valor não possui raiz
            imagSquared.den.val = this->imag.den.val*this->imag.den.val;
            imagSquared.den.rootIndex = 0;
        }else if(this->imag.den.rootIndex==2){ // o valor está como raiz quadrada
            imagSquared.den.val = this->imag.den.val;
            imagSquared.den.rootIndex = 0;
        }
    
        return realSquared.sum(&imagSquared); // soma as frações e retorna o resultado
    }
};

class UniqueQubitState{
public:
    Amplitude alpha;
    Amplitude beta;
    //float* measureQuantumState(...){...}

};

/** Aloca e retorna um complexo */
UniqueQubitState* makeQubitState() {
    return (UniqueQubitState*) malloc(sizeof(UniqueQubitState));
}


// float* measureQuantumState(UniqueQubitState *state){
//     float alphaModSquared = calcModSquared(state->alpha); // calculo de |alpha|²
//     float betaModSquared = calcModSquared(state->beta); // calculo de |beta|²

//     // verifica a necessidade de normalização e realiza, caso necessário
//     if(alphaModSquared+betaModSquared!=1){ // teste de normalização
//         float normalizationFactor = sqrt(alphaModSquared+betaModSquared); // calculo do fator de normalização
//         alpha->real = (alpha->real)/(normalizationFactor);// recalculando a parte real do alpha
//         alpha->imag = (alpha->imag)/(normalizationFactor);// recalculando a parte imaginária do alpha
//         beta->real = (beta->real)/(normalizationFactor);// recalculando a parte real do beta
//         beta->imag = (beta->imag)/(normalizationFactor);// recalculando a parte imaginária do beta
//         alphaModSquared = calcModSquared(alpha); // recalcula |alpha|²
//         betaModSquared = calcModSquared(beta); // recalcula |beta|²
//     }

//     float* results = (float*) malloc(sizeof(float)*2); // vetor para os dois resultados
//     results[0]=alphaModSquared; // probabilidade de medir |0⟩
//     results[1]=betaModSquared; // probabilidade de medir |1⟩
//     return results; // retorna as probabilidades calculadas
// }

int main(){
    UniqueQubitState *state = makeQubitState();

    // float *results =  measureQuantumState(state);
    Value f1n(1,0,1);
    Value f1d(6,0,1);
    Value f2n(1,0,1);
    Value f2d(6,0,1);
    Fraction f1(f1n,f1d);
    Fraction f2(f2n,f2d);
    Amplitude amp1(f1,f2);
    Fraction *result =  amp1.calcModSquared();

    // printf("Probabilidade de medir |0> = %f%%\n",results[0]);
    // printf("Probabilidade de medir |1> = %f%%\n",results[1]);
    return 0;
}