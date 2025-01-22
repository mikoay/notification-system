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
    int dispatcher = msgget(123456, IPC_CREAT | 0644);
    struct Message MSG;
    int choice = 0, input;
    int client_id;
    char buf[MSG_SIZE]="";
    system("clear");
    printf("Podaj swoj identyfikator: ");
    scanf("%d", &client_id);
    sleep(1);
    system("clear");
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
                system("clear");
                MSG.mtype=1;
                strcpy(MSG.mtext, "1");
                msgsnd(dispatcher, &MSG, strlen(MSG.mtext)+1, 0);
                // Odebranie listy producentów
                msgrcv(dispatcher, &MSG, MSG_SIZE, 2, 0);
                printf("\nDostepne typy powiadomien: %s\n", MSG.mtext);
                printf("Wybor: ");
                scanf("%d", &input);
                printf("\n");
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