output: base.o first_fit.o first_fit_icp.o config.o helper.o bin.o generate_box.o run.o
	g++ -g base.o first_fit.o first_fit_icp.o config.o helper.o bin.o generate_box.o run.o -o run -pthread

first_fit.o: first_fit.cpp
	g++ -c -g first_fit.cpp

first_fit_icp.o: first_fit_icp.cpp
	g++ -c -g first_fit_icp.cpp

base.o: base.cpp
	g++ -c -g base.cpp
	
config.o: config.cpp config.h
	g++ -c -g config.cpp

helper.o: helper.cpp helper.h
	g++ -c -g helper.cpp

bin.o: bin.cpp bin.h
	g++ -c -g bin.cpp

generate_box.o: generate_box.cpp generate_box.h
	g++ -c -g generate_box.cpp

run.o: run.cpp bin.h generate_box.h
	g++ -c -g run.cpp

clean:
	rm *.o run *.gch
target: dependencies
	action