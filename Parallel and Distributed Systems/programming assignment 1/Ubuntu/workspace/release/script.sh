arr=(3 10 30 100 200 500 1000 2000)
numThreadsOrProcess=6

echo "Number of Threads or Process : ${numThreadsOrProcess}"
echo "creating logs directory"
mkdir logs
echo "moving to logs directory"
cd logs
echo "creating serial directory"
mkdir serial
echo "moving to serial directory"
cd serial

echo "running serial code"
export CWD_MY_VAR=$(pwd)
for i in "${arr[@]}"
do
	echo "running for Matrix ${i}"
	rm -rf "${CWD_MY_VAR}/serial${i}.log"
	echo "../../gausselimination/1serial/serial ${i} 3 ${CWD_MY_VAR}/serial${i}.txt"
   	../../gausselimination/1serial/serial "${i}" 3 "${CWD_MY_VAR}/serial${i}.txt"
done

echo "Press any key to continue"
read a

cd ../
echo "creating mpi directory"
mkdir mpi
echo "moving to mpi directory"
cd mpi
export CWD_MY_VAR=$(pwd)
echo "running mpi code"

for i in "${arr[@]}"
do
	rm -rf ${CWD_MY_VAR}/"mpi${i}.log"
	echo "running for Matrix ${i}"
	echo "mpirun -n ${numThreadsOrProcess} ../../gausselimination/2mpi/mpi $i 3 ${CWD_MY_VAR}/mpi${i}.txt"
   	mpirun -n ${numThreadsOrProcess} ../../gausselimination/2mpi/mpi "$i" 3 "${CWD_MY_VAR}/mpi${i}.txt"
done

echo "Press any key to continue"
read a

cd ../
echo "creating pthread directory"
mkdir pthread
echo "moving to pthread directory"
cd pthread
echo "running pthread code"
export CWD_MY_VAR=$(pwd)
for i in "${arr[@]}"
do
	rm -rf ${CWD_MY_VAR}/"pthread${i}.log"
	echo "running for Matrix ${i}"
	echo "../../gausselimination/3pthread/pthread $i 3 ${CWD_MY_VAR}/pthread${i}.txt ${numThreadsOrProcess}"
   	../../gausselimination/3pthread/pthread "$i" 3 ${CWD_MY_VAR}/"pthread${i}.txt" ${numThreadsOrProcess}
done