#pragma once

#include "keccak-tiny.h"
#include <secp256k1.h>
#include <sodium.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "keccak-tiny.h"
#include <secp256k1.h>
// #include <secp256k1_extrakeys.h>
#include <time.h>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <fstream> 
#include <sstream>
#include <iomanip>
// #include <format>


#define PRIVATE_KEY_SIZE 32
#define ADDRESS_SIZE 20

class c_bf {
  
  public:
    std::map<std::vector<unsigned char>, int> look_array_eth;
    std::map<std::string, int>                look_array_btc;
    long long int                             cpt = 0;
    unsigned char *                           pattern_mode = NULL;
    int                                       pattern_len = 0;
    secp256k1_context *                       p_eth_ctx; 
    secp256k1_context *                       p_btc_ctx; 
    int                                       p_complexity = 0;

   
    void            gen_eth_key_pair(unsigned char *priv, unsigned char *address, 
                                      std::vector<unsigned char> & e);
    void            gen_btc_key_pair(unsigned char *priv, unsigned char *address);


    c_bf() { 
      //p_eth_ctx = secp256k1_context_create(SECP256K1_CONTEXT_NONE);
      p_eth_ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
      p_btc_ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);
    }
    ~c_bf() {
      secp256k1_context_destroy(p_eth_ctx);
      secp256k1_context_destroy(p_btc_ctx);
    }
};


extern c_bf *p_bf;