#include <stddef.h>
#include "queue.h"


void message_Q_initialize(struct message_Q *pMessage_Q, struct Message_Data *pMessage_Box, unsigned int size)
{
	pMessage_Q->Q_size = size;
	pMessage_Q->pMessage_Q = pMessage_Box;
	pMessage_Q->in = pMessage_Q->out = 0;
    //semaphore_initialize(&pMessagebox->semaphore, count);    
}

void message_Q_put(struct message_Q *msg_Q, struct Message_Data *new_msg)
{
	msg_Q->pMessage_Q[msg_Q->in] = *new_msg;
	msg_Q->in++;
	if(msg_Q->in >= msg_Q->Q_size)
	{
		msg_Q->in = 0;	
	}		
	return;		
}

struct Message_Data *message_Q_get(struct message_Q *msg_Q) 
{
	unsigned char i;

	i = msg_Q->out;
	if(msg_Q->out != msg_Q->in)
	
	{	
		msg_Q->out++;
		if(msg_Q->out >= msg_Q->Q_size)
		{
			msg_Q->out = 0;
		}
		
	//	msg_Q->pMessage_Q->tag = 0;
		
		return &msg_Q->pMessage_Q[i];	
	}
//	return 0;							//no message present
//	semaphore_get(&msg_Q->semaphore,waitTime);		
	return NULL;	
}

//
// returns: 0 => message Q is empty
//          non 0 => message Q is not empty 
//
uint8_t message_Q_status(struct message_Q *msg_Q)
{
	return msg_Q->in != msg_Q->out;
}
