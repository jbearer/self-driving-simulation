CXX = g++
INCLUDE = -Iinclude/ -Itpc/logging/include
CXX_FLAGS = -c -g -std=c++14 -Wall -Wextra -pedantic -Werror $(INCLUDE)
LINK_FLAGS = -lpthread

TARGETS = test_objects

all: $(TARGETS)

clean:
	rm -rf $(TARGETS) *.o

tpc/logging/%:
	mkdir -p tpc; git clone https://github.com/gabime/spdlog.git tpc/logging

logging: include/logging/logging.h tpc/logging/include/spdlog/spdlog.h

objects: include/objects/objects.h src/objects/objects_impl.h

test_objects: test_objects.o objects
	$(CXX) -o $@ $< $(LINK_FLAGS)

test_objects.o: src/objects/test/test_objects.cpp objects
	$(CXX) $(CXX_FLAGS) $<

####################################################################################################
# HARDWARE OBJECTS
####################################################################################################

motor.o: src/hardware/motor.cpp include/hardware/motor.h objects logging
	$(CXX) $(CXX_FLAGS) $<

sensor.o: src/hardware/sensor.cpp include/hardware/sensor.h objects logging
	$(CXX) $(CXX_FLAGS) $<

button.o: src/hardware/button.cpp include/hardware/button.h objects logging
	$(CXX) $(CXX_FLAGS) $<

raspi.o: src/hardware/raspi.cpp include/hardware/raspi.h objects logging
	$(CXX) $(CXX_FLAGS) $<

car.o: src/simulation/car.cpp include/simulation/car.h logging
	$(CXX) $(CXX_FLAGS) $<

#grid.o: src/simulation/grid.cpp include/simulation/grid.h logging
#	$(CXX) $(CXX_FLAGS) $<
