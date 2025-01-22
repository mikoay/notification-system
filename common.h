#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_SIZE 512
#define MAX_PRODUCERS 100

struct Message
{
    long mtype;
    char mtext[MSG_SIZE];
};

enum Tasks
{
    EXIT=0,
    SUBSCRIBE,
    UNSUBSCRIBE,
    RECEIVE,
    INIT
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