/*---------------- File: pd.cpp  ----------------------+
| Problema da Designação (PD)                          |
|                                                      |
| Adaptado por Guilherme Francis e Lucas Rocha         |
+----------------------------------------------------+ */

#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

using namespace std;
ILOSTLBEGIN

#define CPLEX_TIME_LIM 3600

int N;
vector<vector<int>> custo;

void cplex() {
    IloEnv env;
    IloModel model(env);
    IloCplex cplex(model);

    IloArray<IloNumVarArray> x(env, N);
    for (int i = 0; i < N; i++) {
        x[i] = IloNumVarArray(env, N, 0, 1, ILOBOOL);
    }

    // Função Objetivo: Minimizar o custo total
    IloExpr obj(env);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            obj += custo[i][j] * x[i][j];
        }
    }
    model.add(IloMinimize(env, obj));

    // Restrições
    // Cada agente deve executar exatamente uma tarefa
    for (int i = 0; i < N; i++) {
        IloExpr sum(env);
        for (int j = 0; j < N; j++) {
            sum += x[i][j];
        }
        model.add(sum == 1);
        sum.end();
    }

    // Cada tarefa deve ser atribuída a exatamente um agente
    for (int j = 0; j < N; j++) {
        IloExpr sum(env);
        for (int i = 0; i < N; i++) {
            sum += x[i][j];
        }
        model.add(sum == 1);
        sum.end();
    }

    // Informações antes da execução
    cout << "-------- Informações da Execução ----------\n";
    cout << "#Variáveis: " << (N * N) << "\n";
    cout << "#Restrições: " << (2 * N) << "\n";
    cout << "Uso de memória antes da solução: " << env.getMemoryUsage() / (1024. * 1024.) << " MB\n";

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

    cout << "\nStatus da Função Objetivo: " << status << "\n";
    if (sol) {
        cout << "Custo mínimo: " << cplex.getObjValue() << "\n";
        cout << "Atribuições encontradas:\n";
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (cplex.getValue(x[i][j]) > 0.5) {
                    cout << "Agente " << i << " -> Tarefa " << j << " (Custo: " << custo[i][j] << ")\n";
                }
            }
        }
        printf("Tempo de execução: %.6lf segundos\n\n", difftime(timer2, timer));
    } else {
        cout << "Nenhuma solução ótima encontrada.\n";
    }

    cout << "Uso de memória após solução: " << env.getMemoryUsage() / (1024. * 1024.) << " MB\n";

    obj.end();
    cplex.end();
    model.end();
    env.end();
}

int main() {
    cin >> N;
    custo.resize(N, vector<int>(N));

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            cin >> custo[i][j];
        }
    }

    cout << "Matriz de Custos Lida:\n";
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            cout << custo[i][j] << " ";
        }
        cout << endl;
    }

    cplex();
    return 0;
}
