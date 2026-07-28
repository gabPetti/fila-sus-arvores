#include "bibliotecas.h"
#include "AVL.h"
#include "internacao.h"
#include "hash.h"
#include <chrono>
#include <random>

// Data atual usada na chave
string obterDataAtual(){
    time_t agora = time(nullptr);
    tm* data = localtime(&agora);
    stringstream ss;
    ss << data->tm_mday << "/"
       << data->tm_mon + 1 << "/"
       << data->tm_year + 1900;
    return ss.str();
}
string dataAtual = obterDataAtual();

// Calcula a espera em dias
int qtdDiasEsperados(const internacao& i){
    int diaRegistro, mesRegistro, anoRegistro;
    sscanf(i.data.c_str(), "%d/%d/%d", &diaRegistro, &mesRegistro, &anoRegistro); 
    int diaAtual, mesAtual, anoAtual;
    sscanf(dataAtual.c_str(), "%d/%d/%d", &diaAtual, &mesAtual, &anoAtual);
    int diasRegistro = anoRegistro * 365 + mesRegistro * 30 + diaRegistro;
    int diasAtual = anoAtual * 365 + mesAtual * 30 + diaAtual;
    return diasAtual - diasRegistro;
}

int calculaChave(const internacao& i){
    int pesoPrioridade = 100;
    return i.prioridade * pesoPrioridade + qtdDiasEsperados(i);
}

// Ordenacao da chave
bool operator<(const internacao& a, const internacao& b){
    int chaveA = calculaChave(a);
    int chaveB = calculaChave(b);
    if(chaveA != chaveB) return chaveA < chaveB;
    return a.id < b.id;
}
bool operator>(const internacao& a, const internacao& b){
    int chaveA = calculaChave(a);
    int chaveB = calculaChave(b);
    if(chaveA != chaveB) return chaveA > chaveB;
    return a.id > b.id;
}
bool operator==(const internacao& a, const internacao& b){ return a.id == b.id; }
bool operator<=(const internacao& a, const internacao& b){ return !(a > b); }
bool operator>=(const internacao& a, const internacao& b){ return !(a < b); }

ostream& operator<<(ostream& os, const internacao& i){
    os << "id=" << i.id << " idade=" << i.idade << " prioridade=" << i.prioridade
       << " procedimento=" << i.procedimento << " data=" << i.data << " chave=" << calculaChave(i);
    return os;
}

void lerArquivoInternacoes(avlTree<internacao>& arvore, tabelaHash& indice, string nomeArquivo){
    ifstream arquivo(nomeArquivo); 
    if(!arquivo.is_open()) { cout << "Erro ao abrir arquivo\n"; return; }
    string linha;
    getline(arquivo, linha);
    while(getline(arquivo, linha)){
        stringstream ss(linha);
        string id, idade, prioridade, procedimento, data;
        getline(ss, id, ','); getline(ss, idade, ','); getline(ss, prioridade, ',');
        getline(ss, procedimento, ','); getline(ss, data, ',');
        internacao nova(stoi(id), stoi(idade), stoi(prioridade), procedimento, data);
        arvore.insert(nova);
        indice.inserir(nova);
    }
    arquivo.close();
}

internacao lerInternacao() {
    internacao a;
    string linha;
    cout << "Digite o elemento (id, idade, prioridade, procedimento, data): ";
    cin.ignore();
    getline(cin, linha);
    stringstream ss(linha);
    string id, idade, prioridade;
    getline(ss, id, ','); getline(ss, idade, ','); getline(ss, prioridade, ',');
    getline(ss, a.procedimento, ','); getline(ss, a.data);
    a.id = stoi(id); a.idade = stoi(idade); a.prioridade = stoi(prioridade);
    if(a.procedimento[0] == ' ') a.procedimento.erase(0,1);
    if(a.data[0] == ' ') a.data.erase(0,1);
    return a;
}

