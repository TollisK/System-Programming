#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <stdio.h>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <fstream>
#include <sys/wait.h>
#include <signal.h>

#include "hw/bloom.h"
using namespace std;
#define PERMS   0666

//Oi metavlhtes einai global giati tis xeirizontai oi handlers

//lista apo bloom filters gia kathe io
struct Nodebloom *bloomhead = NULL;
//Monopatia twn xwrwn
string dir_path = "";
//Dedomena eisodou
int Monitor_num;
int buff_global;
int bloom_global;
//Pinakas apo monitos gia thn apothhkeysh dedomenwn
Monitor *monitors;
//counters gia toys handlers
int counter_acc;
int counter_dec;
//An to sigint toy parent process termatisei ta paidia
//na mhn energopoihsei to sigchld
bool flg = false;


//Handler gia to SIGCHLD
void child_handle(int sig){
  //An to termatisei to parent mhn 3anaferei ta child processes
  if(flg){
    return;
  }

  pid_t pid;
  int status;
  int index;
  pid_t childpid;
  //Perimenei na vrei poio paidi pethane kai vriskei thn thesh toy
  while ((pid = waitpid(-1, &status, WNOHANG)) <= 0);
  for(int i = 0;i<Monitor_num;i++){
    if(monitors[i].pid == pid){
      index = i;
    }
  }
  //Kanei fork to neo paidi
  if((childpid = fork()) < 0 ){
    perror("Failed to fork");
    return;
  }
  if(childpid == 0){
    const char *temp = "./Monitor";
    const char* args[] = {temp,monitors[index].path_read.c_str(),monitors[index].path_write.c_str(),(char*)NULL};
    if(execvp(args[0], (char* const*)args)==-1){
      cout<<"Failed execvp!"<<endl;
      return;
    }
  }
  else{
    //Vazei to neo process id ston pinaka
    monitors[index].pid = childpid;
  }
  cout<<"Created new child process with id:"<<childpid<<endl;
  //Kanei open ta paths
  if ( (monitors[index].writefd = open(monitors[index].path_write.c_str(), 1))  < 0) {
    perror("server: can't open write fifo");
  }
  if ( (monitors[index].readfd = open(monitors[index].path_read.c_str(), 0))  < 0) {
    perror("server: can't open write fifo");
  }
  //Grafei dedomena
  write(monitors[index].writefd, &buff_global, sizeof(int));
  write(monitors[index].writefd, &bloom_global, sizeof(int));

  //Stelnei ta monopatia twn xwrwn poy tha analavoyn
  istringstream words(monitors[index].countries);
  string word;
  char buff[buff_global+1];
  while(words>>word){
    word = word + " ";
    strcpy(buff,word.c_str());
    for(int count = 0;count<strlen(buff); count+=buff_global){
      write(monitors[index].writefd, buff+count, buff_global);
    }
  }
  strcpy(buff," ");
  write(monitors[index].writefd, buff, buff_global);

  //Diavazei to neo bloom
  int co = 0;
  int n;
  string buff3 = "";
  struct Nodebloom *bloom = NULL;
  char* buff5 = new char[bloom_global];
  //Diavazei posoi fores prepei na ektelesei thn for
  //Kai topothetei ta nea dedomena sthn bloom filter
  n = read(monitors[index].readfd, &co, sizeof(int));
  for(int i=0;i<co;i++){
    buff3 = "";
    while(1){
      n = read(monitors[index].readfd, buff, buff_global);
      buff[n] = '\0';
      if(strcmp(buff," ")==0){
        break;
      }
      buff3 = buff3 + buff;
    }
    int index2 = 0;
    int split = bloom_global/buff_global;
    for (int i = 0; i < split; i++){
      n = read(monitors[index].readfd,buff5 + index2, buff_global);
      index2 += buff_global;
    }
    if (index2 < bloom_global){
      n = read(monitors[index].readfd,buff5 + index2, bloom_global - index2);
    }
    buff5[bloom_global] = '\0';
    if(bloomhead == NULL){
      pushbloom(&bloomhead,buff3,bloom_global);
      memset(bloomhead->bloomarray,0,bloom_global);
      memcpy(bloomhead->bloomarray,buff5,bloom_global);
    }
    else{
      bloom = searchbloom(bloomhead,buff3);
      if(bloom!=NULL){
        for(int i=0;i<bloom_global;i++){
          bloom->bloomarray[i] = bloom->bloomarray[i] | buff5[i];
        }
      }
      else{
        pushbloom(&bloomhead,buff3,bloom_global);
        memset(bloomhead->bloomarray,0,bloom_global);
        memcpy(bloomhead->bloomarray,buff5,bloom_global);
      }
    }
  }
  delete buff5;
}

