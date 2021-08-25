#ifndef UNTITLED_MAIN_H
#define UNTITLED_MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "tree.h"
#include "receiver.h"

typedef struct Node{
    int amount;
    char *date;
    int nTransaction;
    char *time;
    char *receiverID;
    treeNode *tree;
    struct Node *next;
} Node;

typedef struct bucketItems{
    char *walletID;
    Node *node;
} bucketItems;

typedef struct senderBucket {
    int pointSize;
    struct bucketItems **points;
    struct senderBucket *nextBucket;
} senderBucket;


typedef struct user1{
    int nTransaction;
    char *senderWalletId;
    char *receiverWalletId;
    int amount;
    char *date;
    char *time;
}user;

typedef struct walletPerson{
    int *bitcoinID;
    int listSize;
    char *walletID;
}walletPerson;

int hash(char *, int);
Node * insertNewNode(user *,bucketItems *,int);
int countWords(char *);
bucketItems *search(char *, int, senderBucket **);
senderBucket **createHT(int);
int bucketExists(senderBucket **, int);
int hasNext(senderBucket **, int);
int isFull(senderBucket **, int);
void insert(senderBucket **, user *, int, int,int);
void print(senderBucket **, int);
senderBucket * createBucket (user *, int,int);
time_t to_seconds(char *);
int checkTime(char *,char *);
char *getCurrentTime(char *);
int findPayments(char *,senderBucket **,char *,char *,char *,char *,int);
int checkHours(char *,char *);
time_t seconds(char *);
void destroySenderList(Node *);
void destroySenderBucket(senderBucket *,int );
void destroyHT(senderBucket **,receiverBucket **,int ,int ,int );

#endif //UNTITLED_MAIN_H
