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
    int mdc = MDC(frac[0],frac[1]);
    // simplifica a fração, caso precise
    if(mdc!=1){
        frac[0] /= mdc; // simplify numerator
        frac[1] /= mdc; // simplify denominator
    }
}

/** verifica se dois valores são iguais */
bool equals(int *A, int *B){
    return A[0]==B[0]&&A[1]==B[1]&&A[2]==B[2];
}

/** Calcula da |amplitude|²*/
int* calcAmpModSquared(int *amp){
    // aloca o array para armazenar a fração resultante de (x²+y²)/d² = z/w
    int *ampSqrd = new int[2]; // tamanho 2, pois a representação simbólica da fração resultante não armazena raizes!
    int xSqrd,ySqrd,dSqrd;

    /** calcula x² */
    if(amp[1]==0){ // se x não for raiz
        xSqrd = amp[0]*amp[0]; // apenas eleva o valor ao quadrado
    }else{ // se x for raiz
        xSqrd = (amp[2]*amp[2]*amp[0]); // eleva o fator ao quadrado e multiplica pelo valor. Ex.: (a√x)² = a².x
    }
    /** calcula y² */
    if(amp[4]==0){ // se y não for raiz
        ySqrd = amp[3]*amp[3]; // apenas eleva o valor ao quadrado
    }else{ // se y for raiz
        ySqrd = (amp[5]*amp[5]*amp[3]); // eleva o fator ao quadrado e multiplica pelo valor. Ex.: (a√y)² = a².y
    }
    /** calcula d² */
    if(amp[7]==0){ // se d não for raiz
        dSqrd = amp[6]*amp[6]; // apenas eleva o valor ao quadrado
    }else{ // se d for raiz
        dSqrd = (amp[8]*amp[8]*amp[6]); // eleva o fator ao quadrado e multiplica pelo valor. Ex.: (a√d)² = a².d
    }

    ampSqrd[0]= xSqrd + ySqrd; // define x²+y², de (x²+y²)/d² (z de z/w)
    ampSqrd[1]= dSqrd; // define d², de (x²+y²)/d² (w de z/w)

    simplifyFraction(ampSqrd); // simplifica (se possível) a fração z/w

    return ampSqrd; // retorna z/w
}

/** Calcula da |amplitude'|² Normalizada*/
int* calcAmpModSquaredNorm(int *amp, int zf, int wf){
    // aloca o array para armazenar a fração resultante de (wf.x²+wf.y²)/zf.d²
    int *ampSqrd = new int[2]; // tamanho 2, pois a representação simbólica da fração resultante não armazena raizes!
    int xSqrd,ySqrd,dSqrd;

    /** calcula wf.x² */
    if(amp[1]==0){ // se x não for raiz
        xSqrd = wf*(amp[0]*amp[0]); // calcula wf.x²
    }else{ // se x for raiz
        xSqrd = wf*(amp[2]*amp[2]*amp[0]); // eleva o fator ao quadrado e multiplica pelo valor. Ex.: wf(a√x)² = wf.a².x
    }
    /** calcula wf.y² */
    if(amp[4]==0){ // se y não for raiz
        ySqrd = wf*(amp[3]*amp[3]); // apenas eleva o valor ao quadrado
    }else{ // se y for raiz
        ySqrd = wf*(amp[5]*amp[5]*amp[3]); // eleva o fator ao quadrado e multiplica pelo valor. Ex.: wf(a√y)² = wf.a².y
    }
    /** calcula zf.d² */
    if(amp[7]==0){ // se d não for raiz
        dSqrd = zf*(amp[6]*amp[6]); // apenas eleva o valor ao quadrado
    }else{ // se d for raiz
        dSqrd = zf*(amp[8]*amp[8]*amp[6]); // eleva o fator ao quadrado e multiplica pelo valor. Ex.: zf(a√d)² = zf.a².d
    }

    ampSqrd[0]= xSqrd + ySqrd; // define wf.x²+wf.y², de (wf.x²+wf.y²)/zf.d²
    ampSqrd[1]= dSqrd; // define zf.d², de (wf.x²+wf.y²)/zf.d²

    simplifyFraction(ampSqrd); // simplifica (se possível) a fração final

    return ampSqrd; // retorna a fração
}

