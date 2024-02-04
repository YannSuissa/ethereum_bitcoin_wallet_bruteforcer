

#include "bf.hpp"

using namespace std;

//https://github.com/coruus/keccak-tiny
//https://github.com/imitko/ethereum-address-generator
//https://www.rfctools.com/ethereum-address-test-tool/
//https://keys.lol/
//https://privatekeyfinder.io/bitcoin-puzzle/
//https://privatekeys.pw/puzzles/bitcoin-puzzle-tx

// install clang 14 
//  wget https://apt.llvm.org/llvm.sh
//  chmod +x llvm.sh
//  sudo ./llvm.sh 14
// set as default
//  update-alternatives --install /usr/bin/c++ c++ /usr/bin/clang++-14 60

// -------------------- GLOBALS --------------------

c_bf *p_bf = NULL;

// -------------------- UTILITIES --------------------

// print a binary key to string hex
std::string    c_bf::bin_to_str(unsigned char *key, int len) {
  std::string e;

  for(int i=0; i < len; ++i) {
    std::ostringstream ss;
    ss << std::setfill('0') << std::setw(2) << std::hex << (int)key[i];
    e += ss.str();
  }
  return e;
}

void          c_bf::print_key(unsigned char *key, int len, std::string label) {
  printf("%30s : %s\n", label.c_str(), bin_to_str(key, len).c_str());
  // printf("%30s : 0x", label.c_str());
  // for(int i=0; i < len; ++i) {
  //   printf("%02x", key[i]);
  // }
  // printf("\n");
}

// transform str hex to binary data
void skstr_to_sk(const unsigned char *sk_str, unsigned char *sk, int len)
{
  const char unsigned *sk_pos = sk_str;
  int i;

  for (i = 0; i < len; i++) {
      sscanf((const char *)sk_pos, "%2hhx", &sk[i]);
      sk_pos += 2;
  }
}


// -------------------- FILE SAVING (RESULT) --------------------

void save_result(unsigned char *priv, unsigned char *pub, int type) {
  fstream FileName;
  FileName.open("win.txt", std::fstream::out | std::fstream::app);                
  if (!FileName) {                            
      cout<<" Error while creating the file ";          
  }
  else {
      // cout<<"File created and data got written to file";    
      // FileName<<"This is a blog posted on Great Learning";  

      if (type == 1)
        FileName << "ETH Priv : ";  
      if (type == 2)
        FileName << "BTC Priv : ";  

      FileName << p_bf->bin_to_str(priv, PRIVATE_KEY_SIZE);
      FileName << "\nPub : ";  

      if (type == 1)
        FileName << p_bf->bin_to_str(pub, ADDRESS_SIZE);
      if (type == 2)
        FileName << pub;

      FileName << "\n";  
      FileName.close();                   
  }
}

// -------------------- CSV PARSE & ADDRESS LIST MANAGMENT --------------------

// CSV open
unsigned char* csv_getfield(char* line, int num)
{
  char* tok;
  for (tok = strtok(line, ";");
          tok && *tok;
          tok = strtok(NULL, ";\n\r"))
  {
    if (!--num)
      return (unsigned char *)tok;
  }
  return NULL;
}

void add_in_vector(unsigned char *hexstr, int type) {
  // printf("add_in_vector %s\n", hexstr);

  std::vector<unsigned char>  v;
  unsigned char *tmp_bin = new unsigned char[ADDRESS_SIZE + 1];
  int tmp_len = strlen((const char *)hexstr) / 2;
  // printf("add_in_vector1\n");
  skstr_to_sk(hexstr, tmp_bin, tmp_len);
  // printf("add_in_vector2, %d\n", tmp_len);
  for (int i = 0; i < tmp_len; i++) {
    v.push_back(tmp_bin[i]);
  }
  if (type == 1)
    p_bf->look_array_eth[v] = 1;
  if (type == 3)
    p_bf->look_array_btc_p[v] = 1;
  // printf("add_in_vector4\n");

}

void open_src_csv(const char *csv, int type)
{
  FILE* stream = fopen(csv, "r");
  if (!stream)
    return;
  char line[1024];
  while (fgets(line, 1024, stream))
  {
    char *tmp = strdup(line);
    // printf("key : %s\n", line);

    std::string a = (char *)csv_getfield(tmp, 1);
    if (a[a.size() - 1] == '\n')
      a.resize(a.size() - 1);
    // printf("key : %s\n", a.c_str());
    free(tmp);

    if (type == 2)
      p_bf->look_array_btc[a] = 1;
    if (type == 1)
      add_in_vector((unsigned char *)a.c_str(), type);
    if (type == 3)
      add_in_vector((unsigned char *)a.c_str(), type);

  }
  #if 1
  // for (int i = 0; i < p_bf->look_array.size(); i++) {
    // print_key(p_bf->look_array[i], ADDRESS_SIZE, "parse");
  // }
  #endif
}

