INC := -I${CUDA_HOME}/include
LIB := -L${CUDA_HOME}/lib64 -lcudart -lcuda -lfftw3f -lgsl -lgslcblas

GCC = g++
NVCC = ${CUDA_HOME}/bin/nvcc


NVCCFLAGS = -O3 -arch=sm_70 --ptxas-options=-v -Xcompiler -Wextra -lineinfo
GCC_OPTS =-O3 -Wall -Wextra $(INC)

PFPROTOTYPE = pfl.exe
PFPROTOTYPEEXAMPLE = pfl_example.exe

all: clean pfl pflexample

pfl: Makefile
	$(GCC) -o $(PFPROTOTYPE) pfl.cpp $(GCC_OPTS) -std=c++11

pflexample: Makefile
	$(GCC) -o $(PFPROTOTYPEEXAMPLE) pflexample.cpp  $(GCC_OPTS) -std=c++11

clean:	
	rm -f *.o *.exe
	



