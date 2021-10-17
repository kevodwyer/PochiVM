
Attempt at using [PochiVM](https://sillycross.github.io/PochiVM) to build a [toy dynamic core](https://github.com/kevodwyer/dynamic-core) for a toy emulator.


to build

assumes running inside a ubuntu 18.04 virtual machine

apt update && apt install -y git wget tar xz-utils sudo cmake make ninja-build python ccache

apt update && apt-get install -y g++ libtinfo-dev 

# install llvm 10.0.0
wget -O llvm.tar.xz https://github.com/llvm/llvm-project/releases/download/llvmorg-10.0.0/clang+llvm-10.0.0-x86_64-linux-gnu-ubuntu-18.04.tar.xz

tar xf llvm.tar.xz

cp -r clang+llvm-10.0.0-x86_64-linux-gnu-ubuntu-18.04/* /usr/local/

rm -rf clang+llvm-10.0.0-x86_64-linux-gnu-ubuntu-18.04

rm -f llvm.tar.xz

modify directory in ccache.conf and copy to /etc/ccache.conf

use either [debug/release] in following

python3 pochivm-build cmake debug

python3 pochivm-build make debug

./main
