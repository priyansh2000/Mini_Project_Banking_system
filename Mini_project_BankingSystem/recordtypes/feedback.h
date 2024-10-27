#ifndef FEEDBACK_RECORD
#define FEEDBACK_RECORD

struct Feedback
{
    int id;
    char message[500];
    int state;
    int account;
};

#endif