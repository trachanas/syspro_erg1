#include "main.h"
#include "receiver.h"
#include "walletID.h"


//Input:    the size of the HT
//Output:   the receiverHT
receiverBucket **createReceiverHT(int bsize){
    receiverBucket **rb = calloc(bsize, sizeof(receiverBucket));
    if(rb == NULL){
        printf("Memory allocation problem!Please return!\n");
        return NULL;
    }
    return rb;
}


//Input:    a transactionUser with the necessary information
//Output:   a pointer to newly created bucket

receiverBucket * createReceiverBucket ( transactionUser *info, int pSize){
    receiverBucket *newBucket = malloc(sizeof(receiverBucket));
    if (newBucket == NULL) {
        printf("Bad memory allocation!\n");
        return NULL;
    }
    newBucket->nextBucket = NULL;

    newBucket->pointSize = pSize;

    newBucket->points = calloc(newBucket->pointSize, sizeof(bucketItems));

    rBucketItems *first = malloc(sizeof(rBucketItems));

    first->receiverWalletId = strdup(info->receiverWalletId);

    rNode *r = malloc(sizeof(rNode));
    r->senderWalletId = strdup(info->senderWalletId);
    r->amount = info->amount;
    r->next = NULL;
    r->date = strdup(info->date);
    r->time = strdup(info->time);
    first->rnode = r;
    newBucket->points[0] = first;

    return newBucket;
}


//Input:    receiverHT and a hash value
//Output:   if bucket is full, or not
int isReceiverFull(receiverBucket **rb, int index){
    receiverBucket *temp = rb[index];

    while (temp != NULL) {

        for (int j = 0; j < temp->pointSize; j++) {

            if (temp->points[j] == NULL) return 0;
        }

        temp = temp->nextBucket;
    }

    return 1;
}

//
int receiverBucketExists(receiverBucket **rb, int index){
    if (rb[index] == NULL) return 0;
    return 1;
}



//Input:    a wallet and receiver HT with its size
//Output:   a pointer to the position of the walletID inside a bucket if it exists
//          else NULL
rBucketItems *searchReceiver(char *walletID,int hTsize,receiverBucket **rb){
    int index = hash(walletID,hTsize);
    int i = 0;

    receiverBucket *bucket = rb[index];
    if(bucket == NULL) return NULL;

    char *pid = bucket->points[0]->receiverWalletId;

    while(bucket != NULL){
        while(bucket->points[i] != NULL){
            if(strcmp(pid,walletID) == 0){
                return bucket->points[i];
                //return i;
            }
            i++;
            if(bucket->points[i] != NULL){
                pid = bucket->points[i]->receiverWalletId;
            }
        }
        i = 0;
        bucket = bucket->nextBucket;
    }
    return NULL;
}



//Input:    a receiver HT,its size and a struct transactionUser with the necessary informations about the transactions
//Output:   -
//          inserts a user into the receiverHT
void insertReceiver(receiverBucket **rb, transactionUser *info, int hTsize, int pSize){

    int index = hash(info->receiverWalletId, hTsize);

    if (!receiverBucketExists(rb, index)) {

        receiverBucket * newBucket = createReceiverBucket(info,pSize);

        rb[index] = newBucket;

        return;
    }

    rBucketItems *b = searchReceiver(info->receiverWalletId,hTsize,rb);

    if(b != NULL) {
        rNode *temp = b->rnode;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        b->receiverWalletId = strdup(info->receiverWalletId);
        rNode *n = malloc(sizeof(Node));
        n->next = NULL;
        n->amount = info->amount;
        n->date = strdup(info->date);
        n->time = strdup(info->time);
        n->nTransaction = info->nTransaction;
        n->senderWalletId = strdup(info->senderWalletId);
        temp->next = n;
        return;
    }


    if (!isReceiverFull(rb, index)) {
        receiverBucket *temp = rb[index];

        while (temp->nextBucket != NULL) temp = temp->nextBucket;

        for (int i = 0; i < temp->pointSize; i++) {

            if (temp->points[i] == NULL) {

                temp->points[i] = malloc(sizeof(bucketItems));

                temp->points[i]->receiverWalletId = strdup(info->receiverWalletId);

                rNode *r = malloc(sizeof(rNode));
                r->next = NULL;
                r->amount = info->amount;
                r->nTransaction = info->nTransaction;
                r->date = strdup(info->date);
                r->time = strdup(info->time);
                r->senderWalletId = strdup(info->senderWalletId);
                temp->points[i]->rnode = r;
                        //createList(info);

                return;
            }
        }
    }

    receiverBucket * current = rb[index];

    while (current->nextBucket != NULL) current = current->nextBucket;

    current->nextBucket = createReceiverBucket(info, pSize);
}

