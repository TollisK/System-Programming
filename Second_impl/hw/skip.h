#ifndef SKIP_H
#define SKIP_H
#include <string>
#include "person.h"
//Komvos gia thn skip list
struct NodeSkip{
    std::string virusname; //Se kathe io toy antistoixei enan komvo gia thn skip list
    struct vaclist* vachead; //Pinakas apo listes gia thn ylopoihsh toy vaccinated skip list
    struct vaclist* nonvachead; //Pinakas apo listes gia thn ylopoihsh toy nonvaccinated skip list
    struct NodeSkip *next;
};

//Komvos gia thn lista toy skip list
struct vaclist{
    Person *person; //Exei ws dedomena enan deikth se anthrwpo
    struct vaclist* next;
};
struct vaclist* insertafter(struct vaclist** head_ref,Person* person_data);
void insertskiplist(struct NodeSkip **skip,Person *person,bool vacbool);
Person *searchlistskip(struct vaclist *head,int id);
struct NodeSkip* insertskip(struct NodeSkip **head,std::string virus);
struct NodeSkip* searchskip(struct NodeSkip* head, std::string virus);
void destroyskip(struct NodeSkip** head_ref);
#endif