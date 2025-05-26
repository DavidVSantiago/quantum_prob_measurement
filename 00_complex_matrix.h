#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/** Tipo Abstrato de Dados que representa um número complexo */
typedef struct{
    float real, imag;
}Complex;

/** Imprime uma matriz complexa */
void printMatrix(Complex** matrix, int l, int c){
    for(int i=0;i<l;i++){
        for(int j=0;j<c;j++){
            int real = matrix[i][j].real;
            int imag = matrix[i][j].imag;
            if(j==0)printf("|\t");
            if(real!=0)printf("%d",real);
            if(imag>0&&real!=0)printf("+");
            if(imag!=0){
                if(imag==1) printf("i");
                else if(imag==-1) printf("-i");
                else printf("%di",imag);
            }
            if(real==0&&imag==0) printf("0");
            printf("\t");
            if(j==(c-1))printf("|");
        }
        printf("\n");
    }
}

/** Imprime uma matriz complexa, mas mantém os valores com casas decimais */
void printMatrixAsFloat(Complex** matrix, int l, int c){
    for(int i=0;i<l;i++){
        for(int j=0;j<c;j++){
            float real = matrix[i][j].real;
            float imag = matrix[i][j].imag;
            if(j==0)printf("|\t");
            if(real!=0)printf("%0.1f",real);
            if(imag>0&&real!=0)printf("+");
            if(imag!=0){
                if(imag==1) printf("i");
                else if(imag==-1) printf("-i");
                else printf("%0.1fi",imag);
            }
            if(real==0&&imag==0) printf("0");
            printf("\t");
            if(j==(c-1))printf("|");
        }
        printf("\n");
    }
}

/** Imprime uma valor complexo */
void printComplex(Complex complex){
    if(complex.real!=0)printf("%d",(int)complex.real);
    if(complex.imag>0&&complex.real!=0)printf("+");
    if(complex.imag!=0){
        if(complex.imag==1) printf("i");
        else if(complex.imag==-1) printf("-i");
        else printf("%di",(int)complex.imag);
    }
    if(complex.real==0&&complex.imag==0) printf("0");
}

/** Imprime uma valor complexo, mas mantém os valores com casas decimais */
void printComplexAsFloat(Complex complex){
    if(complex.real!=0)printf("%0.1f",complex.real);
    if(complex.imag>0&&complex.real!=0)printf("+");
    if(complex.imag!=0){
        if(complex.imag==1) printf("i");
        else if(complex.imag==-1) printf("-i");
        else printf("%0.1fi",complex.imag);
    }
    if(complex.real==0&&complex.imag==0) printf("0");
}

/** Aloca e retorna uma matriz de dimensões lxc */
Complex** makeMatrix(int l, int c) {
    Complex** matriz = (Complex**) malloc(l * sizeof(Complex*));
    for (int i = 0; i < l; i++) {
        matriz[i] = (Complex*) malloc(c * sizeof(Complex));
    }
    return matriz;
}

/** Aloca e retorna um complexo */
Complex* makeComplex() {
    return (Complex*) malloc(sizeof(Complex));
}

/** Aloca e retorna uma matriz de dimensões lxc ja preenchida pelo usuário */
Complex** makeMatrixFilled(int l, int c) {
    Complex** matrix = makeMatrix(l,c);
    for(int i=0;i<l;i++){// percorre as linhas da matriz
        for(int j=0;j<c;j++){// percorre as colunas da matriz
            printf("\n[%d,%d]-parte real: ",i,j);
            scanf("%f",&matrix[i][j].real);
            printf("[%d,%d]-parte imaginaria: ",i,j);
            scanf("%f",&matrix[i][j].imag);
        }
    }
    return matrix;
}

/** Recebe um matrix e retorna uma cópia */
Complex** copyMatrix(Complex **A, int l, int c){
    Complex** copy = makeMatrix(l,c);
    for(int i=0;i<l;i++){
        for(int j=0;j<c;j++){
            copy[i][j] = A[i][j]; // copia cada um dos valores
        }
    }
    return copy;
}

