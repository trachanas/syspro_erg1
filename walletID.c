#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "walletID.h"
#include "main.h"
#include "math.h"
#include "receiver.h"

//Input:    the size of the wallet
//Output:   the wallet
wallet **createWallet(int tsize){
    wallet **newWallet = calloc(tsize, sizeof(wallet));
    if(newWallet == NULL){
        printf("Bad memory allocation!\n");
        return NULL;
    }
    return newWallet;
}


//Input:    a wallet and hash value
//Output:   if the entry corresponding to index exists or not
int walletExists(wallet **w,int index){
    if(w[index] == NULL){
        return 0;
    }
    return 1;
}


//Input:    a wallet, a user with essential information and a variable k
//Output:   if a senderWalletID or a receiverWalletID exists in the wallet
//          k = 1 for the sender and 1 if exists,else -1
//          else for for the receiver and 1 if exists, else -1
int idExists(wallet **w, user *u,int tsize,int k){
    char *tempString = NULL;
    if(k == 1){//ama einai sender
        tempString = strdup(u->senderWalletId);
    }else{
        tempString = strdup(u->receiverWalletId);
    }

    int index = hash(tempString,tsize);

    if(w[index] == NULL){
        return -1;
    }

    wallet *temp = w[index];
    while(temp != NULL){
        if(strcmp(temp->walletID,tempString) == 0){
            return 1;
        }
        temp = temp->next;
    }
    return -1;
}


//Input:    a name and a wallet
//Output:   return the wallets balance for the user "name",else -1(if name is not in the wallet)
//          The implementation of "walletStatus" command
int walletStatus(char *name,int tsize,wallet **w){

    int index = hash(name,tsize);

    if(w[index] == NULL){
        //ama den yparxei,na epistrepseis -1
        return -1;
    }

    wallet *temp = w[index];
    while(temp != NULL){
        if(strcmp(temp->walletID,name) == 0){
            return temp->balance;
        }
        temp = temp->next;
    }
    return -1;
}


//Input:    a pointer to a wallet bucket
//Output:   -
//          free allocated memory
void destroyWalletBucket(wallet *w){
    if(w->next != NULL){
        destroyWalletBucket(w->next);
    }
    for(int i = 0; i < w->pointSize; i++) {
        if (w->bitCoinID[i] != NULL) {
             free(w->bitCoinID[i]);
        }
    }
    free(w->bitCoinID);
    free(w->walletID);
    free(w);
}


//Input:    a wallet
//Output:   -
//          free allocated memory
void destroyWallet(wallet **w,int size){
    for(int i = 0; i < size; i++){
        if(w[i] != NULL){
            destroyWalletBucket(w[i]);
        }
    }
    free(w);
}

//Input:    a wallet,its size,a user with the essential information
//Output:   return 1 if the transaction was successful(message)
//          else 0 if something went wrong (negative amount,no money)(message)
//          It updates the data structure "wallet"
int updateWallet(wallet **w,user *u,int tsize,int bitcoinValue){
    int index1 = hash(u->senderWalletId,tsize);
    int index2 = hash(u->receiverWalletId,tsize);
    wallet *temp = w[index1];
    int retval = 0;
    wallet *k = w[index2];
    int remain = 0;
    while(temp!= NULL){
        if(strcmp(temp->walletID,u->senderWalletId) == 0){
            if(temp->balance >= u->amount && u->amount > 0){
                retval = 1;
                printf("<Transaction is OK!>\n");
                remain = u->amount;
                for(int i = 0; i < temp->pointSize; i++){
                    while(remain > 0){
                       if(remain >= temp->bitCoinID[i][1]){
                           remain -= temp->bitCoinID[i][1];
                           temp->bitCoinID[i][1] = 0;
                           break;
                       }
                       else{
                           temp->bitCoinID[i][1] -= remain;
                           remain = 0;
                           break;
                       }
                    }
                }
                temp->balance -= u->amount;
            }
            else{
                printf("<There is no money for the transaction!>\n");
                printf("<Invalid transaction!>\n");
                return 0;
            }
            break;
        }
        temp = temp->next;
    }
    while(k != NULL) {
        if (strcmp(k->walletID, u->receiverWalletId) == 0) {
            k->balance += u->amount;
        }
        k = k->next;
    }
    return retval;
}


