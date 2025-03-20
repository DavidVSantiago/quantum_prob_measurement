#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

/** Tipo Abstrato de Dados que representa a matriz simples de qutib único */
class SimpleQubitState{
private:
    // ATRIBUTES ------------------------------------------------------
    int data[18]; // symbolic data state
    int alphaSqrd[2]; // temporary stores |α|^2 and |α'|^2
    int betaSqrd[2]; // temporary stores |β|^2 and |β'|^2
    int zf_wf[2]; // temporary stores "alphaSqrd" + "betaSqrd"
    bool normalized = true; // para indicar se o estado se encontra normalizado
    bool calculated = false; // para indicar se os cálculos do estado já foram realidaos
    
    // UTILS FUNCTIONS ---------------------------------------------------
    // Função para calcular o MDC usando o Algoritmo de Euclides
    int MDC(int a, int b) {
        while (b != 0) {
            int temp = b;
            b = a % b;
            a = temp;
        }
        return a;
    }
    /** Função para calcular o MMC usando a relação MMC(a, b) = (a * b) / MDC(a, b) */
    int MMC(int a, int b) {
        return (a / MDC(a, b)) * b; // Evita overflow ao dividir antes de multiplicar
    }
    /** Simplifica, se possível, uma fração */
    void simplifyFraction(int *frac) {
        int mdc = MDC(frac[0],frac[1]);
        // simplifica a fração, caso precise
        if(mdc!=1){
            frac[0] /= mdc; // simplify numerator
            frac[1] /= mdc; // simplify denominator
        }
    }
    
