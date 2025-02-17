#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

using namespace std;
ILOSTLBEGIN; //MACRO - "using namespace" for ILOCPEX

#define CPLEX_TIME_LIM 3600 //3600 segundos
#define MAX_INT 100
// DADOS PARA O PROBLE PFCM

typedef struct edges
{
    int cost, l, u;
} Edges;

typedef struct nodeOffer{
    int id, offer;
} NodeOffer;

typedef struct nodeDemand{
    int id, demand;
} NodeDemand;

typedef struct nodeTransfer{
    int id;
} NodeTransfer;

int vertexCount;
int edgeCount;
int offerCount;
int demandCount;
int transferCount;

vector<vector<Edges>> A(MAX_INT, vector<Edges>(MAX_INT));
vector<NodeOffer> S(MAX_INT);
vector<NodeDemand> D(MAX_INT);
vector<NodeTransfer> T(MAX_INT);

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
            x[i].add(IloIntVar(env, 0, A[i][j].u));
        }
    }

    IloModel model(env);

    // somatórios para restrições e função objetivo
    IloExpr sum1(env);
    IloExpr sum2(env);

    // Função Objetivo:
    sum1.clear();
    for(int i = 0; i < vertexCount; i++){
        for(int j = 0; j < vertexCount; j++){
            // se a aresta existe
            if(A[i][j].u != 0){
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
            if(A[S[i].id][j].u != 0){
                sum1 += x[S[i].id][j];
            }
        }

        // somatorio de tudo que entra
        sum2.clear();
        for(int k = 0; k < vertexCount; k++){
            // se a aresta existe
            if(A[k][S[i].id].u != 0){
                sum2 += x[k][S[i].id];
            }
        }

        // tudo que sai - tudo que entra <= oferta
        model.add(sum1 - sum2 <= S[i].offer);
    }

    // Restrição da demanda
    for(int i = 0; i < demandCount; i++){
        // somatorio de tudo que sai
        sum1.clear();
        for(int j = 0; j < vertexCount; j++){
            // se a aresta existe
            if(A[D[i].id][j].u != 0){
                sum1 += x[D[i].id][j];
            }
        }

        // somatorio de tudo que entra
        sum2.clear();
        for(int k = 0; k < vertexCount; k++){
            // se a aresta existe
            if(A[k][D[i].id].u != 0){
                sum2 += x[k][D[i].id];
            }
        }

        // tudo que sai - tudo que entra <= -Demanda
        model.add(sum1 - sum2 <= -D[i].demand);
    }

    // Restrição da conservação de fluxo
    for(int i = 0; i < transferCount; i++){
        // somatorio de tudo que sai
        sum1.clear();
        for(int j = 0; j < vertexCount; j++){
            // se a aresta existe
            if(A[T[i].id][j].u != 0){
                sum1 += x[T[i].id][j];
            }
        }

        // somatorio de tudo que entra
        sum2.clear();
        for(int k = 0; k < vertexCount; k++){
            // se a aresta existe
            if(A[k][T[i].id].u != 0){
                sum2 += x[k][T[i].id];
            }
        }

        // tudo que sai - tudo que entra <= -Demanda
        model.add(sum1 - sum2 == 0);
    }

    // restrições de capacidade
    for(int i = 0; i < vertexCount; i++){
        for(int j = 0; j < vertexCount; j++){
            if(A[i][j].u != 0){
                model.add(x[i][j] <= A[i][j].u);
            }
        }
    }

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
                if(A[i][j].u != 0){
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
        cin >> x >> y >> c >> limL >> limU;
        A[x][y] = {c, limL, limU};
    }

    cin >> offerCount;
    for (int i = 0; i < offerCount; i++) 
        cin >> S[i].id >> S[i].offer;
    
    cin >> demandCount;
    for (int i = 0; i < demandCount; i++) 
        cin >> D[i].id >> D[i].demand;
    
    cin >> transferCount;
    for (int i = 0; i < transferCount; i++) 
        cin >> T[i].id;

    // Impressão dos dados de entrada formatados
    cout << "\nGrafo (arestas com custos e capacidades):\n";
    for (int i = 0; i < vertexCount; i++) {
        for (int j = 0; j < vertexCount; j++) {
            if (A[i][j].u > 0) {
                cout << "Aresta " << i << " -> " << j << ": Custo = " << A[i][j].cost << ", Capacidade = [" << A[i][j].l << ", " << A[i][j].u << "]\n";
            }
        }
    }
    
    cout << "\nNós de oferta:";
    for (int i = 0; i < offerCount; i++)
        cout << " (" << S[i].id << ", " << S[i].offer << ")";

    cout << "\nNós de demanda:";
    for (int i = 0; i < demandCount; i++) 
        cout << " (" << D[i].id << ", " << D[i].demand << ")";

    cout << "\nNós de transferência:";
    for (int i = 0; i < transferCount; i++)
        cout << " " << T[i].id;
    cout << "\n\n";
    
    cplex();
    return 0;

    return 0;
}


