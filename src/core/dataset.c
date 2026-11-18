
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>

#include "dataset.h"

#define MAX_LINE_LENGTH 256

// Remove leading and trailing spaces
static char *trim(char *str)
{
    while (isspace((unsigned char)*str))
    {
        str++;
    }

    char *end = str + strlen(str);

    while (end > str &&
           isspace((unsigned char)*(end - 1)))
    {
        end--;
    }

    *end = '\0';

    return str;
}

// Convert string into unsigned long long
static int parse_unsigned(const char *str,
                          unsigned long long *value)
{
    char *end;

    if (str == NULL || *str == '\0' || *str == '-')
    {
        return 0;
    }

    errno = 0;

    unsigned long long result = strtoull(str, &end, 10);

    if (errno == ERANGE || end == str || *trim(end) != '\0')
    {
        return 0;
    }

    *value = result;

    return 1;
}

// Convert string into double
static int parse_amount(const char *str, double *value)
{
    char *end;

    errno = 0;

    double result = strtod(str, &end);

    if (errno == ERANGE ||
        end == str ||
        *trim(end) != '\0' ||
        !isfinite(result) ||
        result < 0)
    {
        return 0;
    }

    *value = result;

    return 1;
}

// Convert string into integer
static int parse_integer(const char *str, int *value)
{
    char *end;

    errno = 0;

    long result = strtol(str, &end, 10);

    if (errno == ERANGE ||
        end == str ||
        *trim(end) != '\0' ||
        result < INT_MIN ||
        result > INT_MAX)
    {
        return 0;
    }

    *value = (int)result;

    return 1;
}

// Load transactions from CSV file
int load_dataset(const char *filename, Dataset *dataset)
{
    if (filename == NULL || dataset == NULL)
    {
        printf("Error: Invalid input.\n");
        return 0;
    }

    dataset->items = NULL;
    dataset->count = 0;

    size_t capacity = 0;

    FILE *file = fopen(filename, "r");

    if (file == NULL)
    {
        printf("Error: Cannot open dataset.\n");
        return 0;
    }

    char line[MAX_LINE_LENGTH];

    // Skip CSV header
    if (fgets(line, sizeof(line), file) == NULL)
    {
        printf("Error: Empty dataset.\n");
        fclose(file);
        return 0;
    }

    size_t line_number = 1;
    size_t invalid_rows = 0;

    // Read every transaction
    while (fgets(line, sizeof(line), file) != NULL)
    {
        line_number++;

        if (strlen(line) == MAX_LINE_LENGTH - 1 &&
            line[MAX_LINE_LENGTH - 2] != '\n')
        {
            int ch;

            while ((ch = fgetc(file)) != '\n' && ch != EOF)
            {
                // Discard remaining part of long line
            }

            printf("Warning: Line %zu is too long.\n",
                   line_number);

            invalid_rows++;
            continue;
        }

        if (strlen(trim(line)) == 0)
        {
            continue;
        }

        Transaction t;

        // Extract CSV fields
        char *fields[7];

        char *token = strtok(line, ",");

        int field_count = 0;

        while (token != NULL && field_count < 7)
        {
            fields[field_count] = trim(token);

            field_count++;

            token = strtok(NULL, ",");
        }

        // Check number of columns
        if (field_count != 7 || token != NULL)
        {
            printf("Warning: Invalid row at line %zu.\n",
                   line_number);

            invalid_rows++;
            continue;
        }

        // Convert CSV values
        int valid = 1;

        valid &= parse_unsigned(fields[0], &t.transaction_id);
        valid &= parse_unsigned(fields[1], &t.account_id);
        valid &= parse_unsigned(fields[2], &t.device_id);
        valid &= parse_unsigned(fields[3], &t.merchant_id);

        valid &= parse_amount(fields[4], &t.amount);

        valid &= parse_integer(fields[6], &t.is_fraud);

        if (!valid ||
            (t.is_fraud != 0 && t.is_fraud != 1) ||
            strlen(fields[5]) == 0 ||
            strlen(fields[5]) >= sizeof(t.timestamp))
        {
            printf("Warning: Invalid data at line %zu.\n",
                   line_number);

            invalid_rows++;
            continue;
        }

        // Copy timestamp
        strcpy(t.timestamp, fields[5]);

        // Initialize risk score
        t.risk_score = 0;

        
/* Allocate more memory only when capacity is full */
if (dataset->count == capacity)
{
    size_t new_capacity;

    if (capacity == 0)
    {
        new_capacity = 8;
    }
    else
    {
        /* Prevent integer overflow */
        if (capacity > SIZE_MAX / 2)
        {
            printf("Error: Dataset capacity overflow.\n");

            free(dataset->items);
            dataset->items = NULL;
            dataset->count = 0;

            fclose(file);
            return 0;
        }

        new_capacity = capacity * 2;
    }

    /* Check allocation size */
    if (new_capacity > SIZE_MAX / sizeof(Transaction))
    {
        printf("Error: Dataset is too large.\n");

        free(dataset->items);
        dataset->items = NULL;
        dataset->count = 0;

        fclose(file);
        return 0;
    }

    Transaction *temp = realloc(
        dataset->items,
        new_capacity * sizeof(Transaction)
    );

    if (temp == NULL)
    {
        printf("Error: Memory allocation failed.\n");

        free(dataset->items);
        dataset->items = NULL;
        dataset->count = 0;

        fclose(file);
        return 0;
    }

    dataset->items = temp;
    capacity = new_capacity;
}


        // Store transaction
        dataset->items[dataset->count] = t;

        dataset->count++;
    }

    fclose(file);

    // Display loading report
    printf("\n========== DATASET REPORT ==========\n");

    printf("Valid Transactions : %zu\n",
           dataset->count);

    printf("Invalid Rows       : %zu\n",
           invalid_rows);

    printf("====================================\n");

    if (dataset->count == 0)
    {
        printf("Error: No valid transactions found.\n");
        return 0;
    }

    return 1;
}

// Release allocated memory
void free_dataset(Dataset *dataset)
{
    if (dataset == NULL)
    {
        return;
    }

    free(dataset->items);

    dataset->items = NULL;

    dataset->count = 0;
}
