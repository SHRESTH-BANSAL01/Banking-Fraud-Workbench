
#include <stdio.h>
#include <stddef.h>

#include "dataset.h"
#include "fraud.h"
#include "fraud_evaluation.h"

#define FRAUD_THRESHOLD 25

typedef struct
{
    const char *name;
    const char *path;
} EvaluationDataset;

typedef struct
{
    size_t tp;
    size_t tn;
    size_t fp;
    size_t fn;
} ConfusionMatrix;

static double safe_divide(double a, double b)
{
    return b == 0.0 ? 0.0 : a / b;
}

static void evaluate_dataset(
    Dataset *dataset,
    ConfusionMatrix *cm)
{
    *cm = (ConfusionMatrix){0};

    for (size_t i = 0; i < dataset->count; i++)
    {
        Transaction *t = &dataset->items[i];

        evaluate_fraud(t);

        int actual = t->is_fraud;
        int predicted =
            t->risk_score >= FRAUD_THRESHOLD;

        if (actual == 1 && predicted == 1)
            cm->tp++;

        else if (actual == 0 && predicted == 0)
            cm->tn++;

        else if (actual == 0 && predicted == 1)
            cm->fp++;

        else
            cm->fn++;
    }
}

int run_fraud_evaluation(const char *output_path)
{
    if (output_path == NULL)
    {
        fprintf(stderr, "Invalid report path.\n");
        return 1;
    }

    const EvaluationDataset datasets[] = {
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

    const size_t dataset_count =
        sizeof(datasets) / sizeof(datasets[0]);

    FILE *report = fopen(output_path, "w");

    if (report == NULL)
    {
        perror("Cannot create fraud evaluation report");
        return 1;
    }

    if (fprintf(
        report,
        "Dataset,Transactions,TP,TN,FP,FN,"
        "Accuracy,Precision,Recall,F1\n"
    ) < 0)
    {
        fclose(report);
        return 1;
    }

    printf("\n============================================\n");
    printf("         FRAUD DETECTION EVALUATION\n");
    printf("============================================\n");
    printf("Risk Threshold: %d\n", FRAUD_THRESHOLD);

    int failures = 0;

    for (size_t i = 0; i < dataset_count; i++)
    {
        Dataset dataset = {0};

        if (!load_dataset(datasets[i].path, &dataset))
        {
            printf(
                "FAILED to load dataset: %s\n",
                datasets[i].name
            );

            failures++;
            free_dataset(&dataset);
            continue;
        }

        ConfusionMatrix cm;

        evaluate_dataset(&dataset, &cm);

        double total = (double)dataset.count;

        double accuracy = safe_divide(
            (double)(cm.tp + cm.tn),
            total
        );

        double precision = safe_divide(
            (double)cm.tp,
            (double)(cm.tp + cm.fp)
        );

        double recall = safe_divide(
            (double)cm.tp,
            (double)(cm.tp + cm.fn)
        );

        double f1 = safe_divide(
            2.0 * precision * recall,
            precision + recall
        );

        printf("\nDataset: %s\n", datasets[i].name);
        printf("Transactions : %zu\n", dataset.count);

        printf("TP: %zu | TN: %zu\n", cm.tp, cm.tn);
        printf("FP: %zu | FN: %zu\n", cm.fp, cm.fn);

        printf("Accuracy  : %.2f%%\n", accuracy * 100);
        printf("Precision : %.2f%%\n", precision * 100);
        printf("Recall    : %.2f%%\n", recall * 100);
        printf("F1-Score  : %.2f%%\n", f1 * 100);

        if (fprintf(
            report,
            "%s,%zu,%zu,%zu,%zu,%zu,"
            "%.4f,%.4f,%.4f,%.4f\n",
            datasets[i].name,
            dataset.count,
            cm.tp,
            cm.tn,
            cm.fp,
            cm.fn,
            accuracy,
            precision,
            recall,
            f1
        ) < 0)
        {
            fprintf(stderr, "Failed to write CSV.\n");
            free_dataset(&dataset);
            fclose(report);
            return 1;
        }

        free_dataset(&dataset);
    }

    if (fclose(report) != 0)
    {
        fprintf(stderr, "Failed to close CSV.\n");
        return 1;
    }

    printf("\n============================================\n");
    printf("Evaluation completed.\n");
    printf("Failed datasets: %d\n", failures);
    printf("Report: %s\n", output_path);
    printf("============================================\n");

    return failures == 0 ? 0 : 1;
}
