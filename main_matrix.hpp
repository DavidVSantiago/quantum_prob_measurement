#include <iostream>
#include <string>

using namespace std;

/** Tipo Abstrato de Dados que representa a matriz simples de qutib único */
class SimpleQubitState{
private:
    // ATRIBUTOS SIMBÓLICOS DO ESTADO QUÂNTICO ---------------------------------------------------------
    int x1; // parte real da amplitude α, onde x1 ∈ Z
    int y1; // parte imaginária da amplitude α, onde y1 ∈ Z
    int d1[3]; // representação simbólica do denominador da amplitude α (d1[0] = radicando, d1[1] = indice, d1[2] = fator multiplicativo), onde d1 ∈ R

    int x2; // parte real da amplitude β, onde x2 ∈ Z
    int y2; // parte imaginária da amplitude β , onde y2 ∈ Z
    int d2[3]; // representação simbólica do denominador da amplitude β (d2[0] = radicando, d2[1] = indice, d2[2] = fator multiplicativo), onde d2 ∈ R

    // ATRIBUTOS SIMBÓLICOS PARA SIMPLIFICAÇÃO O CÁLCULO ---------------------------------------------------------
    int z1; // representação simbólica do numerador da amplitude α (z1 = x1² + y1²), onde z1 ∈ Z
    int w1; // representação simbólica do denominador da amplitude α (w1 = d1²), onde w1 ∈ Z
    int alphaSqrd[2]; // representação simbólica para |α|²=z1/w1 (alphaSqrd[0] = z1, alphaSqrd[1] = w1)

    int z2; // representação simbólica do numerador da amplitude β (z2 = x2² + y2²), onde z2 ∈ Z
    int w2; // representação simbólica do denominador da amplitude β (w2 = d2²), onde zw ∈ Z
    int betaSqrd[2]; // representação simbólica para |β|²=z2/w2 (betaSqrd[0] = z2, betaSqrd[1] = w2)
    
    // ATRIBUTOS SIMBÓLICOS PARA SIMPLIFICAÇÃO DA NORMALIZAÇÃO ---------------------------------------------------------
    int wf; // componente denominador do fator de normalização (wf = mmc(w1,w2)), onde wf ∈ Z
    int zf; // componente numerador do fator de normalização (zf = wf*(z1/w1 + z2/w2)), onde zfs ∈ Z

    int zl1; // representação simbólica para wf*(x1² + y1²), onde zl1 ∈ Z
    int wl1; // representação simbólica para zf*d1², onde wl1 ∈ Z

    int zl2; // representação simbólica para wf*(x2² + y2²), onde wl2 ∈ Z
    int wl2; // representação simbólica para zf*d2², onde wl2 ∈ Z

    // ATRIBUTOS UTILITÁRIOS ---------------------------------------------------------
    bool normalized = false; // para indicar se o estado, originalmente, estava normalizado
    bool calculated = false; // para indicar se os cálculos do estado já foram realidaos
    int probAlpha[2]; // probabilidade de medir |0⟩, (z1/w1) ou (zl1/wl1)
    int probBeta[2]; // probabilidade de medir |1⟩, (z2/w2) ou (zl2/wl2)
    
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
    
    // PRINT FUNCTIONS --------------------------------------------------- 
    // obtem o valor do denominador d1 ou d2 como uma string
    string getDenVal(int *den){
        if(den[0]==0) return "0"; // se o valor for 0
        string text = to_string(den[0]); // obtem o valor do radicando
        if(den[1]==2) text="√"+text; // se for raiz quadrada, adiciona o símbolo de raiz
        if(den[2]!=1) text=to_string(den[2])+text;
        return text;
    }
    string getAmpAsString(int &x1, int &y1, int *d1){
        string real = to_string(x1);
        string imag = to_string(y1);
        string den = getDenVal(d1);
        
        if(imag=="1") imag=""; // remoção do valor 1
        else if(imag=="-1") imag="-"; // remoção do valor -1
        if(imag[0]!='-') imag="+"+imag; // adiciona o sinal de '+' apenas se não for negativo

        if(real=="0") return "("+imag+"i/"+den+")"; // se não existe parte real, omite-a
        if(imag=="0"||imag=="+0") return "("+real+"/"+den+")"; // se não existe parte imaginaria, omite-a
        return "(("+real+imag+"i)/"+den+")"; // caso existam ambas (real e imaginária)
    }
    string getStateAsString(){
        return "|ψ⟩ = "+getAmpAsString(x1,y1,d1)+"|0⟩ + "+getAmpAsString(x2,y2,d2)+"|1⟩";
    }
    
public:
    // CONSTRUCTORS ------------------------------------------------------
    SimpleQubitState(){}
    SimpleQubitState(int data[10]){
        // transfere os dados do array para os atributos
        // equivalente à etapa 1 da figura 1 do artigo
        for(int i=0;i<10;i++){
            x1= data[0];
            y1= data[1];
            d1[0]= data[2]; d1[1]= data[3]; d1[2]= data[4];
            x2= data[5];
            y2= data[6];
            d2[0]= data[7]; d2[1]= data[8]; d2[2]= data[9];
        }
    }
    
