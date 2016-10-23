CXX = g++
INCLUDE = -Iinclude/ -Itpc/logging/include
CXX_FLAGS = -c -g -std=c++14 -Wall -Wextra -pedantic -Werror $(INCLUDE)
LINK_FLAGS = -lpthread

TARGETS = test_objects test_motor test_motor_simple test_button test_grid test_sensor

LOGGING_HEADERS = include/logging/logging.h tpc/logging/include/spdlog/spdlog.h

OBJECTS_HEADERS = include/objects/objects.h src/objects/objects_impl.h $(LOGGING_HEADERS)

HARDWARE_HEADERS = $(wildcard include/hardware/*.h) $(OBJECTS_HEADERS)

all: $(TARGETS)

clean:
	rm -rf $(TARGETS) *.o

again: clean all

tpc/logging/%:
	mkdir -p tpc; git clone https://github.com/gabime/spdlog.git tpc/logging

objects.o: src/objects/objects.cpp $(OBJECTS_HEADERS)
	$(CXX) $(CXX_FLAGS) $<

####################################################################################################
# HARDWARE OBJECTS
####################################################################################################

motor.o: src/hardware/motor.cpp $(HARDWARE_HEADERS) $(OBJECTS_HEADERS) $(LOGGING_HEADERS)
	$(CXX) $(CXX_FLAGS) $<

sensor.o: src/hardware/sensor.cpp $(HARDWARE_HEADERS) $(OBJECTS_HEADERS) $(LOGGING_HEADERS)
	$(CXX) $(CXX_FLAGS) $<

button.o: src/hardware/button.cpp $(HARDWARE_HEADERS) $(OBJECTS_HEADERS) $(LOGGING_HEADERS)
	$(CXX) $(CXX_FLAGS) $<

raspi.o: src/hardware/raspi.cpp $(HARDWARE_HEADERS) $(OBJECTS_HEADERS) $(LOGGING_HEADERS)
	$(CXX) $(CXX_FLAGS) $<

####################################################################################################
# SIMULATION OBJECTS
####################################################################################################


car.o: src/simulation/car.cpp include/simulation/car.h $(LOGGING_HEADERS)
	$(CXX) $(CXX_FLAGS) $<

grid.o: src/simulation/grid.cpp include/simulation/grid.h $(LOGGING_HEADERS)
	$(CXX) $(CXX_FLAGS) $<

intersection.o: src/simulation/intersection.cpp include/simulation/intersection.h $(LOGGING_HEADERS)
	$(CXX) $(CXX_FLAGS) $<


####################################################################################################
# EXECUTABLES
####################################################################################################

test_objects: test_objects.o objects.o
	$(CXX) -o $@ test_objects.o objects.o $(LINK_FLAGS)

test_objects.o: src/objects/test/test_objects.cpp $(OBJECTS_HEADERS) $(LOGGING_HEADERS)
	$(CXX) $(CXX_FLAGS) $<

test_motor: test_motor.o motor.o objects.o raspi.o
	$(CXX) -o $@ $^ $(LINK_FLAGS)

test_motor.o: src/hardware/test/test_motor.cpp $(HARDWARE_HEADERS) $(OBJECTS_HEADERS) $(LOGGING_HEADERS)
	$(CXX) $(CXX_FLAGS) $<

test_motor_simple: test_motor_simple.o motor.o objects.o raspi.o
	$(CXX) -o $@ $^ $(LINK_FLAGS)

test_motor_simple.o: src/hardware/test/test_motor_simple.cpp $(HARDWARE_HEADERS) $(OBJECTS_HEADERS) $(LOGGING_HEADERS)
	$(CXX) $(CXX_FLAGS) $<

test_button: test_button.o button.o objects.o raspi.o
	$(CXX) -o $@ $^ $(LINK_FLAGS)

test_button.o: src/hardware/test/test_button.cpp $(HARDWARE_HEADERS) $(OBJECTS_HEADERS) $(LOGGING_HEADERS)
	$(CXX) $(CXX_FLAGS) $<

test_sensor: test_sensor.o sensor.o objects.o raspi.o
	$(CXX) -o $@ $^ $(LINK_FLAGS)

test_sensor.o: src/hardware/test/test_sensor.cpp $(HARDWARE_HEADERS) $(OBJECTS_HEADERS) $(LOGGING_HEADERS)
	$(CXX) $(CXX_FLAGS) $<

test_grid: test_grid.o grid.o car.o objects.o intersection.o motor.o raspi.o
	$(CXX) -o $@ $^ $(LINK_FLAGS)

test_grid.o: src/simulation/test/test_grid.cpp include/simulation/grid.h $(OBJECTS_HEADERS) $(LOGGING_HEADERS)
	$(CXX) $(CXX_FLAGS) $<
