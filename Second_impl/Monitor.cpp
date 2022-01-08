#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <fstream>
#include <sstream>
#include <ctime>
#include <cmath>
#include <cstring>
#include <signal.h>

#include <dirent.h>
#include "hw/person.h"
#include "hw/bloom.h"
#include "hw/country.h"
#include "hw/skip.h"
using std::string;
using std::cout;
using std::endl;
using std::to_string;
using std::stoi;
using std::ifstream;
using std::istringstream;
using std::cin;
using std::ofstream;

void insertCitizenRecord(struct NodeCountry **countryhead,struct NodePerson** personhead,struct NodeSkip **skiphead,struct Nodebloom **bloomhead,Person *person,string country_name,string virus_name,string boolean,int bloomsize,int K);
void insert_record(string file_name,const char *inputdir,struct NodeCountry** countryhead,struct NodePerson** personhead,struct NodeSkip **skiphead,struct Nodebloom **bloomhead,int sizeofbloom);

//Global metavlhtes gia thn xrhsimopoihsh toys apo toys handlers

//Counters 
int acc_counter;
int dec_counter;
//Apothhkeyei ta monopatia twn xwrwn
string countries;
string input;
//Lista apo xwres
struct NodeCountry *countryhead;
//Apothhkeyei ta arxeia
static string files_txt;
//Skip list
struct NodeSkip *skiphead;
//Bloom list
struct Nodebloom *bloomhead;
struct NodeSkip *skip;
//Person list
struct NodePerson* personhead;
int size_of_bloom;

void sigusr1_handler(int sig){
   DIR *dir = NULL;
   struct dirent *ent = NULL;
   //Diavazei ta monopatia twn xwrwn
   istringstream words_c(countries);
   string word_c;
   while(words_c>>word_c){
      if ((dir = opendir (word_c.c_str())) != NULL) {
         while ((ent = readdir (dir)) != NULL) {
            if(strcmp(ent->d_name,".")==0||strcmp(ent->d_name,"..")==0)
               continue;
            string temp = ent->d_name;
            istringstream words(files_txt);
            string word;
            bool flg = false;
            //An yparxei ena arxeio poy den yphrxe prin
            while(words>>word){
               if(temp == word){
                  flg = true;
               }
            }
            if(!flg){
               //Prosthetei stis domes
               insert_record(temp,word_c.c_str(),&countryhead,&personhead,&skiphead,&bloomhead,size_of_bloom);
            }
         }
         closedir (dir);
      }
      else{
         perror("server: can't open read fifo");
         return;
      }
   }
   cout<<"New files are in bloom and skip list"<<endl;
   return;
}

void sign_handler(int sig){
   //Dhmioyrgei to log_file 
   ofstream myfile;
   string file_name = "log_file." + to_string(getpid());
   const char *filen = file_name.c_str();
   myfile.open(filen);
   istringstream words(countries);
   string word;
   //Vriskei tis xwres apo ta monopatia
   while(words>>word){
      int first = word.find("/");
      int last = word.find_last_of("/");
      string strNew = word.substr (first+1,last-first-1);
      myfile << strNew << endl;
   }
   myfile << "TOTAL TRAVEL REQUESTS ";
   myfile << (acc_counter + dec_counter);
   myfile << "\nACCEPTED ";
   myfile << acc_counter;
   myfile << "\nREJECTED ";
   myfile << dec_counter;
   cout<<"Terminated child with id"<<getpid()<<endl;
   myfile.close();
   exit(0);
}

