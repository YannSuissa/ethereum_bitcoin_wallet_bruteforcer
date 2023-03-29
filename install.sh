cd ../secp256k1
apt -y update ; apt -y install make libssl-dev clang automake libsodium-dev
make distclean
./autogen.sh
./configure
make -j 8
make install
ldconfig
cd -
make
