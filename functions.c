#include "main.h"
#include <time.h>
#include "receiver.h"
#include "walletID.h"

//Input: size of Sender HT
//Output: Sender HT
senderBucket **createHT(int bsize) {

    senderBucket **sb = calloc(bsize, sizeof(senderBucket));

    if(sb == NULL){

        printf("Memory allocation problem!Please return!\n");

        return NULL;
    }

    return sb;
}

//Input:    a string
//Output:   the number of words that contains
int countWords(char *string){
    int count = 0;
    int len = 0;
    if(string == NULL){
        return 0;
    }
    while(*string){
        if(*string == ' ') {
            len = 0;
        }
        else if(++len == 1)
            count++;
        string++;
    }
    return count;
}



//Input:    walletID and  HT size
//Output:   hash value using ASCII sum

int hash(char * walletID,int tableSize) {

    unsigned int sum = 0;

    int c;

    while((c = *walletID++)) sum +=c;

    return sum % tableSize;
}


//Input:    a wallet and sender HT with its size
//Output:   a pointer to the position of the walletID inside a bucket if it exists
//          else NULL

bucketItems* search(char *walletID,int hTsize,senderBucket **sb) {

    int index = hash(walletID,hTsize);
    int i = 0;

    senderBucket *bucket = sb[index];

    if(sb[index] == NULL) return NULL;

    char *pid = bucket->points[0]->walletID;

    while(bucket != NULL){
        while(bucket->points[i] != NULL){
            if(strcmp(pid,walletID) == 0){
                return bucket->points[i];
                //return i;
            }
            i++;
            if(bucket->points[i] != NULL){
                pid = bucket->points[i]->walletID;
            }
        }
        i=0;
        bucket = bucket->nextBucket;
    }
    return NULL;
}


//Input:    a date in format %H:%M
//Output:   conversion in seconds

time_t seconds(char *date){
    struct tm storage = {0};
    char *p=NULL;
    time_t retval=0;
    memset(&storage, 2, sizeof(struct tm));
    p = strptime(date,"%H:%M",&storage);
    if(p==NULL){
        retval=0;
    }
    else{
        retval=mktime(&storage);
    }
    return retval;
}


//Input:    2 dates in format %H:%M
//Output:   if date2 is later than date1 or not

int checkHours(char *date1,char *date2){
    time_t d1 = seconds(date1);
    time_t d2 = seconds(date2);
    if(d2>d1){
        return 1;
    }
    return 0;
}

//Input:    a walletID, a sender HT and dates
//Output:   returns the sum of the payments between the dates, else -1
//          the implementation of the command "findPayments"
//          if year1, year2 and(or) time1,time2 are not necessary for the command, they will be assigned to -1
int findPayments(char *walletID,senderBucket **sb,char *year1,char *year2,char *time1,char *time2,int tsize){
    int index = hash(walletID,tsize);
    if(sb[index] == NULL){
        return -1;
    }

    senderBucket *bucket = sb[index];
    int sum = 0;
    int i = 0;
    char *pid = bucket->points[0]->walletID;
    char *tempHour = NULL;
    char *date1 = NULL;
    char *date2 = NULL;
    char *nDate = NULL;
    while(bucket != NULL){
        while(bucket->points[i] != NULL){
            if(strcmp(pid,walletID) == 0){
                Node *n =  bucket->points[i]->node;
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
            if(bucket->points[i] != NULL){
                pid = bucket->points[i]->walletID;
            }
        }
        i = 0;
        bucket = bucket->nextBucket;
    }
    free(tempHour);
    free(date1);
    free(date2);
    free(nDate);
    return -1;
}


//Input:    a hash value and a sender HT
//Output:   if the hash value corresponds to a existed bucket
int bucketExists(senderBucket **sb, int index) {

    if (sb[index] == NULL) return 0;

    return 1;
}


//Input:    a hash value and a sender HT
//Output:   if the bucket has a next bucket
int hasNext(senderBucket **sb,int index) {

    if (sb[index]->nextBucket == NULL) return 0;

    return 1;
}


//Input:    a hash value and a sender HT
//Output:   if the hash value corresponds to a full bucket
int isFull(senderBucket **sb, int index) {

    senderBucket *temp = sb[index];

    while (temp != NULL) {

        for (int j = 0; j < temp->pointSize; j++) {

            if (temp->points[j] == NULL) return 0;
        }

        temp = temp->nextBucket;
    }

    return 1;
}


//Input:    a date in format %d-%m-%Y %H:%M
//Output:   time conversion to seconds
time_t to_seconds(char *date){
    struct tm storage = {0};
    char *p=NULL;
    time_t retval=0;
    memset(&storage, 0, sizeof(struct tm));
    p = strptime(date,"%d-%m-%Y %H:%M",&storage);

    if(p==NULL){
        retval=0;
    }
    else{
        retval=mktime(&storage);
    }
    return retval;
}

//Input:    2 dates in format %d-%m-%Y %H:%M
//Output:   if date2 is later than date1 or not

int checkTime(char *date1,char *date2){
    time_t d1=to_seconds(date1);
    time_t d2=to_seconds(date2);

    if(d2>d1){
        return 1;
    }
    return 0;
}


//Output:   get the current time in format %d-%m-%Y %H:%M
char *getCurrentTime(char *buffer){

    time_t rawtime;
    struct tm * timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,30,"%d-%m-%Y %H:%M",timeinfo);
    return buffer;
}


