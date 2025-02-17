/*---------------- File: pt.cpp  ----------------------+
| Problema do Transporte (PT)                          |
|                                                      |
| Adaptado por Guilherme Francis e Lucas Rocha         |
+----------------------------------------------------+ */

#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

using namespace std;
ILOSTLBEGIN; //MACRO - "using namespace" for ILOCPEX

#define CPLEX_TIME_LIM 3600 //3600 segundos
#define MAX_INT 100
// DADOS PARA O PROBLE PFCM

typedef struct edges
{
    int cost = -1;
} Edges;

typedef struct nodeOffer{
    int id, offer;
} NodeOffer;

typedef struct nodeDemand{
    int id, demand;
} NodeDemand;


int vertexCount;
int edgeCount;
int offerCount;
int demandCount;

vector<vector<Edges>> A(MAX_INT, vector<Edges>(MAX_INT));
vector<NodeOffer> S(MAX_INT);
vector<NodeDemand> D(MAX_INT);

void cplex(){
    // CPLEX
    IloEnv env; // Define o ambiente do CPLEX

    // Variaveis
    int numberVar = 0;
    int numberRes = 0;

    /*
        MODELAGEM
    */

    // Variáveis de decisão
    IloArray<IloNumVarArray> x(env);
    for(int i = 0; i < vertexCount; i++){
        x.add(IloNumVarArray(env));
        for(int j = 0; j < vertexCount; j++){
            if(A[i][j].cost == -1){
                x[i].add(IloIntVar(env, 0, 0));
            }else{
                x[i].add(IloIntVar(env, 0, INT_MAX));
                numberVar++;
            }
        }
    }

    IloModel model(env);

    // somatórios para restrições e função objetivo
    IloExpr sum1(env);

    // Função Objetivo:
    sum1.clear();
    for(int i = 0; i < vertexCount; i++){
        for(int j = 0; j < vertexCount; j++){
            // se a aresta existe
            if(A[i][j].cost != -1){
                sum1 += (A[i][j].cost * x[i][j]);
            }
        }
    }
    model.add(IloMinimize(env, sum1)); // Minimização

    // Restrições:

    // Restrição de oferta
    for(int i = 0; i < offerCount; i++){
        // somatorio de tudo que sai
        sum1.clear();
        for(int j = 0; j < vertexCount; j++){
            // se a aresta existe
            if(A[S[i].id][j].cost != -1){
                sum1 += x[S[i].id][j];
            }
        }

        // tudo que sai <= oferta
        model.add(sum1 <= S[i].offer);
    }
    numberRes++;

    // Restrição da demanda
    for(int i = 0; i < demandCount; i++){
        // somatorio de tudo que sai
        sum1.clear();
        for(int j = 0; j < vertexCount; j++){
            // se a aresta existe
            if(A[j][D[i].id].cost != -1){
                sum1 += x[j][D[i].id];
            }
        }

        // tudo que sai == Demanda
        model.add(sum1 == D[i].demand);
    }
    numberRes++;

    //------ EXECUCAO do MODELO ----------
	time_t timer, timer2;
	IloNum value, objValue;
	double runTime;
	string status;
	
	//Informacoes ---------------------------------------------	
	printf("--------Informacoes da Execucao:----------\n\n");
	printf("#Var: %d\n", numberVar);
	printf("#Restricoes: %d\n", numberRes);
	cout << "Memory usage after variable creation:  " << env.getMemoryUsage() / (1024. * 1024.) << " MB" << endl;
	
	IloCplex cplex(model);
	cout << "Memory usage after cplex(Model):  " << env.getMemoryUsage() / (1024. * 1024.) << " MB" << endl;

	cplex.setParam(IloCplex::TiLim, CPLEX_TIME_LIM);

	time(&timer);
	cplex.solve();//COMANDO DE EXECUCAO
	time(&timer2);
	
	bool sol = true;
	switch(cplex.getStatus()){
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

	cout << endl << endl;
	cout << "Status da FO: " << status << endl;

	if(sol){ 
		objValue = cplex.getObjValue();
		runTime = difftime(timer2, timer);
		
		cout << "Variaveis de decisao: " << endl;

        for(int i = 0; i < vertexCount; i++){
            for(int j = 0; j < vertexCount; j++){
                // se existe aresta
                if(A[i][j].cost != -1){
                    value = IloRound(cplex.getValue(x[i][j]));
			        printf("x[%d][%d]: %.0lf\n", i, j, value);
                }
            }   
        }
		
		cout << "Funcao Objetivo Valor = " << objValue << endl;
		printf("..(%.6lf seconds).\n\n", runTime);

	}else{
		printf("No Solution!\n");
	}

	//Free Memory
	cplex.end();
	sum1.end();

	cout << "Memory usage before end:  " << env.getMemoryUsage() / (1024. * 1024.) << " MB" << endl;
	env.end();
}

int main(){
    // Leitura dos dados
    cin >> vertexCount >> edgeCount;
    for (int i = 0; i < edgeCount; i++) {
        int x, y, c;
        cin >> x >> y >> c;
        A[x][y].cost = c;
    }

    cin >> offerCount;
    for (int i = 0; i < offerCount; i++) 
        cin >> S[i].id >> S[i].offer;
    
    cin >> demandCount;
    for (int i = 0; i < demandCount; i++) 
        cin >> D[i].id >> D[i].demand;

    // Impressão dos dados de entrada formatados
    cout << "\nGrafo (arestas com custos e capacidades):\n";
    for (int i = 0; i < vertexCount; i++) {
        for (int j = 0; j < vertexCount; j++) {
            if (A[i][j].cost != -1) {
                cout << "Aresta " << i << " -> " << j << ": Custo = " << A[i][j].cost << "\n";
            }
        }
    }
    
    cout << "\nNós de oferta:";
    for (int i = 0; i < offerCount; i++)
        cout << " (" << S[i].id << ", " << S[i].offer << ")";

    cout << "\nNós de demanda:";
    for (int i = 0; i < demandCount; i++) 
        cout << " (" << D[i].id << ", " << D[i].demand << ")";
    cout << "\n\n";
    
    cplex();
    return 0;

    return 0;
}