int main(int argc, char *argv[]){
   if(argc!=3){
      perror("Error with arguments");
      return 1;
   }
   //Dedomena
   int buffersize;
   int sizeofbloom;
   int readfd, writefd=-1;
   acc_counter = 0;
   dec_counter = 0;
   int count;
  
   //Signals
   signal(SIGINT, sign_handler);
   signal(SIGQUIT, sign_handler);
   signal(SIGUSR1, sigusr1_handler);

   int n;
   //Kanei open ta monopatia
   if ( (readfd = open(argv[2], 0))  < 0)  {
      perror("client: can't open read fifo \n");
   }
   if ( (writefd = open(argv[1], 1))  < 0) {
      perror("server: can't open write fifo");
   }

   //diavazei buffersize kai bloom size
   n = read(readfd,&buffersize,sizeof(int));
   n = read(readfd,&sizeofbloom,sizeof(int));
   size_of_bloom = sizeofbloom;

   char buff[buffersize+1];
   string buffer="";

   count = 0;
   //Diavazei ta monopatia
   while(1){
      n = read(readfd, buff, buffersize);
      buff[n] = '\0';
      if(strcmp(buff," ")==0){
         break;
      }
      buffer = buffer + buff;
      count++;
   }
   countries = buffer;

   istringstream words(buffer);
   string word;
   //Mhdenizei tis domes
   skiphead= NULL;
   bloomhead = NULL;
   skip = NULL;
   Person *person;
   person = new Person;
   //Gia kathe monopati
   while(words>>word){
      const char *inputdir = word.c_str();
      buffer = "";
      int K = 16;
      string country_name = "";
      string virus = "";
      string vacflag = "";

      personhead = NULL;

      countryhead= NULL;
      struct NodeCountry *country = NULL;

      
      struct Nodebloom *bloom = NULL;

      ifstream MyReadFile;
      DIR *dir = NULL;
      struct dirent *ent = NULL;
      //Gia kathe arxeio sto monopati
      if ((dir = opendir (inputdir)) != NULL) {
         while ((ent = readdir (dir)) != NULL) {
            if(strcmp(ent->d_name,".")==0||strcmp(ent->d_name,"..")==0)
               continue;
            //Apothhkeyei to arxeio
            files_txt = files_txt + " " +  ent->d_name;
            //Eisagei ta dedomena stis domes
            insert_record(ent->d_name,inputdir,&countryhead,&personhead,&skiphead,&bloomhead,sizeofbloom);
         }
         closedir (dir);
      }
      else{
         perror("server: can't open read fifo");
         return 1;
      }
   }
   struct Nodebloom *bloom = NULL;
   //Ypologizei posa blooms prepei na steilei
   bloom = bloomhead;
   int co = 0;
   while(bloom!=NULL){
      co++;
      bloom = bloom->next;
   }
   bloom = bloomhead;
   //Stelnei ton counter
   write(writefd,&co,sizeof(int));
   //Gia kathe ena
   for(int j=0;j<co;j++){
      //Stelnei to onoma toy ioy
      for(int count = 0;count<strlen(bloom->virusname.c_str()); count+=buffersize){
         write(writefd, bloom->virusname.c_str()+count, buffersize);
      }
      strcpy(buff," ");
      write(writefd, buff, buffersize);
      //Kovei ton buffer se kommatia kai ta stelnei
      int index = 0;
      int split = sizeofbloom/buffersize;
      for (int i = 0; i < split; i++){
         write(writefd,bloom->bloomarray + index, buffersize);
         index += buffersize;
      }
      //Stelnei kai ta epomena
      if (index < sizeofbloom){
         write(writefd,bloom->bloomarray + index, sizeofbloom - index);
      }
      bloom = bloom->next;
   }
   int switchnum = 9;
   int cou = 0;
   int citid = 0;
   string virus;
   int counter = 0;
   int age;
   bool flg = false;
   while(1){
      cou++;
      //Diavazei ton aritmo ths entolhs
      n = read(readfd,&switchnum,sizeof(int));
      switch (switchnum)
      {
         case 0:
            citid = 0;
            //Diavazei to id
            n = read(readfd,&citid,sizeof(int));
            count = 0;
            buffer = "";
            //Diavazei kai to onoma toy ioy
            while(1){
               n = read(readfd, buff, buffersize);
               buff[n] = '\0';
               if(strcmp(buff," ")==0){
                  break;
               }
               buffer = buffer + buff;
               count++;
            }
            cout<<"Testing person with citid: "<<citid<<" for "<<buffer<<endl;
            virus = buffer;
            //Psaxnei ton io sthn skip list
            skip = searchskip(skiphead,virus);
            if(skip != NULL){
               //Psaxnei to atomo me to id sthn sygkekrimenh skip
               person = searchlistskip(skip->vachead,citid);
               if(person != NULL){
                  //An ton vrei au3anei ton counter kai stelnei ta dedomena
                  acc_counter++;
                  strcpy(buff,"YES");
                  write(writefd, buff, buffersize);
                  for(int i=0;i<3;i++){
                     int temp;
                     if(i==0)
                        temp = person->date.day;
                     else if(i==1)
                        temp = person->date.month;
                     else
                        temp = person->date.year;
                     write(writefd,&temp,sizeof(int));
                  }
                  break;
               }
            }
            //An den vrethhke stelnei analogo mhnyma
            dec_counter++;
            strcpy(buff,"NO");
            write(writefd, buff, buffersize);
            break;
         case 2:
            //Afoy exei ftiaxtei to neo bloom ton 3anastelnei sto parent 
            co = 0;
            bloom = bloomhead;
            while(bloom!=NULL){
               co++;
               bloom = bloom->next;
            }
            bloom = bloomhead;
            write(writefd,&co,sizeof(int));
            for(int i=0;i<co;i++){
               for(int count = 0;count<strlen(bloom->virusname.c_str()); count+=buffersize){
                  write(writefd, bloom->virusname.c_str()+count, buffersize);
               }
               strcpy(buff," ");
               write(writefd, buff, buffersize);

               int index = 0;
               int split = sizeofbloom/buffersize;
               for (int i = 0; i < split; i++){
                  write(writefd,bloom->bloomarray + index, buffersize);
                  index += buffersize;
               }
               if (index < sizeofbloom){
                  write(writefd,bloom->bloomarray + index, sizeofbloom - index);
               }
               bloom = bloom->next;
            }
            break;
         case 3:
            citid = 0;
            //Diavazei to citizen id
            n = read(readfd,&citid,sizeof(int));
            skip = skiphead;
            //Gia kathe skip sthn lista
            while(skip!=NULL){
               //An vrei to atomo me ayto to id
               person = searchlistskip(skip->vachead,citid);
               if(person!=NULL){
                  //Stelnei ta dedomena
                  flg = true;
                  age = person->age;
                  buffer = to_string(person->citizenID) + " " + person->firstName +  " " + person->lastName + " " + person->country->name;

                  write(writefd, &age, sizeof(int));

                  strcpy(buff,buffer.c_str());
                  for(int count = 0;count<strlen(buff); count+=buffersize){
                     write(writefd, buff+count, buffersize);
                  }
                  strcpy(buff," ");
                  write(writefd,buff, buffersize);
                  break;
               }
               skip = skip->next;
            }
            if(!flg){
               //An den yparxei sthn skip
               age = -1;
               write(writefd, &age, sizeof(int));
               break;
            }
            skip = skiphead;
            while(skip!=NULL){
               //3anadiatrexei thn skip kai stelnei gia kathe io an yparxei to atomo
               person = searchlistskip(skip->vachead,citid);
               if(person!=NULL){
                  buffer = skip->virusname + " VACCINATED ON "  + to_string(person->date.day) + "-" + to_string(person->date.month) + "-" + to_string(person->date.year);
                  strcpy(buff,buffer.c_str());
                  for(int count = 0;count<strlen(buff); count+=buffersize){
                     write(writefd, buff+count, buffersize);
                  }
                  strcpy(buff," ");
                  write(writefd,buff, buffersize);
               }
               else{
                  buffer = skip->virusname + " NOT YET VACCINATED";
                  strcpy(buff,buffer.c_str());
                  for(int count = 0;count<strlen(buff); count+=buffersize){
                     write(writefd, buff+count, buffersize);
                  }
                  strcpy(buff," ");
                  write(writefd,buff, buffersize);
               }
               skip = skip->next;
            }
            strcpy(buff,"e");
            write(writefd,buff, buffersize);
            break;
         case 9:
            cout<<"Exiting..."<<endl;
            return 0;
      }
   }
   cout<<"Exiting...2"<<endl;


   return 0;
}

