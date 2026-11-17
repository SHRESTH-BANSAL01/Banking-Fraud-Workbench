
#ifndef FRAUD_H
#define FRAUD_H

#include "transaction.h"

int calculate_risk_score(Transaction *t);

void evaluate_fraud(Transaction *t);

#endif
