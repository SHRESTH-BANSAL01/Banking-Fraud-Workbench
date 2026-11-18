
"use strict";

// Initial preview data from the C evaluation report.
// Step 31 will load these values through Flask instead.
const evaluationData = {
  Small: {
    transactions: 10, tp: 4, tn: 5, fp: 1, fn: 0,
    accuracy: 90, precision: 80, recall: 100, f1: 88.89
  },
  Medium: {
    transactions: 5000, tp: 376, tn: 4599, fp: 0, fn: 25,
    accuracy: 99.50, precision: 100, recall: 93.77, f1: 96.78
  },
  Shock: {
    transactions: 5000, tp: 755, tn: 1773, fp: 1745, fn: 727,
    accuracy: 50.56, precision: 30.20, recall: 50.94, f1: 37.92
  },
  Large: {
    transactions: 50000, tp: 3641, tn: 46113, fp: 0, fn: 246,
    accuracy: 99.51, precision: 100, recall: 93.67, f1: 96.73
  },
  Adversarial: {
    transactions: 1000, tp: 214, tn: 203, fp: 204, fn: 379,
    accuracy: 41.70, precision: 51.20, recall: 36.09, f1: 42.33
  }
};

const numberFormat = new Intl.NumberFormat("en-IN");

function setText(id, value) {
  document.getElementById(id).textContent = value;
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
  container.replaceChildren();

  Object.entries(values).forEach(([label, value]) => {
    container.appendChild(createBar(label, value));
  });
}

function updateDashboard(datasetName) {
  const data = evaluationData[datasetName];
  if (!data) return;

  setText("stat-transactions", numberFormat.format(data.transactions));
  setText("stat-fraud", numberFormat.format(data.tp + data.fn));
  setText("stat-accuracy", `${data.accuracy.toFixed(2)}%`);
  setText("stat-f1", `${data.f1.toFixed(2)}%`);

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

const titles = {
  dashboard: "Dashboard Overview",
  analysis: "Fraud Analysis",
  algorithms: "Algorithm Workbench",
  reports: "Generated Reports"
};

function openTab(target) {
  document.querySelectorAll(".tab-page").forEach(page => {
    page.classList.toggle("active", page.id === target);
  });

  document.querySelectorAll(".tab").forEach(button => {
    button.classList.toggle("active", button.dataset.tab === target);
  });

  document.querySelectorAll(".side-link").forEach(button => {
    button.classList.toggle("active", button.dataset.target === target);
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

    if (titles[target]) {
      event.preventDefault();
      openTab(target);
    }
  });
});

document.getElementById("dataset-select").addEventListener(
  "change",
  event => updateDashboard(event.target.value)
);

// Algorithm execution is deliberately disabled until
// the Flask backend is connected.
document.querySelectorAll("[data-command]").forEach(button => {
  button.addEventListener("click", () => {
    const command = button.dataset.command;

    const output = document.getElementById("console-output");

    output.textContent =
      `Selected command: --${command}\n\n` +
      "Frontend preview mode.\n" +
      "No C program was executed.\n\n" +
      "Flask backend integration is the next step.";

    output.scrollIntoView({
      behavior: "smooth",
      block: "nearest"
    });
  });
});

renderAccuracyChart();
updateDashboard("Large");
