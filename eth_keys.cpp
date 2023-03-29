#include "bf.hpp"

// -------------------- GEN KEY PAIR --------------------

void    c_bf::gen_eth_key_pair(unsigned char *priv, unsigned char *address, 
                               std::vector<unsigned char> & e) { 
  secp256k1_pubkey  pubkey;
  unsigned char     public_key[65];
  size_t            len = 65;

  if (p_bf->cpt == 4242) {
    memset(priv, 1, PRIVATE_KEY_SIZE);
  }

  secp256k1_ec_pubkey_create(p_eth_ctx, &pubkey, priv);
  // print_key(priv, crypto_sign_SEEDBYTES, "start private2");
  // print_key(pubkey.data, 64, "secp256k1_ec_pubkey_create");
  secp256k1_ec_pubkey_serialize(p_eth_ctx, public_key, &len, &pubkey, SECP256K1_EC_UNCOMPRESSED);
  // print_key(public_key, len, "secp256k1_ec_pubkey_serialize");

  keccak_256(address, PRIVATE_KEY_SIZE, public_key + 1, crypto_sign_SECRETKEYBYTES);
  // print_key(address, PRIVATE_KEY_SIZE, "address");
  // print_key(address + 12, PRIVATE_KEY_SIZE - 12, "address");

  // address bin to vector<unsigned char> for map searching
  unsigned char *tmp_bin = address + 12;
  e.clear();
  for (int i = 0; i < ADDRESS_SIZE; i++) {
    e.push_back(tmp_bin[i]);
  }
}
