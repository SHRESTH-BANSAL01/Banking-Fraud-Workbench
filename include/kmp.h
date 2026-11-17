
#ifndef KMP_H
#define KMP_H

void kmp_search(
    const char *text,
    const char *pattern
);

void analyze_transaction_description(
    unsigned long long transaction_id,
    const char *description
);

#endif
