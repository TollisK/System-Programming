#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <ctime>
#include <cmath>
#include "country.h"
using namespace std;

//Synarthsh gia thn eyresh ypar3hs xwras sthn lista
struct NodeCountry* searchcountry(struct NodeCountry* head, string data){
    struct NodeCountry* current = head;
    while (current != NULL)
    {
        if (current->name == data)
            return current;
        current = current->next;
    }
    return NULL;
}
//Synarthsh eisagwghs stoixeioy sthn listas xwrwn
void appendcountry(struct NodeCountry** head_ref, string new_data){
    struct NodeCountry* new_node = new NodeCountry();  
    struct NodeCountry *last = *head_ref;  
    new_node->name = new_data;   
    new_node->next = NULL;   
    if (*head_ref == NULL)   
    {   
        *head_ref = new_node;   
        return;   
    }   
    while (last->next != NULL)   
        last = last->next;   
    last->next = new_node;   
    return;   
}

void destroycountrylist(struct NodeCountry** head_ref){
    struct NodeCountry* current = *head_ref;
    struct NodeCountry* old = NULL;
    while (current != NULL) 
    {
        old = current;
        current = current->next;
        delete old;
    }
    *head_ref = NULL;
}
