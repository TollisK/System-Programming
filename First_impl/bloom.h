#ifndef BLOOM_H
#define BLOOM_H
#include <string>

//Komvos gia thn lista apo bloom gia kathe virus
struct Nodebloom{
    std::string virusname; //O kathe ios toy antistoixei ena bloom
    //Gia ton pinaka toy bloom xrhsimopoioyme char(opoy char = 1 byte)
    char *bloomarray; //Ylopoihsh ths domhs dedomenwn me lista apo bits
    struct Nodebloom *next;
};
void pushbloom(struct Nodebloom** head, std::string data,int size);
void destroybloom(struct Nodebloom** head_ref);
struct Nodebloom* searchbloom(struct Nodebloom* head, std::string data);
void setbit(char *bit_array, int k);
int testbit(char *bit_array,int k);
unsigned long sdbm(std::string str);
unsigned long djb2(std::string str);
unsigned long hash_i(std::string str, unsigned int i);
void insertbloom(char *A,int K,int bloomsize,std::string str);
bool findbloom(char *A,int K,int bloomsize,std::string str);

#endif