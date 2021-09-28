CFLAGS:=-g -Wno-unused-value -pthread -Wall
CC:=g++
DIR=build
OBJECTS1 := $(addprefix $(DIR)/,sim.o base.o smart_algo_withlookahead.o random_algorithm.o smart_algorithm2.o smart_algorithm.o first_fit.o first_fit_icp.o config.o helper.o bin.o generate_box.o run.o)
output: $(OBJECTS1)
	$(CC) $(CFLAGS) $(OBJECTS1) -o $(DIR)/run -pthread

$(DIR)/run.o: run.cpp sim.h generate_box.h
	$(CC) -c -g run.cpp -o $@

$(DIR)/%.o: %.cpp %.h
	mkdir -p $(DIR);
	$(CC) -c -g $< -o $@

$(DIR)/%.o: %.cpp
	mkdir -p $(DIR);
	$(CC) -c -g $< -o $@

all clean:
	@rm -rf $(DIR)
target: dependencies
	action