void sign_handler(int sig){
  //Ektypwnei to log_file
  flg = true;
  ofstream myfile;
  string file_name = "log_file." + to_string(getpid());
  const char *filen = file_name.c_str();
  myfile.open(filen);
  istringstream words(dir_path);
  string word;
  //Gia kathe monopati vriskei tis xwres
  while(words>>word){
    int first = word.find("/");
    int last = word.find_last_of("/");
    string strNew = word.substr (first+1,last-first-1);
    myfile << strNew << endl;
  }
  //Ektypwnei sto file ta counters
  myfile << "TOTAL TRAVEL REQUESTS ";
  myfile << (counter_acc + counter_dec);
  myfile << "\nACCEPTED ";
  myfile << counter_acc;
  myfile << "\nREJECTED ";
  myfile << counter_dec;
  myfile.close();
  //Unlink paths
  for(int i =0;i<Monitor_num;i++){
    if ( unlink(monitors[i].path_read.c_str()) < 0) {
      perror("client: can't unlink \n");
    }
    if ( unlink(monitors[i].path_write.c_str()) < 0) {
      perror("client: can't unlink \n");
    }
  }
  for(int i =0;i<Monitor_num;i++){
    kill(monitors[i].pid,SIGKILL);
  }
  cout<<"Terminated parent process"<<endl;
  exit(0);
  return;
}

//Vriskei se poio paidi exei eisaxthei mia xwra
int countrypath(string country,string input_dir,int numMonitors){
  DIR *dir;
  struct dirent *ent;
  int child_num=0;
  bool flg = false;
  if ((dir = opendir (input_dir.c_str())) != NULL) {
    while ((ent = readdir (dir)) != NULL) {
      if(strcmp(ent->d_name,".")==0||strcmp(ent->d_name,"..")==0)
        continue;
      if(country == ent->d_name){
        flg = true;
        break;
      }
      child_num++;
      if(child_num==numMonitors){
        child_num=0;
      }
    }
    if(!flg){
      child_num = -1;
    }
    closedir (dir);
  }
  else{
    perror("server: can't open read fifo");
    return -1;
  }
  return child_num;
}

