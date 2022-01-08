#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <ctime>
#include <cmath>
#include "person.h"
#include "bloom.h"
#include "country.h"
#include "skip.h"
using std::string;
using std::cout;
using std::endl;
using std::to_string;
using std::stoi;
using std::ifstream;
using std::istringstream;
using std::cin;

void populationStatus(struct NodeSkip *node,struct NodeCountry *countryhead,string country,Date date1,Date date2){
    if(date1.day!=0 && date2.day==0){ //An yparxei mono ena apo ta 2 dates tote error
        cout<<"An error has occurred";
        return;
    }
    string country_name;
    if(node->vachead[0]==NULL){ //An den yparxei h lista tote petaei error
        cout<<"node is NULL"<<endl;
        return;
    }
    while (countryhead != NULL){ //Gia kathe xwra sthn lista
        struct vaclist *current = node->vachead[0];
        int counter = 0;
        int counter2 = 0;
        if(country != " "){ //An exei dwthei xwra tote xrhsimopoioyme aythn
            if(searchcountry(countryhead,country)!=NULL) //An exoyme dei thn xwra aythn
                country_name = country;
            else{ //An den yparxei sthn lista tote petaei error
                cout<<"The country is not in the database\n";
                return;
            }
        }
        else{ //An den exei dwthei xwra tote tha diatre3ei oles tis xwres sthn lista
            country_name = countryhead->name;
        }
        while (current != NULL){ //gia kathe komvo sthn lista skip
            if(current->person->country->name == country_name){//An o antrwpos anoikei sthn xwra
                counter++;
                if(date1.day == 0) //An den exei dwthei date tote toys metrame oloys
                    counter2++;
                else if(datecalc(current->person->date,date1)&&datecalc(date2,current->person->date)){//An h hmeromhnia einai endiamesa twn orismatwn
                    counter2++;
                }
            }
            current = current->next;
        }
        if(current ==NULL && country!=" "){ //An exoyme dwsei xwra tote ektypwse ta dedomena kai return
            if(counter == 0){
                counter++;
            }
            cout<<country<<" "<<counter2<<" "<<(counter2*100)/counter<<"%"<<endl;
            return;
        }
        if(current == NULL && counter != 0){ //Ektypwnei ta dedomena ths mias xwras
            if(counter == 0){
                counter++;
            }
            cout<<countryhead->name<<" "<<counter2<<" "<<(counter2*100)/counter<<"%"<<endl;
        }
        countryhead = countryhead->next;//Synexizei sthn epomenh xwra
    }
    if(countryhead== NULL)
        return;
}

void popStatusByAge(struct NodeSkip *node,struct NodeCountry *countryhead,string country,Date date1,Date date2){
    if(date1.day!=0 && date2.day==0){ //An yparxei mono ena apo ta 2 dates tote error
        cout<<"An error occurred"<<endl;
        return;
    }
    if(node->vachead[0]==NULL){//An den yparxei h lista tote petaei error
        cout<<"node is NULL"<<endl;
        return;
    }
    string country_name;
    int num;
    while (countryhead != NULL){//Gia kathe xwra sthn lista
        bool flag=true;
        int age1=0,age2=20;
        if(country != " "){ //An exei dwthei xwra tote xrhsimopoioyme aythn
            if(searchcountry(countryhead,country)!=NULL)//An exoyme dei thn xwra aythn
                country_name = country;
            else{//An den yparxei sthn lista tote petaei error
                cout<<"The country is not in the database\n";
                return;
            }
        }
        else{//An den exei dwthei xwra tote tha diatre3ei oles tis xwres sthn lista
            country_name = countryhead->name;
        }
        while(age2<=120){//Gia kathe diasthma xronwn(1-20,21-40,41-60,61-120)
            int counter = 0;
            int counter2 = 0;
            struct vaclist *current = node->vachead[0];
            while (current != NULL){//Gia kathe xwra sthn lista
                if(current->person->country->name == country_name &&current->person->age>age1&&current->person->age<=age2){//An vrisketai mesa sto diasthma xronwn kai anoikei sthn xwra 
                    counter++;
                    if(date1.day == 0)//An den exei dwthei date tote toys metrame oloys
                        counter2++;
                    else if(datecalc(current->person->date,date1)&&datecalc(date2,current->person->date))//An h hmeromhnia einai endiamesa twn orismatwn
                        counter2++;
                }
                current = current->next;
            }
            if(current==NULL){ //Otan teleiwsei h lista
                if(country != " "){
                    if(counter == 0){
                        num = 0;
                    }
                    else{
                        num = (counter2*100)/counter;
                    }
                    if(flag){//Grafei to country mono mia fora
                        flag = false;
                        cout<<country_name<<endl;
                    }
                    cout<<age1<<"-"<<age2<<" "<<counter<<" "<<num<<"%"<<endl;
                }
                else{
                    if(counter == 0){
                        num = 0;
                    }
                    else{
                        num = (counter2*100)/counter;
                    }
                    if(flag&&counter!=0){ //Grefei thn xwra mono mia fora otan vriskei noymero diaforo tou mhdenοs
                        flag = false;
                        cout<<country_name<<endl;
                    }
                    if(!flag)
                        cout<<age1<<"-"<<age2<<" "<<counter<<" "<<num<<"%"<<endl;
                }
            }
            age1+=20;
            age2+=20;
            if(age1==60)
                age2=120;
        }
        if(country!=" "){
            return;
        }
        countryhead = countryhead->next;
    }
    if(countryhead == NULL)
        return;
}


