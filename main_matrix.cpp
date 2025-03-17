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
    
    // member functions
    int** getMatrix(){return this->data;} // get raw matrix data
    // int* getAlphaReal(){return this->alphaReal;} // gets real part of |0⟩ amplitude
    // int* getAlphaImag(){return this->alphaImag;} // gets imaginary part of |0⟩ amplitude
    // int* getBetaReal(){return this->betaReal;} // gets real part of |1⟩ amplitude
    // int* getBetaImag(){return this->betaImag;} // gets imaginary part of |1⟩ amplitude
    int** getAlphaCopy(){
        int **data = new int*[2]; // alpha data, 2 lines
        for(int i=0;i<2;i++){ // percorre as linhas
            data[i] = new int[6]; // 6 columns
            for(int j=0;j<6;j++){ // percorre as colunas
                data[i][j]=this->data[i][j]; // transfer data
            }
        }
        return data;
    }
    int** getBetaCopy(){
        int **data = new int*[2]; // alpha data, 2 lines
        for(int i=0;i<2;i++){ // percorre as linhas
            data[i] = new int[6]; // 6 columns
            for(int j=0;j<6;j++){ // percorre as colunas
                data[i][j]=this->data[i+2][j]; // transfer data
            }
        }
        return data;
    }
    
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

/** Realiza a soma das frações. Pelo fato de que (|α|^2) = (x^2+y^2) a soma de frações nunca terão radicais! */
int* sum(int *A,int *B){
    // arrays para armazenar os valores de num e den das frações
    int *A_num = new int[3];
    int *A_den = new int[3]; 
    int *B_num = new int[3];
    int *B_den = new int[3];
    int *C_num = new int[3];
    int *C_den = new int[3];
    
    for(int i=0;i<3;i++){A_num[i]=A[i];B_num[i]=B[i];}// extrai o num das frações
    for(int i=3;i<6;i++){A_den[i-3]=A[i];B_den[i-3]=B[i];}// extrai o den das frações

    if(equals(A_den,B_den)){ // denominadores iguais (não precisa fazer MDC)
        // C_den = A_den; // mantém o denominador
        for(int i=0;i<3;i++){C_den[i]=A_den[i];}

        // Numerator SUM (they will never have radical)
        C_num[1]=0; // pois não será raiz quadrada
        C_num[2]=1; // pois não será raiz quadrada
        C_num[0]=A_num[0]+B_num[0]; // soma os valores dos numeradores

    }else{ // denominadores diferentes (precisa fazer MDC)
        int mmc = MMC(A_den[0], B_den[0]); // find common denominator
        A_den[0]=mmc;
        A_den[1]=0; // pois não será raiz quadrada
        A_den[2]=1; // pois não será raiz quadrada
            
        // Numerator SUM //
        C_num[1]=0; // pois não será raiz quadrada
        C_num[2]=1; // pois não será raiz quadrada
        C_num[0]=((mmc/A_den[0])*A_num[0])+((mmc/B_den[0])*B_num[0]); // soma os valores dos numeradores (considerando o MMC)
    }

    // junta o numerador e denominador em uma única fração resultado
    int *C = new int[6]; // fração resultado
    for(int i=0;i<3;i++){C[i]=C_num[i];}
    for(int i=3;i<6;i++){C[i]=C_den[i-3];}

    simplifyFraction(C);
    return C;
}

/** Calcula |α|^2 da amplitude*/
int* calcAmpModSquared(int **amp){
    
    int real[6]; // array para armazenar a parte real da amplitude
    int imag[6]; // array para armazenar a parte imaginária da amplitude
    for(int i=0;i<6;i++){real[i]=amp[0][i];}// obtém a parte real da amplitude
    for(int i=0;i<6;i++){imag[i]=amp[1][i];}// obtém a parte imaginária da amplitude

    // verifica se x(parte real) ou y(parte imaginária) são iguais a zero
    bool realZero = (real[0]==0||real[3]==0)?true:false;
    bool imagZero = (imag[0]==0||imag[3]==0)?true:false;

    // processamento da parte real
    int realSquared[6]; // array para armazenar a parte real da amplitude ao quadrado (já processada)
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
    int imagSquared[6]; // array para armazenar a parte real da amplitude ao quadrado (já processada)
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

    int *result = new int[6]; // fração a ser retornada
    if(imagZero) std::copy(realSquared, realSquared + 6, result); // parte imaginária é zero, somente a parte real²(x²) será retornada
    else if(realZero) std::copy(imagSquared, imagSquared + 6, result); // parte real é zero, somente a parte imaginaria²(y²) será retornada
    else result = sum(realSquared,imagSquared); // o retorno será x²+y² 
    return result;
}

// ****************************************************************************************
// END FUNCTIONS
// ****************************************************************************************

int main() {
    vector<SimpleQubitState> *states = loadQuantumStates("data_matriz.txt");
    if(states==nullptr) {cout<<"dados corrompidos!"<<endl; return 1;}
    for(int i=0;i<states->size();i++){
        SimpleQubitState *state = &(*states)[i];
        cout << state->getStateAsString() << endl;
        int *alphaResult = calcAmpModSquared(state->data[0]); // TODO
        int *betaResult = calcAmpModSquared(state->getBetaCopy());
        cout << "O módulo da amplitude "<<state->getAlphaAsString()<<" ao quadrado = ("<<alphaResult[0]<<"/"<<alphaResult[3]<<")"<<endl;
        cout << "O módulo da amplitude "<<state->getBetaAsString()<<" ao quadrado = ("<<betaResult[0]<<"/"<<betaResult[3]<<")"<<endl;
    }
    
    return 0;
}