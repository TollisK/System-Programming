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

#include <arpa/inet.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "hw/bloom.h"
using namespace std;

#define h_addr h_addr_list[0]

//lista apo bloom filters gia kathe io
struct Nodebloom *bloomhead = NULL;
//Monopatia twn xwrwn
string dir_path = "";
//Pinakas apo monitos gia thn apothhkeysh dedomenwn
Monitor *monitors;
//counters gia toys handlers
int counter_acc;
int counter_dec;
//An to sigint toy parent process termatisei ta paidia
//na mhn energopoihsei to sigchld
bool flg = false;

//Vriskei se poio paidi exei eisaxthei mia xwra
int countrypath(string country,string input_dir,int numMonitors);
//Ektypwnei to logfile
void logfile_print();

int main(int argc,char *argv[]){
  //Dedomena
  string directory = "dir";
  int buffersize = 200;
  int cyclicBufferSize = 10;
  int sizeOfBloom = 10000;
  int numMonitors = 1;
  int numThreads = 1;
  int i=0;
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
    else if(str == "-c"){
      cyclicBufferSize = stoi(argv[i+1]);
      i++;
    }
    else if(str == "-t"){
      numThreads = stoi(argv[i+1]);
      i++;
    }
  }
  //Topothetei merikes metavlhtes se global.
  string input_dir = directory;
  pid_t  childpid;
  string countries = "";

  //Pinakas apo dedomena gia ta monitos
  monitors = new Monitor[numMonitors];

  //Diavasma fakelwn
  DIR *dir;
  struct dirent *ent;
  char buff[buffersize+1];

  //Lista poy krataei tis aithseis
  struct Record* recordhead = NULL;
  struct Record* record = NULL;

  i=0;
  //Diavazei ton fakelo kai apothtkheyei ta paths ston pinaka dedomenwn
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
      monitors[i].countries = monitors[i].countries + buff;
      i++;
      //An den exei allo paidi 3anapei sto prwto me systhma round robin
      if(i==numMonitors){
        i=0;
      }
    }
    closedir (dir);
  }
  else{
    perror("server: can't open read fifo");
    return 1;
  }

  // Kanei fork ta paidia kai se kathe ena ektelei thn monitor me orismata ta paths
  for(int i=0;i<numMonitors;i++){
    if((childpid = fork()) < 0 ){
      perror("Failed to fork");
      return 1;
    }
    if(childpid == 0){

      const char *temp = "./monitorServer";

      string port_str =to_string(8000+i);
      string thread_str =to_string(numThreads);
      string buff_str =to_string(buffersize);
      string cyclic_str =to_string(cyclicBufferSize);
      string bloom_str =to_string(sizeOfBloom);
      const char* args[] = {temp,"-p",port_str.c_str(),"-t",thread_str.c_str(),"-b",buff_str.c_str(),"-c",cyclic_str.c_str(),"-s",bloom_str.c_str(),monitors[i].countries.c_str(),(char*)NULL};
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

  //Kwdikas gia ta sockets
  int sock = 0, valread;
  struct hostent *rem;
  char hostname[1024];
  hostname[1023] = '\0';
  //Pairnei to onoma ths syskeyhs
  gethostname(hostname, 1023);
  struct hostent* h;
  h = gethostbyname(hostname);
  //Ip mhxanhmatos
  if ((rem = gethostbyname(h->h_name)) == NULL) {	
    herror("gethostbyname"); exit(1);
  }
  //Syndeetai se kathe ena Monitor server 
  for(int i = 0;i<numMonitors;i++){
    sock = 0;
    struct sockaddr_in serv_addr;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
      printf("\n Socket creation error \n");
      return -1;
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8000+i);
    memcpy(&serv_addr.sin_addr, rem->h_addr, rem->h_length);

    int connectStatus;
    do
      connectStatus = connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    while (connectStatus < 0);
    monitors[i].socket = sock;
  }

  memset(buff, 0, sizeof(buff));
  /////////////////BLOOM FILTER//////////////////////
  struct Nodebloom *bloom = NULL;
  struct Nodebloom *bloomtmp2 = NULL;
  int n = 0;
  fd_set set;
  //Mhdenizei to select buffer
  FD_ZERO(&set);
  //Vriskei to megisto socket ip gia thn xrhsh toy sthn select
  int megisto = monitors[0].socket;
  FD_SET(monitors[0].socket, &set);
  for (int i = 1; i < numMonitors; i++){
    if (monitors[i].socket > megisto){
      megisto = monitors[i].socket;
    }
    FD_SET(monitors[i].socket, &set);
  }
  int counter = 0;
  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 0;
  string buff3 = "";
  char* buff5 = new char[sizeOfBloom];
  //diavazei ton buffer apo ta monitos mesw select
  while(1){
    if(counter==numMonitors)
      break;
    select(megisto + 1, &set, nullptr, nullptr, &timeout);
    for (int mon_num = 0; mon_num < numMonitors; mon_num++){
      FD_SET(monitors[mon_num].socket, &set);
      if (FD_ISSET(monitors[mon_num].socket, &set)){
        string buff3 = "";
        int co = 0;
        //Diavazei ton arithmo twn iwn 
        n = read(monitors[mon_num].socket, &co, sizeof(int));
        //Gia kathe io
        for(int i=0;i<co;i++){
          buff3 = "";
          //Diavazei to onoma toy
          while(1){
            n = read(monitors[mon_num].socket, buff, buffersize);
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
            n = read(monitors[mon_num].socket,buff5 + index, buffersize);
            index += buffersize;
          }
          if (index < sizeOfBloom){
            n = read(monitors[mon_num].socket,buff5 + index, sizeOfBloom - index);
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
        cout<<"bloom error"<<endl;
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
            write(monitors[child_num].socket, &num, sizeof(int));

            write(monitors[child_num].socket,&citid,sizeof(int));

            strcpy(buff,virus.c_str());
            for(int count = 0;count<strlen(buff); count+=buffersize){
              write(monitors[child_num].socket, buff+count, buffersize);
            }
            strcpy(buff," ");
            write(monitors[child_num].socket, buff, buffersize);

            n = read(monitors[child_num].socket, buff, buffersize);
            buff[n] = '\0';
            int person_dates[3];
            //An steilei oti yparxei
            if(strcmp(buff,"YES")==0){
              int temp;
              //Diavazei tis hmeromhnies
              for(int i=0;i<3;i++){
                n = read(monitors[child_num].socket,&temp,sizeof(int));
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
              cout<<"child not found error"<<endl;
              cout<<"REQUEST REJECTED – YOU ARE NOT VACCINATED"<<endl;
              counter_dec++;
            }
          }
          else{
            int *temp=NULL;
            pushrecord(&recordhead,country,temp,virus,false);
            cout<<"no country path found error"<<endl;
            cout<<"REQUEST REJECTED – YOU ARE NOT VACCINATED"<<endl;
            counter_dec++;
          }
        }
        else{
          int *temp=NULL;
          pushrecord(&recordhead,country,temp,virus,false);
          cout<<"not found in bloom error"<<endl;
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
        //3anadiavazei to bloom filter
        num = 2;
        write(monitors[child_num].socket, &num, sizeof(int));

        int co = 0;
        n = read(monitors[child_num].socket, &co, sizeof(int));
        for(int i =0;i<co;i++){
          buff3 = "";
          while(1){
            n = read(monitors[child_num].socket, buff, buffersize);
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
            n = read(monitors[child_num].socket,buff5 + index, buffersize);
            index += buffersize;
          }
          if (index < sizeOfBloom){
            n = read(monitors[child_num].socket,buff5 + index, sizeOfBloom - index);
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
        write(monitors[i].socket, &num, sizeof(int));

        write(monitors[i].socket, &citid, sizeof(int));
        int age = -1;
        //An yparxei tote to age > 0
        n = read(monitors[i].socket, &age, sizeof(int));
        if(age >= 0){
          //Diavazei kai ektypwnei ta dedomena
          buff3 = "";
          int count = 0;
          while(1){
            n = read(monitors[i].socket, buff, buffersize);
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
      cout<<"travelMonitorClient: Exiting"<<endl;
      logfile_print();
      num = 9;
      for(int i=0;i<numMonitors;i++){
        write(monitors[i].socket, &num, sizeof(int));
        close(monitors[i].socket);
      }
      return 0;
    }
    else{
      cout<<"wrong input!"<<endl;
    }
  }

  for(int i=0;i<numMonitors;i++){
    close(monitors[i].socket);
  }

  return 0;
}

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

void logfile_print(){
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
}