//Input:    a walletID, a receiver HT and dates
//Output:   returns the sum of the earnings between the dates, else -1
//          the implementation of the command "findEarnings"
//          if year1, year2 and(or) time1,time2 are not necessary for the command, they will be assigned to -1
int findEarnings(char *walletID,receiverBucket **rb,char *year1,char *year2,char *time1,char *time2,int tsize){

    int index = hash(walletID,tsize);

    if(rb[index] == NULL) return -1;

    receiverBucket *bucket = rb[index];
    int sum = 0;
    int i = 0;
    char *pid = bucket->points[0]->receiverWalletId;
    char *tempHour = NULL;
    char *date1 = NULL;
    char *date2 = NULL;
    char *nDate = NULL;

    while(bucket != NULL) {

        while(bucket->points[i] != NULL) {

            if (strcmp(pid, walletID) == 0) {

                rNode *n =  bucket->points[i]->rnode;

                while(n != NULL) {

                    tempHour = strdup(n->time);

                    if(strcmp(time1,"-1") == 0 || strcmp(time2,"-1") == 0){
                        sum += n->amount;
                    }else if(strcmp(year1,"-1") == 0 || strcmp(year2,"-1") == 0){
                        if (checkHours(time1, tempHour) && checkHours(tempHour, time2)) {
                            sum += n->amount;
                        }
                    }else{
                        date1 = malloc(sizeof(year1) + sizeof(time1));
                        sprintf(date1,"%s %s",year1,time1);
                        date2 = malloc(sizeof(year2) + sizeof(time2));
                        sprintf(date1,"%s %s",year2,time2);
                        nDate = malloc(sizeof(n->date) + sizeof(n->time));
                        sprintf(nDate,"%s %s",n->date,n->time);
                        if(checkTime(nDate,date1) && checkTime(date2,nDate)){
                            sum += n->amount;
                        }
                    }
                    n = n->next;
                }
                return sum;
            }
            i++;
            if(bucket->points[i] != NULL) {
                pid = bucket->points[i]->receiverWalletId;
            }
        }
        i=0;
        bucket = bucket->nextBucket;
    }
//    free(date1);
//    free(date2);
//    free(nDate);
//    free(tempHour);
    return -1;
}


void destroyReceiverList(rNode *r){
    if(r->next != NULL){
        destroyReceiverList(r->next);
    }
    free(r);
}

void destroyReceiverBucket(receiverBucket *rb,int pointSize){
    if(rb->nextBucket != NULL){
        destroyReceiverBucket(rb->nextBucket,pointSize);
    }

    for(int i = 0; i < pointSize; i++){
        if(rb->points[i] != NULL){
            destroyReceiverList(rb->points[i]->rnode);
            free(rb->points[i]->receiverWalletId);
            free(rb->points[i]);
        }
    }
    free(rb->points);
    free(rb);
}

void printReceiver(receiverBucket **rb, int hTsize){

    for (int i = 0; i < hTsize; i++) {

        receiverBucket *temp = rb[i];

        printf("Bucket %d\n\n", i);

        while(temp != NULL) {

            for (int j = 0; j < temp->pointSize ; j++) {

                if (temp->points[j] == NULL) break;

                printf("Wallet ID: %s\n", temp->points[j]->receiverWalletId);

                rNode * t = temp->points[j]->rnode;

                while (t) {
                    printf("|Transaction %d|\n",t->nTransaction);
                    printf("Amount of the transaction: %d\n", t->amount);
                    printf("SenderID: %s\n",t->senderWalletId);
                    printf("Date: %s\n",t->date);
                    printf("Time: %s\n",t->time);
                    t = t->next;
                }

                printf("\n");
            }

            temp = temp->nextBucket;
        }
    }
}

