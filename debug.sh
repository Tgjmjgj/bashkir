rm -r build

mkdir build

cd build

source ~/conan/bin/activate

conan install ..

cmake .. -DCMAKE_BUILD_TYPE=Debug

make

cd ..
