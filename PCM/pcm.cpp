/*---------------- File: pcm.cpp  ---------------------+
| Problema do Caminho Mínimo (PCM)                     |
|                                                      |
| Adaptado por Guilherme Francis e Lucas Rocha         |
+-------------------------------------------------------+ */

#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

using namespace std;
ILOSTLBEGIN

#define CPLEX_TIME_LIM 3600

struct Edge {
    int origem, destino, custo;
};

int N, M;
vector<Edge> arestas;
int origemFonte, destinoDestino;

void cplex() {
    IloEnv env;
    IloModel model(env);
    IloCplex cplex(model);

    IloNumVarArray x(env, M, 0, 1, ILOBOOL);

    IloExpr obj(env);
    for (int i = 0; i < M; i++) {
        obj += arestas[i].custo * x[i];
    }
    model.add(IloMinimize(env, obj));

    for (int v = 0; v < N; v++) {
        IloExpr fluxoEntrada(env);
        IloExpr fluxoSaida(env);
        for (int i = 0; i < M; i++) {
            if (arestas[i].origem == v) fluxoSaida += x[i];
            if (arestas[i].destino == v) fluxoEntrada += x[i];
        }
        if (v == origemFonte)
            model.add(fluxoSaida - fluxoEntrada >= 1);
        else if (v == destinoDestino)
            model.add(fluxoEntrada - fluxoSaida >= 1);
        else
            model.add(fluxoEntrada - fluxoSaida == 0);
    }

    cplex.setParam(IloCplex::TiLim, CPLEX_TIME_LIM);
    cplex.solve();

    if (cplex.getStatus() == IloAlgorithm::Infeasible) {
        cout << "Erro: O problema é inviável! Verifique se há um caminho possível entre origem e destino." << endl;
        return;
    }

    cout << "Caminho mínimo encontrado! Custo: " << cplex.getObjValue() << endl;
    for (int i = 0; i < M; i++) {
        if (cplex.getValue(x[i]) > 0.5)
            cout << "Aresta no caminho mínimo: " << arestas[i].origem << " -> " << arestas[i].destino << endl;
    }
}

int main() {
    cin >> N >> M;
    arestas.resize(M);
    for (int i = 0; i < M; i++) {
        cin >> arestas[i].origem >> arestas[i].destino >> arestas[i].custo;
    }
    cin >> origemFonte >> destinoDestino;

    cplex();
    return 0;
}