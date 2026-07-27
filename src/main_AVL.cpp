#include "bibliotecas.h"
#include "AVL.h"
#include "internacao.h"
#include "hash.h"

//Variavel global para a data atual de comparacao
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

//Determina a chave para a comparação e implementação dos nós
int qtdDiasEsperados(const internacao& i){
    // Separando a data da internação
    int diaRegistro, mesRegistro, anoRegistro;
    sscanf(i.data.c_str(), "%d/%d/%d", &diaRegistro, &mesRegistro, &anoRegistro); //transforma de string para int 

    // Separando a data atual
    int diaAtual, mesAtual, anoAtual;
    sscanf(dataAtual.c_str(), "%d/%d/%d", &diaAtual, &mesAtual, &anoAtual);

    // Converte cada data para uma quantidade aproximada de dias
    int diasRegistro = anoRegistro * 365 + mesRegistro * 30 + diaRegistro;
    int diasAtual = anoAtual * 365 + mesAtual * 30 + diaAtual;
    // Retorna o tempo que a pessoa está esperando
    return diasAtual - diasRegistro;
}

//Chave de ordenacao
int calculaChave(const internacao& i){
    int pesoPrioridade = 100;

    return i.prioridade * pesoPrioridade + qtdDiasEsperados(i);
}

//Reordenando os operadores para a implementacao da arvore de Brown
bool operator<(const internacao& a, const internacao& b){
    int chaveA = calculaChave(a);
    int chaveB = calculaChave(b);
    if(chaveA != chaveB)
        return chaveA < chaveB;
    return a.id < b.id;
}

bool operator>(const internacao& a, const internacao& b){
    int chaveA = calculaChave(a);
    int chaveB = calculaChave(b);
    if(chaveA != chaveB)
        return chaveA > chaveB;
    return a.id > b.id;
}

bool operator==(const internacao& a, const internacao& b){
    return a.id == b.id;
}

bool operator<=(const internacao& a, const internacao& b){ return !(a > b); }
bool operator>=(const internacao& a, const internacao& b){ return !(a < b); }

//Definindo o operador<<
ostream& operator<<(ostream& os, const internacao& i){
    os << "id=" << i.id
       << " idade=" << i.idade
       << " prioridade=" << i.prioridade
       << " procedimento=" << i.procedimento
       << " data=" << i.data
       << " chave=" << calculaChave(i);
    return os;
}

void lerArquivoInternacoes(avlTree<internacao>& arvore, tabelaHash& indice, string nomeArquivo){
    ifstream arquivo(nomeArquivo); //Faz a leitura do arquivo do diretório
    if(!arquivo.is_open()) {
        cout << "Erro ao abrir arquivo\n";
        return;
    }

    string linha;

    // Ignora o cabeçalho
    getline(arquivo, linha);
    while(getline(arquivo, linha)){
        stringstream ss(linha);
        string id;
        string idade;
        string prioridade;
        string procedimento;
        string data;

        getline(ss, id, ',');
        getline(ss, idade, ',');
        getline(ss, prioridade, ',');
        getline(ss, procedimento, ',');
        getline(ss, data, ',');

        internacao nova(
            stoi(id),
            stoi(idade),
            stoi(prioridade),
            procedimento,
            data
        );
        arvore.insert(nova);
        indice.inserir(nova);
    }
    arquivo.close();
}

// Função auxiliar para ler uma internação
internacao lerInternacao() {
    internacao a;
    string linha;

    cout << "Digite o elemento (id, idade, prioridade, procedimento, data): ";
    //usar getline por causa dos espaços de string.
    cin.ignore();
    getline(cin, linha);

    stringstream ss(linha);
    string id, idade, prioridade;
    getline(ss, id, ',');
    getline(ss, idade, ',');
    getline(ss, prioridade, ',');
    getline(ss, a.procedimento, ',');
    getline(ss, a.data);
    // Remove espaços depois das vírgulas
    a.id = stoi(id);
    a.idade = stoi(idade);
    a.prioridade = stoi(prioridade);

    if(a.procedimento[0] == ' ')
        a.procedimento.erase(0,1);
    if(a.data[0] == ' ')
        a.data.erase(0,1);
    return a;
}

int main(){
    avlTree<internacao> arvore;
    tabelaHash indice;
    int opcao;

do
{
    cout << "\nEscolha uma das opcoes a seguir:\n"
        << "1) Ler arquivo com internações registradas\n"
        << "2) Inserir elemento na árvore\n"
        << "3) Excluir elemento da árvore\n"    
        << "4) Buscar elemento na árvore\n"  //Buscar elemento pro cpf
        << "5) Alterar um id na árvore\n"   //Alterar id a partir do endereço
        << "6) Retornar a quantidade de nós na árvore\n"
        << "7) Retornar a altura da árvore\n"
        << "8) Mudar data atual\n"
        << "9) Apagar árvore\n"
        << "10) Sair do programa\n";

    cin >> opcao;

    switch(opcao){
        case 1:
        {
            lerArquivoInternacoes(arvore, indice, "internacoes.csv");
            cout << "Arquivo carregado.\n";
            break;
        }
        case 2:
        {
            internacao a = lerInternacao();
            arvore.insert(a);
            indice.inserir(a);
            cout << "Elemento inserido.\n";
            break;
        }
        case 3:
        {
            int id;
            cout << "Digite o id do paciente: ";
            cin >> id;
            internacao paciente;
            if(indice.buscar(id, paciente)){
                arvore.erase(paciente);
                indice.remover(id);
                cout << "Elemento removido.\n";
            }
            else{
                cout << "Paciente não encontrado.\n";
            }
            break;
        }
        case 4:
        {
            int id;
            cout << "Digite o id do paciente: ";
            cin >> id;
            internacao paciente;
            if(indice.buscar(id, paciente)){
                cout << "Paciente encontrado:\n";
                cout << paciente << endl;
            }
            else{
                cout << "Paciente não encontrado.\n";
            }
            break;
        }
       case 5:
        {
        int id;
        cout << "Digite o id: ";
        cin >> id;
        internacao paciente;
        if(indice.buscar(id, paciente)){
            bool removeu = arvore.erase(paciente);
            if (!removeu) {
                cout << "ERRO: paciente nao encontrado na arvore (inconsistencia com o indice).\n";
                break;
            }
            cout << "Nova prioridade: ";
            cin >> paciente.prioridade;
            arvore.insert(paciente);
            indice.inserir(paciente);
            cout << "Paciente atualizado.\n";
        }
        else{
            cout << "Paciente nao encontrado.\n";
        }
        break;
        }
        case 6:
        {
            cout << "Quantidade de elementos na árvore: " << arvore.size() << '\n';
            break;
        }
        case 7:
        {
            cout << "Altura da árvore: " << arvore.height() << '\n';
            break;
        }
        case 8:
        {
            cout << "Digite a nova data atual (dd/mm/aaaa): ";
            cin >> dataAtual;
            break;
        }
        case 9:
        {
            arvore.clear();
            indice.clear();
            cout << "Árvore apagada.\n";
            break;
        }
        case 10:
        {
            cout << "Encerrando programa...\n";
            break;
        }
        default:
        {
            cout << "Opção inválida.\n";
        }
    }

} while(opcao != 10);
    return 0;
}