// Benchmark da AVL
void executarBenchmarkAVL(avlTree<internacao>& arvore, tabelaHash& indice) {
    if (arvore.empty()) {
        cout << "\nERRO: A arvore esta vazia! Carregue o arquivo (Opcao 1) antes de testar.\n";
        return;
    }
    cout << "\n=======================================================\n";
    cout << "   INICIANDO BENCHMARK AVL (10.000 operacoes/cenario)    \n";
    cout << "=======================================================\n";

    mt19937 rng(42); 
    uniform_int_distribution<int> distID(1, 99995); 
    uniform_int_distribution<int> distPrioridade(0, 3);
    int novoIdCount = 200000; 

    auto getRotacoes = [&]() { return arvore.lli + arvore.lri + arvore.rli + arvore.rri + arvore.lle + arvore.lre + arvore.rle + arvore.rre; };
    auto resetRotacoes = [&]() { arvore.lli = arvore.lri = arvore.rli = arvore.rri = arvore.lle = arvore.lre = arvore.rle = arvore.rre = 0; };

    // Cenario A
    resetRotacoes();
    auto start = chrono::high_resolution_clock::now();
    for(int i=0; i<10000; i++) {
        if(i % 10 != 0) {
            internacao pac; int idBusca = distID(rng);
            if(indice.buscar(idBusca, pac)) arvore.contains(pac);
        } else {
            internacao nova(novoIdCount++, 30, distPrioridade(rng), "Proc A", "10/05/2026");
            arvore.insert(nova); indice.inserir(nova);
        }
    }
    auto end = chrono::high_resolution_clock::now();
    long long tempoA = chrono::duration_cast<chrono::nanoseconds>(end - start).count() / 10000;
    long long rotA = getRotacoes();
    cout << "Cenario A (Consulta Pesada) : " << tempoA << " ns/op | Rotacoes: " << rotA << "\n";

    // Cenario B
    resetRotacoes();
    start = chrono::high_resolution_clock::now();
    for(int i=0; i<10000; i++) {
        int chance = i % 10;
        if(chance == 0) {
            internacao pac; int idBusca = distID(rng);
            if(indice.buscar(idBusca, pac)) arvore.contains(pac);
        } else if (chance <= 5) {
            internacao pac; int idBusca = distID(rng);
            if(indice.buscar(idBusca, pac)) { arvore.erase(pac); indice.remover(idBusca); }
        } else {
            internacao nova(novoIdCount++, 45, distPrioridade(rng), "Proc B", "12/05/2026");
            arvore.insert(nova); indice.inserir(nova);
        }
    }
    end = chrono::high_resolution_clock::now();
    long long tempoB = chrono::duration_cast<chrono::nanoseconds>(end - start).count() / 10000;
    long long rotB = getRotacoes();
    cout << "Cenario B (Escrita Pesada)  : " << tempoB << " ns/op | Rotacoes: " << rotB << "\n";

    // Cenario C
    resetRotacoes();
    start = chrono::high_resolution_clock::now();
    for(int i=0; i<10000; i++) {
        int chance = i % 4;
        if(chance <= 1) {
            internacao pac; int idBusca = distID(rng);
            if(indice.buscar(idBusca, pac)) arvore.contains(pac);
        } else if (chance == 2) {
            internacao pac; int idBusca = distID(rng);
            if(indice.buscar(idBusca, pac)) { arvore.erase(pac); indice.remover(idBusca); }
        } else {
            internacao nova(novoIdCount++, 60, distPrioridade(rng), "Proc C", "15/05/2026");
            arvore.insert(nova); indice.inserir(nova);
        }
    }
    end = chrono::high_resolution_clock::now();
    long long tempoC = chrono::duration_cast<chrono::nanoseconds>(end - start).count() / 10000;
    long long rotC = getRotacoes();
    cout << "Cenario C (Misto Realista)  : " << tempoC << " ns/op | Rotacoes: " << rotC << "\n";

    // Cenario D
    resetRotacoes();
    start = chrono::high_resolution_clock::now();
    int removidos = 0;
    for(int i=0; i<10000; i++) {
        internacao pac; int idBusca = distID(rng);
        if(indice.buscar(idBusca, pac)) { arvore.erase(pac); indice.remover(idBusca); removidos++; }
    }
    end = chrono::high_resolution_clock::now();
    long long tempoD = removidos > 0 ? chrono::duration_cast<chrono::nanoseconds>(end - start).count() / removidos : 0;
    long long rotD = getRotacoes();
    cout << "Cenario D (Rajada Agendam.): " << tempoD << " ns/op | Rotacoes: " << rotD << "\n";
    cout << "=======================================================\n";

    // Exporta os resultados
    ofstream arquivoSaida("benchmark_avl.csv");
    if (arquivoSaida.is_open()) {
        arquivoSaida << "Cenario,Tempo_ns,Rotacoes\n";
        arquivoSaida << "A (Consulta Pesada)," << tempoA << "," << rotA << "\n";
        arquivoSaida << "B (Escrita Pesada)," << tempoB << "," << rotB << "\n";
        arquivoSaida << "C (Misto Realista)," << tempoC << "," << rotC << "\n";
        arquivoSaida << "D (Rajada Agend.)," << tempoD << "," << rotD << "\n";
        arquivoSaida.close();
        cout << "-> Resultados salvos com sucesso em 'benchmark_avl.csv'\n";
    } else {
        cout << "-> ERRO: Nao foi possivel criar o arquivo 'benchmark_avl.csv'\n";
    }
}

