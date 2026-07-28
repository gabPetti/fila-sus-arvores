#ifndef HASH_H
#define HASH_H

#include <unordered_map> //biblioteca que mapeia um indice para um objeto
#include "internacao.h"


class tabelaHash{
private:
    unordered_map<int, internacao> tabela;

public:
    void inserir(internacao i){
        tabela[i.id] = i;
    }

    bool buscar(int id, internacao& resultado){
        auto it = tabela.find(id);

        if(it == tabela.end())
            return false;

        resultado = it->second;
        return true;
    }

    void remover(int id){
        tabela.erase(id);
    }
    void clear(){
        tabela.clear();
    }
};

#endif