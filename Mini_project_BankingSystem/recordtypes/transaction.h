#ifndef TRANSACTIONS
#define TRANSACTIONS

#include <time.h>

struct Transaction
{
    int transactionID; 
    int accountNumber;
    bool operation; // 0 -> Withdraw, 1 -> Deposit
    long int oldBalance;
    long int newBalance;
    int receiverAccNumber;
    time_t transactionTime;
};

#endif