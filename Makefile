CXX = g++
INCLUDE = -Iinclude/ -Itpc/logging/include
CXX_FLAGS = -c -g -std=c++11 -Wall -Wextra -pedantic -Werror $(INCLUDE)
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

test_objects.o: src/objects/test/test_objects.cpp include/objects/objects.h
	$(CXX) $(CXX_FLAGS) $<

####################################################################################################
# HARDWARE OBJECTS
####################################################################################################

motor.o: src/hardware/motor.cpp include/hardware/motor.h logging
	$(CXX) $(CXX_FLAGS) $<

sensor.o: src/hardware/sensor.cpp include/hardware/sensor.h logging
	$(CXX) $(CXX_FLAGS) $<
