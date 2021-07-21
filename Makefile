output: helper.o sim.o generate_box.o controller.o run.o
	g++ helper.o sim.o generate_box.o controller.o run.o -o run

helper.o: helper.cpp helper.h
	g++ -c helper.cpp

sim.o: sim.cpp sim.h
	g++ -c sim.cpp

generate_box.o: generate_box.cpp generate_box.h
	g++ -c generate_box.cpp

controller.o: controller.cpp controller.h
	g++ -c controller.cpp

run.o: run.cpp controller.h sim.h generate_box.h
	g++ -c run.cpp

clean:
	rm *.o run *.gch
target: dependencies
	action