    /** calcula zf/wf somando |α|^2 e |β|^2*/
    void sumFractions(){ 
        int mmc = MMC(alphaSqrd[1],betaSqrd[1]); // mmc dos denominadores
        zf_wf[1] = mmc; // atribui o denominador final
        zf_wf[0] = ((mmc/alphaSqrd[1])*alphaSqrd[0])+((mmc/betaSqrd[1])*betaSqrd[0]); // atribui o numerador final
        simplifyFraction(zf_wf);
    }
    // CALC FUNCTIONS ---------------------------------------------------
    /** Calculate |α|^2 and |β|^2 */
    void calcAmpModSquared(){
        /** calcula x² (alpha) */
        if(data[1]==0) alphaSqrd[0] = data[0]*data[0]; // x is not root, apenas calcula x² e o atribui a z de z/w
        else alphaSqrd[0] = (data[2]*data[2]*data[0]); // se x for raiz, eleva o fator ao quadrado e multiplica pelo valor. Ex.: (a√b)² = a².b - Após isso, atribui a z de z/w
        /** calcula y² (alpha) */
        if(data[4]==0) alphaSqrd[0] += data[3]*data[3]; // y is not root, apenas calcula y² e o adiciona a z de z/w (lembrar que z=x²+y²)
        else alphaSqrd[0] += (data[5]*data[5]*data[3]); // se y for raiz, eleva o fator ao quadrado e multiplica pelo valor. Ex.: (a√b)² = a².b - Após isso, adiciona a z de z/w (lembrar que z=x²+y²)
        /** calcula d² (alpha) */
        if(data[7]==0) alphaSqrd[1] = data[6]*data[6]; // d is not root, apenas calcula d² e o atribui a w de z/w (lembrar que w=d²)
        else alphaSqrd[1] = (data[8]*data[8]*data[6]); // se d for raiz, eleva o fator ao quadrado e multiplica pelo valor. Ex.: (a√b)² = a².b - Após isso, atribui a w de z/w (lembrar que w=d²)

        /** calcula x² (beta) */
        if(data[10]==0) betaSqrd[0] = data[9]*data[9]; // x is not root, apenas calcula x² e o atribui a z de z/w
        else betaSqrd[0] = (data[11]*data[11]*data[9]); // se x for raiz, eleva o fator ao quadrado e multiplica pelo valor. Ex.: (a√b)² = a².b - Após isso, atribui a z de z/w  
        /** calcula y² (beta) */
        if(data[13]==0) betaSqrd[0] += data[12]*data[12]; // y is not root, apenas calcula y² e o adiciona a z de z/w (lembrar que z=x²+y²)
        else betaSqrd[0] += (data[14]*data[14]*data[12]); // se y for raiz, eleva o fator ao quadrado e multiplica pelo valor. Ex.: (a√b)² = a².b - Após isso, adiciona a z de z/w (lembrar que z=x²+y²)
        /** calcula d² (beta) */
        if(data[16]==0) betaSqrd[1] = data[15]*data[15]; // d is not root, apenas calcula d² e o atribui a w de z/w (lembrar que w=d²)
        else betaSqrd[1] = (data[17]*data[17]*data[15]); // se d for raiz, eleva o fator ao quadrado e multiplica pelo valor. Ex.: (a√b)² = a².b - Após isso, atribui a w de z/w (lembrar que w=d²)

        simplifyFraction(alphaSqrd); // simplifica (se possível) a fração z/w (alpha)
        simplifyFraction(betaSqrd); // simplifica (se possível) a fração z/w (beta)
    }
    /** Calculate |α'|^2 and |β'|^2 */
    void calcAmpModSquaredNormalized(int *zf_wf){
        /** calcula x² (alpha) */
        if(data[1]==0) alphaSqrd[0] = zf_wf[1]*(data[0]*data[0]); // x is not root, apenas calcula wf*x² e o atribui a z' de z'/w'
        else alphaSqrd[0] = zf_wf[1]*(data[2]*data[2]*data[0]); // se x for raiz, eleva o fator ao quadrado e multiplica pelo valor. Ex.: (a√b)² = a².b - Após isso, multiplica por wf e atribui a z' de z'/w'
        /** calcula y² (alpha) */
        if(data[4]==0) alphaSqrd[0] += zf_wf[1]*(data[3]*data[3]); // y is not root, apenas calcula wf*y² e o adiciona a z' de z'/w'
        else alphaSqrd[0] += zf_wf[1]*(data[5]*data[5]*data[3]); // se y for raiz, eleva o fator ao quadrado e multiplica pelo valor. Ex.: (a√b)² = a².b - Após isso,  multiplica por wf e adiciona a z' de '/w'
        /** calcula d² (alpha) */
        if(data[7]==0) alphaSqrd[1] = zf_wf[0]*(data[6]*data[6]); // d is not root, apenas calcula zf*d² e o atribui a w' de z'/w'
        else alphaSqrd[1] = zf_wf[0]*(data[8]*data[8]*data[6]); // se d for raiz, eleva o fator ao quadrado e multiplica pelo valor. Ex.: (a√b)² = a².b - Após isso,  multiplica por zf e atribui a w' de z'/w'

        /** calcula x² (beta) */
        if(data[10]==0) betaSqrd[0] = zf_wf[1]*(data[9]*data[9]); // x is not root, apenas calcula wf*x² e o atribui a z' de z'/w'
        else betaSqrd[0] = zf_wf[1]*(data[11]*data[11]*data[9]); // se x for raiz, eleva o fator ao quadrado e multiplica pelo valor. Ex.: (a√b)² = a².b - Após isso, multiplica por wf e atribui a z' de z'/w'  
        /** calcula y² (beta) */
        if(data[13]==0) betaSqrd[0] += zf_wf[1]*(data[12]*data[12]); // y is not root, apenas calcula wf*y² e o adiciona a z' de z'/w'
        else betaSqrd[0] += zf_wf[1]*(data[14]*data[14]*data[12]); // se y for raiz, eleva o fator ao quadrado e multiplica pelo valor. Ex.: (a√b)² = a².b - Após isso,  multiplica por wf e adiciona a z' de z'/w'
        /** calcula d² (beta) */
        if(data[16]==0) betaSqrd[1] = zf_wf[0]*(data[15]*data[15]); // d is not root, apenas calcula zf*d² e o atribui a w' de z'/w'
        else betaSqrd[1] = zf_wf[0]*(data[17]*data[17]*data[15]); // se d for raiz, eleva o fator ao quadrado e multiplica pelo valor. Ex.: (a√b)² = a².b - Após isso,  multiplica por zf e atribui a w' de z'/w'

        simplifyFraction(alphaSqrd); // simplifica (se possível) a fração z/w (alpha)
        simplifyFraction(betaSqrd); // simplifica (se possível) a fração z/w (beta)
    }
    