void listnonVaccinatedPersons(struct vaclist *node){
    if(node==NULL){//An den yparxei h lista tote petaei error
        cout<<"node is NULL"<<endl;
        return;
    }
    struct vaclist *current = node;
    while (current != NULL){ //Gia kathe komvo ths listas ektypwnei ta dedomena
        cout<<current->person->citizenID<<" "<<current->person->firstName<<" "<<current->person->lastName<<" "<<current->person->country->name<<" "<<current->person->age<<endl;
        current = current->next;
    }
    if(current== NULL)
        return;
}

//Synarthsh poy psaxnei gia kathe komvo ths skip list an yparxei ena person
void vaccineStatus(struct NodeSkip *node,int citid,int maxlevel){
    Date date;
    Person *person;
    struct NodeSkip *current = node;
    while (current != NULL){//Gia kathe komvo ioy
        person = searchlistskip(current->vachead,citid,maxlevel);
        if(person != NULL){ //An yparxei sthn vaccinated skip list
            date = person->date;
            cout<<endl<<current->virusname;
            cout<<" YES "<<date.day<<"-"<<date.month<<"-"<<date.year<<endl;
        }//An yparxei sthn non-vaccinated skip list h katholoy
        else{
            cout<<endl<<current->virusname;
            cout<<" NO"<<endl;
        }
        current = current->next;
    }
}


//Synarthsh eisagwghs sthn lista twn person
void insertCitizenRecord(struct NodeCountry **countryhead,struct NodePerson** personhead,struct NodeSkip **skiphead,struct Nodebloom **bloomhead,Person *person,string country_name,string virus_name,string boolean,int maxlevel,int bloomsize,int K){
    struct NodeSkip *skip = NULL;
    struct NodePerson* per = NULL;
    struct NodeCountry *country = NULL;
    struct Nodebloom *bloom = NULL;
    int block;
    if (person->age <=0||person->age>120){ //Error handling gia thn hlikia
        cout<<"ERROR IN RECORD "<<person->citizenID<<"\n";
        return;
    }
    if(person->date.day == 0 && boolean == "YES"){ //Error handling gia thn hmeromhnia
        cout<<"ERROR IN RECORD "<<person->citizenID<<"\n";
        return;
    }
    
    skip = searchskip(*skiphead,virus_name);//Epistrefei ton komvo ths skip list gia ton io
    if(skip == NULL){ //An den yparxei
        skip = insertskip(skiphead,virus_name,maxlevel);//Eisagei neo komvo sthn lista
    }
    else{
        if(searchlistskip(skip->vachead,person->citizenID,maxlevel)!=NULL||searchlistskip(skip->nonvachead,person->citizenID,maxlevel)!=NULL){//An yparxei hdh se mia apo tis 2 skip lists toy komvoy
            cout<<"ERROR IN RECORD "<<person->citizenID<<"\n";
            return;
        }
    }
    country = searchcountry(*countryhead,country_name); //Epistrefei ton komvo ths listas xwrwn
    if(country == NULL){ //An den yparxei sthn lista
        appendcountry(countryhead,country_name);//Eisagei neo komvo sthn lista
        person->country = searchcountry(*countryhead,country_name); //Eisagei sto person ton deikth toy komvoy
    }
    else{
        person->country = country;//Eisagei sto person ton deikth toy komvoy
    }
    if(boolean=="YES"){//An anoikei stoys vaccinated eisagei ton anthrwpo sthn bloom 
        bloom = searchbloom(*bloomhead,virus_name); //Epistrfei ton komvo toy bloom gia ton io
        if(bloom == NULL){ //An den yparxei sthn lista
            pushbloom(bloomhead,virus_name,bloomsize);//Eisagei neo komvo bloom 
            bloom = searchbloom(*bloomhead,virus_name);//Neos komvos
            insertbloom(bloom->bloomarray,K,bloomsize,to_string(person->citizenID));//Eisagei to person sthn bloom
        }
        else{
            insertbloom(bloom->bloomarray,K,bloomsize,to_string(person->citizenID));//Eisagei to person sthn bloom
        }
    }
    per = append(personhead,person);//Eisagei to persons sthn lista anthrwpwn
    if(boolean == "YES"){//eisagei to person sthn skip list
        insertskiplist(&skip,&per->person,maxlevel,true);
    }
    else if(boolean == "NO"){
        insertskiplist(&skip,&per->person,maxlevel,false);
    }
    else{
        cout<<"Wrong YES/NO input\n"<<endl;
    }
}

