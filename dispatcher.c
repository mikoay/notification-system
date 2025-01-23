#include "common.h"

void broadcast(int source, int destination, int type, int client_channel)
{
    struct Message message;
    if (msgrcv(source, &message, MSG_SIZE, type, IPC_NOWAIT) != -1)
    {
        message.mtype=client_channel;
        msgsnd(destination, &message, strlen(message.mtext)+1, 0);
    }
}

int main(void)
{
    int clients = msgget(123456, IPC_CREAT | 0644);
    int producers = msgget(987654, IPC_CREAT | 0644);
    struct Message MSG;
    struct ClientSubs clients_subscriptions[MAX_CLIENTS];
    int tmp;
    char list_of_types[MSG_SIZE]="";
    int types[MAX_PRODUCERS];
    int num_of_types = 0;
    int num_of_clients = 0;
    while(1)
    {   
        // Oczekiwanie na nowych klientow
        if(msgrcv(clients, &MSG, MSG_SIZE, 0xFFF0, IPC_NOWAIT) != -1)
        {
            printf("Dodaje nowego klienta...\n");
            clients_subscriptions[num_of_clients].client_id = str2int(MSG.mtext);
            clients_subscriptions[num_of_clients].num_of_subscriptions = 0;
            clients_subscriptions[num_of_clients].listening = 0;
            num_of_clients++;
        }
        // Oczekiwanie na prosbe o liste dostepnych typow powiadomien
        if(msgrcv(clients, &MSG, MSG_SIZE, 0xFFF1, IPC_NOWAIT) != -1)
        {
            printf("Odebrano prosbe o przeslanie listy dostepnych typow powiadomien...\n");
            tmp = str2int(MSG.mtext);
            strcpy(MSG.mtext, list_of_types);
            MSG.mtype=0xFFF2;
            msgsnd(clients, &MSG, strlen(MSG.mtext)+1, 0);
        }   
        // Oczekiwanie na przeslanie numeru powiadomienia, ktore klient chce zasubskrybowac 
        if(msgrcv(clients, &MSG, MSG_SIZE, 0xFFF3, IPC_NOWAIT) != -1)
        {
            printf("Klient %d. chce zasubskrybowac powiadomienie typu %d...\n", tmp, str2int(MSG.mtext));
            int found_client = 0;
            for(int i=0; i<num_of_clients; i++)
            {
                if(clients_subscriptions[i].client_id == tmp)
                {
                    found_client = 1;
                    int found_type = 0;
                    for(int j=0; j<clients_subscriptions[i].num_of_subscriptions; j++)
                    {
                        if(clients_subscriptions[i].subscriptions[j] == str2int(MSG.mtext))
                        {
                            found_type = 1;
                            break;
                        }
                    }
                    if(found_type == 0)
                    {
                        clients_subscriptions[i].subscriptions[clients_subscriptions[i].num_of_subscriptions++]=str2int(MSG.mtext);
                        strcpy(MSG.mtext, "Zasubskrybowano nowe powiadomienie");
                    }
                    else
                    {
                        strcpy(MSG.mtext, "Subskrybujesz juz powiadomienie tego typu");
                    }
                    break;
                }
            }
            if (found_client == 0)
            {
                strcpy(MSG.mtext, "Nie znalezniono klienta o takim ID");
            }
            MSG.mtype=0xFFF4;
            msgsnd(clients, &MSG, strlen(MSG.mtext)+1, 0);
        }
        // Oczekiwanie na prosbe o przeslanie listy subskrybowanych przez klienta typow powiadomien
        if(msgrcv(clients, &MSG, MSG_SIZE, 0xFFF5, IPC_NOWAIT) != -1)
        {
            printf("Odebrano prosbe o przeslanie listy subskrybowanych typow powiadomien...\n");
            int client = str2int(MSG.mtext);
            tmp = client;
            for(int i=0; i<num_of_clients; i++)
            {
                if(clients_subscriptions[i].client_id == client)
                {
                    char list_of_subscriptions[MSG_SIZE]="";
                    for(int j=0; j<clients_subscriptions[i].num_of_subscriptions; j++)
                    {
                        strcat(list_of_subscriptions, int2str(clients_subscriptions[i].subscriptions[j]));
                        strcat(list_of_subscriptions, " ");
                    }
                    strcpy(MSG.mtext, list_of_subscriptions);
                    break;
                }
            }
            MSG.mtype=0xFFF6;
            msgsnd(clients, &MSG, strlen(MSG.mtext)+1, 0);
        } 
        // Oczekiwanie na przeslanie typu powiadomienia, z ktorego klienta chce zrezygnowac
        if(msgrcv(clients, &MSG, MSG_SIZE, 0xFFF7, IPC_NOWAIT) != -1)
        {
            printf("Klient %d. chce zrezygnowac z subskrypcji powiadomienia typu %d...\n", tmp, str2int(MSG.mtext));
            for(int i=0; i<num_of_clients; i++)
            {
                if(clients_subscriptions[i].client_id == tmp)
                {
                    for(int j=0; j<clients_subscriptions[i].num_of_subscriptions; j++)
                    {
                        if(clients_subscriptions[i].subscriptions[j] == str2int(MSG.mtext))
                        {
                            if(clients_subscriptions[i].num_of_subscriptions > 1)
                            {
                                clients_subscriptions[i].subscriptions[j] = clients_subscriptions[i].subscriptions[clients_subscriptions[i].num_of_subscriptions-1];
                            }
                            clients_subscriptions[i].num_of_subscriptions--;
                            strcpy(MSG.mtext, "Pomyslnie zrezygnowano z subskrypcji");
                            break;
                        }
                    }
                    break;
                }
            }
            MSG.mtype=0xFFF8;
            msgsnd(clients, &MSG, strlen(MSG.mtext)+1, 0);
        }
        // Oczekiwanie na nowych producentow
        if(msgrcv(producers, &MSG, MSG_SIZE, 0xFFF0, IPC_NOWAIT) != -1)
        {
            types[num_of_types++] = str2int(MSG.mtext);
            strcat(list_of_types, MSG.mtext);
            strcat(list_of_types, " ");
            printf("Dodaje nowego producenta...\n");
        }
        // Oczekiwanie na prosbe o rozglaszanie
        if(msgrcv(clients, &MSG, MSG_SIZE, 0xFFF9, IPC_NOWAIT) != -1)
        {
            printf("Klient %d. rozpoczal odbieranie powiadomien...\n", str2int(MSG.mtext));
            for(int i=0; i<num_of_clients; i++)
            {
                if(clients_subscriptions[i].client_id == str2int(MSG.mtext))
                {
                    clients_subscriptions[i].listening = 1;
                    break;
                }   
            } 
        }
        // Oczekiwanie na prosbe o zakonczenie rozglaszania
        if(msgrcv(clients, &MSG, MSG_SIZE, 0xFFFA, IPC_NOWAIT) != -1)
        {
            printf("Klient %d. zakonczyl odbieranie powiadomien...\n", str2int(MSG.mtext));
            for(int i=0; i<num_of_clients; i++)
            {
                if(clients_subscriptions[i].client_id == str2int(MSG.mtext))
                {
                    clients_subscriptions[i].listening = 0;
                    break;
                }   
            } 
        }
        // Rozglaszanie
        for(int i=0; i<num_of_clients; i++)
        {
            if(clients_subscriptions[i].listening == 1)
            {
                for(int j=0; j<clients_subscriptions[i].num_of_subscriptions; j++)
                {
                    broadcast(producers, clients, clients_subscriptions[i].subscriptions[j], clients_subscriptions[i].client_id);
                }
            }
        }
    }
    return 0;
}