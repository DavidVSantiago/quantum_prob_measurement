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
    int alphaData[9]; // symbolic alpha data state
    int betaData[9]; // symbolic beta data state
    int alphaSqrd[2]; // temporary stores |α|^2
    int betaSqrd[2]; // temporary stores |β|^2
    int alphaSqrdNorm[2]; // temporary stores |α'|^2
    int betaSqrdNorm[2]; // temporary stores |β'|^2
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
    int MMC(int &a, int &b) {
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
        if(alphaData[1]==0) alphaSqrd[0] = alphaData[0]*alphaData[0]; // x is not root, apenas calcula x² e o atribui a z de z/w
        else alphaSqrd[0] = (alphaData[2]*alphaData[2]*alphaData[0]); // se x for raiz, eleva o fator ao quadrado e multiplica pelo valor. Ex.: (a√b)² = a².b - Após isso, atribui a z de z/w
        /** calcula y² (alpha) */
        if(alphaData[4]==0) alphaSqrd[0] += alphaData[3]*alphaData[3]; // y is not root, apenas calcula y² e o adiciona a z de z/w (lembrar que z=x²+y²)
        else alphaSqrd[0] += (alphaData[5]*alphaData[5]*alphaData[3]); // se y for raiz, eleva o fator ao quadrado e multiplica pelo valor. Ex.: (a√b)² = a².b - Após isso, adiciona a z de z/w (lembrar que z=x²+y²)
        /** calcula d² (alpha) */
        if(alphaData[7]==0) alphaSqrd[1] = alphaData[6]*alphaData[6]; // d is not root, apenas calcula d² e o atribui a w de z/w (lembrar que w=d²)
        else alphaSqrd[1] = (alphaData[8]*alphaData[8]*alphaData[6]); // se d for raiz, eleva o fator ao quadrado e multiplica pelo valor. Ex.: (a√b)² = a².b - Após isso, atribui a w de z/w (lembrar que w=d²)

        /** calcula x² (beta) */
        if(betaData[1]==0) betaSqrd[0] = betaData[0]*betaData[0]; // x is not root, apenas calcula x² e o atribui a z de z/w
        else betaSqrd[0] = (betaData[2]*betaData[2]*betaData[0]); // se x for raiz, eleva o fator ao quadrado e multiplica pelo valor. Ex.: (a√b)² = a².b - Após isso, atribui a z de z/w  
        /** calcula y² (beta) */
        if(betaData[4]==0) betaSqrd[0] += betaData[3]*betaData[3]; // y is not root, apenas calcula y² e o adiciona a z de z/w (lembrar que z=x²+y²)
        else betaSqrd[0] += (betaData[5]*betaData[5]*betaData[3]); // se y for raiz, eleva o fator ao quadrado e multiplica pelo valor. Ex.: (a√b)² = a².b - Após isso, adiciona a z de z/w (lembrar que z=x²+y²)
        /** calcula d² (beta) */
        if(betaData[7]==0) betaSqrd[1] = betaData[6]*betaData[6]; // d is not root, apenas calcula d² e o atribui a w de z/w (lembrar que w=d²)
        else betaSqrd[1] = (betaData[8]*betaData[8]*betaData[6]); // se d for raiz, eleva o fator ao quadrado e multiplica pelo valor. Ex.: (a√b)² = a².b - Após isso, atribui a w de z/w (lembrar que w=d²)

        simplifyFraction(alphaSqrd); // simplifica (se possível) a fração z/w (alpha)
        simplifyFraction(betaSqrd); // simplifica (se possível) a fração z/w (beta)
    }
    /** Calculate |α'|^2 and |β'|^2 */
    void calcAmpModSquaredNormalized(int *zf_wf){
        /** calcula x² (alpha) */
        if(alphaData[1]==0) alphaSqrdNorm[0] = zf_wf[1]*(alphaData[0]*alphaData[0]); // x is not root, apenas calcula wf*x² e o atribui a z' de z'/w'
        else alphaSqrdNorm[0] = zf_wf[1]*(alphaData[2]*alphaData[2]*alphaData[0]); // se x for raiz, eleva o fator ao quadrado e multiplica pelo valor. Ex.: (a√b)² = a².b - Após isso, multiplica por wf e atribui a z' de z'/w'
        /** calcula y² (alpha) */
        if(alphaData[4]==0) alphaSqrdNorm[0] += zf_wf[1]*(alphaData[3]*alphaData[3]); // y is not root, apenas calcula wf*y² e o adiciona a z' de z'/w'
        else alphaSqrdNorm[0] += zf_wf[1]*(alphaData[5]*alphaData[5]*alphaData[3]); // se y for raiz, eleva o fator ao quadrado e multiplica pelo valor. Ex.: (a√b)² = a².b - Após isso,  multiplica por wf e adiciona a z' de '/w'
        /** calcula d² (alpha) */
        if(alphaData[7]==0) alphaSqrdNorm[1] = zf_wf[0]*(alphaData[6]*alphaData[6]); // d is not root, apenas calcula zf*d² e o atribui a w' de z'/w'
        else alphaSqrdNorm[1] = zf_wf[0]*(alphaData[8]*alphaData[8]*alphaData[6]); // se d for raiz, eleva o fator ao quadrado e multiplica pelo valor. Ex.: (a√b)² = a².b - Após isso,  multiplica por zf e atribui a w' de z'/w'

        /** calcula x² (beta) */
        if(betaData[1]==0) betaSqrdNorm[0] = zf_wf[1]*(betaData[0]*betaData[0]); // x is not root, apenas calcula wf*x² e o atribui a z' de z'/w'
        else betaSqrdNorm[0] = zf_wf[1]*(betaData[2]*betaData[2]*betaData[0]); // se x for raiz, eleva o fator ao quadrado e multiplica pelo valor. Ex.: (a√b)² = a².b - Após isso, multiplica por wf e atribui a z' de z'/w'  
        /** calcula y² (beta) */
        if(betaData[4]==0) betaSqrdNorm[0] += zf_wf[1]*(betaData[3]*betaData[3]); // y is not root, apenas calcula wf*y² e o adiciona a z' de z'/w'
        else betaSqrdNorm[0] += zf_wf[1]*(betaData[5]*betaData[5]*betaData[3]); // se y for raiz, eleva o fator ao quadrado e multiplica pelo valor. Ex.: (a√b)² = a².b - Após isso,  multiplica por wf e adiciona a z' de z'/w'
        /** calcula d² (beta) */
        if(betaData[7]==0) betaSqrdNorm[1] = zf_wf[0]*(betaData[6]*betaData[6]); // d is not root, apenas calcula zf*d² e o atribui a w' de z'/w'
        else betaSqrdNorm[1] = zf_wf[0]*(betaData[8]*betaData[8]*betaData[6]); // se d for raiz, eleva o fator ao quadrado e multiplica pelo valor. Ex.: (a√b)² = a².b - Após isso,  multiplica por zf e atribui a w' de z'/w'

        simplifyFraction(alphaSqrdNorm); // simplifica (se possível) a fração z/w (alpha)
        simplifyFraction(betaSqrdNorm); // simplifica (se possível) a fração z/w (beta)
    }
    
    // PRINT FUNCTIONS --------------------------------------------------- 
    string getValAsString(int *data, int index){
        if(data[index]==0) return "0";
        string text = to_string(data[index]);
        if(data[index+1]==2) text="√"+text;
        if(data[index+2]!=1) text=to_string(data[index+2])+text;
        return text;
    }
    string getAmpAsString(int *data){
        string real = getValAsString(data,0);
        string imag = getValAsString(data,3);
        string den = getValAsString(data,6);
        
        if(imag=="1") imag=""; // remoção do valor 1
        else if(imag=="-1") imag="-"; // remoção do valor -1
        if(imag[0]!='-') imag="+"+imag; // adiciona o sinal de '+' apenas se não for negativo

        if(real=="0") return "("+imag+"i/"+den+")"; // se não existe parte real, omite-a
        if(imag=="0"||imag=="+0") return "("+real+"/"+den+")"; // se não existe parte imaginaria, omite-a
        return "(("+real+imag+"i)/"+den+")"; // caso existam ambas (real e imaginária)
    }
    string getStateAsString(){
        return "|ψ⟩ = "+getAmpAsString(alphaData)+"|0⟩ + "+getAmpAsString(betaData)+"|1⟩";
    }
    
