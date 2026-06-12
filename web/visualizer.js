/* ═══════════════════════════════════════════════════════════════════════════
   PathFind Pro — Visualizer
   Reads output/results.json (or a user-dropped file), draws the graph on a
   Canvas, animates each algorithm's traversal step by step, and renders a
   live benchmark comparison panel.
   ═══════════════════════════════════════════════════════════════════════════ */

const ALGO_COLORS = {
    Dijkstra: '#6c8cff',
    'A*':     '#34d399',
    BFS:      '#fbbf24',
    DFS:      '#f87171',
};

// ── State ────────────────────────────────────────────────────────────────────
let graphData   = null;   // parsed JSON from results.json
let animFrameId = null;
let animState   = null;   // per-algorithm animation queues
let activeAlgos = new Set(['Dijkstra', 'A*', 'BFS', 'DFS']);
let animSpeed   = 120;    // ms per step

// ── DOM refs ─────────────────────────────────────────────────────────────────
const canvas     = document.getElementById('graph-canvas');
const ctx        = canvas.getContext('2d');
const loadBtn    = document.getElementById('load-btn');
const animateBtn = document.getElementById('animate-btn');
const resetBtn   = document.getElementById('reset-btn');
const fileInput  = document.getElementById('json-file');
const speedRange = document.getElementById('speed-range');
const speedLabel = document.getElementById('speed-label');
const benchPanel = document.getElementById('bench-rows');
const statCards  = document.getElementById('stat-cards');
const statusEl   = document.getElementById('status');

// ── Utility ───────────────────────────────────────────────────────────────────
function showStatus(msg, duration = 2500) {
    statusEl.textContent = msg;
    statusEl.classList.add('show');
    clearTimeout(statusEl._t);
    statusEl._t = setTimeout(() => statusEl.classList.remove('show'), duration);
}

function lerp(a, b, t) { return a + (b - a) * t; }

// Map graph coords → canvas pixels
function project(node, margin = 50) {
    const xs = graphData.nodes.map(n => n.x);
    const ys = graphData.nodes.map(n => n.y);
    const minX = Math.min(...xs), maxX = Math.max(...xs);
    const minY = Math.min(...ys), maxY = Math.max(...ys);
    const W = canvas.width  - margin * 2;
    const H = canvas.height - margin * 2;
    const rangeX = (maxX - minX) || 1;
    const rangeY = (maxY - minY) || 1;
    const scale  = Math.min(W / rangeX, H / rangeY);
    const offX   = margin + (W - rangeX * scale) / 2;
    const offY   = margin + (H - rangeY * scale) / 2;
    return {
        x: offX + (node.x - minX) * scale,
        y: offY + (node.y - minY) * scale,
    };
}

// ── Canvas resize ─────────────────────────────────────────────────────────────
function resizeCanvas() {
    // 1. Get the exact pixel dimensions the CSS gave to the canvas
    const rect = canvas.getBoundingClientRect();
    
    // 2. Set the internal drawing board to match perfectly
    canvas.width  = rect.width;
    canvas.height = rect.height;
    
    // 3. Redraw the graph with crystal clear resolution
    if (graphData) drawAll();
}

// ── Drawing helpers ───────────────────────────────────────────────────────────
function drawEdges(alpha = 1) {
    ctx.save();
    ctx.globalAlpha = alpha;
    ctx.strokeStyle = '#2e3350';
    ctx.lineWidth   = 1.5;
    for (const e of graphData.edges) {
        const a = project(graphData.nodes[e.from]);
        const b = project(graphData.nodes[e.to]);
        ctx.beginPath();
        ctx.moveTo(a.x, a.y);
        ctx.lineTo(b.x, b.y);
        ctx.stroke();

        // Edge weight label
        ctx.fillStyle = '#3d4566';
        ctx.font = '10px Inter, sans-serif';
        ctx.textAlign = 'center';
        ctx.fillText(
            e.weight % 1 === 0 ? e.weight : e.weight.toFixed(1),
            (a.x + b.x) / 2, (a.y + b.y) / 2 - 4
        );
    }
    ctx.restore();
}

