default: getEnvInfo

getEnvInfo.o: getEnvInfo.c
	gcc -c getEnvInfo.c -fopenmp

getEnvInfo: getEnvInfo.o
	gcc -o getEnvInfo getEnvInfo.o -fopenmp

clean:
	rm -f *.o getEnvInfo
