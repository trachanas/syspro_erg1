#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
#include "walletID.h"

treeNode *createTree(char *id,int bitcoin,int value){
    treeNode *root = malloc(sizeof(treeNode));
    if(root == NULL){
        printf("Bad memory allocation for the Tree\n");
        return NULL;
    }
    root->value = value;
    root->transaction = NULL;
    root->remain = NULL;
    root->walletID = strdup(id);
    root->bitCoinID = bitcoin;
    return root;
}

treeNode *bitcoinExists(treeNode **t,int token,int size){
    for(int i = 0; i < size; i++){
        if(t[i]->bitCoinID == token){
            return t[i];
        }
    }
    return 0;
}

void bitcoinOwner(treeNode **t,char *walletID,int size){
    for(int i = 0; i < size; i++){
        if(strcmp(t[i]->walletID, walletID) == 0){
            printf("User: %s has the bitcoin %d \n",t[i]->walletID,t[i]->bitCoinID);
        }
    }
}


treeNode* newTransaction(treeNode *root,char *id,int tvalue){

    treeNode *newTransaction = malloc(sizeof(treeNode));
    if(newTransaction == NULL){
        printf("Bad memory allocation for a new transaction!\n");
        return NULL;
    }
    newTransaction->value = tvalue;
    newTransaction->transaction = NULL;
    newTransaction->remain = NULL;
    newTransaction->walletID = strdup(id);

    root->transaction = newTransaction;
    return newTransaction;
}

treeNode* newRemainder(treeNode *root,char *id,int tvalue){
    treeNode *newRemainder = malloc(sizeof(treeNode));
    if(newRemainder == NULL){
        printf("Bad memory allocation for a new remainder!\n");
        return NULL;
    }
    newRemainder->transaction = NULL;
    newRemainder->remain = NULL;
    newRemainder->walletID = strdup(id);
    newRemainder->value = root->value - tvalue;
    return newRemainder;
}



void printTree(treeNode *root){
    if(root != NULL){
        printf("%d\n",root->value);
        printTree(root->transaction);
        printTree(root->remain);
    }
}




