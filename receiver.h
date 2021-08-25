#ifndef UNTITLED_RECEIVER_H
#define UNTITLED_RECEIVER_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct rNode{
    char *senderWalletId;
    char *date;
    char *time;
    int amount;
    int nTransaction;
    struct rNode *next;
}rNode;

typedef struct rBucketItems{
    char *receiverWalletId;
    rNode *rnode;
}rBucketItems;

typedef struct receiverBucket{
    int pointSize;
    struct rBucketItems **points;
    struct receiverBucket *nextBucket;
}receiverBucket;


typedef struct transactionUser{
    int nTransaction;
    char *senderWalletId;
    char *receiverWalletId;
    int amount;
    char *date;
    char *time;
}transactionUser;


receiverBucket **createReceiverHT(int);
int receiverBucketExists(receiverBucket **, int);
int isReceiverFull(receiverBucket **, int);
void printReceiver(receiverBucket **, int);
void insertReceiver(receiverBucket **, transactionUser *, int, int);
rNode *createReceiverList(transactionUser *);
rBucketItems *searchReceiver(char *,int ,receiverBucket **);
receiverBucket * createReceiverBucket (transactionUser *, int);
void readTransactionFileAndInsert (FILE *, receiverBucket **, int, int);
int findEarnings(char *,receiverBucket **,char *,char *,char *,char *,int );
void destroyReceiverList(rNode *);
void destroyReceiverBucket(receiverBucket *,int );

#endif