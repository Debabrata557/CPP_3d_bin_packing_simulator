output: helper.o sim.o generate_box.o controller.o run.o
	g++ helper.o sim.o generate_box.o controller.o run.o -o run

helper.o: helper.cpp helper.h
	g++ -c helper.cpp

sim.o: sim.cpp config.h
	g++ -c sim.cpp

generate_box.o: generate_box.cpp config.h
	g++ -c generate_box.cpp

controller.o: controller.cpp config.h helper.h
	g++ -c controller.cpp

run.o: run.cpp controller.cpp sim.cpp generate_box.cpp
	g++ -c run.cpp

clean:
	rm *.o run *.gch
target: dependencies
	action