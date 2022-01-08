#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <ctime>
#include <cmath>
#include "skip.h"

struct vaclist* insertafter(struct vaclist** head_ref, Person* person_data) 
{
    struct vaclist* new_node = new struct vaclist (); 
    new_node->person = person_data;
    new_node->next = (*head_ref); 
    (*head_ref) = new_node;

    return new_node;
}

void insertskiplist(struct NodeSkip **skip,Person *person,bool vacbool){
    if(vacbool){
        insertafter(&(*skip)->vachead,person);
    }
    else{//An den einai vaccinated
        insertafter(&(*skip)->nonvachead,person);
    }
}

//Synarthsh gia thn eyresh stoixeioy sthn skip list
Person *searchlistskip(struct vaclist *head,int id){
    struct vaclist* current = head;
    while (current != NULL)  
    {
        if (current->person->citizenID == id)
            return current->person;
        current = current->next;  
    }
    return NULL;
}

//Synarthsh eisagwghs stoixeioy sthn lista twn skip list gia kathe io
struct NodeSkip* insertskip(struct NodeSkip **head,std::string virus){
    struct NodeSkip* new_node = new NodeSkip(); //Dhmiorygia komvoy
    //Arxikopoihsh
    new_node->vachead = new vaclist(); 
    new_node->nonvachead = new vaclist();

    new_node->vachead = NULL;
    new_node->nonvachead = NULL;

    struct NodeSkip *last = *head;
    //Dedomena
    new_node->virusname = virus;
    new_node->next = NULL;
    //An einai o prwtos komvos ths listas
    if (*head == NULL)
    {
        *head = new_node;
        return new_node;
    }
    //Topothetei ton komvo ston telos ths listas
    while (last->next != NULL)
        last = last->next;
    last->next = new_node;
    return new_node;
}

//Epistrefei ton komvo ths listas skip list me pedio kleidi ton io
struct NodeSkip* searchskip(struct NodeSkip* head, std::string virus){
    struct NodeSkip* current = head; 
    while (current != NULL)  
    {  
        if (current->virusname == virus)  
            return current;
        current = current->next;  
    }  
    return NULL;  
}

void destroyskip(struct NodeSkip** head_ref){
    struct NodeSkip* current = *head_ref;
    struct NodeSkip* old = NULL;
    while (current != NULL) 
    {
        old = current;
        current = current->next;
        delete old->vachead;
        delete old->nonvachead;
        delete old;
    }
    *head_ref = NULL;
}
