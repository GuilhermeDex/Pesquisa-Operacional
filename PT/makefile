INCLUDE=-I/opt/ibm/ILOG/CPLEX_Studio_Community221/cplex/include -I/opt/ibm/ILOG/CPLEX_Studio_Community221/concert/include

FLAGS=-DIL_STD -fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -w

LPATH=-L/opt/ibm/ILOG/CPLEX_Studio_Community221/concert/lib/x86-64_linux/static_pic -L//opt/ibm/ILOG/CPLEX_Studio_Community221/cplex/lib/x86-64_linux/static_pic

LIBRARIES=-lconcert -lilocplex -lcplex -lpthread -ldl

all: pt.o
		g++ -O3 pt.o -o pt.exe $(INCLUDE) $(FLAGS) $(LPATH) $(LIBRARIES)

pt.o: pt.cpp
		g++ -O3 -c pt.cpp $(INCLUDE) $(FLAGS) $(LPATH) $(LIBRARIES)

clean:
	rm *.exe
	rm *.o