/** Calcula a soma de duas fracoes passadas como parâmetro */
int *sumFractions(int *f1, int* f2){ 
    int *result = new int[2]; // será que é realmente necessário alocar um array com 2 posições, uma vez que zf/wf nunca vai possuir raiz?
    int mmc = MMC(f1[1],f2[1]); // mmc dos denominadores
    result[1] = mmc; // atribui o denominador final
    result[0] = ((mmc/f1[1])*f1[0])+((mmc/f2[1])*f2[0]); // atribui o numerador final
    simplifyFraction(result);
    return result;
}

void calcProbState(SimpleQubitState* state){
    cout << "--------------------------------------------" << endl;
    cout << state->getStateAsString()<<endl;
    int *alphaSqrd = calcAmpModSquared(state->getAlpha()); // calcula |α|², z/w (alpha)
    int *betaSqrd = calcAmpModSquared(state->getBeta()); // calcula |β|², z/w (beta)
    // teste de normalização
    int *sumSqrd = sumFractions(alphaSqrd, betaSqrd); // calcula zf/wf
    cout << "|α|² = ("<<alphaSqrd[0]<<"/"<<alphaSqrd[1]<<")"<<endl;
    cout << "|β|² = ("<<betaSqrd[0]<<"/"<<betaSqrd[1]<<")"<<endl;
    if(sumSqrd[0]!=1||sumSqrd[1]!=1){ // se zf/wf for diferente de 1 (verifica normalização)
        cout << "Não nomalizado!" << endl;
        int zf = sumSqrd[0];
        int wf = sumSqrd[1];
        alphaSqrd = calcAmpModSquaredNorm(state->getAlpha(),zf,wf); // calcula |α'|²
        betaSqrd = calcAmpModSquaredNorm(state->getBeta(),zf,wf); // calcula |β'|²
        cout << "|α'|² = ("<<alphaSqrd[0]<<"/"<<alphaSqrd[1]<<")"<<endl;
        cout << "|β'|² = ("<<betaSqrd[0]<<"/"<<betaSqrd[1]<<")"<<endl;
    }else cout << "Nomalizado!" << endl;
    cout << "Probabilidade de medir |0⟩ = " << ((float)alphaSqrd[0]/alphaSqrd[1])*100 << "%" << endl;
    cout << "Probabilidade de medir |1⟩ = " << ((float)betaSqrd[0]/betaSqrd[1])*100 << "%" << endl;
    cout << "--------------------------------------------" << endl;
}

void calcProbStateSimple(SimpleQubitState* state){
    cout << "--------------------------------------------" << endl;
    cout << state->getStateAsString();
    int *alphaSqrd = calcAmpModSquared(state->getAlpha()); // calcula |α|², z/w (alpha)
    int *betaSqrd = calcAmpModSquared(state->getBeta()); // calcula |β|², z/w (beta)
    // teste de normalização
    int *sumSqrd = sumFractions(alphaSqrd, betaSqrd); // calcula zf/wf
    if(sumSqrd[0]!=1||sumSqrd[1]!=1){ // se zf/wf for diferente de 1 (verifica normalização)
        cout << " - Não nomalizado! " << endl;
        int zf = sumSqrd[0];
        int wf = sumSqrd[1];
        alphaSqrd = calcAmpModSquaredNorm(state->getAlpha(),zf,wf); // calcula |α'|²
        betaSqrd = calcAmpModSquaredNorm(state->getBeta(),zf,wf); // calcula |β'|²
    }else cout << " - Nomalizado! " << endl;
    cout << "prob.|0⟩ = " << ((float)alphaSqrd[0]/alphaSqrd[1])*100 << "%";
    cout << " - prob.|1⟩ = " << ((float)betaSqrd[0]/betaSqrd[1])*100 << "%" << endl;
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
        calcProbState(state);
    }
    return 0;
}