/** Recebe duas matrizes 'A' e 'B' e retorna o seu produto 'C'*/
Complex** multiplyMatrices(Complex **A, int la, int ca, Complex **B, int lb, int cb){
    Complex **C = makeMatrix(la,cb); // matriz que armazena o resultado
    for(int l=0;l<la;l++){ // percorre as linhas da matriz A
        for(int c=0;c<cb;c++){ // percorre as colunas da matriz B
            int realSum=0,imagSum=0;
            for(int r=0;r<ca;r++){ // percorre as colunas de A e linhas de B, fazendo as multiplicações e somatórios
                int val_a = A[l][r].real*B[r][c].real; // real
                int val_b = A[l][r].real*B[r][c].imag; // imaginárias
                int val_c = A[l][r].imag*B[r][c].real; // imaginárias
                int val_d = A[l][r].imag*B[r][c].imag; // real (pois i² = -1)
                realSum+=val_a-val_d;
                imagSum+=val_b+val_c;
            }
            C[l][c].real=realSum;
            C[l][c].imag=imagSum;
        }
    }
    return C;
}

/** Recebe uma matriz e um escalar e retorna o seu produto*/
Complex** multiplyMatrixScalar(Complex **A, int l, int c, Complex scalar){
    Complex **C = copyMatrix(A,l,c);
    for(int i=0;i<l;i++){
        for(int j=0;j<c;j++){
            int a = C[i][j].real*scalar.real; // resulta em real
            int b = C[i][j].real*scalar.imag; // resulta em imaginario
            int c = C[i][j].imag*scalar.real; // resulta em imaginario
            int d = C[i][j].imag*scalar.imag; // resulta em real (pois multiplica por i², ou seja, por -1)
            C[i][j].real = (a-d);
            C[i][j].imag = (b+c);
        }
    }
    return C;
}

/** Recebe uma matriz e retorna a sua transposta */
Complex** calculateTranspose(Complex** matrix, int l, int c){
    Complex **tranposeMatrix = makeMatrix(c,l);// alocação da matriz transposta
    for(int i=0;i<l;i++){ // percorre as linhas da matriz
        for(int j=0;j<c;j++){ // percorre as colunas da matriz
            tranposeMatrix[j][i]=matrix[i][j]; // transposição
        }
    }
    return tranposeMatrix;
}

/** Recebe uma matriz e retorna a sua conjugada */
Complex** calculateConjugate(Complex** matrix, int l, int c){
    Complex **conjugateMatrix = makeMatrix(l,c);// alocação da matriz transposta
    for(int i=0;i<l;i++){ // percorre as linhas da matriz
        for(int j=0;j<c;j++){ // percorre as colunas da matriz
            conjugateMatrix[i][j]=matrix[i][j]; // cópia
            conjugateMatrix[i][j].imag *= -1; // conjugado
        }
    }
    return conjugateMatrix;
}

/** Recebe uma matriz e retorna a sua adjunta */
Complex** calculateAdjoint(Complex** matrix, int l, int c){
    Complex** adjointMatrix = makeMatrix(c,l); // aloca a matriz adjunta
    for(int i=0;i<l;i++){// percorre as linhas da matriz
        for(int j=0;j<c;j++){// percorre as colunas da matriz
            adjointMatrix[j][i] = matrix[i][j]; // transposição
            adjointMatrix[j][i].imag*=-1; // conjugado
        }
    }
    return adjointMatrix;
}

/** Recebe uma matriz e verifica se ela é uma matriz identidade */
int checkIdentity(Complex **matrix, int l, int c){
    for(int i=0;i<l;i++){// percorre as linhas da matriz
        for(int j=0;j<c;j++){// percorre as colunas da matriz
            if(i!=j){ // se não for a diagonal principal
                if(((int)matrix[i][j].real)!=0 && ((int)matrix[i][j].imag)!=0) return 0; // se o valor for diferente de zero
            }else{ // se for a diagonal principal
                if(((int)matrix[i][j].real)!=1 || ((int)matrix[i][j].imag)!=0) return 0; // se o valor for diferente de 1
            }
        }
    }
    return 1;
}

