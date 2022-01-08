#ifndef PERSON_H
#define PERSON_H
#include<string>
#include "country.h"

class string;
//Struct gia thn ylopoihsh toy Date
//Otan ena date theloyme na einai null thewroyme oti ta dedomena mhdenizontai
struct Date{
    int day;
    int month;
    int year;
};
//Klash gia thn ylopoihsh enos anthrwpoy
class Person{
    public:
        int citizenID; //Pedio kleidi
        std::string firstName;
        std::string lastName;
        NodeCountry* country; //Deikths se komvo ths listas country
        int age;
        Date date;
};
//Komvos gia thn dhmioyrgia listas apo persons
struct NodePerson{
    Person person;
    struct NodePerson *next;
};

bool datecalc(Date date1,Date date2);
struct NodePerson *append(struct NodePerson** head, Person *person_data);
void destroypersonlist(struct NodePerson** head_ref);
void personnull(Person *person);
void Datenull(Date *date);
#endif