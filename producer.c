#include "common.h"

int main(void)
{
    struct Message MSG;
    int msgid = msgget(987654, IPC_CREAT | 0644);
    system("clear");
    int producer_id, notification_type;
    char *buf = (char *)malloc(MSG_SIZE * sizeof(char));
    size_t buf_size = MSG_SIZE;
    printf("Podaj identyfikator producenta: ");
    scanf("%d", &producer_id);
    printf("Typ wysylanego powiadomienia: ");
    scanf("%d", &notification_type);
    sleep(1);
    system("clear");
    strcpy(MSG.mtext, int2str(notification_type));
    MSG.mtype=1;
    msgsnd(msgid, &MSG, strlen(MSG.mtext)+1, 0);
    MSG.mtype=notification_type;
    while(getchar() != '\n');
    while(1)
    {
        printf("Podaj tresc komunikatu: ");
        getline(&buf, &buf_size, stdin);
        buf[strlen(buf)-1]=0;
        strcpy(MSG.mtext, buf);
        msgsnd(msgid, &MSG, strlen(MSG.mtext)+1, 0);
    }
    return 0;
}