blocksize=(1 2 4 8 16)

echo "creating logs directory"
mkdir 3logs


echo "creating mpiio-write directory"
mkdir 3logs/mpiio-write

mkdir 4data
mkdir 4data/mpiio-write
mkdir 4data/mpiio-read

echo "compiling mpiio-write code"
mpicc 1src/mpiio-write/mpiio-write.c -o 2bin/mpiio-write/mpiio-write

echo "running mpiio-write code"

export CWD_MY_VAR=$(pwd)
export WRITE_DIR=/home/hdevarajan/orangefs
mkdir /home/hdevarajan/orangefs/4data
mkdir /home/hdevarajan/orangefs/4data/mpiio-write
mkdir /home/hdevarajan/orangefs/4data/mpiio-read
for i in "${blocksize[@]}"
do
	echo "running for block size ${i}"
	rm -rf "${CWD_MY_VAR}/3logs/mpiio-write/write${i}MB.log"
	rm -rf "${CWD_MY_VAR}/4data/mpiio-write/write${i}MB.bat"
	echo "mpirun -np 4 2bin/mpiio-write/mpiio-write -f ${WRITE_DIR}/4data/mpiio-write/write${i}MB.bat -l "${i}"  -o ${CWD_MY_VAR}/3logs/mpiio-write/write${i}MB.log"
   	mpirun -np 4 2bin/mpiio-write/mpiio-write -f "${WRITE_DIR}/4data/mpiio-write/write${i}MB.bat" -l "${i}"  -o "${CWD_MY_VAR}/3logs/mpiio-write/write${i}MB.log"
   	echo "Press Any Key to continue"
   	read a
done

echo "copying write files for read"

for i in "${blocksize[@]}"
do
	echo "copy for block size ${i}"
	rm -rf "${WRITE_DIR}/4data/mpiio-read/read${i}MB.bat"
	echo "cp ${WRITE_DIR}/4data/mpiio-write/write${i}MB.bat ${WRITE_DIR}/4data/mpiio-read/read${i}MB.bat"
   	cp "${WRITE_DIR}/4data/mpiio-write/write${i}MB.bat" "${WRITE_DIR}/4data/mpiio-read/read${i}MB.bat"
done

echo "creating mpiio-read directory"
mkdir 3logs/mpiio-read

echo "compiling mpiio-read code"
mpicc 1src/mpiio-read/mpiio-read.c -o 2bin/mpiio-read/mpiio-read

echo "running mpiio-read code"

export CWD_MY_VAR=$(pwd)

for i in "${blocksize[@]}"
do
	echo "running for block size ${i}"
	rm -rf "${CWD_MY_VAR}/3logs/mpiio-read/read${i}MB.log"
	echo "mpirun -np 4 2bin/mpiio-read/mpiio-wrreadite -f ${WRITE_DIR}/4data/mpiio-read/read${i}MB.bat  -o ${CWD_MY_VAR}/3logs/mpiio-read/read${i}MB.log"
   	mpirun -np 4 2bin/mpiio-read/mpiio-read -f "${WRITE_DIR}/4data/mpiio-read/read${i}MB.bat"  -o "${CWD_MY_VAR}/3logs/mpiio-read/read${i}MB.log"
   	echo "Press Any Key to continue"
   	read a
done
