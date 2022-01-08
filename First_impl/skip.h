#ifndef SKIP_H
#define SKIP_H
#include <string>
#include "person.h"
//Komvos gia thn skip list
struct NodeSkip{
    std::string virusname; //Se kathe io toy antistoixei enan komvo gia thn skip list
    struct vaclist** vachead; //Pinakas apo listes gia thn ylopoihsh toy vaccinated skip list
    struct vaclist** nonvachead; //Pinakas apo listes gia thn ylopoihsh toy nonvaccinated skip list
    struct NodeSkip *next;
};

//Komvos gia thn lista toy skip list
struct vaclist{
    Person *person; //Exei ws dedomena enan deikth se anthrwpo
    struct vaclist* next;
    struct vaclist* down; //Deikths se komvo poy vrisketai ena epipedo apo katw
};
void destroyskiplist(struct vaclist** head_ref);
struct vaclist* insertafter(struct vaclist** head_ref,Person* person_data,struct vaclist *vacdown);
struct vaclist *insert_rec(struct vaclist **head,struct vaclist** prevdown,Person *person,int lvl);
void insertskiplist(struct NodeSkip **skip,Person *person,int maxlvl,bool vacbool);
Person *searchnodeskip(struct vaclist *head,struct vaclist** prevdown, int id,int lvl);
Person *searchlistskip(struct vaclist **head,int id,int maxlvl);
struct NodeSkip* insertskip(struct NodeSkip **head,std::string virus,int maxlvl);
void destroyskip(struct NodeSkip** head_ref,int maxlevel);
struct NodeSkip* searchskip(struct NodeSkip* head, std::string virus);
void deleteNode(struct vaclist** head_ref,int key);
#endif