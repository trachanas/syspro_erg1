#include "main.h"
#include <time.h>
#include "receiver.h"
#include "walletID.h"
#include <time.h>

int  maxN = -1;

void readTransactionsFile(FILE *transactionFile,senderBucket **sb,receiverBucket **rb,int sSize,int bSize,int pointSize,wallet ** w,int bitcoinValue,treeNode **tree,int count,int *maxN){
    char *line = NULL;
    int c1 = 0;
    char *current_date = "00-00-0000 00:00";
    char *old_date = "00-00-0000 00:00";
    size_t len;
    char *token = NULL;

    transactionUser *t = malloc(sizeof(transactionUser));
    user  *u = malloc(sizeof(user));
    int flag;
    while (getline(&line, &len, transactionFile) != -1) {
        token = strtok(line, " \n");
        flag = 0;
        while (token != NULL) {
            c1 = 0;
            if(flag == 0){
                u->nTransaction = atoi(token);
                t->nTransaction = atoi(token);
                if(u->nTransaction > *maxN){
                    (*maxN) = u->nTransaction;
                }
                flag++;
            }
            else if(flag == 1){
                u->senderWalletId = strdup(token);
                t->senderWalletId = strdup(token);
                flag++;
            }
            else if(flag == 2){
                u->receiverWalletId = strdup(token);
                t->receiverWalletId = strdup(token);
                flag++;
            }
            else if(flag == 3){
                if(atoi(token) <= 0){
                    printf("<Amount of transaction must be positive number!>\n");
                    printf("<Invalid transaction!>\n");
                    c1 = 1;
                    break;
                }
                else{
                    c1 = 0;
                    u->amount = atoi(token);
                    t->amount = atoi(token);
                }
                flag++;
            }
            else if(flag == 4){
                u->date = strdup(token);
                t->date = strdup(token);
                flag++;
            }else if(flag == 5){
                u->time = strdup(token);
                t->time = strdup(token);
                flag++;
            }
            token = strtok(NULL, " \n");
        }

        if(strcmp(u->senderWalletId,u->receiverWalletId) == 0){
            printf("<Sender is the same person with the receiver!>\n");
            break;
        }

        current_date = malloc(sizeof(u->time) + sizeof(u->date));
        sprintf(current_date,"%s %s",u->date ,u->time);
        if(!checkTime(old_date,current_date)){
            printf("<Invalid date!>\n");
            printf("<Invalid transaction!>\n");
            break;
        }else{
            old_date = strdup(current_date);
        }

        if(idExists(w,u,sSize,1) + idExists(w,u,sSize,2)){
            if(c1 == 0 && updateWallet(w,u,sSize,bitcoinValue)){
                insert(sb,u,sSize,pointSize,count);
                insertReceiver(rb,t, bSize, pointSize);
            }
        }
    }
    free(u);
    free(t);
    free(current_date);
    fclose(transactionFile);
}


void printTreeNode(int count,treeNode** t){
    for(int i = 0; i<count; i++){
        printf("%d",t[i]->bitCoinID);
        printf(" %s\n",t[i]->walletID);
    }
}


