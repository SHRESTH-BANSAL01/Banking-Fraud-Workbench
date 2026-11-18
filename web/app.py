

from flask import request
from pathlib import Path
import csv
import subprocess

from flask import Flask, jsonify, render_template, request, send_file

BASE_DIR = Path(__file__).resolve().parent.parent
REPORTS_DIR = BASE_DIR / "reports"
EXECUTABLE = BASE_DIR / "fraud.exe"

app = Flask(__name__)

# Only these commands can be executed through the GUI.
ALLOWED_COMMANDS = {
    "test": "--test",
    "benchmark": "--benchmark",
    "stress": "--stress",
    "compare": "--compare",
    "complexity": "--complexity",
    "scalability": "--scalability",
    "evaluate": "--evaluate",
}

ALLOWED_REPORTS = {
    "fraud_evaluation.csv",
    "sorting_benchmark.csv",
    "stress_test_report.csv",
    "algorithm_comparison.csv",
    "complexity_report.csv",
    "dataset_scalability.csv",
}


@app.route("/")
def home():
    return render_template("index.html")


@app.route("/api/status")
def status():
    return jsonify({
        "connected": EXECUTABLE.is_file(),
        "executable": EXECUTABLE.name,
    })


@app.route("/api/run/<command>", methods=["POST"])
def run_command(command):
    if command not in ALLOWED_COMMANDS:
        return jsonify({
            "success": False,
            "error": "Unsupported command",
        }), 400

    if not EXECUTABLE.is_file():
        return jsonify({
            "success": False,
            "error": "fraud.exe not found. Compile the C project first.",
        }), 404

    REPORTS_DIR.mkdir(parents=True, exist_ok=True)

    try:
        result = subprocess.run(
            [str(EXECUTABLE), ALLOWED_COMMANDS[command]],
            cwd=str(BASE_DIR),
            capture_output=True,
            text=True,
            encoding="utf-8",
            errors="replace",
            timeout=120,
            check=False,
        )

        return jsonify({
            "success": result.returncode == 0,
            "return_code": result.returncode,
            "output": result.stdout,
            "error": result.stderr,
        })

    except subprocess.TimeoutExpired:
        return jsonify({
            "success": False,
            "error": "Command exceeded the 120-second limit.",
        }), 504

    except OSError as exc:
        return jsonify({
            "success": False,
            "error": str(exc),
        }), 500


@app.route("/api/evaluation")
def evaluation():
    report = REPORTS_DIR / "fraud_evaluation.csv"

    if not report.is_file():
        return jsonify({
            "success": False,
            "error": "Evaluation report not found. Run evaluation first.",
        }), 404

    try:
        with report.open("r", encoding="utf-8-sig", newline="") as file:
            rows = list(csv.DictReader(file))

        data = {}

        for row in rows:
            name = row["Dataset"]

            data[name] = {
                "transactions": int(row["Transactions"]),
                "tp": int(row["TP"]),
                "tn": int(row["TN"]),
                "fp": int(row["FP"]),
                "fn": int(row["FN"]),
                "accuracy": float(row["Accuracy"]) * 100,
                "precision": float(row["Precision"]) * 100,
                "recall": float(row["Recall"]) * 100,
                "f1": float(row["F1"]) * 100,
            }

        return jsonify({
            "success": True,
            "datasets": data,
        })

    except (OSError, ValueError, KeyError) as exc:
        return jsonify({
            "success": False,
            "error": f"Unable to read report: {exc}",
        }), 500


@app.route("/api/reports")
def list_reports():
    reports = []

    for filename in sorted(ALLOWED_REPORTS):
        path = REPORTS_DIR / filename

        if path.is_file():
            reports.append({
                "name": filename,
                "url": f"/api/reports/{filename}",
                "size_bytes": path.stat().st_size,
            })

    return jsonify({
        "success": True,
        "reports": reports,
    })


@app.route("/api/reports/<filename>")
def download_report(filename):
    if filename not in ALLOWED_REPORTS:
        return jsonify({"error": "Unknown report"}), 404

    path = REPORTS_DIR / filename

    if not path.is_file():
        return jsonify({"error": "Report not found"}), 404

    return send_file(path, as_attachment=True)


DATASET_FILES = {
    "small": BASE_DIR / "data" / "transactions.csv",
    "medium": BASE_DIR / "data" / "medium" / "transactions_medium.csv",
    "shock": BASE_DIR / "data" / "medium" / "transactions_shock.csv",
    "large": BASE_DIR / "data" / "large" / "transactions_large.csv",
    "adversarial": BASE_DIR / "data" / "adversarial" / "transactions_adversarial.csv",
}


def calculate_transaction_risk(amount):
    # Mirrors the rules in src/core/fraud.c.
    if amount >= 100000:
        return 40
    if amount >= 50000:
        return 25
    if amount >= 20000:
        return 10
    return 0


@app.route("/api/transactions")
def get_transactions():
    dataset_name = request.args.get("dataset", "small").lower()
    query = request.args.get("search", "").strip()
    filter_type = request.args.get("filter", "all").lower()

    if dataset_name not in DATASET_FILES:
        return jsonify({"error": "Invalid dataset"}), 400

    if filter_type not in {"all", "suspicious", "normal"}:
        return jsonify({"error": "Invalid filter"}), 400

    if len(query) > 100:
        return jsonify({"error": "Search query too long"}), 400

    path = DATASET_FILES[dataset_name]

    if not path.is_file():
        return jsonify({
            "error": f"Dataset file not found: {path.name}"
        }), 404

    matches = []
    total_matches = 0
    limit = 100

    try:
        with path.open("r", encoding="utf-8-sig", newline="") as file:
            reader = csv.DictReader(file)

            for row in reader:
                try:
                    amount = float(row["amount"])
                    transaction_id = row["transaction_id"]
                    account_id = row["account_id"]
                    fraud_label = int(row["is_fraud"])

                    if not (amount >= 0 and amount < float("inf")):
                        continue

                    if fraud_label not in (0, 1):
                        continue

                    risk = calculate_transaction_risk(amount)
                    suspicious = risk >= 25

                    if query and (
                        query not in transaction_id
                        and query not in account_id
                    ):
                        continue

                    if filter_type == "suspicious" and not suspicious:
                        continue

                    if filter_type == "normal" and suspicious:
                        continue

                    total_matches += 1

                    if len(matches) < limit:
                        matches.append({
                            "transaction_id": transaction_id,
                            "account_id": account_id,
                            "amount": amount,
                            "risk_score": risk,
                            "suspicious": suspicious,
                            "fraud_label": fraud_label,
                        })

                except (KeyError, ValueError, TypeError):
                    continue

    except OSError as exc:
        return jsonify({"error": str(exc)}), 500

    return jsonify({
        "success": True,
        "dataset": dataset_name,
        "total_matches": total_matches,
        "displayed": len(matches),
        "transactions": matches,
    })


if __name__ == "__main__":
    app.run(host="127.0.0.1", port=5000, debug=False)
