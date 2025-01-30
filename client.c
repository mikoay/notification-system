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

int main(int argc, char *argv[])
{
    if(argc != 2){
        printf("Uzycie: %s <klucz kolejki klienci-dyspozytor>\n", argv[0]);
        return -1;
    }
    int dispatcher_queue = msgget(str2int(argv[1]), IPC_CREAT | 0644);
    int client_id;
    struct Message MSG;
    int choice = 0, input;
    system("clear");
    printf("Podaj swoj identyfikator: ");
    scanf("%d", &client_id);
    sleep(1);
    system("clear");
    MSG.mtype=ADD_USER;
    strcpy(MSG.mtext, int2str(client_id));
    msgsnd(dispatcher_queue, &MSG, strlen(MSG.mtext)+1, 0);
    msgrcv(dispatcher_queue, &MSG, MSG_SIZE, VERIFY_USER, 0);
    if(str2int(MSG.mtext) == 0){
        printf("Klient o podanym ID juz istnieje\n");
        sleep(2);
        system("clear");
        return -1;
    }
    printf("Pomyslnie dodano nowego klienta\n");
    sleep(2);
    system("clear");
    while(1){
        show_menu(client_id);
        printf("Wybor: ");
        scanf("%d", &choice);
        sleep(1);
        system("clear");
        switch(choice){
            case EXIT:
                MSG.mtype=DELETE_USER;
                strcpy(MSG.mtext, int2str(client_id));
                msgsnd(dispatcher_queue, &MSG, strlen(MSG.mtext)+1, IPC_NOWAIT);
                return 0;
            break;
            case SUBSCRIBE:
                MSG.mtype=REQUEST_TYPES;
                strcpy(MSG.mtext, int2str(client_id));
                msgsnd(dispatcher_queue, &MSG, strlen(MSG.mtext)+1, 0);
                msgrcv(dispatcher_queue, &MSG, MSG_SIZE, SEND_TYPES, 0);
                if(strcmp(MSG.mtext, "BRAK") != 0){
                    printf("Dostepne typy powiadomien: %s\n", MSG.mtext);
                    printf("Wybor: ");
                    scanf("%d", &input);
                    system("clear");
                    if(strstr(MSG.mtext, int2str(input)) != NULL){
                        MSG.mtype=GET_TYPE;
                        strcpy(MSG.mtext, int2str(input));
                        msgsnd(dispatcher_queue, &MSG, strlen(MSG.mtext)+1, 0);
                        msgrcv(dispatcher_queue, &MSG, MSG_SIZE, VERIFY_TYPE, 0);
                        printf("%s", MSG.mtext);
                        sleep(2);
                        system("clear");
                    }else
                        printf("Nie ma takiego typu powiadomienia\n");
                }else{
                    printf("Brak dostepnych typow powiadomien\n");
                    sleep(2);
                    system("clear");
                }
            break;
            case UNSUBSCRIBE:
                int input;
                MSG.mtype=REQUEST_SUBSCRIBED;
                strcpy(MSG.mtext, int2str(client_id));
                msgsnd(dispatcher_queue, &MSG, strlen(MSG.mtext)+1, 0);
                msgrcv(dispatcher_queue, &MSG, MSG_SIZE, SEND_SUBSCRIBED, 0);
                if(strcmp(MSG.mtext, "BRAK") != 0){
                    printf("Subskrybowane typy powiadomien: %s\nWybor: ", MSG.mtext);
                    scanf("%d", &input);
                    sleep(2);
                    system("clear");
                    if(strstr(MSG.mtext, int2str(input)) == NULL){
                        printf("Nie ma takiego typu powiadomienia\n");
                        sleep(2);
                    }else{
                        MSG.mtype=GET_SUBSCRIBED;
                        strcpy(MSG.mtext, int2str(input));
                        msgsnd(dispatcher_queue, &MSG, strlen(MSG.mtext)+1, 0);
                        msgrcv(dispatcher_queue, &MSG, MSG_SIZE, VERIFY_SUBSCRIBED, 0);
                        printf("%s", MSG.mtext);
                        sleep(1);
                    }
                }else{
                    printf("Nie subskrybujesz zadnych powiadomien\n");
                    sleep(2);
                    system("clear");
                }
            break;
            case LIST:
                MSG.mtype=REQUEST_SUBSCRIBED;
                strcpy(MSG.mtext, int2str(client_id));
                msgsnd(dispatcher_queue, &MSG, strlen(MSG.mtext)+1, 0);
                msgrcv(dispatcher_queue, &MSG, MSG_SIZE, SEND_SUBSCRIBED, 0);
                if(strcmp(MSG.mtext, "BRAK") != 0){
                    printf("Subskrybowane typy powiadomien: %s\n", MSG.mtext);
                    sleep(3);
                    system("clear");
                }else{
                    printf("Nie subskrybujesz zadnych powiadomien\n");
                    sleep(2);
                    system("clear");
                }
            break;            
            case RECEIVE:
                printf("Trwa odbieranie powiadomień, wyślij sygnal SIGINT (Ctrl+C), aby przerwac odbieranie\n\n");
                MSG.mtype=START_RECEIVING;
                strcpy(MSG.mtext, int2str(client_id));
                msgsnd(dispatcher_queue, &MSG, strlen(MSG.mtext)+1, 0);
                listening = 1;
                signal(SIGINT, stop_listening);
                while(listening){
                    if(msgrcv(dispatcher_queue, &MSG, MSG_SIZE, client_id, IPC_NOWAIT) != -1)
                        printf("Powiadomienie: %s\n", MSG.mtext);
                }
                signal(SIGINT, SIG_DFL);
                MSG.mtype=STOP_RECEIVING;
                strcpy(MSG.mtext, int2str(client_id));
                msgsnd(dispatcher_queue, &MSG, strlen(MSG.mtext)+1, 0);
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