CXX = g++
INCLUDE = -Iinclude/ -Itpc/logging/include
CXX_FLAGS = -Wall -Wextra -pedantic -Werror $(INCLUDE)

tpc/logging/%:
	mkdir -p tpc; git clone https://github.com/gabime/spdlog.git tpc/logging
