#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

using namespace std;
ILOSTLBEGIN; //MACRO - "using namespace" for ILOCPEX

#define CPLEX_TIME_LIM 3600 //3600 segundos
#define MAX_INT 100
// DADOS PARA O PROBLE PFCM

typedef struct edges
{
    int maximum_capacity = -1;
} Edges;

int vertexCount;
int edgeCount;
int origin;
int destination;

vector<vector<Edges>> A(MAX_INT, vector<Edges>(MAX_INT));

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
            if(A[i][j].maximum_capacity != -1){
                x[i].add(IloIntVar(env, 0, A[i][j].maximum_capacity));
                numberVar++;
            }else{
                x[i].add(IloIntVar(env, 0, 0));
            }
        }
    }

    IloModel model(env);

    // somatórios para restrições e função objetivo
    IloExpr sum1(env);
    IloExpr sum2(env);

    // Função Objetivo:
    sum1.clear();
    for(int i = 0; i < vertexCount; i++){
        // se a aresta existe
        if(A[origin][i].maximum_capacity != -1){
            sum1 += x[origin][i];
        }
    }
    model.add(IloMaximize(env, sum1)); // Maximização

    // Restrições:

    // Restrição da conservação de fluxo
    for(int i = 0; i < vertexCount; i++){
        if(i == origin || i == destination) continue;

        // somatorio de tudo que sai
        sum1.clear();
        for(int j = 0; j < vertexCount; j++){
            // se a aresta existe
            if(A[i][j].maximum_capacity != -1){
                sum1 += x[i][j];
            }
        }

        // somatorio de tudo que entra
        sum2.clear();
        for(int k = 0; k < vertexCount; k++){
            // se a aresta existe
            if(A[k][i].maximum_capacity != 0){
                sum2 += x[k][i];
            }
        }

        // tudo que sai  == tudo que entra
        model.add(sum1 == sum2);
    }
    numberRes++;

    // restrições de capacidade
    for(int i = 0; i < vertexCount; i++){
        for(int j = 0; j < vertexCount; j++){
            if(A[i][j].maximum_capacity != -1){
                model.add(x[i][j] <= A[i][j].maximum_capacity);
            }
        }
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
                if(A[i][j].maximum_capacity != -1){
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
	sum2.end();

	cout << "Memory usage before end:  " << env.getMemoryUsage() / (1024. * 1024.) << " MB" << endl;
	env.end();
}

int main(){
    // Leitura dos dados
    cin >> vertexCount >> edgeCount;
    for (int i = 0; i < edgeCount; i++) {
        int x, y, c, limL, limU;
        cin >> x >> y >> c;
        A[x][y].maximum_capacity = c;
    }

    cin >> origin >> destination;

    // Impressão dos dados de entrada formatados
    cout << "\nGrafo (arestas com custos e capacidades):\n";
    for (int i = 0; i < vertexCount; i++) {
        for (int j = 0; j < vertexCount; j++) {
            if (A[i][j].maximum_capacity != -1) {
                cout << "Aresta " << i << " -> " << j << ": Custo = " << A[i][j].maximum_capacity << "\n";
            }
        }
    }
    
    cout << "Origin: " << origin << endl;
    cout << "Destination: " << destination << endl << endl;

    cplex();
    return 0;

    return 0;
}


