const EXAMPLES = {
  demo: `int choice = 2;
int result = 0;

switch (choice) {
    case 1:
        result = 10;
        print(result);
        break;

    case 2:
        result = 20;
        print(result);
        break;

    case 3:
        result = 30;
        print(result);
        break;

    default:
        result = 0;
        print(result);
        break;
}`,

  single: `int x = 1;
switch (x) {
    case 1:
        print(10);
        break;
}`,

  multi: `int code = 3;
int out = 0;
switch (code) {
    case 1:
        out = 100;
        break;
    case 2:
        out = 200;
        break;
    case 3:
        out = 300;
        break;
}
print(out);`,

  expr: `int a = 10;
int b = 2;
switch (a + b * 3) {
    case 16:
        print(1);
        break;
    default:
        print(0);
        break;
}`,

  negative: `int temp = -5;
switch (temp) {
    case -10:
        print(1);
        break;
    case -5:
        print(2);
        break;
    case 0:
        print(3);
        break;
    case 5:
        print(4);
        break;
}`,

  nested: `int outer = 1;
int inner = 2;

switch (outer) {
    case 1:
        switch (inner) {
            case 2:
                print(200);
                break;
            default:
                print(0);
                break;
        }
        break;
    default:
        print(999);
        break;
}`,

  err_dup: `int choice = 1;
switch (choice) {
    case 1:
        print(10);
        break;
    case 1:
        print(20);
        break;
}`,

  err_default: `int x = 2;
switch (x) {
    case 1:
        print(1);
        break;
    default:
        print(0);
        break;
    default:
        print(99);
        break;
}`,

  err_break: `int x = 1;
print(x);
break;`,

  err_undeclared: `switch (undeclaredVar) {
    case 1:
        print(1);
        break;
}`,

  err_syntax: `int x = 1;
switch (x) {
    case 1:
        x = 10
        break;
}`
};

const codeInput = document.getElementById("codeInput");
const compileBtn = document.getElementById("compileBtn");
const exampleSelect = document.getElementById("exampleSelect");
const terminalOutput = document.getElementById("terminalOutput");
const astOutput = document.getElementById("astOutput");
const lineCount = document.getElementById("lineCount");
const statusDot = document.getElementById("statusDot");
const statusText = document.getElementById("statusText");

// Set default code
codeInput.value = EXAMPLES.demo;

// Tab Switching
document.querySelectorAll(".tab-btn").forEach(btn => {
  btn.addEventListener("click", () => {
    document.querySelectorAll(".tab-btn").forEach(b => b.classList.remove("active"));
    document.querySelectorAll(".tab-content").forEach(c => c.classList.add("hidden"));

    btn.classList.add("active");
    const target = btn.dataset.tab;
    if (target === "terminal") document.getElementById("tabTerminal").classList.remove("hidden");
    if (target === "ast") document.getElementById("tabAst").classList.remove("hidden");
    if (target === "phases") document.getElementById("tabPhases").classList.remove("hidden");
  });
});

// Update Line / Col
function updateCursorPosition() {
  const text = codeInput.value.substr(0, codeInput.selectionStart);
  const lines = text.split("\n");
  const currentLine = lines.length;
  const currentCol = lines[lines.length - 1].length + 1;
  lineCount.textContent = `Line ${currentLine}, Col ${currentCol}`;
}
codeInput.addEventListener("keyup", updateCursorPosition);
codeInput.addEventListener("click", updateCursorPosition);

// Example Selector Change
exampleSelect.addEventListener("change", (e) => {
  if (EXAMPLES[e.target.value]) {
    codeInput.value = EXAMPLES[e.target.value];
    updateCursorPosition();
    compileCode();
  }
});

// Support TAB key indentation in textarea
codeInput.addEventListener("keydown", (e) => {
  if (e.key === "Tab") {
    e.preventDefault();
    const start = codeInput.selectionStart;
    const end = codeInput.selectionEnd;
    codeInput.value = codeInput.value.substring(0, start) + "    " + codeInput.value.substring(end);
    codeInput.selectionStart = codeInput.selectionEnd = start + 4;
  }
});

// Compile function
async function compileCode() {
  const code = codeInput.value;
  statusDot.className = "status-indicator ready";
  statusText.textContent = "Compiling with GCC + Flex + Bison...";
  terminalOutput.textContent = "Running compiler engine...";
  astOutput.textContent = "Processing...";

  try {
    const res = await fetch("/api/compile", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ code })
    });

    const data = await res.json();
    const fullOutput = (data.stdout || "") + (data.stderr || "");
    terminalOutput.textContent = fullOutput;

    // Extract AST
    const astMarker = " ABSTRACT SYNTAX TREE (AST)\n----------------------------------------\n";
    const astEndMarker = "\n----------------------------------------\nSemantic analysis";
    const astStartIndex = fullOutput.indexOf(astMarker);
    if (astStartIndex !== -1) {
      const remaining = fullOutput.substring(astStartIndex + astMarker.length);
      const astEndIndex = remaining.indexOf(astEndMarker);
      if (astEndIndex !== -1) {
        astOutput.textContent = remaining.substring(0, astEndIndex);
      } else {
        astOutput.textContent = remaining;
      }
    } else {
      astOutput.textContent = "No AST generated (Compilation failed at Lexical/Syntax phase).\n\n" + (data.stderr || "");
    }

    // Update Statuses
    if (data.exit_code === 0) {
      statusDot.className = "status-indicator success";
      statusText.textContent = "Compilation Result: SUCCESS";
      terminalOutput.className = "console-box success";
      updatePhases(true, true, true, true);
    } else {
      statusDot.className = "status-indicator error";
      statusText.textContent = "Compilation Result: FAILURE";
      terminalOutput.className = "console-box error";
      
      const isLexErr = fullOutput.includes("Lexical Error");
      const isSynErr = fullOutput.includes("Syntax Error");
      const isSemErr = fullOutput.includes("Semantic Error");

      if (isLexErr) updatePhases(false, false, false, false);
      else if (isSynErr) updatePhases(true, false, false, false);
      else if (isSemErr) updatePhases(true, true, true, false);
      else updatePhases(false, false, false, false);
    }
  } catch (err) {
    terminalOutput.textContent = "Connection error: " + err.message;
    statusDot.className = "status-indicator error";
    statusText.textContent = "Server Error";
  }
}

function updatePhases(lex, syn, ast, sem) {
  setPhase("phaseLex", lex);
  setPhase("phaseSyn", syn);
  setPhase("phaseAst", ast);
  setPhase("phaseSem", sem);
}

function setPhase(id, passed) {
  const el = document.getElementById(id);
  const statusEl = el.querySelector(".phase-status");
  if (passed) {
    el.className = "phase-card passed";
    statusEl.textContent = "Passed";
  } else {
    el.className = "phase-card failed";
    statusEl.textContent = "Failed";
  }
}

compileBtn.addEventListener("click", compileCode);

// Initial compile on load
compileCode();
