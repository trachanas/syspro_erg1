#ifndef UNTITLED_WALLETID_H
#define UNTITLED_WALLETID_H

#include "main.h"

typedef struct wallet{
    int **bitCoinID;
    char *walletID;
    int pointSize;
    int balance;
    struct wallet *next;
}wallet;

wallet **createWallet(int);
int walletExists(wallet **,int);
void insertWallet(wallet **,walletPerson *,int,int);
void printWallet(wallet **,int );
treeNode** readBalancesFile(FILE *,wallet **,int,int,treeNode **,int,int *);
int idExists(wallet **, user *,int ,int k);
int updateWallet(wallet **,user *,int ,int);
int walletStatus(char *,int ,wallet **);
void destroyWallet(wallet **,int );
void destroyWalletBucket(wallet *);

#endif