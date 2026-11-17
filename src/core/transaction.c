#include <stdio.h>
#include "transaction.h"

void display_transaction(Transaction t)
{
    printf("\n------------------------------\n");
    printf("Transaction ID : %llu\n", t.transaction_id);
    printf("Account ID     : %llu\n", t.account_id);
    printf("Device ID      : %llu\n", t.device_id);
    printf("Merchant ID    : %llu\n", t.merchant_id);
    printf("Amount         : %.2f\n", t.amount);
    printf("Timestamp      : %s\n", t.timestamp);
    printf("Fraud Label    : %d\n", t.is_fraud);
    printf("Risk Score     : %d\n", t.risk_score);
    printf("------------------------------\n");
}