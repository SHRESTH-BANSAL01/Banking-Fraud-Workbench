
import csv
import random
from datetime import datetime, timedelta
from pathlib import Path

# Project root
ROOT = Path(__file__).resolve().parent.parent
DATA = ROOT / "data"

HEADER = [
    "transaction_id",
    "account_id",
    "device_id",
    "merchant_id",
    "amount",
    "timestamp",
    "is_fraud",
]

SEED = 2026


def generate_transactions(count, mode, seed):
    rng = random.Random(seed)
    start = datetime(2026, 11, 1, 0, 0, 0)

    transactions = []

    for i in range(count):
        transaction_id = 1000000 + i

        account_id = rng.randint(5000, 15000)
        device_id = rng.randint(100, 1000)
        merchant_id = rng.randint(200, 2000)

        timestamp = start + timedelta(
            seconds=i * 3
        )

        if mode == "normal":
            is_fraud = int(rng.random() < 0.08)

            if is_fraud:
                amount = rng.uniform(40000, 200000)
            else:
                amount = rng.uniform(100, 30000)

        elif mode == "shock":
            # Simulate a sudden high-value transaction burst.
            is_fraud = int(rng.random() < 0.30)

            if i >= count // 2:
                amount = rng.uniform(50000, 500000)
                timestamp = start + timedelta(
                    hours=12,
                    seconds=(i - count // 2) % 300
                )
            else:
                amount = rng.uniform(100, 25000)

        elif mode == "adversarial":
            # Difficult but syntactically valid inputs.
            pattern = i % 6

            if pattern == 0:
                amount = 0.0
                is_fraud = 0

            elif pattern == 1:
                amount = 999999999.99
                is_fraud = 1

            elif pattern == 2:
                amount = 49999.99
                is_fraud = 1

            elif pattern == 3:
                amount = 50000.00
                is_fraud = 0

            elif pattern == 4:
                amount = 0.01
                is_fraud = 1

            else:
                amount = rng.uniform(1000, 100000)
                is_fraud = rng.randint(0, 1)

            # Repeated account/device combinations
            account_id = 5000 + i % 5
            device_id = 100 + i % 3

        else:
            raise ValueError("Unknown dataset mode")

        transactions.append([
            transaction_id,
            account_id,
            device_id,
            merchant_id,
            f"{amount:.2f}",
            timestamp.strftime("%Y-%m-%dT%H:%M:%S"),
            is_fraud,
        ])

    return transactions


def save_dataset(relative_path, count, mode, seed):
    path = DATA / relative_path
    path.parent.mkdir(parents=True, exist_ok=True)

    rows = generate_transactions(
        count,
        mode,
        seed
    )

    with path.open(
        "w",
        newline="",
        encoding="utf-8"
    ) as file:
        writer = csv.writer(file)
        writer.writerow(HEADER)
        writer.writerows(rows)

    print(f"Created: {path.relative_to(ROOT)}")
    print(f"Transactions: {len(rows):,}")
    print()


def main():
    print("=" * 55)
    print("BANKING FRAUD DATASET GENERATOR")
    print("=" * 55)

    datasets = [
        (
            "medium/transactions_medium.csv",
            5000,
            "normal",
            SEED + 1,
        ),
        (
            "medium/transactions_shock.csv",
            5000,
            "shock",
            SEED + 2,
        ),
        (
            "large/transactions_large.csv",
            50000,
            "normal",
            SEED + 3,
        ),
        (
            "adversarial/transactions_adversarial.csv",
            1000,
            "adversarial",
            SEED + 4,
        ),
    ]

    for path, count, mode, seed in datasets:
        save_dataset(path, count, mode, seed)

    print("=" * 55)
    print("ALL DATASETS GENERATED SUCCESSFULLY")
    print("=" * 55)


if __name__ == "__main__":
    main()
