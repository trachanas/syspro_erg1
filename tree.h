#ifndef UNTITLED_TREE_H
#define UNTITLED_TREE_H

typedef struct treeNode{
    int bitCoinID;
    char *walletID;
    int value;  //exei mpei int to vazoume kai double
    struct treeNode *transaction;   //left leaf
    struct treeNode *remain; //right leaf
}treeNode;

void bitcoinOwner(treeNode **t,char *walletID,int size);
treeNode *createTree(char *,int,int);
treeNode **createRootTable(int );
treeNode* newTransaction(treeNode *,char *,int);
treeNode* newRemainder(treeNode *,char *,int );
void printTree(treeNode *);
treeNode * bitcoinExists(treeNode **t,int token,int size);

#endif