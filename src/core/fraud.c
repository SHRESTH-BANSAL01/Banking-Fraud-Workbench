#include "fraud.h"

int calculate_risk_score(Transaction *t)
{
    int score = 0;

    if (t->amount >= 100000)
        score += 40;

    else if (t->amount >= 50000)
        score += 25;

    else if (t->amount >= 20000)
        score += 10;

    return score;
}

void evaluate_fraud(Transaction *t)
{
    t->risk_score = calculate_risk_score(t);
}