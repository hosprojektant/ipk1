#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
double cpu_usage(){
    FILE *statFP;
    int oldNumbers[8];
    int newNumbers[8];
    int diffNumbers[8];
    char cpu[10];  // Not used

    statFP = fopen("/proc/stat", "r");

    fscanf(statFP,
            "%s %d %d %d %d %d %d %d %d",
            cpu,
            &oldNumbers[0], 
            &oldNumbers[1],
            &oldNumbers[2],
            &oldNumbers[3],
            &oldNumbers[4],
            &oldNumbers[5],
            &oldNumbers[6],
            &oldNumbers[7]);

    sleep(1);
    rewind(statFP);

    fscanf(statFP,
            "%s %d %d %d %d %d %d %d %d",
            cpu,
            &newNumbers[0],
            &newNumbers[1],
            &newNumbers[2],
            &newNumbers[3],
            &newNumbers[4],
            &newNumbers[5],
            &newNumbers[6],
            &newNumbers[7]);

    fclose(statFP);
    double PrevIdle = oldNumbers[3]+ oldNumbers[4];
    double Idle = newNumbers[3]+ newNumbers[4];

    double PrevNonIdle = oldNumbers[0] + oldNumbers[1] + oldNumbers[2] + oldNumbers[5] + oldNumbers[6] + oldNumbers[7];
    double NonIdle = newNumbers[0] + newNumbers[1] + newNumbers[2] + newNumbers[5] + newNumbers[6] + newNumbers[7];

    double PrevTotal = PrevIdle + PrevNonIdle;
    double Total = Idle + NonIdle;

    // differentiate: actual value minus the previous one
    double totald = Total - PrevTotal;
    double idled = Idle - PrevIdle;

    double CPU_Percentage = (totald - idled)/totald;
    return CPU_Percentage*100;
}

char * get_cpu_name()
{
  FILE *fp = fopen("/proc/cpuinfo", "r");
  size_t n = 0;
  char *line = NULL;
  char *name_proc;
  while (getline(&line, &n, fp) > 0) {
    if (strstr(line, "model name")) {
       name_proc = line + 13;
       return name_proc;
    }
  }
  fclose(fp);
}


int main (int argc, char *argv[]) {
  int port =  atoi(argv[1]);
  int _socket;
  _socket =  socket(AF_INET,SOCK_STREAM, IPPROTO_TCP);
   if (_socket < 0) {
        printf("ERROR opening socket");
        exit(1);
    }
  struct sockaddr_in server_address, peer_addr; 
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(port);
  server_address.sin_addr.s_addr = INADDR_ANY;
   if (bind(_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        printf("ERROR on binding");
        exit(1);
    }

  listen(_socket,5);
  int peer_addr_size = sizeof(struct sockaddr_in); 
  int data;
  
  while (data = accept(_socket, (struct sockaddr *) &peer_addr, &peer_addr_size))
  {
    if (data < 0) {
          printf("ERROR on accept");
          exit(1);
      }

    char rec_data[160000];
    peer_addr_size = sizeof(struct sockaddr_in); 
    int byte_count = recv(data, &rec_data, sizeof(rec_data), 0);
    char GET_1line[40000];
    char Host_2line[40000];
    char User_Agent[40000];
    int line = 0;
    int number_word = 0;
    int ii = 0;
    for(int i = 0; i < strlen(rec_data); i++){
      if(line == 0 && number_word == 1)
      {
        GET_1line[ii]=rec_data[i];
        ii++;
      }
      if(line == 1 && number_word == 1)
      {
        Host_2line[ii]=rec_data[i];
        ii++;
      }
      if(line == 2 && number_word == 1)
      {
        User_Agent[ii]=rec_data[i];
        ii++;
      }
         if(rec_data[i] == '\n')
      { 
        line++;
        number_word = 0;
        ii=0;
      }
      if(rec_data[i] == ' ')
      { 
        number_word++;
      }
    }
    printf("%s",GET_1line);
    char response[25000] = "HTTP/1.1 200 OK\r\nContent-Type: text/plain;\r\n\r\n";
    if(strcmp(GET_1line, "/cpu-name ")==0)
    { 
      send(data, strcat(response,get_cpu_name()),sizeof(response),0);
    }
    else if(strcmp(GET_1line, "/load ")==0)
    {
      char output[5000];
      snprintf(output, 5000, "%f", cpu_usage());
      send(data, strcat(response,output),sizeof(response),0);
    }
    else if (strcmp(GET_1line,"/hostname ")==0)
    {

      char host_name[100000];
      FILE *fp = fopen("/proc/sys/kernel/hostname", "r");
      fgets (host_name, 100000, fp);
      send(data, strcat(response,host_name),sizeof(response),0);
      fclose(fp);
    }else{
      send(data, strcat(response,GET_1line),sizeof(response),0);
    }
    memset(GET_1line, 0, strlen(GET_1line));
    memset(response, 0, strlen(response));

    close(data);
  }
  close(_socket);
}
