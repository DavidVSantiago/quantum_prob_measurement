#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <iostream>

using namespace std;

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

/** verifica se duas frações são iguais */
bool equals(short A[3], short B[3]){
    return A[0]==B[0]&&A[1]==B[1]&&A[2]==B[2];
}

void simplifyFraction(short *frac){
    int mdc = MDC(frac[0],frac[3]);
    // simplifica a fração, caso precise
    if(mdc!=1){
        frac[0] /= mdc;
        frac[3] /= mdc;
    }
}

/** Realiza a soma das frações. Pelo fato de que (|α|^2) = (x^2+y^2) a soma de frações nunca terão radicais! */
short* sum(short frac_A[6],short frac_B[6]){
    // arrays para armazenar os valores de num e den das frações
    short frac_C_num[3],frac_C_den[3]; // fração resultado
    short frac_A_num[3],frac_A_den[3]; 
    short frac_B_num[3],frac_B_den[3];
    
    for(int i=0;i<3;i++){frac_A_num[i]=frac_A[i];frac_B_num[i]=frac_B[i];}// extrai o num das frações
    for(int i=3;i<6;i++){frac_A_den[i-3]=frac_A[i];frac_B_den[i-3]=frac_B[i];}// extrai o den das frações

    if(equals(frac_A_den,frac_B_den)){ // denominadores iguais (não precisa fazer MDC)
        // frac_C_den = frac_A_den; // mantém o denominador
        for(int i=0;i<3;i++){frac_C_den[i]=frac_A_den[i];}

        // Numerator SUM (they will never have radical)
        frac_C_num[1]=0; // pois não será raiz quadrada
        frac_C_num[2]=1; // pois não será raiz quadrada
        frac_C_num[0]=frac_A_num[0]+frac_B_num[0]; // soma os valores dos numeradores

    }else{ // denominadores diferentes (precisa fazer MDC)
        int mmc = MMC(frac_A_den[0], frac_B_den[0]); // find common denominator
        frac_A_den[0]=mmc;
        frac_A_den[1]=0; // pois não será raiz quadrada
        frac_A_den[2]=1; // pois não será raiz quadrada
            
        // Numerator SUM //
        frac_C_num[1]=0; // pois não será raiz quadrada
        frac_C_num[2]=1; // pois não será raiz quadrada
        frac_C_num[0]=((mmc/frac_A_den[0])*frac_A_num[0])+((mmc/frac_B_den[0])*frac_B_num[0]); // soma os valores dos numeradores (considerando o MMC)
    }

    // junta o numerador e denominador em uma única fração resultado
    short *frac_C = (short*) malloc(sizeof(short)*6); // fração resultado
    for(int i=0;i<3;i++){frac_C[i]=frac_C_num[i];}
    for(int i=3;i<6;i++){frac_C[i]=frac_C_den[i-3];}

    simplifyFraction(frac_C);
    return frac_C;
}

/** Calcula |α|2 do amplitude*/
short* calcAmpModSquared(short state[2][6]){
    
    short real[6]; // array para armazenar a parte real do estado
    short imag[6]; // array para armazenar a parte imaginária do estado
    for(int i=0;i<6;i++){real[i]=state[0][i];}// obtém a parte real do estado
    for(int i=0;i<6;i++){imag[i]=state[1][i];}// obtém a parte imaginária do estado

    // processamento da parte real
    short realSquared[6]; // array para armazenar a parte real do estado ao quadrado (já processada)
    if(real[1]==0){ // o num da parte real não possui raiz
        realSquared[0] = real[0]*real[0];
        realSquared[1]=0;
        realSquared[2]=1;
    }else if(real[1]==2){ // o num da parte real possui raiz
        realSquared[0] = real[0];
        realSquared[1]=0;
        realSquared[2]=1;
    }
    if(real[4]==0){ // o den da parte real não possui raiz
        realSquared[3] = real[3]*real[3];
        realSquared[4]=0;
        realSquared[5]=1;
    }else if(real[4]==2){ // o den da parte real possui raiz
        realSquared[3] = real[3];
        realSquared[4]=0;
        realSquared[5]=1;
    }

    // processamento da parte imaginaria
    short imagSquared[6]; // array para armazenar a parte real do estado ao quadrado (já processada)
    if(imag[1]==0){ // o num da parte real não possui raiz
        imagSquared[0] = imag[0]*imag[0];
        imagSquared[1]=0;
        imagSquared[2]=1;
    }else if(imag[1]==2){ // o num da parte real possui raiz
        imagSquared[0] = imag[0];
        imagSquared[1]=0;
        imagSquared[2]=1;
    }
    if(imag[4]==0){ // o den da parte real não possui raiz
        imagSquared[3] = imag[3]*imag[3];
        imagSquared[4]=0;
        imagSquared[5]=1;
    }else if(imag[4]==2){ // o den da parte real possui raiz
        imagSquared[3] = imag[3];
        imagSquared[4]=0;
        imagSquared[5]=1;
    }

    return sum(realSquared,imagSquared); // soma as frações e retorna o resultado
}

void printModSquared(short s[2][6], short r[6]){
    string rn,rd,in,id;
    rn = to_string(s[0][0]); //if(s[0][1]==2) rn="√"+rn;
    rd = to_string(s[0][3]); //if(s[0][4]==2) rd="√"+rd;
    in = to_string(s[1][0]); //if(s[1][1]==2) in="√"+in;
    id = to_string(s[1][3]); //if(s[1][4]==2) id="√"+id;
    cout << "O módulo da amplitude (("<<rn<<"/"<<rd<<")+("<<in<<"/"<<id<<")i) ao quadrado = (("<<rn<<"/"<<rd<<")^2 + ("<<in<<"/"<<id<<")^2) = ("<<r[0]<<"/"<<r[3]<<")"<<endl;
}

// void printState(short s[4][6]){
//     string arn,ard,ain,aid,brn,brd,bin,bid;
//     arn = s[0][0]; if(s[0][1]==2) arn="√"+arn;
//     ard = s[0][3]; if(s[0][4]==2) ard="√"+ard;
//     ain = s[1][0]; if(s[1][1]==2) ain="√"+ain;
//     aid = s[1][3]; if(s[1][4]==2) aid="√"+aid;
//     brn = s[2][0]; if(s[2][1]==2) brn="√"+brn;
//     brd = s[2][3]; if(s[2][4]==2) brd="√"+brd;
//     bin = s[3][0]; if(s[3][1]==2) bin="√"+bin;
//     bid = s[3][3]; if(s[3][4]==2) bid="√"+bid;
//     printf("\nO estado |ψ⟩= ((%d/%d)+(%d/%d))|0⟩ + ((%d/%d)+(%d/%d))|1⟩ = (%d/%s)\n",arn,ard,ain,aid,brn,brd,bin,bid);
// }

int main(){
    
    short state1[4][6] = {{1,0,1,6,2,1},
                          {1,0,1,6,2,1},
                          {2,2,1,3,2,1},
                          {0,0,0,0,0,0}};
    
    short *result = calcAmpModSquared(state1);
    printModSquared(state1,result);

    return 0;
}