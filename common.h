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
#define MAX_TYPES 50
#define MAX_SUBSCRIPTIONS 100

struct Message
{
    long mtype;
    char mtext[MSG_SIZE];
};

struct Client
{
    int id;
    int subscriptions[MAX_SUBSCRIPTIONS];
    int num_of_subscriptions;
    int listening;
};

struct Producer
{
    int id;
    int notification_type;
};

struct Types
{
    int notification_type;
    int clients[MAX_CLIENTS];
    int num_of_clients;
};

enum ClientOptions
{
    EXIT=0,
    SUBSCRIBE,
    UNSUBSCRIBE,
    LIST,
    RECEIVE,
};

enum ProducerOptions
{
    NOTIFY_SINGLE=1,
    NOTIFY_STILL
};

enum DispatcherTasks
{
    ADD_USER = 0xFFF0,
    DELETE_USER,
    VERIFY_USER,
    REQUEST_TYPES,
    SEND_TYPES,
    GET_TYPE,
    VERIFY_TYPE,
    REQUEST_SUBSCRIBED,
    SEND_SUBSCRIBED,
    GET_SUBSCRIBED,
    VERIFY_SUBSCRIBED,
    START_RECEIVING,
    STOP_RECEIVING
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