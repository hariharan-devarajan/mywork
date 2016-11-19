arr=(3 10 20 30 100 200 300 1000 2000 3000 8000)

echo "creating logs directory"
mkdir 3logs
echo "creating serial directory"
mkdir 3logs/serial

echo "compiling serial code"
gcc 1src/matrix_norm_serial/main.c -o 2bin/matrix_norm_serial/main -lm

echo "running serial code"

export CWD_MY_VAR=$(pwd)

for i in "${arr[@]}"
do
	echo "running for Matrix ${i}"
	rm -rf "${CWD_MY_VAR}/3logs/serial/serial${i}.log"
	echo "2bin/matrix_norm_serial/main ${i} 1 ${CWD_MY_VAR}/3logs/serial/serial${i}.txt"
   	2bin/matrix_norm_serial/main "${i}" 1 "${CWD_MY_VAR}/3logs/serial/serial${i}.txt"
done

echo "creating cuda directory"
mkdir 3logs/cuda

echo "compiling cuda code"
nvcc 1src/matrix_norm_cuda/main.cu -o 2bin/matrix_norm_cuda/main

echo "running cuda code"

export CWD_MY_VAR=$(pwd)

for i in "${arr[@]}"
do
	echo "running for Matrix ${i}"
	rm -rf "${CWD_MY_VAR}/3logs/cuda/cuda${i}.log"
	echo "2bin/matrix_norm_cuda/main ${i} 1 ${CWD_MY_VAR}/3logs/cuda/cuda${i}.txt"
   	2bin/matrix_norm_cuda/main "${i}" 1 "${CWD_MY_VAR}/3logs/cuda/cuda${i}.txt"
done