/** Recebe uma matriz 2x2 e retorna a sua determinante */
Complex calculateDeterminant_2x2(Complex** matrix){
    // diagonal principal
    float a = matrix[0][0].real*matrix[1][1].real; // real
    float b = matrix[0][0].real*matrix[1][1].imag; // imaginaria
    float c = matrix[0][0].imag*matrix[1][1].real; // imaginaria
    float d = matrix[0][0].imag*matrix[1][1].imag; // real (pois i² = -1)
    float dp_real = a-d;
    float dp_image = b+c;
    // diagonal secundária
    a = matrix[0][1].real*matrix[1][0].real; // real
    b = matrix[0][1].real*matrix[1][0].imag; // imaginaria
    c = matrix[0][1].imag*matrix[1][0].real; // imaginaria
    d = matrix[0][1].imag*matrix[1][0].imag; // real (pois i² = -1)
    float ds_real = a-d;
    float ds_image = b+c;
    // calculo da deteminante
    Complex determinante;
    determinante.real = dp_real-ds_real;
    determinante.imag = dp_image-ds_image;
    return determinante;
}

/** Recebe dois vetores 'A' e 'B' , de mesmo tamanho, e retorna o seu produto interno 'C'*/
Complex* calculateInnerProduct(Complex **A, Complex **B, int size){
    Complex **adjA = calculateAdjoint(A,size,1);
    Complex **result = multiplyMatrices(adjA,1,size,B,size,1);
    Complex *scalar = makeComplex();
    scalar->real = result[0][0].real;
    scalar->imag = result[0][0].imag;
    return scalar;
}

/** Recebe dois vetores 'A' e 'B' , de tamanhos distintos, e retorna o seu produto externo 'C'*/
Complex** calculateOuterProduct(Complex** A, int sizeA, Complex** B, int sizeB){
    Complex** adjB = calculateAdjoint(B,sizeB,1);
    Complex** C = multiplyMatrices(A,sizeA,1,adjB,1,sizeB);
    return C;
}

/** Recebe um vetor e retorna a sua norma */
float calculateVectorNorm(Complex **A, int l){ 
    Complex* innerProduct = calculateInnerProduct(A,A,l); // produto interno de A com A, a parte imaginária sempre será zero
    return sqrt(innerProduct->real); // desconsidera a parte imaginária, pois ela é zero
}

/** Recebe um vetor e rotorna uma cópia normalizada */
Complex** calculateNormalizedVector(Complex **A, int l){
    float normaVetor = calculateVectorNorm(A,l); // calcula a norma do vetor
    Complex **normalizedA = copyMatrix(A,l,1); // cria uma cópia do vetor
    for (int i = 0; i < l; i++){ // percorre as linhas para normalizar o vetor
        normalizedA[i][0].real = normalizedA[i][0].real/normaVetor;
        normalizedA[i][0].imag = normalizedA[i][0].imag/normaVetor;
    }
    return normalizedA;
}

/** Recebe duas matrizes 'A' e 'B' e retorna o seu produto tensorial 'C'*/
Complex** tensorProduct(Complex** A, int la, int ca, Complex** B, int lb, int cb){
    Complex** tensorResult = makeMatrix((la*lb),(ca*cb)); // cria a matriz que irá armazenar o resultado
    for(int i_la=0; i_la<la; i_la++){ // percorre as linhas de A
        for(int i_ca=0; i_ca<ca; i_ca++){ // percorre as colunas de A
            for(int i_lb=0; i_lb<lb; i_lb++){ // percorre as linhas de B
                for(int i_cb=0; i_cb<cb; i_cb++){ // percorre as colunas de B
                    float val_a = B[i_lb][i_cb].real*A[i_la][i_ca].real; // real
                    float val_b = B[i_lb][i_cb].real*A[i_la][i_ca].imag; // imaginário
                    float val_c = B[i_lb][i_cb].imag*A[i_la][i_ca].real; // imaginário
                    float val_d = B[i_lb][i_cb].imag*A[i_la][i_ca].imag; // real
                    tensorResult[(i_la*lb)+i_lb][(i_ca*cb)+i_cb].real = val_a-val_d;
                    tensorResult[(i_la*lb)+i_lb][(i_ca*cb)+i_cb].imag = val_b+val_c;
                }
            }
        }
    }
    return tensorResult;
}