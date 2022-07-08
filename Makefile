PROJECT=pirtimer
CXX := g++
CXXFLAGS = -Wall -Wextra -Werror -Wfatal-errors
LDLIBS = -pthread
	
default: $(PROJECT)
.PHONY: default

.DELETE_ON_ERROR: