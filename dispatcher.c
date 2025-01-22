#include "common.h"

void broadcast(int source, int destination, int type)
{
    struct Message message;
    if (msgrcv(source, &message, MSG_SIZE, type, IPC_NOWAIT) != -1)
    {
        msgsnd(destination, &message, strlen(message.mtext)+1, 0);
    }
}

int main(void)
{
    int clients = msgget(123456, IPC_CREAT | 0644);
    int producers = msgget(987654, IPC_CREAT | 0644);
    struct Message MSG;
    char list_of_producers[MSG_SIZE]="";
    int types[MAX_PRODUCERS];
    int num_of_producers = 0;
    while(1)
    {   
        // Czekanie na prosbe o liste dostepnych typow powiadomien
        if(msgrcv(clients, &MSG, MSG_SIZE, 1, IPC_NOWAIT) != -1)
        {
            printf("Odebralem prosbe o liste dostepnych typow powiadomien...\n");
            strcpy(MSG.mtext, list_of_producers);
            MSG.mtype=2;
            msgsnd(clients, &MSG, strlen(MSG.mtext)+1, 0);
        }    
        // Czekanie na nowych producentow
        if(msgrcv(producers, &MSG, MSG_SIZE, 1, IPC_NOWAIT) != -1)
        {
            types[num_of_producers++] = str2int(MSG.mtext);
            strcat(list_of_producers, MSG.mtext);
            strcat(list_of_producers, " ");
            printf("Dodaje nowego producenta...\n");
        }
    }
    return 0;
}