public:
    // CONSTRUCTORS ---------------------------------------------------
    SimpleQubitState(){
        for(int i=0;i<18;i++){ // preenche os valores com zero
            alphaData[i]=0;
            betaData[i]=0;
        }
    }
    SimpleQubitState(int data[18]){
        for(int i=0;i<18;i++){ // transfer received data
            if(i<9)this->alphaData[i]=data[i];
            else this->betaData[i-9]=data[i];
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
            cout << "|α'|² = ("<<alphaSqrdNorm[0]<<"/"<<alphaSqrdNorm[1]<<")"<<endl;
            cout << "|β'|² = ("<<betaSqrdNorm[0]<<"/"<<betaSqrdNorm[1]<<")"<<endl;
            cout << "Probabilidade de medir |0⟩ = " << ((float)alphaSqrdNorm[0]/alphaSqrdNorm[1])*100 << "%" << endl;
            cout << "Probabilidade de medir |1⟩ = " << ((float)betaSqrdNorm[0]/betaSqrdNorm[1])*100 << "%" << endl;
        }else{
            cout << "Nomalizado!" << endl;
            cout << "Probabilidade de medir |0⟩ = " << ((float)alphaSqrd[0]/alphaSqrd[1])*100 << "%" << endl;
            cout << "Probabilidade de medir |1⟩ = " << ((float)betaSqrd[0]/betaSqrd[1])*100 << "%" << endl;
        }
        cout << "--------------------------------------------" << endl;
    }

};