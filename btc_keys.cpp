#include "bf.hpp"
#include <openssl/sha.h>
#include <openssl/ripemd.h>


// -------------------- GEN KEY PAIR --------------------
typedef unsigned char byte;

/* See https://en.wikipedia.org/wiki/Positional_notation#Base_conversion */
char *base58(byte *s, int s_size, char *out, int out_size)
{
  static const char *tmpl = "123456789"
                            "ABCDEFGHJKLMNPQRSTUVWXYZ"
                            "abcdefghijkmnopqrstuvwxyz";

  int c, i, n;

  out[n = out_size] = 0;
  while (n--)
  {
    for (c = i = 0; i < s_size; i++)
    {
      c = c * 256 + s[i];
      s[i] = c / 58;
      c %= 58;
    }
    out[n] = tmpl[c];
  }

  return out;
}

void    c_bf::gen_btc_key_pair(unsigned char *seckey, unsigned char *address) { 

  secp256k1_pubkey  pubkey;
  unsigned char     public_key[65];
  size_t            len = 65;
  byte              s[65];
  byte              rmd[5 + RIPEMD160_DIGEST_LENGTH];
  int               j;

  if (p_complexity) {
    memset(seckey, 0, 32 - p_complexity);
    if (p_complexity == 1 && !seckey[31])   // avoid all 0
      seckey[31] = 1;
  }

  // for (int i = 0; i < 32; i++) {
  //   printf("%02x", seckey[i]);
  // }
  // printf("\n");

  secp256k1_ec_pubkey_create(p_btc_ctx, &pubkey, seckey);

  // for (int i = 0; i < 32; i++) {
  //   printf("%02x", seckey[i]);
  // }
  // printf("\n");


  // print_key(priv, crypto_sign_SEEDBYTES, "start private2");
  // print_key(pubkey.data, 64, "secp256k1_ec_pubkey_create");
  secp256k1_ec_pubkey_serialize(p_btc_ctx, public_key, &len, &pubkey, 
    // SECP256K1_EC_UNCOMPRESSED
    SECP256K1_EC_COMPRESSED
    );
  // print_key(public_key, len, "secp256k1_ec_pubkey_serialize");


  for (j = 0; j < 65; j++) {
    s[j] = public_key[j];
    // printf("%02x", public_key[j]);
  }
  // printf("\n");


  /* Set 0x00 byte for main net */
  rmd[0] = 0;
  // RIPEMD160(SHA256(s, 65, 0), SHA256_DIGEST_LENGTH, rmd + 1);
  RIPEMD160(SHA256(s, 33, 0), SHA256_DIGEST_LENGTH, rmd + 1);

  memcpy(rmd + 21, SHA256(SHA256(rmd, 21, 0), SHA256_DIGEST_LENGTH, 0), 4);

  // for (j = 0; j < 25; j++) {
  //   printf("%02x", rmd[j]);
  // }
  // printf("\n");

  // char addr[34];
  base58(rmd, 25, (char *)address, 34);

  /* Count the number of 1s at the beginning of the address */
  int n = 0;
  for (n = 0; address[n] == '1'; n++)
    ;

  /* Do we need to remove any 1s? */
  if (n > 1)
  {
    /* Move the memory so that the address begins at the final 1 */
    memmove(address, address + (n - 1), 34 - (n - 1));

    /* Force the address to finish at the correct length */
    // pubaddress[34 - (n - 1)] = '\0';
    // address[34 - (n - 1)] = '\0';
  }

  // printf("Address: %s\n", address);
}
