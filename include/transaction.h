
#ifndef TRANSACTION_H
#define TRANSACTION_H

typedef struct Transaction
{
    unsigned long long transaction_id;
    unsigned long long account_id;
    unsigned long long device_id;
    unsigned long long merchant_id;

    double amount;

    char timestamp[32];

    int is_fraud;
    int risk_score;

} Transaction;

void display_transaction(Transaction t);

#endif
