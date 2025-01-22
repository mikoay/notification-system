#include "common.h"

void show_menu(int id)
{
    printf("Klient nr %d.\n\n", id);
    printf("0. Wyjdz\n");
    printf("1. Zasubskrybuj powiadomienia\n");
    printf("2. Zrezygnuj z powiadomienia\n");
    printf("3. Wyswietlaj powiadomienia\n");
}

int main(void)
{
    struct Message MSG;
    int dispatcher = msgget(12345, IPC_CREAT | 0644);
    int choice = 0;
    int client_id;
    char buf[MSG_SIZE];
    system("clear");
    printf("Podaj swoj identyfikator: ");
    scanf("%d", &client_id);
    sleep(1);
    system("clear");
    strcpy(MSG.mtext, "init");
    MSG.mtype=1;
    msgsnd(dispatcher, &MSG, strlen(MSG.mtext)+1, 0);
    while(1)
    {
        show_menu(client_id);
        printf("Wybor: ");
        scanf("%d", &choice);
        switch(choice)
        {
            case EXIT:
                exit(0);
            break;
            case SUBSCRIBE:
                MSG.mtype=1;
                strcpy(MSG.mtext, "sub");
                msgsnd(dispatcher, &MSG, strlen(MSG.mtext)+1, 0);
                sprintf(buf, "%d", client_id);
                strcpy(MSG.mtext, buf);
                msgsnd(dispatcher, &MSG, strlen(MSG.mtext)+1, 0);
                msgrcv(dispatcher, &MSG, MSG_SIZE, 1, 0);
                printf("Dostepne typy powiadomien: %s\nWybor: ", MSG.mtext);
                scanf("%s", buf);
                while(strstr(MSG.mtext, buf) == NULL)
                {
                    printf("Nie ma takiego powiadomienia. Wybor: ");
                    scanf("%s", buf);
                }
            break;
            case UNSUBSCRIBE:
            break;
            case RECEIVE:
                while(1)
                {
                    if(msgrcv(dispatcher, &MSG, MSG_SIZE, 2, IPC_NOWAIT) != -1)
                    {
                        printf("Odebrano komunikat typu 2: %s\n", MSG.mtext);
                    }
                    if(msgrcv(dispatcher, &MSG, MSG_SIZE, 10, IPC_NOWAIT) != -1)
                    {
                        printf("Odebrano komunikat typu 10: %s\n", MSG.mtext);
                    }
                }
            break;
            default:
                printf("Niepoprawny wybor\n");
                sleep(1);
                system("clear");
            break;
        }
    }
    //system("clear");
    return 0;
}