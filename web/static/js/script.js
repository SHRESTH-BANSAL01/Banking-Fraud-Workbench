
"use strict";

let evaluationData = {};

const numberFormat = new Intl.NumberFormat("en-IN");

function setText(id, value) {
    const element = document.getElementById(id);
    if (element) element.textContent = value;
}

function createBar(label, value) {
    const row = document.createElement("div");
    row.className = "bar-row";

    const name = document.createElement("span");
    name.className = "bar-label";
    name.textContent = label;

    const track = document.createElement("div");
    track.className = "bar-track";

    const fill = document.createElement("div");
    fill.className = "bar-fill";
    fill.style.width = `${Math.max(0, Math.min(100, value))}%`;

    const amount = document.createElement("span");
    amount.className = "bar-value";
    amount.textContent = `${value.toFixed(2)}%`;

    track.appendChild(fill);
    row.append(name, track, amount);

    return row;
}

function renderChart(containerId, values) {
    const container = document.getElementById(containerId);
    if (!container) return;

    container.replaceChildren();

    Object.entries(values).forEach(([label, value]) => {
        container.appendChild(createBar(label, value));
    });
}

function updateDashboard(datasetName) {
    const data = evaluationData[datasetName];
    if (!data) return;

    setText("stat-transactions",
        numberFormat.format(data.transactions));

    setText("stat-fraud",
        numberFormat.format(data.tp + data.fn));

    setText("stat-accuracy",
        `${data.accuracy.toFixed(2)}%`);

    setText("stat-f1",
        `${data.f1.toFixed(2)}%`);

    setText("cm-tp", numberFormat.format(data.tp));
    setText("cm-tn", numberFormat.format(data.tn));
    setText("cm-fp", numberFormat.format(data.fp));
    setText("cm-fn", numberFormat.format(data.fn));

    renderChart("metrics-chart", {
        Accuracy: data.accuracy,
        Precision: data.precision,
        Recall: data.recall,
        "F1-Score": data.f1
    });
}

function renderAccuracyChart() {
    const values = {};

    for (const [name, data] of Object.entries(evaluationData)) {
        values[name] = data.accuracy;
    }

    renderChart("accuracy-chart", values);
}

async function loadEvaluation() {
    try {
        const response = await fetch("/api/evaluation");
        const result = await response.json();

        if (!response.ok || !result.success) {
            throw new Error(result.error || "Cannot load evaluation");
        }

        evaluationData = result.datasets;

        const select = document.getElementById("dataset-select");
        const names = Object.keys(evaluationData);

        if (names.length === 0) {
            throw new Error("Evaluation report is empty");
        }

        const previous = select.value;

        select.replaceChildren();

        for (const name of names) {
            const option = document.createElement("option");
            option.value = name;
            option.textContent =
                `${name} — ${numberFormat.format(
                    evaluationData[name].transactions
                )}`;

            select.appendChild(option);
        }

        select.value = names.includes(previous)
            ? previous
            : names.includes("Large")
                ? "Large"
                : names[0];

        renderAccuracyChart();
        updateDashboard(select.value);

    } catch (error) {
        setText("console-output",
            `Unable to load evaluation data:\n${error.message}`);
    }
}

function openTab(target) {
    document.querySelectorAll(".tab-page").forEach(page => {
        page.classList.toggle("active", page.id === target);
    });

    document.querySelectorAll(".tab").forEach(button => {
        button.classList.toggle(
            "active",
            button.dataset.tab === target
        );
    });

    document.querySelectorAll(".side-link").forEach(button => {
        button.classList.toggle(
            "active",
            button.dataset.target === target
        );
    });

    window.scrollTo({ top: 0, behavior: "smooth" });
}

document.querySelectorAll(".tab").forEach(button => {
    button.addEventListener("click", () => {
        openTab(button.dataset.tab);
    });
});

document.querySelectorAll(".side-link").forEach(button => {
    button.addEventListener("click", () => {
        openTab(button.dataset.target);
    });
});

document.querySelectorAll(".top-links a").forEach(link => {
    link.addEventListener("click", event => {
        const target = link.getAttribute("href").slice(1);

        if (document.getElementById(target)) {
            event.preventDefault();
            openTab(target);
        }
    });
});

document.getElementById("dataset-select").addEventListener(
    "change",
    event => updateDashboard(event.target.value)
);

