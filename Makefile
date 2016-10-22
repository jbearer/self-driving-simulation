CXX = g++
INCLUDE = -Iinclude/ -Itpc/logging/include
CXX_FLAGS = -c -g -std=c++14 -Wall -Wextra -pedantic -Werror $(INCLUDE)
LINK_FLAGS = -lpthread

TARGETS = test_objects test_motor

LOGGING_HEADERS = include/logging/logging.h tpc/logging/include/spdlog/spdlog.h

OBJECTS_HEADERS = include/objects/objects.h src/objects/objects_impl.h

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

motor.o: src/hardware/motor.cpp include/hardware/motor.h $(OBJECTS_HEADERS) $(LOGGING_HEADERS)
	$(CXX) $(CXX_FLAGS) $<

sensor.o: src/hardware/sensor.cpp include/hardware/sensor.h $(OBJECTS_HEADERS) $(LOGGING_HEADERS)
	$(CXX) $(CXX_FLAGS) $<

button.o: src/hardware/button.cpp include/hardware/button.h $(OBJECTS_HEADERS) $(LOGGING_HEADERS)
	$(CXX) $(CXX_FLAGS) $<

raspi.o: src/hardware/raspi.cpp include/hardware/raspi.h $(OBJECTS_HEADERS) $(LOGGING_HEADERS)
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

test_motor.o: src/hardware/test/test_motor.cpp include/hardware/motor.h $(OBJECTS_HEADERS) $(LOGGING_HEADERS)
	$(CXX) $(CXX_FLAGS) $<