//Synarthsh gia otan emvoliazeitai ena atomo
void vaccinateNow(struct NodeCountry** countryhead,struct NodePerson** personhead,struct NodeSkip **skiphead,struct Nodebloom **bloomhead,Person *person,string country_name,string virus_name,int maxlevel,int bloomsize,int K){
    struct NodeSkip *skip = NULL;
    skip = searchskip(*skiphead,virus_name);//Epistrefei ton komvo ths skip list toy iou
    time_t t = time(NULL);
    tm* tPtr = localtime(&t);
    person->date.day = tPtr->tm_mday;
    person->date.month = (tPtr->tm_mon)+1;
    person->date.year = (tPtr->tm_year)+1900;

    if(skip !=NULL){//An yparxei skip list
        if(searchlistskip(skip->vachead,person->citizenID,maxlevel)!=NULL){//An vrhsketai hdh sthn vaccinated list
            cout<<"ERROR: CITIZEN " << person->citizenID<<" ALREADY VACCINATED ON "<<person->date.day<<"-"<<person->date.month<<"-"<<person->date.year<<endl;
            return;
        }
        else if(searchlistskip(skip->nonvachead,person->citizenID,maxlevel)!=NULL){//An einai sthn non-vaccinated lits
            for(int i=maxlevel-1;i>=0;i--){//Diagrafei to person apo kathe epipedo toy skip list
                if(skip->nonvachead[i] == NULL){
                    continue;
                }
                deleteNode(&(skip->nonvachead[i]),person->citizenID);
            }
        }
    }
    //Eisagwgh sthn vaccinated list 
    insertCitizenRecord(countryhead,personhead,skiphead,bloomhead,person,country_name,virus_name,"YES",maxlevel,bloomsize,K);
}