async function runCommand(command, button) {
    const output = document.getElementById("console-output");
    const status = document.getElementById("backend-status");

    button.disabled = true;
    status.textContent = "RUNNING";
    output.textContent = `Executing fraud.exe --${command}...\n`;

    try {
        const response = await fetch(`/api/run/${command}`, {
            method: "POST"
        });

        const result = await response.json();

        output.textContent =
            (result.output || "") +
            (result.error ? `\n${result.error}` : "");

        status.textContent = result.success ? "SUCCESS" : "FAILED";

        if (result.success && command === "evaluate") {
            await loadEvaluation();
        }

        if (result.success) {
            await loadReports();
        }

    } catch (error) {
        status.textContent = "ERROR";
        output.textContent = `Connection error: ${error.message}`;
    } finally {
        button.disabled = false;
        output.scrollIntoView({
            behavior: "smooth",
            block: "nearest"
        });
    }
}

document.querySelectorAll("[data-command]").forEach(button => {
    button.addEventListener("click", () => {
        runCommand(button.dataset.command, button);
    });
});


async function loadReports() {
  const container = document.getElementById("report-downloads");
  if (!container) return;

  try {
    const response = await fetch("/api/reports");
    const result = await response.json();

    if (!response.ok || !result.success) {
      throw new Error("Could not load reports");
    }

    container.replaceChildren();

    if (result.reports.length === 0) {
      container.textContent =
        "No reports found. Run an algorithm to generate reports.";
      return;
    }

    result.reports.forEach(report => {
      const card = document.createElement("div");
      card.className = "report-card";

      const name = document.createElement("strong");
      name.textContent = report.name;

      const size = document.createElement("small");
      size.textContent =
        `${(report.size_bytes / 1024).toFixed(1)} KB`;

      const link = document.createElement("a");
      link.href = report.url;
      link.textContent = "Download CSV";
      link.setAttribute("download", report.name);

      card.append(name, size, link);
      container.appendChild(card);
    });

  } catch (error) {
    container.textContent = `Error: ${error.message}`;
  }
}

async function searchTransactions() {
  const dataset = document.getElementById("transaction-dataset").value;
  const search = document.getElementById("transaction-search").value;
  const filter = document.getElementById("transaction-filter").value;

  const tbody = document.getElementById("transaction-results");
  const summary = document.getElementById("transaction-summary");

  tbody.replaceChildren();
  summary.textContent = "Searching transactions...";

  const params = new URLSearchParams({ dataset, search, filter });

  try {
    const response = await fetch(`/api/transactions?${params}`);
    const result = await response.json();

    if (!response.ok || !result.success) {
      throw new Error(result.error || "Search failed");
    }

    for (const transaction of result.transactions) {
      const row = document.createElement("tr");

      const cells = [
        transaction.transaction_id,
        transaction.account_id,
        `₹${numberFormat.format(transaction.amount)}`,
        String(transaction.risk_score)
      ];

      for (const value of cells) {
        const cell = document.createElement("td");
        cell.textContent = value;
        row.appendChild(cell);
      }

      const statusCell = document.createElement("td");
      const badge = document.createElement("span");

      badge.className =
        `risk-badge ${transaction.suspicious ? "suspicious" : "normal"}`;

      badge.textContent = transaction.suspicious
        ? "Suspicious"
        : "Normal";

      statusCell.appendChild(badge);
      row.appendChild(statusCell);
      tbody.appendChild(row);
    }

    if (result.transactions.length === 0) {
      const row = document.createElement("tr");
      const cell = document.createElement("td");
      cell.colSpan = 5;
      cell.textContent = "No matching transactions found.";
      row.appendChild(cell);
      tbody.appendChild(row);
    }

    summary.textContent =
      `Showing ${result.displayed} of ${result.total_matches} matching transactions.`;

  } catch (error) {
    summary.textContent = `Error: ${error.message}`;
  }
}

document.getElementById("search-transactions")
  .addEventListener("click", searchTransactions);

document.getElementById("transaction-search")
  .addEventListener("keydown", event => {
    if (event.key === "Enter") {
      searchTransactions();
    }
  });

document.getElementById("refresh-reports")
  .addEventListener("click", loadReports);


async function checkBackend() {
    try {
        const response = await fetch("/api/status");
        const result = await response.json();

        setText(
            "backend-status",
            result.connected ? "C ENGINE READY" : "C ENGINE MISSING"
        );

    } catch {
        setText("backend-status", "BACKEND OFFLINE");
    }
}

async function initialize() {
    await checkBackend();
    await loadEvaluation();
    await loadReports();
}

initialize();