void insert_record(string file_name,const char *inputdir,struct NodeCountry** countryhead,struct NodePerson** personhead,struct NodeSkip **skiphead,struct Nodebloom **bloomhead,int sizeofbloom){
   ifstream MyReadFile;
   string sentance = "";
   string word = "";
   string temp = file_name;
   Person *person;
   string country_name;
   string virus;
   int K = 16;
   string vacflag;
   person = new Person;
   string inputfile = inputdir + temp;
   MyReadFile.open(inputfile);
   while (getline (MyReadFile, sentance)){ //Prosthetei ta dedomena apo to input file stis listes
         istringstream words(sentance);
         int counter = 0;
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
               int *dates = Split_date(word);
               // cout<<dates[0]<<" "<<dates[1]<<" "<<dates[2]<<endl;
               person->date.day = dates[0];
               person->date.month = dates[1];
               person->date.year = dates[2];

            }
            counter ++;
         }
         insertCitizenRecord(countryhead,personhead,skiphead,bloomhead,person,country_name,virus,vacflag,sizeofbloom,K);
   }
   MyReadFile.close();
}


void insertCitizenRecord(struct NodeCountry **countryhead,struct NodePerson** personhead,struct NodeSkip **skiphead,struct Nodebloom **bloomhead,Person *person,string country_name,string virus_name,string boolean,int bloomsize,int K){
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
        skip = insertskip(skiphead,virus_name);//Eisagei neo komvo sthn lista
    }
    else{ 
        if(searchlistskip(skip->vachead,person->citizenID)!=NULL||searchlistskip(skip->nonvachead,person->citizenID)!=NULL){//An yparxei hdh se mia apo tis 2 skip lists toy komvoy
            // cout<<"ERROR IN RECORD "<<person->citizenID<<"\n";
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
        insertskiplist(&skip,&per->person,true);
    }
    else if(boolean == "NO"){
        insertskiplist(&skip,&per->person,false);
    }
    else{
        cout<<"Wrong YES/NO input\n"<<endl;
    }
}