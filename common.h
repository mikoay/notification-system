#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_SIZE 512
#define MAX_PRODUCERS 20
#define MAX_CLIENTS 20
#define MAX_SUBSCRIPTIONS 100

struct Message
{
    long mtype;
    char mtext[MSG_SIZE];
};

struct ClientSubs
{
    int client_id;
    int num_of_subscriptions;
    int subscriptions[MAX_SUBSCRIPTIONS];
    int listening;
};

enum ClientTasks
{
    EXIT=0,
    SUBSCRIBE,
    UNSUBSCRIBE,
    LIST,
    RECEIVE,
};

enum ProducerTasks
{
    NOTIFY_SINGLE=1,
    NOTIFY_STILL
};

int str2int(char text[])
{
    return atoi(text);
}

char* int2str(int value)
{
    char *string=(char *)malloc(MSG_SIZE * sizeof(char));
    snprintf(string, MSG_SIZE, "%d", value);
    return string;
}