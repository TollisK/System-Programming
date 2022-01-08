#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <ctime>
#include <cmath>
#include "person.h"
using namespace std;

//Synarthsh poy epistrefei true an to date1 einai meta toy date2
bool datecalc(Date date1,Date date2){
    if(date1.year > date2.year){
        return true;
    }
    else if(date1.year == date2.year && date1.month > date2.month){
        return true;
    }
    else if(date1.year == date2.year && date1.month == date2.month&&date1.day>date2.day){
        return true;
    }
    return false;
}

//Synarthsh eisagwghs stoixeioy sthn listas persons
struct NodePerson *append(struct NodePerson** head, Person *person_data){
    struct NodePerson* new_node = new NodePerson;
    struct NodePerson* last = *head;
    new_node->person.age = person_data->age;
    new_node->person.citizenID = person_data->citizenID;
    new_node->person.country = person_data->country;
    new_node->person.firstName = person_data->firstName;
    new_node->person.lastName = person_data->lastName;
    new_node->person.date.day = person_data->date.day;
    new_node->person.date.month = person_data->date.month;
    new_node->person.date.year = person_data->date.year;
    new_node->next = NULL;   
    if (*head == NULL)
    { 
        *head = new_node;   
        return new_node;   
    }   
    while (last->next != NULL)   
        last = last->next;   
    last->next = new_node;   
    return new_node;
}

void destroypersonlist(struct NodePerson** head_ref){
    struct NodePerson* current = *head_ref;
    struct NodePerson* old = NULL;
    while (current != NULL) 
    {
        old = current;
        current = current->next;
        delete old;
    }
    *head_ref = NULL;
}

//Mhdenismos toy person
void personnull(Person *person){
    person->age = -1;
    person->citizenID = -1;
    person->country = NULL;
    person->date.day = 0;
    person->date.month = 0;
    person->date.year = 0;
    person->firstName = " ";
    person->lastName = " ";
}
//Mhdenismos toy Date
void Datenull(Date *date){
    date->day = 0;
    date->month = 0;
    date->year = 0;
}

