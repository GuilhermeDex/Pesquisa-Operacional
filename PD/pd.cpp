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

    // Restricoes
    // Cada agente deve executar exatamente uma tarefa
    for (int i = 0; i < N; i++) {
        IloExpr sum(env);
        for (int j = 0; j < N; j++) {
            sum += x[i][j];
        }
        model.add(sum == 1);
        sum.end();
    }

    // Cada tarefa deve ser atribuida a exatamente um agente
    for (int j = 0; j < N; j++) {
        IloExpr sum(env);
        for (int i = 0; i < N; i++) {
            sum += x[i][j];
        }
        model.add(sum == 1);
        sum.end();
    }

    cplex.setParam(IloCplex::TiLim, CPLEX_TIME_LIM);
    cplex.setParam(IloCplex::EpGap, 0.0);  
    cplex.setParam(IloCplex::Param::MIP::Strategy::Search, IloCplex::Traditional);  

    cplex.solve();

    if (cplex.getStatus() == IloAlgorithm::Optimal) {
        cout << "Solução ótima encontrada!" << endl;
        cout << "Custo mínimo: " << cplex.getObjValue() << endl;

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (cplex.getValue(x[i][j]) > 0.5) {  
                    cout << "Agente " << i << " -> Tarefa " << j << " (Custo: " << custo[i][j] << ")" << endl;
                }
            }
        }
    } else {
        cout << "Nenhuma solução ótima encontrada." << endl;
    }

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

    cout << "Matriz de Custos Lida:" << endl;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            cout << custo[i][j] << " ";
        }
        cout << endl;
    }

    cplex();
    return 0;
}
