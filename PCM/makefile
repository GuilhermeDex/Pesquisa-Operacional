INCLUDE=-I/opt/ibm/ILOG/CPLEX_Studio_Community221/cplex/include -I/opt/ibm/ILOG/CPLEX_Studio_Community221/concert/include

FLAGS=-DIL_STD -fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -w

LPATH=-L/opt/ibm/ILOG/CPLEX_Studio_Community221/concert/lib/x86-64_linux/static_pic -L//opt/ibm/ILOG/CPLEX_Studio_Community221/cplex/lib/x86-64_linux/static_pic

LIBRARIES=-lconcert -lilocplex -lcplex -lpthread -ldl

all: pcm.o
		g++ -O3 pcm.o -o pcm.exe $(INCLUDE) $(FLAGS) $(LPATH) $(LIBRARIES)

pcm.o: pcm.cpp
		g++ -O3 -c pcm.cpp $(INCLUDE) $(FLAGS) $(LPATH) $(LIBRARIES)

clean:
	rm *.exe
	rm *.o
