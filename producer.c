#include "common.h"

void show_menu(int id)
{
    printf("Producent nr %d.\n\n", id);
    printf("0. Wyjdz\n");
    printf("1. Nadaj komunikat (raz)\n");
    printf("2. Nadawaj komunikaty (ciągle)\n");
}

int notifying = 0;

void stop_notifying()
{
    notifying = 0;
}

int main(int argc, char *argv[])
{
    if(argc != 2){
        printf("Uzycie: %s <klucz kolejki producenci-dyspozytor>\n", argv[0]);
        return -1;
    }
    int dispatcher_queue = msgget(str2int(argv[1]), IPC_CREAT | 0644);
    struct Message MSG;
    int producer_id, notification_type;
    int choice = -1;
    char *buf = (char *)malloc(MSG_SIZE * sizeof(char));
    size_t buf_size = MSG_SIZE;
    system("clear");
    printf("Podaj identyfikator producenta: ");
    scanf("%d", &producer_id);
    printf("Typ wysylanego powiadomienia: ");
    scanf("%d", &notification_type);
    MSG.mtype=ADD_USER;
    strcpy(MSG.mtext, int2str(producer_id));
    strcat(MSG.mtext, " ");
    strcat(MSG.mtext, int2str(notification_type));
    msgsnd(dispatcher_queue, &MSG, strlen(MSG.mtext)+1, 0);
    msgrcv(dispatcher_queue, &MSG, MSG_SIZE, VERIFY_USER, 0);
    system("clear");
    if(str2int(MSG.mtext) == 0){
        printf("Producent o podanym ID lub typie nadawanych powiadomien juz istnieje\n");
        sleep(1);
        system("clear");
        return -1;
    }else{
        printf("Pomyslnie dodano nowego producenta\n");
    }
    sleep(1);
    system("clear");
    MSG.mtype=notification_type;
    while(1){
        show_menu(producer_id);
        printf("Wybor: ");
        scanf("%d", &choice);
        sleep(1);
        system("clear");
        switch (choice){
            case EXIT:
                MSG.mtype=DELETE_USER;
                strcpy(MSG.mtext, int2str(producer_id));
                msgsnd(dispatcher_queue, &MSG, strlen(MSG.mtext)+1, IPC_NOWAIT);
                return 0;
            break;
            case NOTIFY_SINGLE:
                printf("Nadasz teraz jedno powiadomienie typu %d\n\n", notification_type);
                while(getchar() != '\n');
                printf("Tresc powiadomienia: ");
                getline(&buf, &buf_size, stdin);
                buf[strlen(buf)-1]=0;
                strcpy(MSG.mtext, buf);
                msgsnd(dispatcher_queue, &MSG, strlen(MSG.mtext)+1, 0);
                sleep(1);
            break;
            case NOTIFY_STILL:
                printf("Trwa nadawanie powiadomien typu %d.\nWyslij sygnal SIGINT (Ctrl+C) i potwierdz Enterem, aby przerwac nadawanie\n\n", notification_type);
                while(getchar() != '\n');
                signal(SIGINT, stop_notifying);
                notifying = 1;
                while(notifying){
                    printf("Tresc powiadomienia: ");
                    getline(&buf, &buf_size, stdin);
                    buf[strlen(buf)-1]=0;
                    strcpy(MSG.mtext, buf);
                    msgsnd(dispatcher_queue, &MSG, strlen(MSG.mtext)+1, 0);
                }
                signal(SIGINT, SIG_DFL);
            break;
            default:
                printf("Niepoprawny wybor\n");
            break;
        }
        system("clear");
    }
    system("clear");
    return 0;
}