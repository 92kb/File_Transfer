#ifndef UTILITY_H
#define UTILITY_H


//type 0 message type 1 acknowledgement 2 for initial frame
struct Frame
{
  int type;
  int seq_no;
  char data[1492];
};


struct Frame_Ack
{
  int type;
}; 

struct Frame_Init
{
 
  int seq_no;
  char data[50];
  int size;
  int total;

};









#endif