function drawNode(node, color = '#22263a', borderColor = '#4a5180', label = true, r = 14) {
    const p = project(node);
    ctx.beginPath();
    ctx.arc(p.x, p.y, r, 0, Math.PI * 2);
    ctx.fillStyle = color;
    ctx.fill();
    ctx.strokeStyle = borderColor;
    ctx.lineWidth = 2;
    ctx.stroke();

    if (label) {
        ctx.fillStyle = '#e2e8f0';
        ctx.font = `bold 11px Inter, sans-serif`;
        ctx.textAlign = 'center';
        ctx.textBaseline = 'middle';
        ctx.fillText(node.id, p.x, p.y);
    }
}

function drawPath(pathNodes, color, lineWidth = 3.5, alpha = 0.85) {
    if (pathNodes.length < 2) return;
    ctx.save();
    ctx.globalAlpha = alpha;
    ctx.strokeStyle = color;
    ctx.lineWidth   = lineWidth;
    ctx.lineJoin    = 'round';
    ctx.lineCap     = 'round';
    ctx.setLineDash([]);
    ctx.beginPath();
    const p0 = project(graphData.nodes[pathNodes[0]]);
    ctx.moveTo(p0.x, p0.y);
    for (let i = 1; i < pathNodes.length; i++) {
        const p = project(graphData.nodes[pathNodes[i]]);
        ctx.lineTo(p.x, p.y);
    }
    ctx.stroke();
    ctx.restore();
}

function drawVisited(nodeIds, color, alpha = 0.28) {
    ctx.save();
    for (const id of nodeIds) {
        const p = project(graphData.nodes[id]);
        ctx.globalAlpha = alpha;
        ctx.beginPath();
        ctx.arc(p.x, p.y, 18, 0, Math.PI * 2);
        ctx.fillStyle = color;
        ctx.fill();
    }
    ctx.restore();
}

// ── Full redraw (static) ──────────────────────────────────────────────────────
function drawAll(visitedMap = {}, pathMap = {}) {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    drawEdges(0.7);

    // Draw visited halos
    for (const [name, ids] of Object.entries(visitedMap)) {
        if (activeAlgos.has(name)) drawVisited(ids, ALGO_COLORS[name]);
    }

    // Draw paths
    for (const [name, path] of Object.entries(pathMap)) {
        if (activeAlgos.has(name) && path.length > 1) drawPath(path, ALGO_COLORS[name]);
    }

    // Draw all nodes
    for (const node of graphData.nodes) {
        let color = '#1e2235', border = '#4a5180';
        if (node.id === graphData.src)  { color = '#1a3a6b'; border = '#6c8cff'; }
        if (node.id === graphData.dest) { color = '#1a3b2e'; border = '#34d399'; }
        drawNode(node, color, border);
    }

    // Src / dest labels
    const srcP  = project(graphData.nodes[graphData.src]);
    const dstP  = project(graphData.nodes[graphData.dest]);
    ctx.font = 'bold 10px Inter, sans-serif';
    ctx.textAlign = 'center';
    ctx.fillStyle = '#6c8cff';
    ctx.fillText('SRC',  srcP.x, srcP.y - 22);
    ctx.fillStyle = '#34d399';
    ctx.fillText('DEST', dstP.x, dstP.y - 22);
}

