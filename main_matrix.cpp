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
public:
    int **data = new int*[2]; // data matrix, 2 lines
    // int *alphaReal = data[0]; // points to real part of |0⟩ amplitude
    // int *alphaImag = data[1]; // points to imaginary part of |0⟩ amplitude
    // int *betaReal = data[2]; // points to real part of |1⟩ amplitude
    // int *betaImag = data[3]; // points to imaginary part of |1⟩ amplitude

    SimpleQubitState(){
        for(int i=0;i<2;i++){ // percorre as linhas
            data[i] = new int[9]; // 9 columns
            for(int j=0;j<9;j++){ // percorre as colunas
                data[i][j]=0; // fill zeros
            }
        }
    }
    SimpleQubitState(int data[2][9]){
        // transfer received data
        for(int i=0;i<2;i++){ // percorre as linhas
            this->data[i] = new int[9]; // 9 columns
            for(int j=0;j<9;j++){ // percorre as colunas
                this->data[i][j]=data[i][j]; // transfer data
            }
        }
    }
    
    // member functions *********************************
    int* getAlpha(){ return data[0];}
    int* getBeta(){ return data[1];}
    
    string getFractionAsString(int line){
        if(line<0||line>3) return "(invalid line)";
        string num = to_string(data[line][0]); if(data[line][1]==2) num="√"+num;
        string den = to_string(data[line][3]); if(data[line][4]==2) den="√"+den;
        return (num=="0"||den=="0")?"":"("+num+"/"+den+")";
    }

    string getValAsString(int l, int c){ string val = to_string(data[l][c]); if(data[l][c+1]==2) val="√"+val; return val; }
    
    string getRealAsString(int l){ return getValAsString(l,0); }
    string getImagAsString(int l){ return getValAsString(l,3); }
    string getDenAsString(int l){ return getValAsString(l,6); }

    string getAlphaRealAsString(){ return "("+getRealAsString(0)+"/"+getDenAsString(0)+")"; }
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
};

// ****************************************************************************************
// UTILS FUNCTIONS
// ****************************************************************************************

// função para ler arquivo de texto
vector<string> readTextFile(const string& fileName) {
    vector<string> lines;
    ifstream fileStream(fileName);  // Abre o arquivo para leitura
    if (!fileStream) {  // Verifica se o arquivo foi aberto corretamente
        cerr << "Erro in open file: " << fileName << std::endl;
    }else{
        string line;
        while (getline(fileStream, line)) {  // Lê cada linha do arquivo
            lines.push_back(line);
        }
        fileStream.close();  // Fecha o arquivo
    }
    return lines;
}

// verifica a integridade dos dados de cada estado carregado do arquivo
bool checkDataError(vector<int> s){
    bool error = false;
    if(s.size()!=18) error=true; // um estado quântico deve conter exatamente 18 valores
    
    // ou não é raiz (valor 0) ou é raiz (valor 2)
    if((s[1]!=0&&s[1]!=2)||(s[4]!=0&&s[4]!=2)||(s[7]!=0&&s[7]!=2)||
       (s[10]!=0&&s[10]!=2)||(s[13]!=0&&s[13]!=2)||(s[16]!=0&&s[16]!=2)) error=true;

    // o fator multiplicativo não pode ser negativo
    if(s[2]<0||s[5]<0||s[8]<0||s[11]<0||s[14]<0||s[17]<0) error=true;

    // o fator multiplicativo não pode ser ≠ 1 se o valor não for uma raiz
    if((s[2]!=1&&s[1]==0&&s[0]!=0)||(s[5]!=1&&s[4]==0&&s[3]!=0)||(s[8]!=1&&s[7]==0&&s[6]!=0)||
       (s[11]!=1&&s[10]==0&&s[9]!=0)||(s[14]!=1&&s[13]==0&&s[12]!=0)||(s[17]!=1&&s[16]==0&&s[15]!=0)) error=true;

    // o valor do denominador não pode ser negativo
    if(s[6]<0||s[15]<0) error=true; // o valor do denominador não pode ser negativo
    
    return error;
}

// função para carregar um array de estados quânticos
vector<SimpleQubitState>* loadQuantumStates(const string& fileName){
    vector<string> textStates = readTextFile(fileName); // obtem um array das linhas do arquivo
    vector<SimpleQubitState> *states = new vector<SimpleQubitState>(); // cada linha será convertida em um estado quântico
    if (!textStates.empty() && textStates.back().empty()) textStates.pop_back(); // Remove a última linha se estiver vazia
    for(int i=0;i<textStates.size();i++){ // percorre cada uma dos estados quânticos retornados
        string textState = textStates[i]; // obtém cada estado quântico em forma textual
        vector<int> tempData; // vetor temporário para armazenar os valores convertidos do estado
        stringstream ss(textState); // stream para ler os valores e separar as vírgulas
        string number; // armazena cada numero separado por vírgula
        while (getline(ss, number, ',')) { // Lê cada número separado por vírgula
            tempData.push_back(stoi(number)); // Converte para inteiro e adiciona ao vetor temporário
        }
        if(checkDataError(tempData)) return nullptr; // dados corrompidos! retorna null
        int data[2][9]; // matriz para inicializar cada um dos estados
        for(int j=0;j<tempData.size();j++){ // percorre cada valor do vetor temporário
            data[j/9][j%9]=tempData[j]; // transfere os dados para a matriz de inicialização
        }
        states->push_back(SimpleQubitState(data)); // inicializa cada um dos estados quânticos com os dados da matriz
    }
    return states; // retorna o array de estados quâticos
}

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