//Input:    a sender HT,its size and a struct user with the necessary informations about the transactions
//Output:   -
//          inserts a user into the senderHT
void insert(senderBucket **sb, user * info, int hTsize, int pSize,int count) {
    int index = hash(info->senderWalletId, hTsize);
    if (!bucketExists(sb, index)) {

        senderBucket * newBucket = createBucket(info, pSize,count);

        sb[index] = newBucket;

        return;
    }

    bucketItems *b = search(info->senderWalletId,hTsize,sb);
    if(b != NULL) {
        Node *temp = b->node;
        while (temp->next != NULL){
            temp = temp->next;
        }
        b->walletID = strdup(info->senderWalletId);
        Node *n = malloc(sizeof(Node));
        n->next = NULL;
        n->amount = info->amount;
        n->date = strdup(info->date);
        n->time = strdup(info->time);
        n->nTransaction = info->nTransaction;
        n->receiverID = strdup(info->receiverWalletId);
        temp->next = n;
        return;
    }
    if (!isFull(sb, index)) {
        senderBucket *temp = sb[index];
        while (temp->nextBucket != NULL) temp = temp->nextBucket;

        for (int i = 0; i < temp->pointSize; i++) {
            if (temp->points[i] == NULL) {
                temp->points[i] = malloc(sizeof(bucketItems));
                temp->points[i]->walletID = strdup(info->senderWalletId);
                Node *n = malloc(sizeof(Node));
                n->next = NULL;
                n->amount = info->amount;

                n->date = strdup(info->date);
                n->time = strdup(info->time);
                n->nTransaction = info->nTransaction;
                n->receiverID = strdup(info->receiverWalletId);
                temp->points[i]->node = n;
                return;
            }
        }
    }

    senderBucket * current = sb[index];

    while (current->nextBucket != NULL) current = current->nextBucket;

    current->nextBucket = createBucket(info, pSize,count);
}

//Input:    a user with the necessary information
//Output:   a pointer to newly created bucket

senderBucket *createBucket(user * info, int pSize,int count) {
    senderBucket *newBucket = malloc(sizeof(senderBucket));
    if (newBucket == NULL) {
        printf("Bad memory allocation!\n");
        return NULL;
    }

    newBucket->nextBucket = NULL;
    newBucket->pointSize = pSize;

    newBucket->points = calloc(newBucket->pointSize, sizeof(bucketItems));

    bucketItems *first = malloc(sizeof(bucketItems));

    first->walletID = strdup(info->senderWalletId);

    Node *n = malloc(sizeof(Node));
    n->next = NULL;
    n->amount = info->amount;
    n->time = strdup(info->time);
    n->date = strdup(info->date);
    //bitcoinOwner(tree,info->senderWalletId,count);
    n->receiverID = info->receiverWalletId;
    n->nTransaction = info->nTransaction;
    first->node = n;
    newBucket->points[0] = first;
    return newBucket;
}

//Input: a node list
//      free the allocated memory of the node list of each sender in the senderHT
void destroySenderList(Node *n){

    if(n->next != NULL) {
        destroySenderList(n->next);
    }
    free(n->time);
    free(n);
}


//Input: a pointer to a senderbucket
//      free the allocated memory
void destroySenderBucket(senderBucket *sb,int pointSize){

    if(sb->nextBucket != NULL) {

        destroySenderBucket(sb->nextBucket, pointSize);
    }

    for (int i = 0; i < pointSize; i++) {

        if (sb->points[i] != NULL) {

            destroySenderList(sb->points[i]->node);
            free(sb->points[i]->walletID);
            free(sb->points[i]);
        }
    }

    free(sb->points);
    free(sb);
}

//Input:    a sender HT
//          free the allocated memory

void destroyHT(senderBucket **sb,receiverBucket **rb,int sSize,int bSize,int pointSize){
    for(int i = 0; i < sSize; i++) {
        if(sb[i] != NULL) {
            destroySenderBucket(sb[i], pointSize);
        }
    }
    free(sb);

    for(int i = 0; i < bSize; i++){
        if(rb[i] != NULL) {
            destroyReceiverBucket(rb[i],pointSize);
        }
    }

    free(rb);
}


void append(Node *head,user *info){
    Node *temp = head;
    while(temp->next != NULL){
        temp = temp->next;
    }
    Node *n = malloc(sizeof(Node));
    n->next = NULL;
    n->amount = info->amount;
    n->time = strdup(info->time);
    n->date = strdup(info->date);
    n->receiverID = info->receiverWalletId;
    n->nTransaction = info->nTransaction;
    temp->next = n;

}


void print(senderBucket **sb, int hTsize) {
    for (int i = 0; i < hTsize; i++) {
        
        senderBucket *temp = sb[i];
        printf("Bucket %d\n\n", i);

        while(temp != NULL) {
            for (int j = 0; j < temp->pointSize ; j++) {
                if (temp->points[j] == NULL) break;

                printf("Wallet ID: %s\n\n", temp->points[j]->walletID);
                Node * t = temp->points[j]->node;
                while (t) {
                    printf("|Transaction %d|\n",t->nTransaction);
                    printf("Amount of the transaction: %d\n", t->amount);
                    printf("ReceiverID: %s\n", t->receiverID);
                    printf("Date: %s\n",t->date);
                    printf("Time: %s\n",t->time);
                    t = t->next;
                }
                printf("\n\n\n");
            }
            temp = temp->nextBucket;
        }
    }
}