// ── Load & render results ─────────────────────────────────────────────────────
function loadResults(data) {
    graphData = data;
    cancelAnimation();

    // Render stat cards
    statCards.innerHTML = '';
    for (const r of data.results) {
        const card = document.createElement('div');
        card.className = 'stat-card';
        card.innerHTML = `
            <div class="algo-name" style="color:${ALGO_COLORS[r.name] ?? '#aaa'}">${r.name}</div>
            <div class="stat-val">${r.pathFound ? r.totalCost.toFixed(1) : '—'}</div>
            <div class="stat-sub">cost &nbsp;|&nbsp; ${r.nodesVisited} visited</div>
            <div class="stat-sub" style="margin-top:2px">${(r.timeMs * 1000).toFixed(3)} µs</div>
        `;
        statCards.appendChild(card);
    }

    renderBenchPanel(data.results);
    resizeCanvas();
    drawAll();
    animateBtn.disabled = false;
    showStatus('Graph loaded — click Animate to visualise!');
}

// ── Benchmark bars ─────────────────────────────────────────────────────────────
function renderBenchPanel(results, metric = 'nodesVisited') {
    const maxVal = Math.max(...results.map(r => r[metric] || 0)) || 1;
    benchPanel.innerHTML = '';

    for (const r of results) {
        const val   = r[metric] ?? 0;
        const pct   = (val / maxVal * 100).toFixed(1);
        const color = ALGO_COLORS[r.name] ?? '#aaa';
        const label = metric === 'timeMs' ? (val * 1000).toFixed(3) + ' µs' : val;
        const row   = document.createElement('div');
        row.className = 'bench-row';
        row.innerHTML = `
            <span style="color:${color}">${r.name}</span>
            <div class="bench-bar-bg">
                <div class="bench-bar" style="width:${pct}%;background:${color}">${pct > 15 ? r.name : ''}</div>
            </div>
            <span class="bench-val">${label}</span>
        `;
        benchPanel.appendChild(row);
    }
}

// Tab switching for bench panel
document.querySelectorAll('.bench-tab').forEach(tab => {
    tab.addEventListener('click', () => {
        document.querySelectorAll('.bench-tab').forEach(t => t.classList.remove('active'));
        tab.classList.add('active');
        if (graphData) renderBenchPanel(graphData.results, tab.dataset.metric);
    });
});

// ── Animation ─────────────────────────────────────────────────────────────────
/*
  Strategy: for each active algorithm, build a step list:
    - each step = { visited: Set<id>, revealedPath: int[] }
  We then advance all algorithms one step per tick, drawing each frame.
*/
function buildSteps(result) {
    const steps = [];
    const path  = result.path;
    const totalNodes = result.nodesVisited;

    // Simulate discovery order: show path nodes first, then
    // fill remaining visited count with intermediary halos on nearby nodes.
    // Since we don't have per-step data from C++, we animate the path
    // progressively plus a "frontier" halo that grows up to nodesVisited.
    const allNodeIds = graphData.nodes.map(n => n.id);
    // Build a BFS-like ordered list starting from src
    const src    = graphData.src;
    const order  = [];
    const seenSet = new Set();
    const queue  = [src];
    seenSet.add(src);
    while (queue.length && order.length < totalNodes) {
        const u = queue.shift();
        order.push(u);
        for (const e of graphData.edges) {
            let v = -1;
            if (e.from === u && !seenSet.has(e.to))  v = e.to;
            if (e.to   === u && !seenSet.has(e.from)) v = e.from;
            if (v !== -1) { seenSet.add(v); queue.push(v); }
        }
    }
    // Pad to totalNodes if needed
    for (const id of allNodeIds) {
        if (order.length >= totalNodes) break;
        if (!seenSet.has(id)) { order.push(id); seenSet.add(id); }
    }

    for (let i = 1; i <= order.length; i++) {
        const visited      = order.slice(0, i);
        const pathProgress = path.slice(0, Math.round((i / order.length) * path.length));
        steps.push({ visited, path: pathProgress });
    }
    // Final step: full path visible
    steps.push({ visited: order, path });
    return steps;
}

