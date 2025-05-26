#include <fstream>
#include <sstream>
#include <vector>
#include "main_matrix.hpp"

using namespace std;

// ****************************************************************************************
// LOAD DATA FUNCTIONS
// ****************************************************************************************

// função para ler arquivo de texto. retorna um vetor de strings, onde cada string é uma linha do arquivo
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

// verifica a integridade dos dados de cada estado carregado do arquivo. recebe um vetor de inteiros que representa o estado quântico
bool checkDataError(vector<int> s){
    bool error = false; // variável para indicar se há erro nos dados do estado quântico
    if(s.size()!=10) error=true; // verifica se o tamanho do vetor é 10, que é o tamanho esperado para um estado quântico
    
    // TODO - critérios de validação dos dados do estado quântico
    
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
        if(checkDataError(data)) return nullptr; // verifica se os dados estão corrompidos
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