    // PRINT FUNCTIONS ---------------------------------------------------
    string getValue(int v, int r, int f){
       if(data[r]==2){

       }else return  
    }
    string getAlphaRealAsString(){ return "("++"/"+getDenAsString(0)+")"; }
    string getAlphaImagAsString(){ return "("+getImagAsString(0)+"/"+getDenAsString(0)+")"; }
    string getBetaRealAsString(){ return "("+getRealAsString(1)+"/"+getDenAsString(1)+")"; }
    string getBetaImagAsString(){ return "("+getImagAsString(1)+"/"+getDenAsString(1)+")"; } 
    string getAlphaAsString(){
        string alphaReal = getAlphaRealAsString();
        string alphaImag = getAlphaImagAsString();
        if(alphaReal.substr(0, 3) == "(0/") return "("+alphaImag+"i)";
        else if(alphaImag.substr(0, 3) == "(0/") return "("+alphaReal+")";
        else return "("+alphaReal+"+"+alphaImag+"i)";
    }
    string getBetaAsString(){
        string betaReal = getBetaRealAsString();
        string betaImag = getBetaImagAsString();
        if(betaReal.substr(0, 3) == "(0/") return "("+betaImag+"i)";
        else if(betaImag.substr(0, 3) == "(0/") return "("+betaReal+")";
        else return "("+betaReal+"+"+betaImag+"i)";
    }
    string getStateAsString(){
        return "|ψ⟩ = "+getAlphaAsString()+"|0⟩ + "+getBetaAsString()+"|1⟩";
    }
    
public:
    // CONSTRUCTORS ---------------------------------------------------
    SimpleQubitState(){
        for(int i=0;i<18;i++){ // preenche os valores com zero
            data[i]=0;
        }
    }
    SimpleQubitState(int data[18]){
        for(int i=0;i<18;i++){ // transfer received data
            this->data[i]=data[i];
        }
    }
    
    // FUNCTIONS ------------------------------------------------------
    /** Função principal para o calculo das probabilidades */
    void calcProbState(){
        calcAmpModSquared(); // calcula |α|^2 e |β|^2
        // teste de normalização
        sumFractions(); // calcula zf/wf
        if(zf_wf[0]!=1||zf_wf[1]!=1){ // se zf/wf for diferente de 1
            this->normalized=false;
            calcAmpModSquaredNormalized(zf_wf); // calcula |α'|^2 e |β'|^2 (passa zf/wf como parâmetro)
        }
        calculated = true; // marca como "já calculado"
    }
    /** Função para imprimir os resultados do calculo */
    void printState(){
        if(!calculated) {
            cout << "Estado ainda não calculado!!!" << endl;
            return;
        }
        cout << "--------------------------------------------" << endl;
        cout << getStateAsString()<<endl;
        cout << "|α|² = ("<<alphaSqrd[0]<<"/"<<alphaSqrd[1]<<")"<<endl;
        cout << "|β|² = ("<<betaSqrd[0]<<"/"<<betaSqrd[1]<<")"<<endl;
        if(!normalized){
            cout << "Não nomalizado!" << endl;
            cout << "|α'|² = ("<<alphaSqrd[0]<<"/"<<alphaSqrd[1]<<")"<<endl;
            cout << "|β'|² = ("<<betaSqrd[0]<<"/"<<betaSqrd[1]<<")"<<endl;
        }else cout << "Nomalizado!" << endl;
        cout << "Probabilidade de medir |0⟩ = " << ((float)alphaSqrd[0]/alphaSqrd[1])*100 << "%" << endl;
        cout << "Probabilidade de medir |1⟩ = " << ((float)betaSqrd[0]/betaSqrd[1])*100 << "%" << endl;
        cout << "--------------------------------------------" << endl;
    }

};