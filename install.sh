cd ../secp256k1
apt update ; apt install make libssl-dev clang automake libsodium-dev
make distclean
./configure
make -j 8
make install
cd -
make