// ****************************************************************************************
// END UTILS FUNCTIONS
// ****************************************************************************************

// ****************************************************************************************
// FUNCTIONS
// ****************************************************************************************

/** Simplifica, se possível, uma fração */
void simplifyFraction(int *frac) {
    int mdc = MDC(frac[0],frac[3]);
    // simplifica a fração, caso precise
    if(mdc!=1){
        frac[0] /= mdc;
        frac[3] /= mdc;
    }
}

/** verifica se dois valores são iguais */
bool equals(int *A, int *B){
    return A[0]==B[0]&&A[1]==B[1]&&A[2]==B[2];
}

/** Calcula |α|^2 da amplitude*/
int* calcAmpModSquared(int *amp){
    
    bool realZero = (amp[0]==0)?true:false; // verifica se x(parte real) é igual a zero
    bool imagZero = (amp[3]==0)?true:false; // verifica se y(parte imaginária) é igual a zero
    bool denZero = (amp[6]==0)?true:false; // verifica se d(denominador) é igual a zero

    int *ampSqrd = new int[6]; // aloca o array para armazenar a fração (x²+y²)/d² = z/w
    int xSqrd,ySqrd,dSqrd;

    /** eleva x ao quadrado */
    if(amp[1]==0){ // se x não for raiz
        xSqrd = amp[0]*amp[0]; // calcula x², de (x²+y²)/d²
    }else{ // se x for raiz
        xSqrd = (amp[2]*amp[2]*amp[0]); // calcula x², de (x²+y²)/d²
    }
    /** eleva y ao quadrado */
    if(amp[4]==0){ // se y não for raiz
        ySqrd = amp[3]*amp[3]; // calcula y², de (x²+y²)/d²
    }else{ // se y for raiz
        ySqrd = (amp[5]*amp[5]*amp[3]); // calcula y², de (x²+y²)/d²
    }
    /** eleva d ao quadrado */
    if(amp[7]==0){ // se d não for raiz
        dSqrd = amp[6]*amp[6]; // calcula d², de (x²+y²)/d²
    }else{ // se d for raiz
        dSqrd = (amp[8]*amp[8]*amp[6]); // calcula d², de (x²+y²)/d²
    }

    ampSqrd[0]= xSqrd + ySqrd; // define x²+y² (x²+y²)/d² (z de z/w)
    ampSqrd[1] = 0; // o resultado sempre perde a raiz
    ampSqrd[2] = 1; // como não possui raiz, o fator é sempre 1
    ampSqrd[3]= dSqrd; // define d² (x²+y²)/d² (w de z/w)
    ampSqrd[4] = 0; // o resultado sempre perde a raiz
    ampSqrd[5] = 1; // como não possui raiz, o fator é sempre 1

    simplifyFraction(ampSqrd); // simplifica (se possível) a fração z/w

    return ampSqrd; // retorna z/w
}

void calcProbStateExtend(SimpleQubitState* state){
    cout << "--------------------------------------------" << endl;
    cout << state->getStateAsString() << endl;
    int *alphaSqrd = calcAmpModSquared(state->getAlpha());
    int *betaSqrd = calcAmpModSquared(state->getBeta());
    cout << "O módulo da amplitude "<<state->getAlphaAsString()<<" ao quadrado = ("<<alphaSqrd[0]<<"/"<<alphaSqrd[3]<<")"<<endl;
    cout << "O módulo da amplitude "<<state->getBetaAsString()<<" ao quadrado = ("<<betaSqrd[0]<<"/"<<betaSqrd[3]<<")"<<endl;
    cout << "Probabilidade de medir |0⟩ = " << ((float)alphaSqrd[0]/alphaSqrd[3])*100 << "%" << endl;
    cout << "Probabilidade de medir |1⟩ = " << ((float)betaSqrd[0]/betaSqrd[3])*100 << "%" << endl;
    cout << "--------------------------------------------" << endl;
}
void calcProbStateSimple(SimpleQubitState* state){
    cout << "--------------------------------------------" << endl;
    cout << state->getStateAsString() << endl;
    int *alphaSqrd = calcAmpModSquared(state->getAlpha());
    int *betaSqrd = calcAmpModSquared(state->getBeta());
    cout << "Probabilidade de medir |0⟩ = " << ((float)alphaSqrd[0]/alphaSqrd[3])*100 << "%" << endl;
    cout << "Probabilidade de medir |1⟩ = " << ((float)betaSqrd[0]/betaSqrd[3])*100 << "%" << endl;
    cout << "--------------------------------------------" << endl;
}
// ****************************************************************************************
// END FUNCTIONS
// ****************************************************************************************

int main() {
    vector<SimpleQubitState> *states = loadQuantumStates("data_matriz.txt");
    if(states==nullptr) {cout<<"dados corrompidos!"<<endl; return 1;}
    for(int i=0;i<states->size();i++){
        SimpleQubitState *state = &(*states)[i];
        calcProbStateSimple(state);
    }
    return 0;
}