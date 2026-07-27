#ifndef INTERNACAO_H
#define INTERNACAO_H

#include "bibliotecas.h"

//Estrutura do nó da árvore
struct internacao{
    int id;
    int idade;
    int prioridade;
    string procedimento;
    string data;
    internacao(){
        id = 0; idade = 0; prioridade = 0; procedimento = ""; data = "";
    }
    //Construtor geral
    internacao(int id, int idade, int prioridade, string procedimento, string data){
        this->id = id; this->idade = idade; this->prioridade = prioridade; this->procedimento = procedimento; this->data = data;
    }
};

#endif