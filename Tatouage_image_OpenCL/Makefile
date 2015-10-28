CC=gcc
BMP_LIB=bmpfuncs.o
CL_LIB=clExtend.o
CFLAGS=-lOpenCL

EXO1=reduction_seq reduction_gpu1 reduction_gpu2
EXO2=encodage_seq1 decodage_seq1 decodage_gpu1 decodage_gpu2
EXO3=encodage_seq2 encodage_gpu1 encodage_gpu2
EXO4=encodage_seq3 encodage_gpu3 encodage_gpu4

all: $(EXO1) $(EXO2) $(EXO3) $(EXO4)

exo1: $(EXO1)
exo2: $(EXO2)
exo3: $(EXO3)
exo4: $(EXO4)

# Exercice 1 :

reduction_seq: question1_seq_01.cpp $(BMP_LIB)
	$(CC) -o reduction_seq $(BMP_LIB) -c question1_seq_01.cpp
				
reduction_gpu1: question1_gpu_01.cpp $(BMP_LIB) $(CL_LIB) opencl_errors.h
	$(CC) -o reduction_gpu1 $(BMP_LIB) $(CL_LIB) -c question1_gpu_01.cpp $(CFLAGS)
				
reduction_gpu2: question1_gpu_02.cpp $(BMP_LIB) $(CL_LIB) opencl_errors.h
	$(CC) -o reduction_gpu2 $(BMP_LIB) $(CL_LIB) -c question1_gpu_02.cpp $(CFLAGS)

# Exercice 2 :
				
encodage_seq1: question2_seq_01.cpp $(BMP_LIB)
	$(CC) -o encodage_seq1 $(BMP_LIB) -c question2_seq_01.cpp

decodage_seq1: question2_seq_02.cpp $(BMP_LIB)
	$(CC) -o decodage_seq1 $(BMP_LIB) -c question2_seq_02.cpp

decodage_gpu1: question2_seq_02.cpp $(BMP_LIB) $(CL_LIB) opencl_errors.h
	$(CC) -o decodage_gpu1 $(BMP_LIB) $(CL_LIB) -c question2_seq_02.cpp $(CFLAGS)

decodage_gpu2: question2_gpu_02.cpp $(BMP_LIB) $(CL_LIB) opencl_errors.h
	$(CC) -o decodage_gpu2 $(BMP_LIB) $(CL_LIB) -c question2_gpu_02.cpp $(CFLAGS)

# Exercice 3 :
				
encodage_seq2:question3_seq_01.cpp $(BMP_LIB)
	$(CC) -o encodage_seq2 $(BMP_LIB) -c question3_seq_01.cpp

encodage_gpu1:question3_gpu_01.cpp $(BMP_LIB) $(CL_LIB) opencl_errors.h
	$(CC) -o encodage_gpu1 $(BMP_LIB) $(CL_LIB) -c question3_gpu_01.cpp $(CFLAGS)

encodage_gpu2:question3_gpu_02.cpp $(BMP_LIB) $(CL_LIB) opencl_errors.h
	$(CC) -o encodage_gpu2 $(BMP_LIB) $(CL_LIB) -c question3_gpu_02.cpp $(CFLAGS)

# Exercice 4 :

encodage_seq3:question4_seq_01.cpp $(BMP_LIB)
	$(CC) -o encodage_seq3 $(BMP_LIB) -c question4_seq_01.cpp
		
encodage_gpu3: question4_gpu_01.cpp $(BMP_LIB) $(CL_LIB) opencl_errors.h
	$(CC) -o encodage_gpu3 $(BMP_LIB) $(CL_LIB) -c question4_gpu_01.cpp $(CFLAGS)
	
encodage_gpu4:question4_gpu_02.cpp $(BMP_LIB) $(CL_LIB) opencl_errors.h
	$(CC) -o encodage_gpu4 $(BMP_LIB) $(CL_LIB) -c question4_gpu_02.cpp $(CFLAGS)

# Dependancies :

bmpfuncs.o: bmpfuncs.cpp bmpfuncs.h
	$(CC) -o bmpfuncs.o -c bmpfuncs.cpp
			
clExtend.o: clExtend.cpp clExtend.h opencl_errors.h
	$(CC) -o clExtend.o -c clExtend.cpp $(CFLAGS)