    // FUNCTIONS ---------------------------------------------------------
    /** Função principal para o calculo das probabilidades (figura 1 do artigo)*/
    void calcProbState(){
        // etapa 2 da figura 1 do artigo
        z1 = x1*x1 + y1*y1; // calcula z1 = x1² + y1².
        
        // etapa 3 da figura 1 do artigo
        if(d1[1]==0) w1 = d1[0]*d1[0]; // se d1 não for raiz, calcula w1 = d1²
        else w1 = (d1[2]*d1[2]*d1[0]); // se d1 for raiz, calcula w1 = (a√b)² = a².b
        
        // etapa 4 da figura 1 do artigo
        z2 = x2*x2 + y2*y2; // calcula z2 = x2² + y2².
        
        // etapa 5 da figura 1 do artigo
        if(d2[1]==0) w2 = d2[0]*d2[0]; // se d2 não for raiz, calcula w2 = d2²
        else w2 = (d2[2]*d2[2]*d2[0]); // se d2 for raiz, calcula w2 = (a√b)² = a².b
       
        // etapa 6 da figura 1 do artigo
        wf = MMC(w1,w2); // calcula o wf = mmc entre w1 e w2

        // etapa 7 da figura 1 do artigo
        zf = (wf/w1)*z1 + (wf/w2)*z2; // calcula o zf. É importante alientar que tanto (wf/w1) como (wf/w2) ∈ Z. Por isso, não há necessidade de simplificação da fração zf/wf nem riscos de geração de números fracionários.

        // etapa 8 da figura 1 do artigo
        if(zf/wf==1 && zf%wf==0){ // verificação de normalização. Se zf/wf for igual a 1
            normalized = true; // marca como "normalizado"
            probAlpha[0] = z1; // atribui z1 ao numerador da probabilidade de medir |0⟩
            probAlpha[1] = w1; // atribui w1 ao denominador da probabilidade de medir |0⟩
            probBeta[0] = z2; // atribui z2 ao numerador da probabilidade de medir |1⟩
            probBeta[1] = w2; // atribui w2 ao denominador da probabilidade de medir |1⟩
            calculated = true; // marca como "já calculado"
            return; // finaliza o cálculo antecipadamente, pois não há necessidade de normalização
        }
        // etapa 9 da figura 1 do artigo
        zl1 = wf*z1; // calcula zl1
        
        // etapa 10 da figura 1 do artigo
        wl1 = zf*w1; // calcula wl1

        // etapa 11 da figura 1 do artigo
        zl2 = wf*z2; // calcula zl2

        // etapa 12 da figura 1 do artigo
        wl2 = zf*w2; // calcula wl2

        // etapa 13 da figura 1 do artigo  
        probAlpha[0] = zl1; // atribui zl1 ao numerador da probabilidade de medir |0⟩
        probAlpha[1] = wl1; // atribui wl1 ao denominador da probabilidade de medir |0⟩

        // etapa 14 da figura 1 do artigo
        probBeta[0] = zl2; // atribui zl2 ao numerador da probabilidade de medir |1⟩
        probBeta[1] = wl2; // atribui wl2 ao denominador da probabilidade de medir |1⟩
        
        // simplifica as frações do resultado, se possível
        simplifyFraction(probAlpha);
        simplifyFraction(probBeta);

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
        cout << "|α|² + |β|² = "<<(float)zf/wf<<endl;
        if(!normalized) cout << "Não nomalizado!" << endl;
        else cout << "Nomalizado!" << endl;
        cout << "Probabilidade de medir |0⟩ = " << ((float)(probAlpha[0])/(probAlpha[1]))*100 << "%" << endl;
        cout << "Probabilidade de medir |1⟩ = " << ((float)(probBeta[0])/(probBeta[1]))*100 << "%" << endl;
        cout << "--------------------------------------------" << endl;
    }

};