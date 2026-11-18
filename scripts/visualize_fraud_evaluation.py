
from pathlib import Path

import pandas as pd
import matplotlib.pyplot as plt

ROOT = Path(__file__).resolve().parent.parent
REPORTS = ROOT / "reports"
REPORTS.mkdir(exist_ok=True)

df = pd.read_csv(REPORTS / "fraud_evaluation.csv")

metrics = ["Accuracy", "Precision", "Recall", "F1"]

# Plot evaluation metrics
ax = df.plot(
    x="Dataset",
    y=metrics,
    kind="bar",
    figsize=(12, 6),
    rot=0
)

ax.set_title("Fraud Detection Performance")
ax.set_ylabel("Score (%)")
ax.set_xlabel("Dataset")
ax.set_ylim(0, 105)

# CSV stores scores between 0 and 1
for container in ax.containers:
    labels = [
        f"{bar.get_height() * 100:.1f}%"
        for bar in container
    ]
    ax.bar_label(container, labels=labels,
                 fontsize=7, padding=2)

# Convert displayed axis to percentage
from matplotlib.ticker import PercentFormatter
ax.yaxis.set_major_formatter(PercentFormatter(1.0))

plt.tight_layout()
plt.savefig(
    REPORTS / "fraud_metrics_comparison.png",
    dpi=300
)
plt.close()

# Plot confusion matrix counts
fig, axes = plt.subplots(1, 5, figsize=(18, 4))

for ax, (_, row) in zip(axes, df.iterrows()):
    matrix = [
        [row["TN"], row["FP"]],
        [row["FN"], row["TP"]]
    ]

    ax.imshow(matrix, cmap="Blues")

    for i in range(2):
        for j in range(2):
            ax.text(
                j, i, str(matrix[i][j]),
                ha="center", va="center",
                color="black"
            )

    ax.set_title(row["Dataset"])
    ax.set_xticks([0, 1], ["Normal", "Fraud"])
    ax.set_yticks([0, 1], ["Normal", "Fraud"])
    ax.set_xlabel("Predicted")
    ax.set_ylabel("Actual")

plt.tight_layout()
plt.savefig(
    REPORTS / "fraud_confusion_matrices.png",
    dpi=300
)
plt.close()

print("Visualization reports generated successfully.")
