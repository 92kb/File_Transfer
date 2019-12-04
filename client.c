#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include<unistd.h>
#include <stdbool.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include "utility.h"


#define max_data 1492
#define port "4500"
//#define max_data 1491




int main(int argc, char *argv[]){
  if(argc != 3){
    printf("Enter server address and file name\n");
    exit(0);
  }
  
  int n;

 // int sockfd;
  int length;
  int socketFD;
 // struct sockaddr_in serverAddr;
 // char ack_buf[1];
 const char *inputFileName = argv[2];
  FILE *in_file;
  
  struct stat in_file_info;

   struct addrinfo hints, *server_info, *my_info;
  
  // setting the server and client address
    memset (&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    // Get IP address of the server
    getaddrinfo(argv[1], port, &hints, &server_info);

    // Get client's IP address
    getaddrinfo(NULL, port, &hints, &my_info);
  
 
  length=sizeof(struct sockaddr_in);
  struct sockaddr_storage dummyVar;
  int dummyVar_len = sizeof(dummyVar);
   
  //setting up the socket
  if (server_info != NULL) {

        // Create socket
        socketFD = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

        struct timeval tv;

        // 1-second timeout on recv calls with socket
        tv.tv_sec = 1;

        // Not initialzingthis can cause strange errors
        tv.tv_usec = 0;
        setsockopt(socketFD, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));

        bind(socketFD, my_info->ai_addr, my_info->ai_addrlen);
   }
    else
    {
      printf("cannot connect to server\n");
      return 1;
    }

  in_file = fopen(inputFileName, "rb"); // openning the input file
  
  
  stat(inputFileName, &in_file_info);//getting file information

  int total_frame ;
  if(in_file_info.st_size % max_data == 0)
  total_frame = ((in_file_info.st_size) / max_data);
  else
  total_frame  = ((in_file_info.st_size) / max_data) + 1; 
   
 // struct Frame fr[total_frame];
 // printf("%d\n",total_frame);

  int m;
  int q;

  // initial frame
  struct Frame_Init fr_i;
  memset(&fr_i,0,sizeof(fr_i));
  fr_i.seq_no = htonl(-1);
  fr_i.size =  htonl(in_file_info.st_size);
  fr_i.total = htonl(total_frame);
  strncpy (fr_i.data, inputFileName, strlen(inputFileName) + 1);
  
  // ack.type = 0;
  //sending the initial frame

  struct Frame_Ack ack;
  ack.type = 0;

// sending the initial frame
  do{
      struct sockaddr_storage dummyVar;
      int dummyVar_len = sizeof(dummyVar);
     //ubzero(ack_buf,1);
     m =  sendto(socketFD, &fr_i, (sizeof (int) * 3) + 50, 0, server_info->ai_addr,length);
      //sendto(socketFD, byteArrayPacket, currentPacketTotalSize, 0, server_info->ai_addr, sizeof(struct sockaddr_storage));
     q =  recvfrom(socketFD,&ack, (sizeof (int)), 0, (struct sockaddr *) &dummyVar, &dummyVar_len);
      // recvfrom(socketFD, buf, BUFLEN, 0, (struct sockaddr *) &dummyVar, &dummyVar_len);
    
     ack.type = (int)ntohl(ack.type);
     
  
    
    }while(ack.type == 0);

 // printf("reach1\n");
  bool file_end = false;
  
  char val; 
  int j;
   
 // sending all the packets in a loop:checks for acknowledgement
  for(int i=0;i<total_frame;i++)
  {
   
     struct Frame fr;
    
     
  
     
    //memset(&fr[i],0,sizeof(fr[0]));

    fr.seq_no = htonl(i);
    fr.type = htonl(0);
   /*    for( j=0;j<max_data && !file_end;j++)
      {
   
        val = getc(in_file);
        
        if (feof(in_file))
        {
         file_end = true;
         j--;
        } 
        else
        {
          fr.data[j] = val;
        }
    }
     fr.data[j] = '\0'; */
   char ch;
   if(i != total_frame-1)
   {
   char buf[1492];
   fread(buf, sizeof(char), max_data, in_file);
   memcpy(fr.data,buf,max_data);
   
   
   }
   else
   {
    
    int rem = in_file_info.st_size % max_data;
    char buf_l[rem];
    fread(buf_l, sizeof(char), rem, in_file);
    memcpy(fr.data, buf_l,rem);
   }
     
    ack.type = 0;
 

    do{

     // printf("reach2\n");
   
    m =  sendto(socketFD, &fr, (sizeof (int) * 2) + 1492, 0,server_info->ai_addr,length);
    q =  recvfrom(socketFD,&ack, (sizeof (int)), 0, (struct sockaddr *) &dummyVar, &dummyVar_len);
    
    
    ack.type = (int)ntohl(ack.type);
    
    
    
    } while(ack.type == 0);

  }

  


}