function startAnimation() {
    if (!graphData) return;
    cancelAnimation();

    animateBtn.disabled = true;
    resetBtn.disabled   = false;

    animState = {};
    for (const r of graphData.results) {
        if (!activeAlgos.has(r.name)) continue;
        animState[r.name] = {
            steps:    buildSteps(r),
            stepIdx:  0,
            done:     false,
            result:   r,
        };
    }

    let lastTick = 0;
    function tick(ts) {
        if (ts - lastTick < animSpeed) {
            animFrameId = requestAnimationFrame(tick);
            return;
        }
        lastTick = ts;

        const visitedMap = {};
        const pathMap    = {};
        let allDone = true;

        for (const [name, state] of Object.entries(animState)) {
            if (!state.done) {
                state.stepIdx = Math.min(state.stepIdx + 1, state.steps.length - 1);
                if (state.stepIdx >= state.steps.length - 1) state.done = true;
                else allDone = false;
            }
            const step           = state.steps[state.stepIdx];
            visitedMap[name]     = step.visited;
            pathMap[name]        = step.path;
        }

        drawAll(visitedMap, pathMap);

        if (allDone) {
            showStatus('Animation complete!');
            animateBtn.disabled = false;
        } else {
            animFrameId = requestAnimationFrame(tick);
        }
    }
    animFrameId = requestAnimationFrame(tick);
}

function cancelAnimation() {
    if (animFrameId) { cancelAnimationFrame(animFrameId); animFrameId = null; }
}

function resetView() {
    cancelAnimation();
    animState = null;
    if (graphData) drawAll();
    animateBtn.disabled = false;
    resetBtn.disabled   = true;
}

// ── Event wiring ──────────────────────────────────────────────────────────────
animateBtn.addEventListener('click', startAnimation);
resetBtn.addEventListener('click', resetView);

loadBtn.addEventListener('click', () => fileInput.click());
fileInput.addEventListener('change', () => {
    const file = fileInput.files[0];
    if (!file) return;
    const reader = new FileReader();
    reader.onload = e => {
        try {
            loadResults(JSON.parse(e.target.result));
        } catch {
            showStatus('Error: Invalid JSON file.');
        }
    };
    reader.readAsText(file);
});

speedRange.addEventListener('input', () => {
    animSpeed = Math.round(20 + (10 - speedRange.value) * 50);
    const labels = ['', 'Slowest', 'Slow', 'Slow', 'Medium', 'Medium', 'Medium', 'Fast', 'Fast', 'Fastest', 'Fastest'];
    speedLabel.textContent = labels[speedRange.value] || '';
});

document.querySelectorAll('.algo-toggle').forEach(tog => {
    tog.addEventListener('click', () => {
        const name = tog.dataset.algo;
        tog.classList.toggle('active');
        if (tog.classList.contains('active')) activeAlgos.add(name);
        else activeAlgos.delete(name);
        if (graphData && !animFrameId) drawAll();
    });
});

// Default: all active
document.querySelectorAll('.algo-toggle').forEach(t => t.classList.add('active'));

// ── Auto-load results.json (same origin) ──────────────────────────────────────
async function tryAutoLoad() {
    try {
        const res = await fetch('../output/results.json');
        if (!res.ok) throw new Error('not found');
        const data = await res.json();
        loadResults(data);
    } catch {
        showStatus('No results.json found. Run the C++ backend first, or load a JSON file.', 5000);
        drawPlaceholder();
    }
}

function drawPlaceholder() {
    resizeCanvas();
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    ctx.fillStyle = '#2e3350';
    ctx.font = 'bold 16px Inter, sans-serif';
    ctx.textAlign = 'center';
    ctx.textBaseline = 'middle';
    ctx.fillText('Run the C++ backend to generate results.json', canvas.width / 2, canvas.height / 2 - 16);
    ctx.font = '13px Inter, sans-serif';
    ctx.fillStyle = '#4a5180';
    ctx.fillText('Or click "Load JSON" to open a file manually', canvas.width / 2, canvas.height / 2 + 16);
}

window.addEventListener('resize', () => { if (graphData) resizeCanvas(); else drawPlaceholder(); });
window.addEventListener('load',   tryAutoLoad);
