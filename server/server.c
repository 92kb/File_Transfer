#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<unistd.h>
#include<unistd.h>
#include<fcntl.h>

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include"utility.h"

#define port "4500"
#define max_data 1492

int main(int argc, char *argv[])
{
  
  
  
  int n;
  int total_frame ;// total frames
 
  int data_rec;

  
  // structs for server address
     struct addrinfo hints, *server_info;
    struct sockaddr_storage their_addr;
    int their_addrlen = sizeof(their_addr);
    int socketFD;
 


  memset (&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL, port, &hints, &server_info);

    if (server_info != NULL) 
    {
        socketFD = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
        bind(socketFD, server_info->ai_addr, server_info->ai_addrlen);

        freeaddrinfo(server_info);
    }

    
  struct Frame_Ack ack;  //acknowledgement frame
  ack.type = htonl(1);
  
  
  
  int rec_seq = -1;
  struct Frame_Init fr_i; //struct for holding the initial frame
  
   data_rec =0;
   int init_seq = 0;

    //listening for initial frame which gives the file info
    while(data_rec == 0  && init_seq == 0)
    {
      
      memset(&fr_i,0,sizeof(fr_i));
    // data_rec = recvfrom(socketFD, &fr_i, (sizeof (int) * 2) + 50, 0, (struct sockaddr*)& si_other, &from_length);
      //recieving data
     data_rec = recvfrom(socketFD, &fr_i, (sizeof (int) * 3) + 50, 0, (struct sockaddr *)&their_addr, &their_addrlen);
     
       // sending data
       sendto(socketFD,&ack,sizeof(int), 0, (struct sockaddr*)& their_addr, sizeof(struct sockaddr_in));
      // sendto(socketFD, byteArrayPacket, currentPacketTotalSize, 0, (struct sockaddr *)&their_addr, sizeof(struct sockaddr_in));
       fr_i.size = ntohl (fr_i.size);
       fr_i.seq_no = (int)ntohl (fr_i.seq_no);
       init_seq = fr_i.seq_no;
      // fr_i.total = (int)ntohl (fr_i.total);
        
        if(fr_i.size % max_data == 0)
       total_frame = ((fr_i.size) / max_data);
       else
       total_frame  = ((fr_i.size) / max_data) + 1; //grtting the total number of frames to recieve
       
      
     
   //   printf("%dtot\n",total_frame);
       
     
    }

  


  // output file pointer
  FILE * outputFile;
  
  
  //outputFile = fopen("joke.txt", "wb");
   int seq_rec = -1;
   
   outputFile = fopen(fr_i.data, "wb");
   
 // loop for recieving all packets
   while(1)
   {
       
       struct Frame fr;
       memset(&fr,0,sizeof(fr));
       // recieving packet
       data_rec = recvfrom(socketFD, &fr, (sizeof (int) * 2) + 1492, 0, (struct sockaddr *)&their_addr, &their_addrlen);
       
      //  printf("reached here\n");
        fr.type = ntohl (fr.type);
        fr.seq_no =ntohl (fr.seq_no);
       // printf("%dseq\n",fr.seq_no);
        if(fr.seq_no > seq_rec)
        {
           
           // sending acknowledgement packet
           sendto(socketFD, &ack,sizeof(int), 0, (struct sockaddr*)& their_addr, sizeof(struct sockaddr_in));
     
           
           /*for(int i =0;fr.data[i]!='\0';i++)
           {
             fputc(fr.data[i],outputFile);
           
            } */
          /*  char ch;
    
            if(fr.seq_no == total_frame-1)
            {
               printf("reach last\n");
               int rem = fr_i.size % max_data ;
               memcpy(outputFile,fr.data,rem);
               fclose(outputFile); // closing file
               break;
             }
             else
             {
               //printf("%d\n",fr.seq_no);
               memcpy(outputFile,fr.data,max_data);
               for(int k=0;k<max_data;k++)
               ch = getc(outputFile);
      
             }   */
          
            
            if(fr.seq_no == total_frame-1)
            {
              // printf("reach last\n");
               int rem = fr_i.size % max_data ;
               char buf_l[rem];
                memcpy(buf_l,fr.data,rem);
               fwrite (buf_l, sizeof(char), rem, outputFile);
               fclose(outputFile); // closing file
               break;
             }
             else
             {
               //printf("%d\n",fr.seq_no);
               char buf[max_data];
               memcpy(buf,fr.data,max_data);
               fwrite (buf, sizeof(char), max_data, outputFile);
               
      
             }   
            seq_rec++;
           
         } 
        
      /*  if(fr.seq_no == total_frame-1)
            {
               
               int rem = fr_i.size % max_data ;
               char buff1[rem];
               memcpy(buff1,fr.data,rem);
               fwrite (buff1, sizeof(char), rem, outputFile);
               fclose(outputFile); // closing file
               break;
             }
             else
             {
               //printf("%d\n",fr.seq_no);
               char buff2[max_data];
               memcpy(buff2,fr.data,max_data);
               fwrite (buff2, sizeof(char), max_data, outputFile);
               
      
             }   */
      
     
     
    } 
   


 }


