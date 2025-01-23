#include "common.h"

void show_menu(int id)
{
    printf("Klient nr %d.\n\n", id);
    printf("0. Wyjdz\n");
    printf("1. Zasubskrybuj powiadomienia\n");
    printf("2. Zrezygnuj z powiadomienia\n");
    printf("3. Pokaz subskrybowane powiadomienia\n");
    printf("4. Wyswietlaj powiadomienia\n");
}

int listening = 0;

void stop_listening()
{
    listening = 0;
}

int main(void)
{
    int dispatcher = msgget(123456, IPC_CREAT | 0644);
    int client_id;
    struct Message MSG;
    int choice = 0, input;
    char buf[MSG_SIZE]="";
    system("clear");
    printf("Podaj swoj identyfikator: ");
    scanf("%d", &client_id);
    MSG.mtype=0xFFF0;
    strcpy(MSG.mtext, int2str(client_id));
    msgsnd(dispatcher, &MSG, strlen(MSG.mtext)+1, 0);
    sleep(1);
    system("clear");
    while(1)
    {
        show_menu(client_id);
        printf("Wybor: ");
        scanf("%d", &choice);
        sleep(1);
        system("clear");
        switch(choice)
        {
            case EXIT:
                exit(0);
            break;
            case SUBSCRIBE:
                MSG.mtype=0xFFF1;
                strcpy(MSG.mtext, int2str(client_id));
                // Wyslanie prosby o przeslanie listy dostepnych typow powiadomien
                msgsnd(dispatcher, &MSG, strlen(MSG.mtext)+1, 0);
                // Odebranie listy dostepnych typow powiadomien
                msgrcv(dispatcher, &MSG, MSG_SIZE, 0xFFF2, 0);
                printf("Dostepne typy powiadomien: %s\n", MSG.mtext);
                printf("Wybor: ");
                scanf("%d", &input);
                sleep(1);
                system("clear");
                if(strstr(MSG.mtext, int2str(input)) != NULL)
                {
                    MSG.mtype=0xFFF3;
                    strcpy(MSG.mtext, int2str(input));
                    msgsnd(dispatcher, &MSG, strlen(MSG.mtext)+1, 0);
                    msgrcv(dispatcher, &MSG, MSG_SIZE, 0xFFF4, 0);
                    printf("%s", MSG.mtext);
                    sleep(1);
                    system("clear");
                }
                else
                {
                    printf("Nie ma takiego typu powiadomienia\n");
                }
            break;
            case UNSUBSCRIBE:
                int input;
                MSG.mtype=0xFFF5;
                strcpy(MSG.mtext, int2str(client_id));
                // Wyslanie prosby o przeslanie listy subskrybowanych typow powiadomien
                msgsnd(dispatcher, &MSG, strlen(MSG.mtext)+1, 0);
                // Czekanie na liste subskrybowanych typow powiadomien
                msgrcv(dispatcher, &MSG, MSG_SIZE, 0xFFF6, 0);
                printf("Subskrybowane typy powiadomien: %s\nWybor: ", MSG.mtext);
                scanf("%d", &input);
                sleep(1);
                system("clear");
                if(strstr(MSG.mtext, int2str(input)) != NULL)
                {
                    MSG.mtype=0xFFF7;
                    strcpy(MSG.mtext, int2str(input));
                    msgsnd(dispatcher, &MSG, strlen(MSG.mtext)+1, 0);
                    msgrcv(dispatcher, &MSG, MSG_SIZE, 0xFFF8, 0);
                    printf("%s", MSG.mtext);
                }
                else
                {
                    printf("Nie ma takiego typu powiadomienia\n");
                }
                sleep(1);
                system("clear");
            break;
            case LIST:
                MSG.mtype=0xFFF5;
                strcpy(MSG.mtext, int2str(client_id));
                // Wyslanie prosby o przeslanie listy subskrybowanych typow powiadomien
                msgsnd(dispatcher, &MSG, strlen(MSG.mtext)+1, 0);
                // Czekanie na liste subskrybowanych typow powiadomien
                msgrcv(dispatcher, &MSG, MSG_SIZE, 0xFFF6, 0);
                printf("Subskrybowane typy powiadomien: %s", MSG.mtext);
                sleep(1);
                printf("Wcisnij Enter, aby kontynuowac...");
                while(getchar() != '\n');
                system("clear");
            break;            
            case RECEIVE:
                printf("Trwa odbieranie powiadomień, wyślij sygnal SIGINT (Ctrl+C), aby przerwac odbieranie\n\n");
                MSG.mtype=0xFFF9;
                strcpy(MSG.mtext, int2str(client_id));
                msgsnd(dispatcher, &MSG, strlen(MSG.mtext)+1, 0);
                listening = 1;
                signal(SIGINT, stop_listening);
                while(listening)
                {
                    if(msgrcv(dispatcher, &MSG, MSG_SIZE, client_id, IPC_NOWAIT) != -1)
                    {
                        printf("Odebrano komunikat: %s\n", MSG.mtext);
                    }
                }
                signal(SIGINT, SIG_DFL);
                MSG.mtype=0xFFFA;
                strcpy(MSG.mtext, int2str(client_id));
                msgsnd(dispatcher, &MSG, strlen(MSG.mtext)+1, 0);
            break;
            default:
                printf("Niepoprawny wybor\n");
            break;
        }
        sleep(1);
        system("clear");
    }
    //system("clear");
    return 0;
}