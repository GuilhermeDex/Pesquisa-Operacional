/*---------------- File: pcm.cpp  ---------------------+
| Problema do Caminho Minimo (PCM)                     |
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

    cout << "-------- Informacoes da Execucao ----------\n";
    cout << "#Variaveis: " << M << "\n";
    cout << "#Restricoes: " << N << "\n";
    cout << "Uso de memoria antes da solucao: " << env.getMemoryUsage() / (1024. * 1024.) << " MB\n";

    cplex.setParam(IloCplex::TiLim, CPLEX_TIME_LIM);

    time_t timer, timer2;
    time(&timer);
    cplex.solve();
    time(&timer2);

    bool sol = true;
    string status;
    switch (cplex.getStatus()) {
        case IloAlgorithm::Optimal:
            status = "Optimal";
            break;
        case IloAlgorithm::Feasible:
            status = "Feasible";
            break;
        default:
            status = "No Solution";
            sol = false;
    }

    cout << "\nStatus da Funcao Objetivo: " << status << "\n";
    if (sol) {
        cout << "Custo minimo: " << cplex.getObjValue() << "\n";
        cout << "Arestas no caminho minimo:\n";
        for (int i = 0; i < M; i++) {
            if (cplex.getValue(x[i]) > 0.5)
                cout << " " << arestas[i].origem << " -> " << arestas[i].destino << " (Custo: " << arestas[i].custo << ")\n";
        }
        printf("Tempo de execucao: %.6lf segundos\n\n", difftime(timer2, timer));
    } else {
        cout << "Erro: O problema eh inviavel! Verifique se ha um caminho possivel entre origem e destino.\n";
    }

    cout << "Uso de memoria apos solucao: " << env.getMemoryUsage() / (1024. * 1024.) << " MB\n";

    obj.end();
    cplex.end();
    model.end();
    env.end();
}

int main() {
    cin >> N >> M;
    arestas.resize(M);
    for (int i = 0; i < M; i++) {
        cin >> arestas[i].origem >> arestas[i].destino >> arestas[i].custo;
    }
    cin >> origemFonte >> destinoDestino;

    cout << "Lista de Arestas:\n";
    for (const auto& e : arestas) {
        cout << " " << e.origem << " -> " << e.destino << " (Custo: " << e.custo << ")\n";
    }
    cout << "Origem: " << origemFonte << ", Destino: " << destinoDestino << "\n\n";

    cplex();
    return 0;
}