int main(int argc,char *argv[]){
  signal(SIGINT, sign_handler);
  signal(SIGCHLD,child_handle);
  string directory = "dir";
  int buffersize = 200;
  int sizeOfBloom = 10000;
  int numMonitors = 1;
  string str;
  //Diavazei dedomena eisodou
  for(int i=0;i<argc;i++){//Pairnei ta dedomena apo ta orismata
    str = argv[i];
    if(str=="-b"){
      buffersize = stoi(argv[i+1]);
      i++;
    }
    else if(str=="-s"){
      sizeOfBloom = stoi(argv[i+1]);
      i++;
    }
    else if(str=="-i"){
      directory = argv[i+1];
      i++;
    }
    else if(str == "-m"){
      numMonitors = stoi(argv[i+1]);
      i++;
    }
  }
  buff_global = buffersize;
  bloom_global = sizeOfBloom;
  string input_dir = directory;
  Monitor_num = numMonitors;
  pid_t  childpid;
  string countries = "";

  //Pinakas apo dedomena gia ta monitos
  monitors = new Monitor[numMonitors];

  DIR *dir;
  struct dirent *ent;
  char buff[buffersize+1];

  //Lista poy krataei tis aithseis
  struct Record* recordhead = NULL;
  struct Record* record = NULL;

  // Ftiaxnei 2 named pipes gia kathe monitor
  for(int i=0;i<numMonitors;i++){
    const char *temp =  "/tmp/fifo.";
    monitors[i].path_read = temp + to_string(2*i);
    monitors[i].path_write = temp + to_string(2*i+1);
    if ((mkfifo(monitors[i].path_read.c_str(), PERMS) < 0) && (errno != EEXIST)){
      for(int k=0;k<i;k++){
        unlink(monitors[k].path_read.c_str());
        unlink(monitors[k].path_write.c_str());
      }
      perror("can't create fifo");
    }
    if ((mkfifo(monitors[i].path_write.c_str(), PERMS) < 0) && (errno != EEXIST) ) {
      unlink(monitors[i].path_read.c_str());
      for(int k=0;k<i;k++){
        unlink(monitors[k].path_read.c_str());
        unlink(monitors[k].path_write.c_str());
      }
      perror("can't create fifo");
    }
  }

  // Kanei fork ta paidia kai se kathe ena ektelei thn monitor me orismata ta paths
  for(int i=0;i<numMonitors;i++){
    if((childpid = fork()) < 0 ){
      perror("Failed to fork");
      return 1;
    }
    if(childpid == 0){
      const char *temp = "./Monitor";
      const char* args[] = {temp,monitors[i].path_read.c_str(),monitors[i].path_write.c_str(),(char*)NULL};
      if(execvp(args[0], (char* const*)args)==-1){
        cout<<"Failed execvp!"<<endl;
        return 0;
      }
      break;
    }
    else{
      monitors[i].pid = childpid;
    }
  }

  //Kanei open ta paths
  for(int i=0;i<numMonitors;i++){
    if ( (monitors[i].writefd = open(monitors[i].path_write.c_str(), 1))  < 0) {
      perror("server: can't open write fifo");
    }
    if ( (monitors[i].readfd = open(monitors[i].path_read.c_str(), 0))  < 0) {
      perror("server: can't open write fifo");
    }
  }

  //Grafei ta dedomena eisodou
  for(int i=0;i<numMonitors;i++){
    write(monitors[i].writefd, &buffersize, sizeof(int));
    write(monitors[i].writefd, &sizeOfBloom, sizeof(int));
  }

  //Stelnei sta paidia ta monopatia twn xwrwn poy tha analavoyn
  int i=0;
  if ((dir = opendir (input_dir.c_str())) != NULL){
    input_dir = " "+input_dir + "/";
    while ((ent = readdir (dir)) != NULL) {
      if(strcmp(ent->d_name,".")==0||strcmp(ent->d_name,"..")==0)
        continue;
      //Krataei se domh ola ta monopatia
      countries = countries + " " + ent->d_name;
      strcpy(buff,input_dir.c_str());
      string dir_name = ent->d_name;
      dir_name = dir_name + "/";
      strcat(buff,dir_name.c_str());
      dir_path = dir_path + " " + buff;
      //Stelnei ta monopatia
      for(int count = 0;count<strlen(buff); count+=buffersize){
        write(monitors[i].writefd, buff+count, buffersize);
      }
      monitors[i].countries = monitors[i].countries + buff;
      i++;
      //An den exei allo paidi 3anapei sto prwto me systhma round robin
      if(i==numMonitors){
        i=0;
      }
    }
    //Stelnei adeio buffer gia na teleiwsei to diavasma
    for(int i=0;i<numMonitors;i++){
      strcpy(buff," ");
      write(monitors[i].writefd, buff, buffersize);
    }
    closedir (dir);
  }
  else{
    perror("server: can't open read fifo");
    return 1;
  }

  /////////////////BLOOM FILTER//////////////////////
  struct Nodebloom *bloom = NULL;
  struct Nodebloom *bloomtmp2 = NULL;
  int n = 0;

  fd_set set;
  FD_ZERO(&set);
  int megisto = monitors[0].readfd;
  FD_SET(monitors[0].readfd, &set);
  for (int i = 1; i < numMonitors; i++){
    if (monitors[i].readfd > megisto){
      megisto = monitors[i].readfd;
    }
    FD_SET(monitors[i].readfd, &set);
  }

  int counter = 0;
  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 0;
  string buff3 = "";
  char* buff5 = new char[sizeOfBloom];
  while(1){
    if(counter==numMonitors)
      break;
    select(megisto + 1, &set, nullptr, nullptr, &timeout);
    for (int mon_num = 0; mon_num < numMonitors; mon_num++){
      FD_SET(monitors[mon_num].readfd, &set);
      if (FD_ISSET(monitors[mon_num].readfd, &set)){
        string buff3 = "";
        int co = 0;
        //Diavazei ton arithmo twn iwn 
        n = read(monitors[mon_num].readfd, &co, sizeof(int));
        //Gia kathe io
        for(int i=0;i<co;i++){
          buff3 = "";
          //Diavazei to onoma toy
          while(1){
            n = read(monitors[mon_num].readfd, buff, buffersize);
            buff[n] = '\0';
            if(strcmp(buff," ")==0){
              break;
            }
            buff3 = buff3 + buff;
          }
          memset(buff5,0,sizeOfBloom);
          int index = 0;
          int split =sizeOfBloom/buffersize;
          //Diavazei to buffer prosthetontas se neo buffer
          for (int i = 0; i < split; i++){
            n = read(monitors[mon_num].readfd,buff5 + index, buffersize);
            index += buffersize;
          }
          if (index < sizeOfBloom){
            n = read(monitors[mon_num].readfd,buff5 + index, sizeOfBloom - index);
          }
          buff5[sizeOfBloom] = '\0';
          //An den yparxei lista to prothetei
          if(bloomhead == NULL){
            pushbloom(&bloomhead,buff3,sizeOfBloom);
            memset(bloomhead->bloomarray,0,sizeOfBloom);
            memcpy(bloomhead->bloomarray,buff5,sizeOfBloom);
          }
          else{
            bloom = searchbloom(bloomhead,buff3);
            //An yparxei bloom gia ton io diatrexei ta 2 arrays kai ektelei bitwise OR
            if(bloom!=NULL){
              for(int i=0;i<sizeOfBloom;i++){
                bloom->bloomarray[i] = bloom->bloomarray[i] | buff5[i];
              }
            }
            else{
              //Prosthetei to neo bloom
              pushbloom(&bloomhead,buff3,sizeOfBloom);
              memset(bloomhead->bloomarray,0,sizeOfBloom);
              memcpy(bloomhead->bloomarray,buff5,sizeOfBloom);
            }
          }
        }
      }
      counter++;
      FD_ZERO(&set);
    }
  }
  delete buff5;

  //Stelnei shma SIGINT sta paidia kai termatizoyn
  for(int i=0; i<numMonitors;i++){
    kill(monitors[i].pid,SIGINT);
    pause();
  }

  //////////////////INPUT BY CLIENT///////////////////
  string input;
  string word;
  int num;
  //Infinity loop
  while(1){
    cout<<"Give an input: "<<endl;
    getline(cin, input);
    istringstream words(input);
    words>>word;
    if(word == "/travelRequest"){
      words>>word;
      int citid = -1;
      try{
        citid = stoi(word);
      }
      catch(...){
        cout<<"Wrong input for citizen id\n";
        return 1;
      }

      words>>word;
      int *dates = Split_date(word);
      words>>word;
      string country = word;

      words>>word;
      string countryto = word;

      words>>word;
      string virus = word;
      //Psaxnei to bloom gia ton io
      bloom = searchbloom(bloomhead,virus);
      //An den yparxei tote stelnei mhnyma lathous kai au3anei ton counters
      //Se kathe periptwsh apothhkeyei ta apotelesmata sto record list
      if(bloom == NULL){
        int *temp=NULL;
        pushrecord(&recordhead,country,temp,virus,false);
        cout<<"REQUEST REJECTED – YOU ARE NOT VACCINATED"<<endl;
        counter_dec++;
      }
      else{
        //An vrei to atomo me citizen id sthn bloom 
        if(findbloom(bloom->bloomarray,16,sizeOfBloom,to_string(citid))){
          //Stelnei to id kai ton io sto monitor sto swsto monitor
          cout<<"Maybe vaccinated... sending message to Monitor"<<endl;
          int child_num = countrypath(country,directory,numMonitors);
          if(child_num > -1){
            num = 0;
            write(monitors[child_num].writefd, &num, sizeof(int));

            write(monitors[child_num].writefd,&citid,sizeof(int));

            strcpy(buff,virus.c_str());
            for(int count = 0;count<strlen(buff); count+=buffersize){
              write(monitors[child_num].writefd, buff+count, buffersize);
            }
            strcpy(buff," ");
            write(monitors[child_num].writefd, buff, buffersize);

            n = read(monitors[child_num].readfd, buff, buffersize);
            buff[n] = '\0';
            int person_dates[3];
            //An steilei oti yparxei
            if(strcmp(buff,"YES")==0){
              int temp;
              //Diavazei tis hmeromhnies
              for(int i=0;i<3;i++){
                n = read(monitors[child_num].readfd,&temp,sizeof(int));
                person_dates[i] = temp;
              }
              pushrecord(&recordhead,country,person_dates,virus,true);
              //An exoyn diafora 6 mhnes tote stelnei to analogo mhnyma
              if((dates[2]*(12*30) + dates[1]*30 + dates[0])-(person_dates[2]*(12*30) + person_dates[1]*30 + person_dates[0])>6*30){
                cout<<"REQUEST REJECTED – YOU WILL NEED ANOTHER VACCINATION BEFORE TRAVEL DATE"<<endl;
                counter_dec++;
              }
              else{
                cout<<"REQUEST ACCEPTED – HAPPY TRAVELS"<<endl;
                counter_acc++;
              }
            }
            else{
              int *temp=NULL;
              pushrecord(&recordhead,country,temp,virus,false);
              cout<<"REQUEST REJECTED – YOU ARE NOT VACCINATED"<<endl;
              counter_dec++;
            }
          }
          else{
            int *temp=NULL;
            pushrecord(&recordhead,country,temp,virus,false);
            cout<<"REQUEST REJECTED – YOU ARE NOT VACCINATED"<<endl;
            counter_dec++;
          }
        }
        else{
          int *temp=NULL;
          pushrecord(&recordhead,country,temp,virus,false);
          cout<<"REQUEST REJECTED – YOU ARE NOT VACCINATED"<<endl;
          counter_dec++;
        }
      }
    }
    else if(word == "/travelStats"){
      int counter = 0;
      string virus;
      string country = " ";
      int *dates1;
      int *dates2;
      //Diavazei ta orismata
      while(words>>word){
        if(counter == 0){
          virus = word;
        }
        else if(counter==1){
          dates1 = Split_date(word);
        }
        else if(counter==2){
          dates2 = Split_date(word);
        }
        else if(counter == 3){
          country = word;
        }
        counter++;
      }
      //An den exei san orisma thn xwra
      if(country == " "){
        country = countries;
      }
      istringstream words(country);
      string word;
      int acc_counter = 0;
      int dec_counter = 0;
      struct Record* temp;
      //Diavazei to record list kai au3anei ton analogo counter
      //Gia thn sygkekrimenh xwra
      while(words>>word){
        temp = recordhead;
        while(temp!=NULL){
          if(temp->country == word && temp->virus == virus&&date(temp->dates,dates1)&&date(dates2,temp->dates)){
            if(temp->vac == true){
              acc_counter++;
            }
            else{
              dec_counter++;
            }
          }
          temp = temp->next;
        }
        cout<<endl<<word<<":"<<endl;
        cout<<"TOTAL REQUESTS "<<acc_counter + dec_counter<<endl;
        cout<<"ACCEPTED "<<acc_counter<<endl;
        cout<<"REJECTED "<<dec_counter<<endl;
      }
    }
    else if(word == "/addVaccinationRecords"){
      string country;
      words>>word;
      country = word;
      //Vriskei to paidi me thn xwra aythn
      int child_num = countrypath(country,directory,numMonitors);
      if(child_num == -1){
        cout<<"There is no country with that name!"<<endl;
      }
      else{
        //Stelnei mhnyma SIGUSR1
        kill(monitors[child_num].pid,SIGUSR1);
        //3anadiavazei to bloom filter
        num = 2;
        write(monitors[child_num].writefd, &num, sizeof(int));

        int co = 0;
        n = read(monitors[child_num].readfd, &co, sizeof(int));
        for(int i =0;i<co;i++){
          buff3 = "";
          while(1){
            n = read(monitors[child_num].readfd, buff, buffersize);
            buff[n] = '\0';
            if(strcmp(buff," ")==0){
              break;
            }
            buff3 = buff3 + buff;
          }

          memset(buff5,0,sizeOfBloom);
          int index = 0;
          int split =sizeOfBloom/buffersize;
          for (int i = 0; i < split; i++){
            n = read(monitors[child_num].readfd,buff5 + index, buffersize);
            index += buffersize;
          }
          if (index < sizeOfBloom){
            n = read(monitors[child_num].readfd,buff5 + index, sizeOfBloom - index);
          }
          buff5[sizeOfBloom] = '\0';
          if(bloomhead == NULL){
            pushbloom(&bloomhead,buff3,sizeOfBloom);
            memset(bloomhead->bloomarray,0,sizeOfBloom);
            memcpy(bloomhead->bloomarray,buff5,sizeOfBloom);
          }
          else{
            bloom = searchbloom(bloomhead,buff3);
            if(bloom!=NULL){
              for(int i=0;i<sizeOfBloom;i++){
                bloom->bloomarray[i] = bloom->bloomarray[i] | buff5[i];
              }
            }
            else{
              pushbloom(&bloomhead,buff3,sizeOfBloom);
              memset(bloomhead->bloomarray,0,sizeOfBloom);
              memcpy(bloomhead->bloomarray,buff5,sizeOfBloom);
            }
          }
        }
      }
    }
    else if(word == "/searchVaccinationStatus"){
      words >> word;
      int citid = -1;
      try{
        citid = stoi(word);
      }
      catch(...){
        cout<<"Wrong input for citizen id\n";
        return 1;
      }
      num = 3;
      //Stelnei se kathe paidi to id 
      for(int i = 0; i < numMonitors ; i++){
        write(monitors[i].writefd, &num, sizeof(int));

        write(monitors[i].writefd, &citid, sizeof(int));
        int age = -1;
        //An yparxei tote to age > 0
        n = read(monitors[i].readfd, &age, sizeof(int));
        if(age >= 0){
          //Diavazei kai ektypwnei ta dedomena
          buff3 = "";
          int count = 0;
          while(1){
            n = read(monitors[i].readfd, buff, buffersize);
            buff[n] = '\0';
            if(strcmp(buff," ")==0){
              cout<<buff3<<endl;
              buff3 = "";
              if(count==0){
                cout<<"AGE "<<age<<endl;
                count++;
              }
            }
            else if(strcmp(buff,"e")==0){
              break;
            }
            else
              buff3 = buff3 + buff;
          }
          break;
        }
      }
    }
    else if(word == "/exit"){
      //Stelnei signal SIGINT ston eayto toy
      kill(getpid(),SIGINT);
      num = 9;
      for(int i=0;i<numMonitors;i++){
        write(monitors[i].writefd, &num, sizeof(int));
        close(monitors[i].writefd);
        close(monitors[i].readfd);
      }
      return 0;
    }
    else{
      cout<<"wrong input!"<<endl;
    }
  }

  for(int i=0;i<numMonitors;i++){
    close(monitors[i].writefd);
    close(monitors[i].readfd);
  }

  return 0;
}

