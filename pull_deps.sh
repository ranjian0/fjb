P=$(pwd)
echo "Pulling dependencies..."
git submodule init
git submodule update
git submodule sync
git submodule update --init --recursive
git clone https://github.com/sebbekarlsson/gpp.git --recurse-submodules

echo "Building dependency: gpp"
cd gpp;
git submodule init; git submodule update; git submodule sync;
mkdir build
cd build
cmake ..
make
cd $P
cp gpp/build/gpp_e ./gpp.out
pwd
cp gpp/build/gpp_e ./tests/gpp.out
cd $P
rm -rf ./gpp