int main(int argc, char **argv) {
    int c;
    //int maxN = -1;  //max number of transaction
    int i,k;        //variables for loops

    /*variables for the data structures*/
    int bitcoinValue = 0;
    int senderHashtableNumOfEntries = 0;
    int receiverHashtableNumOfEntries = 0;
    int bucketSize = 0;
    int pointSize; //size of the array inside a bucket
    int nWords; //number of words of each command

    /*helpful variables for string manipulation(strtok,getline etc)*/
    char commands[1000];
    char *arr[150];
    char *tempArr[150];
    char *tk1 = NULL;
    char *tk2 = NULL;
    FILE *input = stdin;
    char *cmd;
    for(int k = 0; k < 150; k++){
        arr[k] = NULL;
    }
    char *buffer = malloc(30 *sizeof(char));
    char *current_date = "00-00-0000 00:00";
    char *old_date = "00-00-0000 00:00";
    char *file = NULL;
    char *token;
    char *line = malloc(sizeof(char *));
    char *tempString;
    size_t len;


    /*FILE for reading and commands*/
    FILE *transactionFile = NULL;
    FILE *bitCoinBalancesFile = NULL;
    FILE *inputFile = NULL;

    //struct for data manipulation
    user *u = malloc(sizeof(user));
    transactionUser *tUser = malloc(sizeof(transactionUser));


    if (argc != 13) {

        printf("Wrong arguments!Please exit!\n");

        return -1;
    }

    for(int i = 1; i < argc; i +=2) {

        if(strcmp(argv[i],"-a") == 0) {

            bitCoinBalancesFile = fopen(argv[i+1], "r");

            if (bitCoinBalancesFile == NULL) return -1;
        }

        else if(strcmp(argv[i],"-t") == 0){

            transactionFile = fopen(argv[i+1], "r");

            if(transactionFile == NULL) return -1;
        }

        else if(strcmp(argv[i],"-v") == 0) bitcoinValue = atoi(argv[i+1]);

        else if(strcmp(argv[i],"-h1") == 0) senderHashtableNumOfEntries = atoi(argv[i+1]);

        else if(strcmp(argv[i],"-h2") == 0) receiverHashtableNumOfEntries = atoi(argv[i+1]);

        else if(strcmp(argv[i],"-b") == 0) bucketSize = atoi(argv[i+1]);
    }

    if (bitcoinValue <= 0 || senderHashtableNumOfEntries <= 0 || receiverHashtableNumOfEntries <= 0) {
        printf("The sizes and the values have to be positive numbers! Please return!\n");
        return -1;
    }

    /*creating the data structures as demanded*/
    senderBucket **sb = createHT(senderHashtableNumOfEntries);
    receiverBucket **rb = createReceiverHT(receiverHashtableNumOfEntries);
    wallet **w = createWallet(receiverHashtableNumOfEntries);
    treeNode **t = NULL;

    int count = 0;


    pointSize = bucketSize - sizeof(senderBucket*) - sizeof(int) - sizeof(bucketItems*);
    pointSize = pointSize / sizeof(bucketItems);


    //a table with roots for the different bitcoinID
    t = readBalancesFile(bitCoinBalancesFile,w,pointSize,senderHashtableNumOfEntries,t,bitcoinValue,&count);

   readTransactionsFile(transactionFile,sb,rb,senderHashtableNumOfEntries,receiverHashtableNumOfEntries,pointSize,w,bitcoinValue,t,count,&maxN);

    //commandline interface
    while(fgets(commands,1000,input) != NULL){

        if(strcmp(commands,"\n") == 0){
            continue;
        }

        commands[strlen(commands)-1] = '\0';
        nWords = countWords(commands);
        //tokenize to find the command
        cmd = strtok(commands," ");

        if(strcmp(cmd,"exit") == 0){
            destroyHT(sb,rb,senderHashtableNumOfEntries,receiverHashtableNumOfEntries,pointSize);
            destroyWallet(w,receiverHashtableNumOfEntries);
            free(u);
            free(line);
            free(tUser);
            free(buffer);
            free(t);
            return 0;
        }
        else if(strlen(commands) == 0){
            continue;
        }
        else if(strcmp(cmd,"requestTransaction") == 0){

            i = 0;
            //save each token in an array
            while(cmd != NULL){
                arr[i] = cmd;
                i++;
                cmd = strtok(NULL," ");
            }

            //Command: "requestTransaction senderWalletID receiverWalletID amount date time"
            if(nWords == 6){
                maxN++;
                printf("%d\n",maxN);

                u->nTransaction = maxN;
                tUser->nTransaction = maxN;
                u->senderWalletId = strdup(arr[1]);
                tUser->senderWalletId = strdup(arr[1]);
                tUser->receiverWalletId = strdup(arr[2]);
                u->receiverWalletId = strdup(arr[2]);
                tUser->amount = atoi(arr[3]);
                u->amount = atoi(arr[3]);


                if(strcmp(u->senderWalletId,u->receiverWalletId) == 0){
                    printf("<Sender is the same person with the receiver!>\n");
                    continue;
                }

                if(u->amount <= 0){
                    printf("<Amount of the transaction must be a positive number!>\n");
                    continue;
                }
                tUser->date = strdup(arr[4]);
                u->date = strdup(arr[4]);
                tUser->time = strdup(arr[5]);
                u->time = strdup(arr[5]);


                current_date = malloc(sizeof(u->time) + sizeof(u->date));
                sprintf(current_date,"%s %s",u->date ,u->time);
                if(!checkTime(old_date,current_date)){
                    printf("<Invalid date!>\n");
                    printf("<Invalid transaction!>\n");
                    continue;
                }else{
                    old_date = strdup(current_date);
                }


                int c = updateWallet(w,u,senderHashtableNumOfEntries,bitcoinValue);
                if(c) {
                    if (idExists(w, u, senderHashtableNumOfEntries, 1) +
                        idExists(w, u, senderHashtableNumOfEntries, 2)) {
                        insert(sb, u, senderHashtableNumOfEntries, pointSize,count);
                        insertReceiver(rb, tUser, receiverHashtableNumOfEntries, pointSize);
                    }
                }

            }
            else if(nWords == 4){
                //Command: "requestTransaction senderWalletID receiverWalletID amount"
                //we have the current timestamp
                maxN++;
                printf("%d\n",maxN);
                u->nTransaction = maxN;
                tUser->nTransaction = maxN;
                u->senderWalletId = strdup(arr[1]);
                tUser->senderWalletId = strdup(arr[1]);
                tUser->receiverWalletId = strdup(arr[2]);
                u->receiverWalletId = strdup(arr[2]);
                tUser->amount = atoi(arr[3]);
                u->amount = atoi(arr[3]);


                if(strcmp(u->senderWalletId,u->receiverWalletId) == 0){
                    printf("<Sender is the same person with the receiver!>\n");
                    continue;
                }

                //check the amount positive or negative
                if(u->amount <= 0){
                    printf("<Amount of the transaction must be a positive number!>\n");
                    continue;
                }

                current_date = strdup(getCurrentTime(buffer));

                char *tk = strtok(current_date," ");
                tUser->date = strdup(tk);
                u->date = strdup(tk);

                tk = strtok(NULL," ");

                tUser->time = strdup(tk);
                u->time = strdup(tk);
                sprintf(current_date,"%s %s",u->date ,u->time);

                //Check if the inserted date is later the old date or not
                if(!checkTime(old_date,current_date)){
                    printf("<Invalid date!>\n");
                    printf("<Invalid transaction!>\n");
                    continue;
                }else{
                    old_date = strdup(current_date);
                }

                c = updateWallet(w,u,senderHashtableNumOfEntries,bitcoinValue);

                if(c) {
                    //Check if the sender and the receiver exist in senderHT and receiverHT,in order to update them
                    if (idExists(w, u, senderHashtableNumOfEntries, 1) + idExists(w, u, receiverHashtableNumOfEntries, 2)) {
                        insert(sb, u, senderHashtableNumOfEntries, pointSize,count);
                        insertReceiver(rb, tUser, receiverHashtableNumOfEntries, pointSize);
                    }
                }
            }
            else{
                printf("<requestTransaction> Invalid command!\n");
                continue;
            }
        }
        else if(strcmp(cmd,"requestTransactions") == 0){
            /*  Command: requestTransactions senderWalletID receiverWalletID amount date time;
                           senderWalletID2 receiverWalletID2 amount2 date2 time2;
                            ...
                              senderWalletIDn receiverWalletIDn amountn daten timen;
              */

            if(nWords > 5){
                i = 0;

                if(strcmp(commands,"\n") == 0){
                    continue;
                }
                while(cmd != NULL){
                    arr[i] = cmd;
                    i++;
                    cmd = strtok(NULL," ");
                }


                maxN++;
                printf("%d\n",maxN);

                u->nTransaction = maxN;
                tUser->nTransaction = maxN;
                u->senderWalletId = strdup(arr[1]);
                tUser->senderWalletId = strdup(arr[1]);
                tUser->receiverWalletId = strdup(arr[2]);
                u->receiverWalletId = strdup(arr[2]);
                tUser->amount = atoi(arr[3]);
                u->amount = atoi(arr[3]);



                if(strcmp(u->senderWalletId,u->receiverWalletId) == 0){
                    printf("<Sender is the same person with the receiver!>\n");
                    continue;
                }
                //check the amount positive or negative
                if(u->amount <= 0){
                    printf("<Amount of the transaction must be a positive number!>\n");
                    continue;
                }
                tUser->date = strdup(arr[4]);
                u->date = strdup(arr[4]);

                tempString = strdup(arr[5]);
                tempString[strlen(tempString)-1] = '\0';


                tUser->time = strdup(tempString);
                u->time = strdup(tempString);


                current_date = malloc(sizeof(u->time) + sizeof(u->date));
                sprintf(current_date,"%s %s",u->date ,u->time);

                //Check if the inserted date is later the old date or not
                if(!checkTime(old_date,current_date)){
                    printf("<Invalid date!>\n");
                    printf("<Invalid transaction!>\n");
                    continue;
                }else{
                    old_date = strdup(current_date);
                }


                c = updateWallet(w,u,senderHashtableNumOfEntries,bitcoinValue);
                if(c) {
                    if (idExists(w, u, senderHashtableNumOfEntries, 1) +
                        idExists(w, u, senderHashtableNumOfEntries, 2)) {
                        insert(sb, u, senderHashtableNumOfEntries, pointSize,count);
                        insertReceiver(rb, tUser, receiverHashtableNumOfEntries, pointSize);
                    }
                }

                //  Command: "senderWalletID2 receiverWalletID2 amount2 date2 time2;"
                //  part of requestTransactions

                while(fgets(commands,1000,input) != NULL){

                    //When we press enter only, we are going to the external command line!
                    if(strcmp(commands,"\n") == 0){
                        printf("<We are going to the external command line!>\n");
                        break;
                    }

                    commands[strlen(commands)-1] = '\0';
                    tk1 = strtok(commands," ");
                    i = 0;

                    while(tk1 != NULL){
                        arr[i] = tk1;
                        i++;
                        tk1 = strtok(NULL," ");
                    }

                    maxN++;
                    printf("%d\n",maxN);

                    u->nTransaction = maxN;
                    tUser->nTransaction = maxN;
                    u->senderWalletId = strdup(arr[0]);
                    tUser->senderWalletId = strdup(arr[0]);
                    tUser->receiverWalletId = strdup(arr[1]);
                    u->receiverWalletId = strdup(arr[1]);
                    tUser->amount = atoi(arr[2]);
                    u->amount = atoi(arr[2]);

                    if(strcmp(u->senderWalletId,u->receiverWalletId) == 0){
                        printf("<Sender is the same person with the receiver!>\n");
                        continue;
                    }


                    //check the amount positive or negative
                    if(u->amount <= 0){
                        printf("<Amount of the transaction must be a positive number!>\n");
                        continue;
                    }
                    tUser->date = strdup(arr[3]);
                    u->date = strdup(arr[3]);
                    char *tempString = strdup(arr[4]);
                    tempString[strlen(tempString)-1] = '\0';


                    tUser->time = strdup(tempString);
                    u->time = strdup(tempString);

                    current_date = malloc(sizeof(u->time) + sizeof(u->date));
                    sprintf(current_date,"%s %s",u->date ,u->time);

                    //Check if the inserted date is later the old date or not
                    if(!checkTime(old_date,current_date)){
                        printf("<Invalid date!>\n");
                        printf("<Invalid transaction!>\n");
                        continue;
                    }else{
                        old_date = strdup(current_date);
                    }


                    c = updateWallet(w,u,senderHashtableNumOfEntries,bitcoinValue);
                    if(c) {
                        if (idExists(w, u, senderHashtableNumOfEntries, 1) +
                            idExists(w, u, senderHashtableNumOfEntries, 2)) {
                            insert(sb, u, senderHashtableNumOfEntries, pointSize,count);
                            insertReceiver(rb, tUser, receiverHashtableNumOfEntries, pointSize);
                        }
                    }
                }
            }
            else if(nWords == 2){
                //Command: "requestTransactions inputFile"

                inputFile = fopen("inputFile.txt","r");

                if(inputFile == NULL){
                    printf("<Opening inputFile went wrong!>\n");
                    break;
                }
                while(getline(&file, &len, inputFile) != -1){
                    k = 0;
                    tk2 = strtok(file," \n");
                    while(tk2 != NULL){
                        tempArr[k] = tk2;
                        k++;
                        tk2 = strtok(NULL," \n");
                    }
                    maxN++;
                    //printf("%d\n",maxN);

                    u->nTransaction = maxN;
                    tUser->nTransaction = maxN;
                    u->senderWalletId = strdup(tempArr[0]);
                    tUser->senderWalletId = strdup(tempArr[0]);
                    tUser->receiverWalletId = strdup(tempArr[1]);
                    u->receiverWalletId = strdup(tempArr[1]);
                    tUser->amount = atoi(tempArr[2]);
                    u->amount = atoi(tempArr[2]);

                    if(strcmp(u->senderWalletId,u->receiverWalletId) == 0){
                        printf("<Sender is the same person with the receiver!>\n");
                        continue;
                    }


                        //check the amount positive or negative
                    if(u->amount <= 0){
                        printf("<Amount of the transaction must be a positive number!>\n");
                        continue;
                    }
                    tUser->date = strdup(tempArr[3]);
                    u->date = strdup(tempArr[3]);

                    tempString = strdup(tempArr[4]);
                    tempString[strlen(tempString)-1] = '\0';


                    tUser->time = strdup(tempString);
                    u->time = strdup(tempString);

                    current_date = malloc(sizeof(u->time) + sizeof(u->date));
                    sprintf(current_date,"%s %s",u->date ,u->time);

                    //Check if the inserted date is later the old date or not
                    if(!checkTime(old_date,current_date)){
                        printf("<Invalid date!>\n");
                        printf("<Invalid transaction!>\n");
                        continue;
                    }else{
                        old_date = strdup(current_date);
                    }


                    c = updateWallet(w,u,senderHashtableNumOfEntries,bitcoinValue);
                    if(c) {
                        if (idExists(w, u, senderHashtableNumOfEntries, 1) +
                            idExists(w, u, senderHashtableNumOfEntries, 2)) {
                            insert(sb, u, senderHashtableNumOfEntries, pointSize,count);
                            insertReceiver(rb, tUser, receiverHashtableNumOfEntries, pointSize);
                        }
                    }
                }
            }
            else{
                printf("<requestTransactions> Invalid command!\n");
                continue;
            }
        }
        else if(strcmp(cmd,"findEarnings") == 0){
            //Command: "findEarnings"
            i = 0;
            while(cmd != NULL){
                arr[i] = cmd;
                i++;
                cmd = strtok(NULL," ");
            }
            int sum = -1;
            char *tempS = NULL;
            char *time1 = NULL;
            char *time2 = NULL;
            char *date2 = NULL;
            char *date1 = NULL;
            if(nWords % 2 == 0) {

                if (nWords == 2) {
                    //Command: "findEarnings walletID"
                    //Show all the earnings history of user walletID

                    tempS = strdup(arr[1]);
                    sum = findEarnings(tempS, rb, "-1", "-1", "-1", "-1", receiverHashtableNumOfEntries);
                }
                else if (nWords == 4) {
                    //Command: "findEarnings walletID [time1] [time2]"
                    //Show all the earnings history of user walletID between [time1] [time2]

                    tempS = strdup(arr[1]);
                    time1 = strdup(arr[2]);
                    time2 = strdup(arr[3]);
                    sum = findEarnings(tempS, rb, "-1", "-1", time1, time2, receiverHashtableNumOfEntries);
                } else if (nWords == 6) {
                    //Command: "findEarnings walletID [time1][year1] [time2][year2]"
                    //Show all the earnings history of user walletID between [time1][year1] [time2][year2]

                    tempS = strdup(arr[1]);
                    time1 = strdup(arr[2]);
                    time2 = strdup(arr[4]);
                    date1 = strdup(arr[3]);
                    date2 = strdup(arr[5]);
                    sum = findEarnings(tempS, rb, date1, date2, time1, time2, receiverHashtableNumOfEntries);
                }
                if (sum >= 0) {
                    printf("<Earnings are %d>\n", sum);
                } else {
                    printf("<Unknown user!>\n");
                }
            }
            else{
                printf("<findEarnings> Invalid command!\n");
                continue;
            }
        }
        else if(strcmp(cmd,"findPayments") == 0){
            i = 0;
            while(cmd != NULL){
                arr[i] = cmd;
                i++;
                cmd = strtok(NULL," ");
            }
            int sum = -1;
            char *tempS = NULL;
            char *time1 = NULL;
            char *time2 = NULL;
            char *date2 = NULL;
            char *date1 = NULL;
            if(nWords % 2 == 0){

                if(nWords == 2){
                    //Command: "findPayments walletID "
                    //Show all the payments history of user walletID

                    tempS = strdup(arr[1]);
                    sum = findPayments(tempS,sb,"-1","-1","-1","-1",senderHashtableNumOfEntries);
                }
                else if(nWords == 4){
                    //Command: "findPayments walletID [time1] [time2]"
                    //Show all the payments history of user walletID between [time1] [time2]

                    tempS = strdup(arr[1]);
                    time1 = strdup(arr[2]);
                    time2 = strdup(arr[3]);
                    sum = findPayments(tempS,sb,"-1","-1",time1,time2,senderHashtableNumOfEntries);
                }
                else if(nWords == 6){
                    //Command: "findPayments walletID [time1][year1] [time2][year2]"
                    //Show all the payments history of user walletID between [time1][year1] [time2][year2]

                    tempS = strdup(arr[1]);
                    time1 = strdup(arr[2]);
                    time2 = strdup(arr[4]);
                    date1 = strdup(arr[3]);
                    date2 = strdup(arr[5]);
                    sum = findPayments(tempS,sb,date1,date2,time1,time2,senderHashtableNumOfEntries);
                }
                if(sum >= 0){
                    printf("<Payments are %d>\n",sum);
                } else{
                    printf("<Unknown user!>\n");
                }
            }
            else{
                printf("<findPayments> Invalid command!\n");
                continue;
            }
        }
        else if(strcmp(cmd,"walletStatus") == 0){
            i = 0;
            while(cmd != NULL){
                arr[i] = cmd;
                i++;
                cmd = strtok(NULL," ");
            }
            if(nWords == 2){
                //Command: "walletStatus walletID"

                char *temp = strdup(arr[1]);
                int status = walletStatus(temp,senderHashtableNumOfEntries,w);
                if(status >= 0){
                    printf("<User: %s has %d bitcoins in his wallet.>\n",arr[1],status);
                } else{
                    printf("<Unknown user!>\n");
                }
            }
            else{
                printf("<walletStatus> Invalid command!\n");
                continue;
            }
        }
        else if(strcmp(cmd,"bitCoinStatus") == 0) {
            i = 0;
            while(cmd != NULL){
                arr[i] = cmd;
                i++;
                cmd = strtok(NULL," ");
            }
            if(nWords == 2){
                //bitcoinStatus id
            }
            else{
                printf("<bitCoinStatus> Invalid command!\n");
                continue;
            }
        }
        else if(strcmp(cmd,"traceCoin") == 0){
            i = 0;
            while(cmd != NULL){
                arr[i] = cmd;
                i++;
                cmd = strtok(NULL," ");
            }
            if(nWords == 2){
                //traceCoin id
            }
            else{
                printf("<traceCoin> Invalid command!\n");
                continue;
            }
        }
        else{
            printf("<Wrong command!Please retry!>\n");
            continue;
        }
    }

    return 0;
}


