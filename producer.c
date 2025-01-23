#include "common.h"

void show_menu(int id)
{
    printf("Producent nr %d.\n\n", id);
    printf("0. Wyjdz\n");
    printf("1. Nadaj komunikat\n");
    printf("2. Nadawaj komunikaty\n");
}

int notifying = 0;

void stop_notifying()
{
    notifying = 0;
}

int main(void)
{
    int dispatcher = msgget(987654, IPC_CREAT | 0644);
    int choice = -1;
    struct Message MSG;
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
    MSG.mtype=0xFFF0;
    msgsnd(dispatcher, &MSG, strlen(MSG.mtext)+1, 0);
    MSG.mtype=notification_type;
    while(1)
    {
        show_menu(producer_id);
        printf("Wybor: ");
        scanf("%d", &choice);
        sleep(1);
        system("clear");
        switch (choice)
        {
        case EXIT:
            exit(0);
        break;
        case NOTIFY_SINGLE:
        break;
        case NOTIFY_STILL:
            printf("Trwa nadawanie komunikatow, wyślij sygnal SIGINT (Ctrl+C), aby przerwac nadawanie\n\n");
            while(getchar() != '\n');
            signal(SIGINT, stop_notifying);
            notifying = 1;
            while(notifying)
            {
                printf("Podaj tresc komunikatu typu %d: ", notification_type);
                getline(&buf, &buf_size, stdin);
                buf[strlen(buf)-1]=0;
                strcpy(MSG.mtext, buf);
                msgsnd(dispatcher, &MSG, strlen(MSG.mtext)+1, 0);
            }
            signal(SIGINT, SIG_DFL);
        break;
        default:
            printf("Niepoprawny wybor\n");
        break;
        }
    }
    return 0;
}