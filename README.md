
Template for a [PochiVM](https://sillycross.github.io/PochiVM) application

to build and run

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




python3 pochivm-build cmake debug

python3 pochivm-build make debug

./main
