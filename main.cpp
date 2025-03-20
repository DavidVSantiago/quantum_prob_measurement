#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "main_matrix.hpp"

using namespace std;

// ****************************************************************************************
// LOAD DATA FUNCTIONS
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
        stringstream ss(textState); // stream para ler os valores e separar as vírgulas
        string number; // armazena cada numero separado por vírgula
        vector<int> data; // vetor temporário para armazenar os valores convertidos do estado
        while (getline(ss, number, ',')) { // Lê cada número separado por vírgula
            data.push_back(stoi(number)); // Converte para inteiro e adiciona ao vetor temporário
        }
        if(checkDataError(data)) return nullptr; // dados corrompidos! retorna null
        states->push_back(SimpleQubitState(data.data())); // inicializa cada um dos estados quânticos com os dados da matriz
    }
    return states; // retorna o array de estados quâticos
}
// ****************************************************************************************
// END LOAD DATA FUNCTIONS
// ****************************************************************************************

int main() {
    vector<SimpleQubitState> *states = loadQuantumStates("data_matriz.txt");
    if(states==nullptr) {cout<<"dados corrompidos!"<<endl; return 1;}
    for(int i=0;i<states->size();i++){
        SimpleQubitState *state = &(*states)[i];
        state->calcProbState();
        state->printState();
    }
    return 0;
}