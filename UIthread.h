
// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef UI_THREAD_H
#define UI_THREAD_H



// TODO Insert declarations or function prototypes (right here) to leverage 
// live documentation
void UI_thread_init(void);
void UI_message_put(struct Message_Data *new_msg);
struct Message_Data *UI_message_get(void);
void UI_message_init(void);


#endif	/* XC_HEADER_TEMPLATE_H */

