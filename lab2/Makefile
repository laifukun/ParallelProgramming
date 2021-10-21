CC = g++ 
NVCC = nvcc
SRCS =./src/*.cpp ./src/*.cu
INC = ./src/
GCCOPTS = -std=c++17 -Wall -Werror -O3
NVCCOPTS= -arch=sm_50
EXEC = ./bin/kmeans

all: clean compile

compile:
	$(NVCC) $(SRCS) $(NVCCOPTS) -I$(INC) -o $(EXEC)


clean:
	rm -f $(EXEC)
