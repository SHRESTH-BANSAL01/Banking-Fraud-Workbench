
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "dataset.h"
#include "transaction.h"
#include "scalability.h"

#define SCALABILITY_TRIALS 3

typedef struct
{
    const char *name;
    const char *path;
} DatasetCase;

static double elapsed_ms(clock_t start, clock_t end)
{
    return 1000.0 * (double)(end - start) /
           CLOCKS_PER_SEC;
}

int run_dataset_scalability(const char *output_path)
{
    if (output_path == NULL)
    {
        fprintf(stderr, "Invalid output path.\n");
        return 1;
    }

    const DatasetCase cases[] = {
        {
            "Small",
            "data/small/transactions_small.csv"
        },
        {
            "Medium",
            "data/medium/transactions_medium.csv"
        },
        {
            "Shock",
            "data/medium/transactions_shock.csv"
        },
        {
            "Large",
            "data/large/transactions_large.csv"
        },
        {
            "Adversarial",
            "data/adversarial/transactions_adversarial.csv"
        }
    };

    const size_t case_count =
        sizeof(cases) / sizeof(cases[0]);

    FILE *report = fopen(output_path, "w");

    if (report == NULL)
    {
        perror("Cannot create scalability report");
        return 1;
    }

    fprintf(
        report,
        "Dataset,Trial,Transactions,Load Time (ms),"
        "Estimated Data Memory (bytes),Status\n"
    );

    printf("\n");
    printf("====================================================\n");
    printf("          DATASET SCALABILITY BENCHMARK\n");
    printf("====================================================\n");

    int failures = 0;
    int total = 0;

    for (size_t i = 0; i < case_count; i++)
    {
        for (int trial = 1;
             trial <= SCALABILITY_TRIALS;
             trial++)
        {
            Dataset dataset = {0};

            clock_t start = clock();

            int success = load_dataset(
                cases[i].path,
                &dataset
            );

            clock_t end = clock();

            double milliseconds = elapsed_ms(
                start,
                end
            );

            size_t memory_bytes = 0;

            if (success)
            {
                memory_bytes =
                    dataset.count * sizeof(Transaction);
            }

            const char *status =
                success ? "PASS" : "FAIL";

            printf(
                "%-12s | Trial %d | "
                "Rows: %-7zu | Time: %10.3f ms | %s\n",
                cases[i].name,
                trial,
                dataset.count,
                milliseconds,
                status
            );

            if (fprintf(
                    report,
                    "%s,%d,%zu,%.6f,%zu,%s\n",
                    cases[i].name,
                    trial,
                    dataset.count,
                    milliseconds,
                    memory_bytes,
                    status
                ) < 0)
            {
                fprintf(stderr, "CSV write failed.\n");
                free_dataset(&dataset);
                fclose(report);
                return 1;
            }

            total++;

            if (!success)
                failures++;

            free_dataset(&dataset);
        }
    }

    if (fclose(report) != 0)
    {
        fprintf(stderr, "Failed to close report.\n");
        return 1;
    }

    printf("\n");
    printf("====================================================\n");
    printf("               SCALABILITY SUMMARY\n");
    printf("====================================================\n");

    printf("Total Runs : %d\n", total);
    printf("Passed     : %d\n", total - failures);
    printf("Failed     : %d\n", failures);
    printf("Report     : %s\n", output_path);

    printf("====================================================\n");

    return failures == 0 ? 0 : 1;
}
