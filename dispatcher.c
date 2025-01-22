#include "common.h"

void dispatch(int source, int destination, int type)
{
    struct Message message;
    if (msgrcv(source, &message, MSG_SIZE, type, IPC_NOWAIT) != -1)
    {
        msgsnd(destination, &message, strlen(message.mtext)+1, 0);
    }
}

int main(void)
{
    struct Message MSG;
    char list_of_producers[MSG_SIZE];
    int clients = msgget(12345, IPC_CREAT | 0644);
    // struct msqid_ds info;
    int types[MAX_PRODUCERS];
    int num_of_producers = 0;
    int producers = msgget(987654, IPC_CREAT | 0644);
    // msgctl(producers, IPC_STAT, &info);
    // msgsnd(clients, &MSG, strlen(MSG.mtext)+1, 0);
    while(1)
    {
        dispatch(producers, clients, 2);
        dispatch(producers, clients, 10);
        if(msgrcv(producers, &MSG, MSG_SIZE, 1, IPC_CREAT) != -1)
        {
            types[num_of_producers++] = str2int(MSG.mtext);
            printf("%d\n", str2int(MSG.mtext));
        }
    }
    return 0;
}