int main(int argc,char* argv[]){
    int bloomsize = 10000; //default timh gia to bloomsize
    string inputfile = "inputFile";//default timh gia ton input file
    string str;
    for(int i=0;i<argc;i++){//Pairnei ta dedomena apo ta orismata
        str = argv[i];
        if(str=="-b"){
            bloomsize = stoi(argv[i+1]);
            i++;
        }
        else if(str=="-c"){
            inputfile = argv[i+1];
            i++;
        }
    }
    Date date1;
    Date date2;
    Date date;
    int K = 16;
    string vacflag;
    int maxlevel = 10;
    int counter;
    string sentance;
    string country_name;
    string virus;
    int block = 0;
    ifstream MyReadFile(inputfile);

    struct NodeCountry *countryhead= NULL;
    struct NodeCountry *country;

    struct Nodebloom *bloomhead = NULL;
    struct Nodebloom *bloom;

    Person *person;
    person = new Person;
    struct NodePerson* personhead = NULL;
    struct NodePerson* per = NULL;

    struct NodeSkip *skiphead= NULL;
    struct NodeSkip *skip;
    
    string word;
    counter = 0;
    while (getline (MyReadFile, sentance)){ //Prosthetei ta dedomena apo to input file stis listes
        istringstream words(sentance);
        counter = 0;
        //Gia kathe le3h ths protashs vazei sto person ta dedomena
        while (words>>word)
        {
            if (counter == 0){ //ARxikopoihsh toy date kai toy citizenid
                person->citizenID = stoi(word);
                person->date.day = 0;
                person->date.month = 0;
                person->date.year = 0;
            }
            else if(counter == 1){//Onoma
                person->firstName = word;
            }
            else if(counter == 2){//Epitheto
                person->lastName = word;
            }
            else if(counter == 3){//Apothhkeyei thn xwra gia thn eisagwgh ths sthn lista
                country_name = word;
            }
            else if(counter == 4){//hlikia
                person->age = stoi(word);
            }
            else if(counter == 5){//Apothhkeyei ton io
                virus = word;
            }
            else if(counter == 6){//YES/NO
                vacflag = word;
            }
            else if(counter ==7){//Hmeromhnia
                person->date.day = stoi(word.substr(0,2));
                person->date.month = stoi(word.substr(3,2));
                person->date.year = stoi(word.substr(6,4));
            }
            counter ++;
        }
        //Synarthsh gia thn eisagwgh ths xwras, toy ioy kai toy anthrwpoy stis analoges listes
        insertCitizenRecord(&countryhead,&personhead,&skiphead,&bloomhead,person,country_name,virus,vacflag,maxlevel,bloomsize,K);
    }
    string input;
    int citid;
    string virus_name;
    while(1){
        //Mhdenismos twn timwn gia kathe loop
        personnull(person);
        Datenull(&date1);
        Datenull(&date2);

        int switchnum=-1;
        cout<<endl;
        cout<<"/vaccineStatusBloom citizenID virusName "<<endl;
        cout<<"/vaccineStatus citizenID [virusName]"<<endl;
        cout<<"/populationStatus [country] virusName date1 date2 "<<endl;
        cout<<"/popStatusByAge [country] virusName date1 date2"<<endl;
        cout<<"/insertCitizenRecord citizenID firstName lastName country age virusName YES/NO [date]"<<endl;
        cout<<"/vaccinateNow citizenID firstName lastName country age virusName "<<endl;
        cout<<"/list-nonVaccinated-Persons virusName "<<endl;
        cout<<"/exit"<<endl;
        cout<<"\nInput: ";
        getline (cin, input);
        counter = 0;
        istringstream words(input);
        words>>word;
        //Analoga thn prwth le3h proxwraei sto if
        if(word == "/vaccineStatusBloom"){// /vaccineStatusBloom citizenID virusName 
            switchnum = 0;
            words>>word;
            //An gia id yparxei string tote petaei exception
            try{
                citid = stoi(word);
            }
            catch(...){
                cout<<"Wrong input for citizen id\n";
                continue;
            }
            words>>word;
            virus_name = word;
        }
        else if(word == "/vaccineStatus"){// /vaccineStatus citizenID [virusName]
            switchnum = 2;
            counter = 0;
            while(words>>word){
                if(counter==0){
                    try{
                        citid = stoi(word);
                    }
                    catch(...){
                        cout<<"Wrong input for citizen id\n";
                        continue;
                    }
                }
                else{
                    //An exei dwthei ios tote paei sto prwto case
                    virus_name = word;
                    switchnum = 1;
                }
                counter++;
            }
        }
        else if(word == "/populationStatus"||word == "/popStatusByAge"){// /populationStatus [country] virusName date1 date2  /popStatusByAge [country] virusName date1 date2
            if(word == "/populationStatus")
                switchnum = 3;
            else
                switchnum = 4;
            words>>word;
            country_name = word;
            country = searchcountry(countryhead,country_name);
            if(country != NULL){ //An yparxei to country sthn lista
                counter = 0;
                while(words>>word){
                    //Dexetai ta orismata
                    if(counter == 0)
                        virus_name = word;
                    else if(counter == 1){
                        date1.day = stoi(word.substr(0,2));
                        date1.month = stoi(word.substr(3,2));
                        date1.year = stoi(word.substr(6,4));
                    }
                    else if(counter == 2){
                        date2.day = stoi(word.substr(0,2));
                        date2.month = stoi(word.substr(3,2));
                        date2.year = stoi(word.substr(6,4));
                    }
                    counter++;
                }
            }
            else{
                country_name = " ";
                virus_name = word;
                skip = searchskip(skiphead,virus_name);
                //An yparxei o ios sthn lista
                if(skip!=NULL){
                    counter = 0;
                    //Dexetai ta orismata
                    while(words>>word){
                        if(counter == 0){
                            date1.day = stoi(word.substr(0,2));
                            date1.month = stoi(word.substr(3,2));
                            date1.year = stoi(word.substr(6,4));
                        }
                        else{
                            date2.day = stoi(word.substr(0,2));
                            date2.month = stoi(word.substr(3,2));
                            date2.year = stoi(word.substr(6,4));
                        }
                        counter++;
                    }
                }
                else{
                    cout<<"Virus is not into our database\n";
                    continue;
                }
            }
        }
        else if(word == "/insertCitizenRecord"){// /insertCitizenRecord citizenID firstName lastName country age virusName YES/NO [date]
            switchnum = 5;
            counter = 0;
            while(words>>word){
                if(counter == 0){
                    try{
                        person->citizenID = stoi(word);
                    }
                    catch(...){
                        cout<<"Wrong input for citizen id\n";
                        continue;
                    }
                }
                else if(counter == 1)
                    person->firstName = word;
                else if(counter == 2)
                    person->lastName = word;
                else if(counter == 3)
                    country_name = word;
                else if(counter == 4){
                    try{
                        person->age = stoi(word);
                    }
                    catch(...){
                        cout<<"Wrong input for age id\n";
                        continue;
                    }
                }
                else if(counter == 5)
                    virus_name = word;
                else if(counter == 6)
                    vacflag = word;
                else if(counter == 7){
                    person->date.day = stoi(word.substr(0,2));
                    person->date.month = stoi(word.substr(3,2));
                    person->date.year = stoi(word.substr(6,4));
                }
                counter++;
            }
        }
        else if(word == "/vaccinateNow"){// /vaccinateNow citizenID firstName lastName country age virusName 
            switchnum = 6;
            counter = 0;
            while(words>>word){
                if(counter == 0){
                    try{
                        person->citizenID = stoi(word);
                    }
                    catch(...){
                        cout<<"Wrong input for citizen id\n";
                        continue;
                    }
                }
                else if(counter == 1)
                    person->firstName = word;
                else if(counter == 2)
                    person->lastName = word;
                else if(counter == 3)
                    country_name = word;
                else if(counter == 4)
                    try{
                        person->age = stoi(word);
                    }
                    catch(...){
                        cout<<"Wrong input for age id\n";
                        continue;
                    }
                else if(counter == 5)
                    virus_name = word;
                counter++;
            }
        }
        else if(word == "/list-nonVaccinated-Persons"){ // /list-nonVaccinated-Persons virusName 
            switchnum = 7;
            words>>word;
            virus_name = word;
        }
        else if(word == "/exit"){ // /exit
            switchnum = 8;
        }
        switch(switchnum){
            case 0: //vaccinestatusbloom citizenID virusName 
                bloom = searchbloom(bloomhead,virus_name);
                if(bloom == NULL){
                    cout<<"\nNOT VACCINATED: No virus found into the database\n";
                }
                else{
                    if(findbloom(bloom->bloomarray,K,bloomsize,to_string(citid))){
                        cout<<"\nMAYBE VACCINATED\n";
                    }
                    else{
                        cout<<"\nNOT VACCINATED\n";
                    }
                }
                break;
            case 1:// /vaccineStatus citizenID virusName
                skip = searchskip(skiphead,virus_name);
                if(skip !=NULL){
                    person = searchlistskip(skip->vachead,citid,maxlevel);
                    if(person != NULL){
                        cout<<virus_name<<" YES "<<person->date.day<<"-"<<person->date.month<<"-"<<person->date.year<<endl;
                    }
                    else {
                        cout<<virus_name<<" NO"<<endl;
                    }
                }
                else{
                    cout<<"Virus is not into our database\n";
                }
                break;
            case 2: // /vaccineStatus citizenID
                vaccineStatus(skiphead,citid,maxlevel);
                break;
            case 3:
                skip = searchskip(skiphead,virus_name);
                populationStatus(skip,countryhead,country_name,date1,date2);
                break;
            case 4:
                skip = searchskip(skiphead,virus_name);
                popStatusByAge(skip,countryhead,country_name,date1,date2);
                break;
            case 5:
                insertCitizenRecord(&countryhead,&personhead,&skiphead,&bloomhead,person,country_name,virus,vacflag,maxlevel,bloomsize,K);
                break;
            case 6:
                vaccinateNow(&countryhead,&personhead,&skiphead,&bloomhead,person,country_name,virus_name,maxlevel,bloomsize,K);
                break;
            case 7:
                skip = searchskip(skiphead,virus_name);
                if(skip == NULL){
                    cout<<"Virus is not into our database\n";
                }
                else
                    listnonVaccinatedPersons(skip->nonvachead[0]);
                    
                break;
            case 8:
                //Diagrafei thn mnhmh
                cout<<"Deleting memory"<<endl;
                destroyskip(&skiphead,maxlevel);
                destroybloom(&bloomhead);
                destroycountrylist(&countryhead);
                destroypersonlist(&personhead);
                delete person;
                cout<<"Exiting..."<<endl;
                MyReadFile.close();
                return 1;
            default:
                cout<<"\nFalse input!\n";
                break;
        }
    }
    cout << "\nend\n";
    return 2;
}