int main(){
    avlTree<internacao> arvore;
    tabelaHash indice;
    int opcao;

    do {
        cout << "\nEscolha uma das opcoes a seguir:\n"
            << "1) Ler arquivo com internacoes registradas\n"
            << "2) Inserir elemento na arvore\n"
            << "3) Excluir elemento da arvore\n"    
            << "4) Buscar elemento na arvore\n" 
            << "5) Alterar um id na arvore\n" 
            << "6) Retornar a quantidade de nos na arvore\n"
            << "7) Retornar a altura da arvore\n"
            << "8) Mudar data atual\n"
            << "9) Apagar arvore\n"
            << "10) Sair do programa\n"
            << "11) Rodar testes de benchmark (Automatizado)\n";

        cin >> opcao;

        switch(opcao){
            case 1: lerArquivoInternacoes(arvore, indice, "internacoes.csv"); cout << "Arquivo carregado.\n"; break;
            case 2: { internacao a = lerInternacao(); arvore.insert(a); indice.inserir(a); cout << "Elemento inserido.\n"; break; }
            case 3: { int id; cout << "ID: "; cin >> id; internacao p; if(indice.buscar(id, p)){ arvore.erase(p); indice.remover(id); cout << "Removido.\n"; } else cout << "Nao encontrado.\n"; break; }
            case 4: { int id; cout << "ID: "; cin >> id; internacao p; if(indice.buscar(id, p)) cout << "Paciente:\n" << p << endl; else cout << "Nao encontrado.\n"; break; }
            case 5: { 
                int id; cout << "ID: "; cin >> id; internacao p; 
                if(indice.buscar(id, p)){ 
                    if (!arvore.erase(p)) break; 
                    cout << "Nova prioridade: "; cin >> p.prioridade; 
                    arvore.insert(p); indice.inserir(p); cout << "Atualizado.\n"; 
                } else cout << "Nao encontrado.\n"; 
                break; 
            }
            case 6: cout << "Quantidade: " << arvore.size() << '\n'; break;
            case 7: cout << "Altura: " << arvore.height() << '\n'; break;
            case 8: cout << "Nova data (dd/mm/aaaa): "; cin >> dataAtual; break;
            case 9: arvore.clear(); indice.clear(); cout << "Arvore apagada.\n"; break;
            case 10: cout << "Encerrando...\n"; break;
            case 11: executarBenchmarkAVL(arvore, indice); break;
            default: cout << "Opcao invalida.\n";
        }
    } while(opcao != 10);
    return 0;
}