//Input:    bitcoinbalancesFile.txt, a wallet
//Output:   a table with pointers to treeNodes
//          Actually,we are reading the file and we are initializing the wallet.
//          We keep the pointers(roots) for every treeNode(bitcoin)
treeNode **readBalancesFile(FILE * bitCoinBalancesFile,wallet **w, int pointSize, int senderHashtableNumOfEntries,treeNode **t,int bitcoinValue,int *count){
    char * line = NULL;
    size_t len;
    char * token = NULL;
    int c;
    walletPerson *person = malloc(sizeof(user));
    int i;
    while (getline(&line, &len, bitCoinBalancesFile) != -1) {
        i = 0;
        int size = countWords(line);
        token = strtok(line, " \n");

        person->bitcoinID = calloc(size-1,sizeof(int));

        for(; token != NULL; token = strtok(NULL," ")){
            if (isalpha(*token)) {
                person->walletID = strdup(token);
            }
            else{
                c = *count;
                if(bitcoinExists(t,atoi(token),c)){
                    printf("<ID: %d is used!>\n",atoi(token));
                    printf("<Invalid transaction!>\n");
                    size -=1;
                    continue;
                }
                t = realloc(t, (c+1) * sizeof(treeNode*));
                t[c] = malloc(sizeof(treeNode));
                t[c]->bitCoinID = atoi(token);
                t[c]->value = bitcoinValue;
                t[c]->walletID = strdup(person->walletID);
                person->bitcoinID[i] = atoi(token);

                (*count)++;
                i++;
            }
        }

        person->listSize = size-1;
        insertWallet(w,person,senderHashtableNumOfEntries,bitcoinValue);
    }
    free(person);
    fclose(bitCoinBalancesFile);
    return t;
}


//Input:    the wallet, a walletPerson with the necessary information
//Output:   -
//          We initialize the wallet
void insertWallet(wallet **w,walletPerson *info,int size,int bitcoinValue){
    int index = hash(info->walletID,size);

    wallet *newWallet = malloc(sizeof(wallet));
    newWallet->walletID = strdup(info->walletID);
    newWallet->next = NULL;
    newWallet->bitCoinID =  malloc(info->listSize * sizeof(int*));
    newWallet->pointSize = info->listSize;
    for(int i = 0; i < info->listSize; i++){
        //2d array with ids and remainder
        newWallet->bitCoinID[i] = malloc(2 * sizeof(int));
    }

    for(int i = 0; i < info->listSize;i++){
        newWallet->bitCoinID[i][0] = info->bitcoinID[i];
        newWallet->bitCoinID[i][1] = bitcoinValue;
    }

    newWallet->balance = info->listSize * bitcoinValue;
    if(w[index] == NULL){
        w[index] = newWallet;
        return;
    }

    wallet *temp = w[index];
    while(temp->next != NULL){
        temp = temp->next;
    }
    temp->next = newWallet;
}



void printWallet(wallet **w,int size){
    for(int i = 0; i < size; i++){
        wallet *temp = w[i];
        if(temp != NULL){
            printf("Bucket: %d\n",i);
            while(temp != NULL) {
                printf("WalletID: %s\n", temp->walletID);
                printf("Balance is: %d\n", temp->balance);
                for(int j = 0; j < temp->pointSize; j++){
                    //for(int k = 0; k < 2; k++){
                        printf("BitcoinID[%d] = %d\n",j,temp->bitCoinID[j][0]);
                        printf("Remain: %d\n", temp->bitCoinID[j][1]);
                    //}
                    printf("\n");
                }
                printf("\n");
                temp = temp->next;
            }
        }
    }
}