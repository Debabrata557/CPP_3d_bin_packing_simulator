output: base.o first_fit.o first_fit_icp.o config.o helper.o sim.o generate_box.o run.o
	g++ -g base.o first_fit.o first_fit_icp.o config.o helper.o sim.o generate_box.o run.o -o run 

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

sim.o: sim.cpp sim.h
	g++ -c -g sim.cpp

generate_box.o: generate_box.cpp generate_box.h
	g++ -c -g generate_box.cpp

run.o: run.cpp sim.h generate_box.h
	g++ -c -g run.cpp

clean:
	rm *.o run *.gch
target: dependencies
	action