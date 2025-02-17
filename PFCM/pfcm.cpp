/*---------------- File: pfcm.cpp ----------------------------+
| Problema de Fluxo de Custo Mínimo (PFCM)                    |
|                                                             |
| Adaptado por Guilherme Francis e Lucas Rocha                |
+------------------------------------------------------------+ */

#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

using namespace std;
ILOSTLBEGIN

#define CPLEX_TIME_LIM 3600

struct Edge {
    int origem, destino, capacidade, custo;
};

int N, M;  
vector<Edge> arestas;
int origemFonte, destinoDestino;


void cplex() {
    IloEnv env;
    IloModel model(env);
    IloCplex cplex(model);

    IloNumVarArray x(env, M, 0, IloInfinity, ILOINT);
    IloNumVar fluxoTotal(env, 0, IloInfinity, ILOINT);  

    IloExpr obj(env);
    for (int i = 0; i < M; i++) {
        obj += arestas[i].custo * x[i];
    }
    model.add(IloMinimize(env, obj));

    for (int i = 0; i < M; i++) {
        model.add(x[i] <= arestas[i].capacidade);
    }

    for (int v = 0; v < N; v++) {
        IloExpr fluxoEntrada(env);
        IloExpr fluxoSaida(env);

        for (int i = 0; i < M; i++) {
            if (arestas[i].origem == v) fluxoSaida += x[i];
            if (arestas[i].destino == v) fluxoEntrada += x[i];
        }

        if (v == origemFonte)
            model.add(fluxoSaida - fluxoEntrada == fluxoTotal); // A origem gera fluxo total
        else if (v == destinoDestino)
            model.add(fluxoEntrada - fluxoSaida == fluxoTotal); // O destino recebe fluxo total
        else
            model.add(fluxoEntrada - fluxoSaida == 0); 
    }

    model.add(fluxoTotal >= 1);

    cplex.setParam(IloCplex::MIPEmphasis, 4);
    cplex.setParam(IloCplex::TiLim, CPLEX_TIME_LIM);
    cplex.setParam(IloCplex::EpGap, 0.0);

    if (cplex.solve()) {
        cout << "Solução ótima encontrada!" << endl;
        cout << "Custo mínimo: " << cplex.getObjValue() << endl;

        double fluxo_total_calculado = cplex.getValue(fluxoTotal);
        cout << "Fluxo total enviado: " << fluxo_total_calculado << endl;

        for (int i = 0; i < M; i++) {
            if (cplex.getValue(x[i]) > 0) {
                cout << "Fluxo na aresta " << arestas[i].origem 
                     << " -> " << arestas[i].destino 
                     << " : " << cplex.getValue(x[i]) 
                     << " (Custo: " << arestas[i].custo << ")" << endl;
            }
        }
    } else {
        cout << "Nenhuma solução ótima encontrada." << endl;
    }
}

int main() {
    cin >> N >> M;
    arestas.resize(M);

    for (int i = 0; i < M; i++) {
        cin >> arestas[i].origem >> arestas[i].destino >> arestas[i].custo >> arestas[i].capacidade;
    }

    cin >> origemFonte >> destinoDestino;

    cplex();

    return 0;
}