void                load_address_db(const char *csv, int type) {
  
  printf(" - Loading CSV %s... ", csv);
  fflush(stdout);
  open_src_csv(csv, type);
  if (type == 1)
    printf("numbers %d\n", (int)p_bf->look_array_eth.size());
  if (type == 2)
    printf("numbers %d\n", (int)p_bf->look_array_btc.size());
  if (type == 3)
    printf("numbers %d\n", (int)p_bf->look_array_btc_p.size());
}





// -------------------- COMPUTE --------------------
#define USE_ETH      0
#define USE_BTC      0
#define USE_BTC_P    1

void                compute() {
  unsigned char     priv_b[crypto_sign_SEEDBYTES + 32];
  unsigned char     priv_e[crypto_sign_SEEDBYTES + 32];

  unsigned char     address_e[PRIVATE_KEY_SIZE + 32];
  unsigned char     address_b[PRIVATE_KEY_SIZE + 32];
  unsigned char     address_bp[PRIVATE_KEY_SIZE + 32];

  memset(address_b, 0, sizeof(address_b));
  memset(address_bp, 0, sizeof(address_bp));

  clock_t begin = clock();
  std::vector<unsigned char> e_e;
  std::vector<unsigned char> e_bp;
  std::string t1;
  e_bp.resize(20);

  if (sodium_init() < 0) {
    fprintf(stderr, "Erreur : libsodium n'a pas pu être initialisé\n");
    exit(0);
  }


  while (1) {
    // gen a key that increment to 0xffffff
    if (!(p_bf->cpt % 0xffffff)) {
      randombytes_buf(priv_e, PRIVATE_KEY_SIZE);
      priv_e[PRIVATE_KEY_SIZE - 3] = 0;
      priv_e[PRIVATE_KEY_SIZE - 2] = 0;
      priv_e[PRIVATE_KEY_SIZE - 1] = 0;      
    }
    else {
      if (priv_e[PRIVATE_KEY_SIZE - 1] == 0xff) {
        priv_e[PRIVATE_KEY_SIZE - 2]++;
        if (priv_e[PRIVATE_KEY_SIZE - 2] == 0xff)
          priv_e[PRIVATE_KEY_SIZE - 3]++;
      }
      priv_e[PRIVATE_KEY_SIZE - 1]++;
    }
    memcpy(priv_b, priv_e, PRIVATE_KEY_SIZE); 

#if USE_ETH
    // search in map for eth address
    if (p_bf->look_array_eth.size()) {
      if (p_bf->look_array_eth.find(e_e) != p_bf->look_array_eth.end()) {
        printf("\n____________________________________________________________________________________________________\n");
        p_bf->print_key(priv_e, crypto_sign_SEEDBYTES, "ETH finish private");
        p_bf->print_key(address_e + 12, ADDRESS_SIZE, "ETH finish address");
        save_result(priv_e, address_e + 12, 1);
        exit(0);
      }
    }

    // search for pattern mode (eth)
    if (p_bf->pattern_mode) {
      if (!memcmp(address_e + 12, p_bf->pattern_mode, p_bf->pattern_len)) {
        printf("\n____________________________________________________________________________________________________\n");
        p_bf->print_key(priv_e, crypto_sign_SEEDBYTES, "ETH_P finish pattern private");
        p_bf->print_key(address_e + 12, ADDRESS_SIZE, "ETH_P finish pattern address");
        save_result(priv_e, address_e + 12, 1);
        exit(0);
      }
    }
#endif


#if USE_ETH
    if (p_bf->look_array_eth.size())
      p_bf->gen_eth_key_pair(priv_e, address_e, e_e);
#endif
#if USE_BTC
    if (p_bf->look_array_btc.size())    
      p_bf->gen_btc_key_pair(priv_b, address_b);
#endif
#if USE_BTC_P
    // if (p_bf->look_array_btc_p.size())    
    p_bf->gen_btc_pub(priv_b, address_bp, e_bp);
#endif
    // print_key(priv, crypto_sign_SEEDBYTES, "start private");
    // print_key(address + 12, ADDRESS_SIZE, "start pub");
    // printf("%s\n", address);
    // printf("%30s : %s\n", "finish address pub", address);

#if USE_BTC
    // search in map for btc address
    if (p_bf->look_array_btc.size()) {
      t1 = (char *)address_b;
      // printf("find : %s\n", t2.c_str());
      // printf("array : %s\n", p_bf->look_array_btc.begin()->first.c_str());

      if (p_bf->look_array_btc.find(t1) != p_bf->look_array_btc.end()) {
        printf("\n____________________________________________________________________________________________________\n");
        p_bf->print_key(priv_b, crypto_sign_SEEDBYTES, "BTC finish private");
        printf("%30s : %s\n", "BTC finish address", address_b);

        exit(0);
      }
    }
#endif

#if USE_BTC_P
    // search in map for btc public 160
    if (p_bf->look_array_btc_p.find(e_bp) != p_bf->look_array_btc_p.end()) {
      printf("\n____________________________________________________________________________________________________\n");
      p_bf->gen_btc_key_pair(priv_b, address_b);
      p_bf->print_key(priv_b, crypto_sign_SEEDBYTES, "BTC finish private");
      printf("%30s : %s\n", "BTC finish address", address_b);

      save_result(priv_b, address_b, 2);
      exit(0);
    }
#endif

    // progress printing 
    if (!(p_bf->cpt % 1000000) || p_bf->cpt == 200000) {
      clock_t curr = clock();
      double time_spent = (double)(curr - begin) / CLOCKS_PER_SEC;
      printf("\nelapsed %.2fs - speed %.2f/sec - done %lld\n", 
                time_spent, p_bf->cpt / time_spent, p_bf->cpt);
      p_bf->gen_btc_key_pair(priv_b, address_b);
#if USE_ETH
      p_bf->print_key(priv_e, crypto_sign_SEEDBYTES, "ETH private prog");
      p_bf->print_key(address_e + 12, ADDRESS_SIZE, "ETH address prog");
#endif
      p_bf->print_key(priv_b, crypto_sign_SEEDBYTES, "BTC private prog");
      printf("%30s : %s\n", "BTC address prog", address_b);

      // fflush(stdout);
    }
    else
      if (!(p_bf->cpt % 10000)) {
        printf(".");
        fflush(stdout);
      }


    // global inc
    p_bf->cpt++;

    // if (p_bf->cpt > 100000)
    //   break;
    // if (p_bf->cpt > 0xc936)
    //   break;
  }

  randombytes_close();

}

