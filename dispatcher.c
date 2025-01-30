#include "common.h"

int running = 1;

void stop()
{
    running = 0;
    printf("\nZakonczenie procesu dyspozytora\n");
}

int is_producer_unique(int input_id, struct Producer producers[], int num_of_producers)
{
    for(int i=0; i<num_of_producers; i++){
        if(input_id == producers[i].id)
            return 0;
    }
    return 1;
}

int is_client_unique(int input_id, struct Client clients[], int num_of_clients)
{
    for(int i=0; i<num_of_clients; i++){
        if(input_id == clients[i].id)
            return 0;
    }
    return 1;
}

int is_type_unique(int input_type, struct Types types[], int num_of_types)
{
    for(int i=0; i<num_of_types; i++){
        if(input_type == types[i].notification_type)
            return 0;
    }
    return 1;
}

int main(int argc, char *argv[])
{
    if(argc != 3){
        printf("Uzycie: %s <klucz kolejki klienci-dyspozytor> <klucz kolejki producenci-dyspozytor>\n", argv[0]);
        return -1;
    }
    printf("Tutaj wyswietlane beda logi, wyslij sygnal SIGINT (Ctrl+C), aby przerwac prace\n\n");
    int clients_queue = msgget(str2int(argv[1]), IPC_CREAT | 0644);
    int producers_queue = msgget(str2int(argv[2]), IPC_CREAT | 0644);
    struct Message MSG;
    struct Client clients[MAX_CLIENTS];
    int num_of_clients = 0;
    struct Producer producers[MAX_PRODUCERS];
    int num_of_producers = 0;
    struct Types types[MAX_TYPES];
    int num_of_types = 0;
    int tmp, nt;
    signal(SIGINT, stop);
    while(running){   
        if(msgrcv(clients_queue, &MSG, MSG_SIZE, ADD_USER, IPC_NOWAIT) != -1){
            if(is_client_unique(str2int(MSG.mtext), clients, num_of_clients) == 1){
                clients[num_of_clients].id = str2int(MSG.mtext);
                clients[num_of_clients].num_of_subscriptions = 0;
                clients[num_of_clients].listening = 0;
                num_of_clients++;
                strcpy(MSG.mtext, "1");
                printf("Dodaje nowego klienta %d...\n", str2int(MSG.mtext));
            }else{
                strcpy(MSG.mtext, "0");
                printf("Nie mozna dodac klienta %d...\n", str2int(MSG.mtext));
            }
            MSG.mtype=VERIFY_USER;
            msgsnd(clients_queue, &MSG, strlen(MSG.mtext)+1, 0);
        }
        if(msgrcv(clients_queue, &MSG, MSG_SIZE, DELETE_USER, IPC_NOWAIT) != -1){
            printf("Usunieto klienta %d...\n", str2int(MSG.mtext));
            if(num_of_clients > 1){
                for(int i=0; i<num_of_clients; i++){
                    if(clients[i].id == str2int(MSG.mtext)){
                        clients[i].id = clients[num_of_clients-1].id;
                        clients[i].num_of_subscriptions = clients[num_of_clients-1].num_of_subscriptions;
                        memcpy(clients[i].subscriptions, clients[num_of_clients-1].subscriptions, sizeof(clients[num_of_clients-1].subscriptions));
                        clients[i].listening = clients[num_of_clients-1].listening;
                    }
                }
            }
            num_of_clients--;
        }
        if(msgrcv(clients_queue, &MSG, MSG_SIZE, REQUEST_TYPES, IPC_NOWAIT) != -1){
            printf("Odebrano prosbe o przeslanie listy dostepnych typow powiadomien od klienta %d...\n", str2int(MSG.mtext));
            tmp = str2int(MSG.mtext);
            if(num_of_producers == 0)
                strcpy(MSG.mtext, "BRAK");
            else{
                char list[MSG_SIZE]="";
                for(int i=0; i<num_of_producers; i++){
                    if(strstr(list, int2str(producers[i].notification_type)) == NULL){
                        strcat(list, int2str(producers[i].notification_type));
                        strcat(list, " ");
                    }
                }
                strcpy(MSG.mtext, list);
            }
            MSG.mtype=SEND_TYPES;
            msgsnd(clients_queue, &MSG, strlen(MSG.mtext)+1, 0);
        }   
        if(msgrcv(clients_queue, &MSG, MSG_SIZE, GET_TYPE, IPC_NOWAIT) != -1){
            printf("Klient %d. chce zasubskrybowac powiadomienie typu %d...\n", tmp, str2int(MSG.mtext));
            int found_client = 0;
            for(int i=0; i<num_of_clients; i++){
                if(clients[i].id == tmp){
                    found_client = 1;
                    int found_type = 0;
                    for(int j=0; j<clients[i].num_of_subscriptions; j++){
                        if(clients[i].subscriptions[j] == str2int(MSG.mtext)){
                            found_type = 1;
                            break;
                        }
                    }
                    if(found_type == 0){
                        clients[i].subscriptions[clients[i].num_of_subscriptions++]=str2int(MSG.mtext);
                        for(int j=0; j<num_of_types; j++){
                            if(str2int(MSG.mtext) == types[j].notification_type){
                                types[j].clients[types[j].num_of_clients]=tmp;
                                types[j].num_of_clients++;
                                break;
                            }
                        }
                        strcpy(MSG.mtext, "Zasubskrybowano nowe powiadomienie\n");
                    }else
                        strcpy(MSG.mtext, "Subskrybujesz juz powiadomienie tego typu\n");
                    break;
                }
            }
            if (found_client == 0)
                strcpy(MSG.mtext, "Nie znalezniono klienta o takim ID\n");
            MSG.mtype=VERIFY_TYPE;
            msgsnd(clients_queue, &MSG, strlen(MSG.mtext)+1, 0);
        }
        if(msgrcv(clients_queue, &MSG, MSG_SIZE, REQUEST_SUBSCRIBED, IPC_NOWAIT) != -1){
            printf("Odebrano prosbe o przeslanie listy subskrybowanych typow powiadomien od klienta %d...\n", str2int(MSG.mtext));
            int client = str2int(MSG.mtext);
            tmp = client;
            for(int i=0; i<num_of_clients; i++){
                if(clients[i].id == client){
                    if(clients[i].num_of_subscriptions == 0)
                        strcpy(MSG.mtext, "BRAK");
                    else{
                        char list_of_subscriptions[MSG_SIZE]="";
                        for(int j=0; j<clients[i].num_of_subscriptions; j++){
                            strcat(list_of_subscriptions, int2str(clients[i].subscriptions[j]));
                            strcat(list_of_subscriptions, " ");
                        }
                        strcpy(MSG.mtext, list_of_subscriptions);
                    }
                    break;
                }
            }
            MSG.mtype=SEND_SUBSCRIBED;
            msgsnd(clients_queue, &MSG, strlen(MSG.mtext)+1, 0);
        } 
        if(msgrcv(clients_queue, &MSG, MSG_SIZE, GET_SUBSCRIBED, IPC_NOWAIT) != -1){
            printf("Klient %d. chce zrezygnowac z subskrypcji powiadomienia typu %d...\n", tmp, str2int(MSG.mtext));
            for(int i=0; i<num_of_clients; i++){
                if(clients[i].id == tmp){
                    for(int j=0; j<clients[i].num_of_subscriptions; j++){
                        if(clients[i].subscriptions[j] == str2int(MSG.mtext)){
                            if(clients[i].num_of_subscriptions > 1)
                                clients[i].subscriptions[j] = clients[i].subscriptions[clients[i].num_of_subscriptions-1];
                            clients[i].num_of_subscriptions--;
                            break;
                        }
                    }
                    break;
                }
            }
            for(int i=0; i<num_of_types; i++){
                if(types[i].notification_type == str2int(MSG.mtext)){
                    for(int j=0; j<types[i].num_of_clients; j++){
                        if(types[i].clients[j] == tmp){
                            if(types[i].num_of_clients == 1)
                                types[i].num_of_clients--;
                            else{
                                types[i].clients[j]=types[i].clients[types[i].num_of_clients-1];
                                types[i].num_of_clients--;
                            }
                        }
                    }
                    break;
                }
            }
            MSG.mtype=VERIFY_SUBSCRIBED;
            strcpy(MSG.mtext, "Pomyslnie zrezygnowano z subskrypcji\n");
            msgsnd(clients_queue, &MSG, strlen(MSG.mtext)+1, 0);
        }
        if(msgrcv(producers_queue, &MSG, MSG_SIZE, ADD_USER, IPC_NOWAIT) != -1){
            char *producer_id=strtok(MSG.mtext, " ");
            char *notification_type=strtok(NULL, " ");
            if(is_producer_unique(str2int(MSG.mtext), producers, num_of_producers) == 1 && is_type_unique(str2int(notification_type), types, num_of_types) == 1){
                producers[num_of_producers].id=str2int(producer_id);
                producers[num_of_producers++].notification_type=str2int(notification_type);
                types[num_of_types].notification_type=str2int(notification_type);
                num_of_types++;
                strcpy(MSG.mtext, "1");
                printf("Dodaje nowego producenta %d...\n", str2int(producer_id));
            }else{
                strcpy(MSG.mtext, "0");
                printf("Nie mozna dodac producenta %d...\n", str2int(producer_id));
            }
            MSG.mtype=VERIFY_USER;
            msgsnd(producers_queue, &MSG, strlen(MSG.mtext)+1, 0);
        }
        if(msgrcv(producers_queue, &MSG, MSG_SIZE, DELETE_USER, IPC_NOWAIT) != -1){
            printf("Usunieto producenta %d...\n", str2int(MSG.mtext));
            if(num_of_producers > 1){
                for(int i=0; i<num_of_producers; i++){
                    if(producers[i].id == str2int(MSG.mtext)){
                        producers[i].id = producers[num_of_producers-1].id;
                        nt=producers[i].notification_type;
                        producers[i].notification_type = producers[num_of_producers-1].notification_type;
                    }
                }
            }else
                nt = producers[0].notification_type;
            num_of_producers--;
            for(int i=0; i<num_of_clients; i++){
                if(clients[i].num_of_subscriptions == 1 && clients[i].subscriptions[0] == nt)
                    clients[i].num_of_subscriptions--;
                else{
                    for(int j=0; j<clients[i].num_of_subscriptions; j++){
                        if(clients[i].subscriptions[j] == nt){
                            clients[i].subscriptions[j]=clients[i].subscriptions[clients[i].num_of_subscriptions-1];
                            clients[i].num_of_subscriptions--;
                        }
                    }
                }
            }
            for(int i=0; i<num_of_types; i++){
                if(types[i].notification_type == nt){
                    if(num_of_types > 1){
                        types[i].notification_type = types[num_of_types-1].notification_type;
                        types[i].num_of_clients = types[num_of_types-1].num_of_clients;
                        memcpy(types[i].clients, types[num_of_types-1].clients, sizeof(types[num_of_types-1].clients));
                    }
                    num_of_types--;
                    break;
                }
            }
        }
        if(msgrcv(clients_queue, &MSG, MSG_SIZE, START_RECEIVING, IPC_NOWAIT) != -1){
            printf("Klient %d. rozpoczal odbieranie powiadomien...\n", str2int(MSG.mtext));
            for(int i=0; i<num_of_clients; i++){
                if(clients[i].id == str2int(MSG.mtext)){
                    clients[i].listening = 1;
                    break;
                }   
            } 
        }
        if(msgrcv(clients_queue, &MSG, MSG_SIZE, STOP_RECEIVING, IPC_NOWAIT) != -1){
            printf("Klient %d. zakonczyl odbieranie powiadomien...\n", str2int(MSG.mtext));
            for(int i=0; i<num_of_clients; i++){
                if(clients[i].id == str2int(MSG.mtext)){
                    clients[i].listening = 0;
                    break;
                }   
            } 
        }
        for(int i=0; i<num_of_types; i++){
            if(msgrcv(producers_queue, &MSG, MSG_SIZE, types[i].notification_type, IPC_NOWAIT) != -1){
                for(int j=0; j<types[i].num_of_clients; j++){
                    MSG.mtype=types[i].clients[j];
                    for(int k=0; k<num_of_clients; k++){
                        if(types[i].clients[j] == clients[k].id){
                            if(clients[k].listening == 1)
                                msgsnd(clients_queue, &MSG, strlen(MSG.mtext)+1, IPC_NOWAIT);
                            break;
                        }
                    }
                }
            }
        }
    }
    msgctl(clients_queue, IPC_RMID, NULL);
    msgctl(producers_queue, IPC_RMID, NULL);
    return 0;
}