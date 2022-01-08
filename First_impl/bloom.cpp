#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <ctime>
#include <cmath>
#include "bloom.h"
using namespace std;

//Synarthsh listas poy eisagei enan neo komvo bloom(otan briskoyme kainoyrgio io)
void pushbloom(struct Nodebloom** head, string data,int size){
    struct Nodebloom* newNode = new Nodebloom; //Dhmioyrgia neoy komvoy
    newNode->virusname = data; //Arxikopoihsh dedomenwn
    newNode->bloomarray = new char[size]; //Dhmioyrgia toy pinaka me megethos oso to orisma ths main
    //To topothetei sthn arxh ths listas
    newNode->next = (*head);
    (*head) = newNode;
}

void destroybloom(struct Nodebloom** head_ref){
    struct Nodebloom* current = *head_ref;
    struct Nodebloom* old = NULL;
    while (current != NULL)
    {
        old = current;
        current = current->next;
        delete [] old->bloomarray;
        delete old;
    }
    *head_ref = NULL;
}

//Epistrefei ton komvo listas bloom symfwna me to onoma toy ioy
struct Nodebloom* searchbloom(struct Nodebloom* head, string data)  {
    struct Nodebloom* current = head;
    while (current != NULL)//Gia kathe komvo ths listas
    {
        if (current->virusname == data) //An vrei ton komvo me dedomena to onoma toy iou
            return current; //Ton epistrefei
        current = current->next;
    }
    return NULL;
}
//Synarthsh gia thn xrhsh toy pinaka apo bits ths bloom
//Kanei to kth bit 1
void setbit(char *bit_array, int k){
    int i = k/8;            // Vriskei to shmeio toy pinaka poy brisketai to k
    int j = k%8;          // Sto shmeio ayto vriskei to bit poy psaxnoyme
    unsigned int bit_one = 1;
    bit_one = bit_one << j;
    bit_array[i] = bit_array[i] | bit_one;      // Kanei to kth bit 1
}

//Epistrefei to kth bit
int testbit(char *bit_array,int k){
    int i = k/8; // Vriskei to shmeio toy pinaka poy brisketai to k
    int j = k%8; // Sto shmeio ayto vriskei to bit poy psaxnoyme
    unsigned int bit_one = 1;
    bit_one = bit_one << j;     // Xrhsimopoiei shift gia na topothethsei to 1 sto shmeio toy k
    if ( bit_array[i] & bit_one ){      // Vlepei to kth bit an 1 h 0
        return 1;
    }
    else{
        return 0;
    }
}

//Etoima hash functions me thn allagh toy char* se string
unsigned long sdbm(string str) {
	unsigned long hash = 0;
	int c;
    for (int i=0;i<str.length()+1;i++){
        c = str[i];
        hash = c + (hash << 6) + (hash << 16) - hash;
    }

	return hash;
}

unsigned long djb2(string str) {
	unsigned long hash = 5381;
	int c;
    for (int i=0;i<str.length()+1;i++){
        c = str[i];
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
	return hash;
}

unsigned long hash_i(string str, unsigned int i) {
	return djb2(str) + i*sdbm(str) + i*i;
}

//Synarthsh gia thn eisagwgh stoixeioy ston pinaka ths bloom
void insertbloom(char *A,int K,int bloomsize,string str){
    for (int i =  0; i < K ; i++) {//Gia kathe hash function
        setbit(A,hash_i(str,i)%bloomsize);//Kane to bit 1
	}
}

//Synarthsh gia thn eyresh an enas anthrwpos emvoliasthke.
bool findbloom(char *A,int K,int bloomsize,string str){
    for (int i = 0;i<K;i++){ //Gia kathe hash function
        if(testbit(A,hash_i(str,i)%bloomsize)==0){ //An estw kai ena bit einai mhden tote epistrefei false
            return false;
        }
    }
    return true; //An ola ta bits htan mhden tote epistrefei oti mallon emvoliasthke
}