// main 

int main(int argc, char **argv) {
  c_bf bf;
  bool noeth = false;
  bool nobtc = false;
  bool fake_success = false;

  p_bf = &bf;
  

  for(;;)
  {
    switch(getopt(argc, argv, "fp:hc:EB")) // note the colon (:) to indicate that 'b' has a parameter and is not a switch
    {
      case 'f':
        fake_success = true;
        continue;

      case 'p':
        // printf("parameter 'b' specified with the value %s\n", optarg);
        p_bf->pattern_mode = new unsigned char[ADDRESS_SIZE + 1];
        p_bf->pattern_len = strlen((const char *)optarg);
        if (p_bf->pattern_len % 2) {
          printf("Error must be a multiple of 2\n");
          exit(-1);
        }
        p_bf->pattern_len /= 2;
        skstr_to_sk((const unsigned char *)optarg, p_bf->pattern_mode, p_bf->pattern_len);
        continue;
      case 'E':
        noeth = true;
        continue;     
      case 'B':
        nobtc = true;
        continue;
      case 'c':
        p_bf->p_complexity = atoi((const char *)optarg);
        continue;
      case '?':
      case 'h':
      default :
        printf("Usage : ./compute [-f]\n");
        printf("       -f : for fake successful found for addr 0x1a64... for private 0x0101010..\n");
        exit(0);
        break;

      case -1:
        break;
    }

    break;
  }

  if (!p_bf->pattern_mode) {
    if (fake_success) {
      if (noeth == false)
        load_address_db("addresses_sample_eth.csv", 1);
      if (nobtc == false)
        // load_address_db("addresses_sample_btc.csv", 2);
        load_address_db("addresses_sample_btc_pub.csv", 3);

    }
    else {
      if (noeth == false)
        load_address_db("addresses_eth.csv", 1);
      if (nobtc == false) {
        // load_address_db("addresses_btc.csv", 2);
        load_address_db("addresses_btc_pub.csv", 3);
      }
    }
  }

  compute();

  return 0;
}

//                                   15141312 11 10  9  8 7 6 5 4 3 2 1
// 000000000000000000000000000000000000000000 00 00 02 0000000000000000   #66
// 000000000000000000000000000000000000000000 00 00 03 ffffffffffffffff   
// 000000000000000000000000000000000000000000 00 00 04 0000000000000000   #67
// 000000000000000000000000000000000000000000 00 00 07 ffffffffffffffff
// 000000000000000000000000000000000000000000 00 00 08 0000000000000000   #68
// 000000000000000000000000000000000000000000 00 00 0f ffffffffffffffff
// 000000000000000000000000000000000000000000 00 00 10 0000000000000000   #69
// 000000000000000000000000000000000000000000 00 00 1f ffffffffffffffff
// 000000000000000000000000000000000000000000 00 00 40 0000000000000000   #71
// 000000000000000000000000000000000000000000 00 00 7f ffffffffffffffff
// 000000000000000000000000000000000000000000 00 00 80 0000000000000000   #72 
// 000000000000000000000000000000000000000000 00 00 ff ffffffffffffffff
    
    
// 000000000000000000000000000000000000000000 00 00 0c acc6c047c230805c
// 000000000000000000000000000000000000000000 00 00 6d eee5fb060947d057
// 000000000000000000000000000000000000000000 00 00 70 6d473bea760e92da
// 000000000000000000000000000000000000000000 00 00 f1 3e22d0ccc66c180f
// 000000000000000000000000000000000000000000 00 00 8e c4e496038307bb74
// 000000000000000000000000000000000000000000 00 00 d3 ca294e9537d6f73b

