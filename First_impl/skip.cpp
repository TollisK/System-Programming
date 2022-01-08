#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <ctime>
#include <cmath>
#include "skip.h"

struct vaclist* insertafter(struct vaclist** head_ref,Person* person_data,struct vaclist *vacdown){
    struct vaclist* new_Node = new vaclist; //Dhmioyrgia neoy komvoy
    new_Node->person = person_data;
    new_Node->next = NULL;
    new_Node->down = vacdown;
    struct vaclist* current;
    //An o komvos einai o arxikos h einai megalytero toy arxikoy
    if (*head_ref == NULL || (*head_ref)->person->citizenID > new_Node->person->citizenID) {
        new_Node->next = *head_ref;
        *head_ref = new_Node;
    }
    else{
        current = *head_ref;
        new_Node->next = current->next; 
        current->next = new_Node;
    }
    return new_Node;
}
struct vaclist *insert_rec(struct vaclist **head,struct vaclist** prevdown,Person *person,int lvl){
    struct vaclist *downtemp;
    if(lvl == -1){
        return NULL;
    }
    if(*head == NULL){
        downtemp = insert_rec(&prevdown[lvl-1],prevdown,person,lvl-1);
        return insertafter(&prevdown[lvl],person,downtemp);
    }
    struct vaclist *current;
    struct vaclist *prev = *head;
    current = *head;
    while(current!=NULL){//Gia kathe komvo ths listas toy epipedoy
        if(current->person->citizenID > person->citizenID){ //An einai megalytero toy id
            if(current == *head){ //An einai o arxikos komvos tote anadromika trexoyme thn synarthsh apo thn arxh toy proigoymenoy epipedoy
                downtemp = insert_rec(&prevdown[lvl-1],prevdown,person,lvl-1);
                return insertafter(&prevdown[lvl],person,downtemp);
            }
            else{
                downtemp = insert_rec(&(prev->down),&prevdown[lvl-1],person,lvl-1);
                return insertafter(&prev,person,downtemp);
            }
        }
        else{
            prev = current;
            current = current->next;
        }
    }
    downtemp = insert_rec(&(prev->down),&prevdown[lvl-1],person,lvl-1);
    return insertafter(&prev,person,downtemp);
}

void insertskiplist(struct NodeSkip **skip,Person *person,int maxlvl,bool vacbool){
    // cout<<"lulu"<<endl;
    int lvl=0;
    struct vaclist *down = NULL;
    while(rand()%2==0&&lvl<maxlvl){
        lvl++;
    }
    if(vacbool){
        insert_rec(&((*skip)->vachead[lvl]),(*skip)->vachead,person,lvl);
    }
    else{//An den einai vaccinated
        insert_rec(&((*skip)->nonvachead[lvl]),(*skip)->nonvachead,person,lvl);
    }
}

//Vriskei to stoixeio me pedio kleidi to citizen id sthn skip list
Person *searchnodeskip(struct vaclist *head,struct vaclist** prevdown, int id,int lvl){
    if(head == NULL){ //An einai null tote den yparxei
        return NULL;
    }
    if(prevdown[lvl]==NULL){ //An sto epipedo ayto den yparxoyn stoixeia tote return false
        return NULL;
    }
    struct vaclist *current;
    struct vaclist *prev = head;
    current = head;
    while(current!=NULL){ //Gia kathe komvo ths listas toy epipedoy
        if(current->person->citizenID == id){ //An vrethei tote epistrefei true
            return current->person;
        }
        else if(current->person->citizenID > id){ //An einai megalytero toy id
            if(current == head&&lvl>0){ //An einai o arxikos komvos tote anadromika trexoyme thn synarthsh apo thn arxh toy proigoymenoy epipedoy
                return searchnodeskip(prevdown[lvl-1],prevdown,id,lvl-1);
            }
            if(lvl>0){ //An yparxei pio katw epipedo tote trexoyme anadromika thn synarthsh kai arxizei apo ton katw komvo toy proigoymenoy
                return searchnodeskip(prev->down,prevdown,id,lvl-1);
            }
            else{ //An einai to arxiko epipedo shmainei oti den yparxei
                return NULL;
            }
        }
        else{
            prev = current;
            current = current->next;
        }
    }
    if(lvl>0){ //An yparxei pio katw epipedo tote trexoyme anadromika thn synarthsh kai arxizei apo ton katw komvo toy proigoymenoy
        return searchnodeskip(prev->down,prevdown,id,lvl-1);
    }//An einai to teleytaio epipedo shmainei oti den yparxei to stoixeio poy psaxnei
    return NULL;
}
//Synarthsh gia thn eyresh stoixeioy sthn skip list
Person *searchlistskip(struct vaclist **head,int id,int maxlvl){
    for(int i=maxlvl-1;i>=0;i--){ //Gia kathe epipedo
        if(head[i]==NULL){ //An den yparxei tote proxwraei sto katw epipedo
            continue;
        }
        return searchnodeskip(head[i],head,id,i);
    }
    return NULL;
}

//Synarthsh eisagwghs stoixeioy sthn lista twn skip list gia kathe io
struct NodeSkip* insertskip(struct NodeSkip **head,std::string virus,int maxlvl){
    struct NodeSkip* new_node = new NodeSkip(); //Dhmiorygia komvoy
    //Arxikopoihsh twn pinakwn
    new_node->vachead = new vaclist*[maxlvl]; 
    for(int i=0;i<maxlvl;i++){
        new_node->vachead[i] = NULL;
    }
    new_node->nonvachead = new vaclist*[maxlvl];
    for(int i=0;i<maxlvl;i++){
        new_node->nonvachead[i] = NULL;
    }
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

void destroyskip(struct NodeSkip** head_ref,int maxlevel){
    struct NodeSkip* current = *head_ref;
    struct NodeSkip* old = NULL;
    while (current != NULL) 
    {
        old = current;
        current = current->next;
        for(int i=maxlevel-1;i>=0;i--){
            destroyskiplist(&(old->vachead[i]));
            destroyskiplist(&(old->nonvachead[i]));
        }
        delete [] old->vachead;
        delete [] old->nonvachead;
        delete old;
    }
    *head_ref = NULL;
}
//Diatrexei thn lista kai diagrafei toys komvoys ths
void destroyskiplist(struct vaclist** head_ref){
    struct vaclist* current = *head_ref;
    struct vaclist* old = NULL;
    while (current != NULL)
    {
        old = current;
        current = current->next;
        delete old;
    }
    *head_ref = NULL;
}

//Diagrafh komvoy apo thn skip list
void deleteNode(struct vaclist** head_ref,int key){
    struct vaclist* temp = *head_ref;
    struct vaclist* prev = NULL;
    //An einai o arxikos komvos
    if (temp != NULL && temp->person->citizenID == key){
        *head_ref = temp->next; 
        delete temp;           
        return;
    }
    //Psaxnei ton komvo poy prepei na diagraftei kai apothhkeyei ton proigoymeno
    else{
        while (temp != NULL && temp->person->citizenID != key){
            prev = temp;
            temp = temp->next;
        }
        if (temp == NULL){
            return;
        }
        prev->next = temp->next;
        delete temp;
    }
}