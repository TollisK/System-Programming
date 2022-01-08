#ifndef COUNTRY_H
#define COUNTRY_H
#include<string>

//Komvos gia thn ylopoihsh listas me dedomena to onoma ths xwras
struct NodeCountry{
    std::string name; //Country name
    struct NodeCountry *next;
};
struct NodeCountry* searchcountry(struct NodeCountry* head, std::string data);
void appendcountry(struct NodeCountry** head_ref, std::string new_data);
void destroycountrylist(struct NodeCountry** head_ref);
#endif