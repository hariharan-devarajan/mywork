[for serial]
compile 	: 	g++ main.cpp -o serial
permitions 	: 	chmod 755 serial
run 		: 	./serial <matrix> <seed> <logfileLocation>   eg:  ./serial 3 3 "serialtest.log"


[for pthread]
compile 	: 	g++ -pthread pthread.cpp -o pthread
permitions 	: 	chmod 755 pthread
run 		: 	./serial <matrix> <seed> <logfileLocation> <num of threads>   eg:  ./pthread 3 3 "serialtest.log" 6


[for serial]
compile 	: 	mpic++ mpi.cpp -o mpi
permitions 	: 	chmod 755 mpi
run 		:  	mpirun -n <number of processes> ./mpi <matrix> <seed> <logfileLocation>   eg:   mpirun -n 2 ./mpi 3 3 "mpitest.log"