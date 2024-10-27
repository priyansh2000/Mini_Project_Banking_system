#ifndef LOAN
#define LOAN

struct Loan
{
    int loanID; // 0, 1, 2, 3 ...
    int amount;
    int custID;
    int status;//0->assigned to mgr, 1->assigned to emp, 2->rejected, 3->accepted
    int empID;
};

#endif