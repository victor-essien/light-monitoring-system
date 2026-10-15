#pragma once

// ESP32 Night Light dashboard (v2)
// Same /api JSON contract as before:
//   { brightness:0-100, darkness:0-100, threshold:0-100, ledBrightness:0-255, nightMode:bool }
// No external fonts, scripts or images, so it works with no internet connection.

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<meta name="theme-color" content="#080B12">
<title>Night Light</title>

<style>
* { box-sizing: border-box; margin: 0; padding: 0; }

:root {
  --bg: #080B12;
  --surface: #151C2B;
  --panel: rgba(21, 28, 43, 0.72);
  --line: rgba(255, 255, 255, 0.07);
  --text: #F3F5FA;
  --muted: #8A94A8;
  --amber: #FFB15C;
  --blue: #6F8FFF;
  --violet: #A78BFA;
  --gold: #FFD166;
  --green: #55E6A5;
  --red: #FF5C70;
  --glow: 0;
}

body {
  min-height: 100vh;
  font-family: ui-rounded, "SF Pro Rounded", Inter, system-ui, -apple-system, "Segoe UI", Roboto, sans-serif;
  color: var(--text);
  background: var(--bg);
  overflow-x: hidden;
  -webkit-font-smoothing: antialiased;
}

/* ---------- ambient background: shifts to amber at night ---------- */
.ambient {
  position: fixed;
  inset: 0;
  pointer-events: none;
  z-index: 0;
}
.ambient::before,
.ambient::after {
  content: "";
  position: absolute;
  inset: 0;
  transition: opacity 1.6s ease;
}
.ambient::before {
  background:
    radial-gradient(circle at 12% 8%, rgba(111, 143, 255, 0.16), transparent 34%),
    radial-gradient(circle at 90% 85%, rgba(111, 143, 255, 0.08), transparent 30%);
  opacity: 1;
}
.ambient::after {
  background:
    radial-gradient(circle at 28% 30%, rgba(255, 177, 92, 0.20), transparent 38%),
    radial-gradient(circle at 85% 90%, rgba(255, 140, 70, 0.10), transparent 32%);
  opacity: 0;
}
body.night .ambient::after { opacity: 1; }
body.night .ambient::before { opacity: 0.35; }

/* ---------- layout ---------- */
.app {
  position: relative;
  z-index: 1;
  width: min(1120px, calc(100% - 28px));
  margin: 0 auto;
  padding: 28px 0 36px;
}

.card {
  background: var(--panel);
  border: 1px solid var(--line);
  border-radius: 24px;
  backdrop-filter: blur(16px);
  -webkit-backdrop-filter: blur(16px);
  box-shadow: 0 18px 50px rgba(0, 0, 0, 0.28);
}

/* ---------- header ---------- */
.header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  gap: 12px;
  margin-bottom: 22px;
}
.brand { display: flex; align-items: center; gap: 12px; min-width: 0; }
.logo {
  flex: none;
  width: 42px;
  height: 42px;
  border-radius: 13px;
  display: grid;
  place-items: center;
  background: linear-gradient(145deg, #1D2640, #0F1524);
  border: 1px solid var(--line);
  transition: box-shadow 1.2s ease;
}
.logo::after {
  content: "";
  width: 14px;
  height: 14px;
  border-radius: 50%;
  background: var(--muted);
  transition: background 1.2s ease, box-shadow 1.2s ease;
}
body.night .logo { box-shadow: 0 0 26px rgba(255, 177, 92, 0.28); }
body.night .logo::after { background: var(--amber); box-shadow: 0 0 18px var(--amber); }

.brand h1 { font-size: 18px; letter-spacing: -0.3px; }
.brand p { color: var(--muted); font-size: 12px; margin-top: 2px; }

.connection {
  flex: none;
  display: flex;
  align-items: center;
  gap: 9px;
  padding: 8px 13px;
  border: 1px solid var(--line);
  background: rgba(255, 255, 255, 0.035);
  border-radius: 999px;
  font-size: 12px;
  line-height: 1.25;
}
.conn-dot {
  width: 8px;
  height: 8px;
  border-radius: 50%;
  background: var(--muted);
}
.connection.ok .conn-dot { background: var(--green); box-shadow: 0 0 10px var(--green); animation: pulse 2.4s infinite; }
.connection.warn .conn-dot { background: var(--amber); box-shadow: 0 0 10px var(--amber); animation: pulse 0.9s infinite; }
.connection.bad .conn-dot { background: var(--red); box-shadow: 0 0 10px var(--red); }
.conn-text { display: flex; flex-direction: column; }
#connState { font-weight: 600; }
#connTime { color: var(--muted); font-size: 11px; }

@keyframes pulse { 0%, 100% { opacity: 1; } 50% { opacity: 0.35; } }

/* ---------- hero ---------- */
.hero {
  display: grid;
  grid-template-columns: 1.35fr 0.65fr;
  gap: 16px;
  margin-bottom: 16px;
}

.lamp-card {
  display: grid;
  grid-template-columns: minmax(180px, 0.9fr) 1.1fr;
  align-items: center;
  gap: 8px;
  padding: 28px 34px 28px 18px;
  min-height: 330px;
  overflow: hidden;
}

.lamp { width: 100%; max-width: 270px; margin: 0 auto; display: block; overflow: visible; }
.lamp .halo { opacity: calc(var(--glow) * 0.95); transition: opacity 1.2s ease; }
.lamp .cone { opacity: calc(var(--glow) * 0.85); transition: opacity 1.2s ease; }
.lamp .shade-lit { opacity: var(--glow); transition: opacity 1.2s ease; }
.lamp .bulb-lit { opacity: var(--glow); transition: opacity 1.2s ease; }
.lamp .filament { transition: stroke 1.2s ease; }

.lamp-text h2 {
  font-size: clamp(26px, 3.4vw, 40px);
  line-height: 1.05;
  letter-spacing: -1.4px;
  font-weight: 750;
}
.lamp-text p {
  color: var(--muted);
  line-height: 1.55;
  margin-top: 14px;
  font-size: 14px;
  max-width: 420px;
}

.chips { display: flex; flex-wrap: wrap; gap: 8px; margin-top: 22px; }
.chip {
  display: inline-flex;
  align-items: center;
  gap: 8px;
  padding: 8px 13px;
  border-radius: 999px;
  background: rgba(255, 255, 255, 0.05);
  border: 1px solid var(--line);
  font-size: 13px;
  transition: border-color 0.8s ease, background 0.8s ease;
}
.chip i {
  width: 8px;
  height: 8px;
  border-radius: 50%;
  background: var(--green);
  box-shadow: 0 0 12px var(--green);
  transition: background 0.8s ease, box-shadow 0.8s ease;
}
body.night #modeChip { border-color: rgba(255, 177, 92, 0.4); background: rgba(255, 177, 92, 0.08); }
body.night #modeChip i { background: var(--amber); box-shadow: 0 0 12px var(--amber); }
#ledChip i { background: #3A4358; box-shadow: none; }
#ledChip.on i { background: var(--gold); box-shadow: 0 0 12px var(--gold); }

/* ---------- ring gauge ---------- */
.gauge-card {
  padding: 24px 22px 22px;
  display: flex;
  flex-direction: column;
  align-items: center;
}
.card-title { align-self: flex-start; font-size: 15px; font-weight: 650; }
.card-sub { align-self: flex-start; color: var(--muted); font-size: 12px; margin: 3px 0 10px; }

.ring { position: relative; width: 190px; height: 190px; margin: 4px 0 14px; }
.ring svg { width: 100%; height: 100%; transform: rotate(-90deg); }
.ring .track { fill: none; stroke: rgba(255, 255, 255, 0.07); stroke-width: 12; }
.ring .bar {
  fill: none;
  stroke: url(#ringGrad);
  stroke-width: 12;
  stroke-linecap: round;
  stroke-dasharray: 452.4;
  stroke-dashoffset: 452.4;
  transition: stroke-dashoffset 0.7s cubic-bezier(.2, .8, .2, 1);
  filter: drop-shadow(0 0 8px rgba(111, 143, 255, 0.45));
}
.ring-center {
  position: absolute;
  inset: 0;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
}
.ring-value { font-size: 44px; font-weight: 750; letter-spacing: -2px; font-variant-numeric: tabular-nums; }
.ring-value small { font-size: 20px; letter-spacing: 0; color: var(--muted); margin-left: 2px; }
.ring-label { color: var(--muted); font-size: 12px; margin-top: -2px; }

.pair { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; width: 100%; }
.pair div {
  padding: 10px 12px;
  border-radius: 14px;
  background: rgba(255, 255, 255, 0.04);
  border: 1px solid var(--line);
  font-size: 12px;
  color: var(--muted);
}
.pair b { display: block; font-size: 17px; color: var(--text); margin-top: 2px; font-variant-numeric: tabular-nums; }

/* ---------- metrics ---------- */
.metrics { display: grid; grid-template-columns: repeat(4, 1fr); gap: 14px; margin-bottom: 16px; }
.metric { padding: 20px 22px; position: relative; overflow: hidden; }
.metric::before {
  content: "";
  position: absolute;
  left: 0;
  top: 18px;
  bottom: 18px;
  width: 3px;
  border-radius: 0 3px 3px 0;
  background: var(--c);
  box-shadow: 0 0 14px var(--c);
}
.metric::after {
  content: "";
  position: absolute;
  right: -40px;
  top: -50px;
  width: 130px;
  height: 130px;
  border-radius: 50%;
  background: var(--c);
  opacity: 0.07;
  filter: blur(22px);
}
.metric-name { color: var(--muted); font-size: 13px; }
.metric-value {
  font-size: 34px;
  font-weight: 750;
  letter-spacing: -1.4px;
  margin-top: 12px;
  font-variant-numeric: tabular-nums;
}
.metric-sub { color: var(--muted); font-size: 12px; margin-top: 4px; }
.m-bright { --c: var(--blue); }
.m-dark { --c: var(--violet); }
.m-thresh { --c: var(--amber); }
.m-led { --c: var(--gold); }

/* ---------- lower ---------- */
.lower { display: grid; grid-template-columns: 1.5fr 1fr; gap: 16px; }
.panel { padding: 24px; }

.chart-head { display: flex; justify-content: space-between; align-items: flex-start; gap: 12px; }
.legend { display: flex; gap: 14px; font-size: 11px; color: var(--muted); flex-wrap: wrap; justify-content: flex-end; }
.legend span { display: inline-flex; align-items: center; gap: 6px; }
.legend i { width: 14px; height: 0; border-top: 2px solid var(--blue); }
.legend i.dash { border-top: 2px dashed var(--amber); }
.legend i.box { width: 10px; height: 10px; border: 0; background: rgba(255, 177, 92, 0.25); border-radius: 3px; }

.chart-wrap { margin-top: 16px; }
#chart { width: 100%; height: 230px; display: block; }

.explain { color: var(--muted); font-size: 13px; line-height: 1.55; margin-bottom: 26px; }

.tt { position: relative; height: 12px; background: rgba(255, 255, 255, 0.07); border-radius: 999px; margin-top: 34px; }
.tt-fill {
  height: 100%;
  width: 0;
  border-radius: inherit;
  background: linear-gradient(90deg, #6D5BD0, var(--violet));
  transition: width 0.6s ease;
}
.tt-marker {
  position: absolute;
  top: 50%;
  left: 0;
  width: 4px;
  height: 28px;
  border-radius: 4px;
  background: var(--amber);
  transform: translate(-50%, -50%);
  box-shadow: 0 0 16px rgba(255, 177, 92, 0.7);
  transition: left 0.5s ease;
}
.tt-flag {
  position: absolute;
  top: -30px;
  left: 0;
  transform: translateX(-50%);
  font-size: 11px;
  color: var(--amber);
  white-space: nowrap;
  transition: left 0.5s ease;
}
.tt-scale { display: flex; justify-content: space-between; margin-top: 8px; color: var(--muted); font-size: 11px; }

.verdict {
  margin-top: 24px;
  padding: 14px 16px;
  border-radius: 16px;
  background: rgba(255, 255, 255, 0.04);
  border: 1px solid var(--line);
  font-size: 13px;
  line-height: 1.5;
  transition: border-color 0.8s ease, background 0.8s ease;
}
body.night .verdict { border-color: rgba(255, 177, 92, 0.35); background: rgba(255, 177, 92, 0.07); }

.footer { color: var(--muted); font-size: 11px; text-align: center; margin-top: 22px; }

/* ---------- when the ESP32 stops answering ---------- */
.app.stale .lamp,
.app.stale .metric-value,
.app.stale .ring-value { opacity: 0.45; transition: opacity 0.6s; }

/* ---------- responsive ---------- */
@media (max-width: 900px) {
  .hero { grid-template-columns: 1fr; }
  .metrics { grid-template-columns: repeat(2, 1fr); }
  .lower { grid-template-columns: 1fr; }
}

@media (max-width: 560px) {
  .app { width: calc(100% - 20px); padding-top: 16px; }
  .brand p { display: none; }
  .lamp-card { grid-template-columns: 1fr; padding: 24px 22px; min-height: 0; text-align: center; justify-items: center; }
  .lamp { max-width: 190px; }
  .lamp-text p { margin-left: auto; margin-right: auto; }
  .chips { justify-content: center; }
  .metrics { gap: 10px; }
  .metric { padding: 16px 16px 16px 18px; }
  .metric-value { font-size: 28px; }
  .panel { padding: 20px; }
  #chart { height: 190px; }
}

@media (prefers-reduced-motion: reduce) {
  *, *::before, *::after { animation: none !important; transition: none !important; }
}
</style>
</head>

<body>
<div class="ambient"></div>

<main class="app" id="app">

  <header class="header">
    <div class="brand">
      <div class="logo"></div>
      <div>
        <h1>Night Light</h1>
        <p>ESP32 ambient light controller</p>
      </div>
    </div>

    <div class="connection" id="connection" role="status" aria-live="polite">
      <span class="conn-dot"></span>
      <span class="conn-text">
        <span id="connState">Connecting</span>
        <span id="connTime">Waiting for first reading</span>
      </span>
    </div>
  </header>

  <section class="hero">

    <div class="card lamp-card">
      <svg class="lamp" viewBox="0 0 260 320" aria-hidden="true">
        <defs>
          <radialGradient id="halo" cx="50%" cy="42%" r="50%">
            <stop offset="0" stop-color="#FFB15C" stop-opacity="0.85"/>
            <stop offset="0.45" stop-color="#FFB15C" stop-opacity="0.26"/>
            <stop offset="1" stop-color="#FFB15C" stop-opacity="0"/>
          </radialGradient>
          <linearGradient id="cone" x1="0" y1="0" x2="0" y2="1">
            <stop offset="0" stop-color="#FFC27A" stop-opacity="0.5"/>
            <stop offset="1" stop-color="#FFC27A" stop-opacity="0"/>
          </linearGradient>
          <linearGradient id="shadeDark" x1="0" y1="0" x2="1" y2="0">
            <stop offset="0" stop-color="#1B2338"/>
            <stop offset="0.5" stop-color="#262F48"/>
            <stop offset="1" stop-color="#1B2338"/>
          </linearGradient>
          <linearGradient id="shadeLit" x1="0" y1="0" x2="1" y2="0">
            <stop offset="0" stop-color="#E8923F"/>
            <stop offset="0.5" stop-color="#FFC987"/>
            <stop offset="1" stop-color="#E8923F"/>
          </linearGradient>
          <radialGradient id="bulbLit" cx="50%" cy="50%" r="50%">
            <stop offset="0" stop-color="#FFFFFF"/>
            <stop offset="0.5" stop-color="#FFE2B0"/>
            <stop offset="1" stop-color="#FFB15C"/>
          </radialGradient>
        </defs>

        <circle class="halo" cx="130" cy="128" r="125" fill="url(#halo)"/>
        <polygon class="cone" points="76,176 184,176 252,310 8,310" fill="url(#cone)"/>

        <!-- table lamp -->
        <ellipse cx="130" cy="296" rx="52" ry="9" fill="#10172A"/>
        <rect x="124" y="188" width="12" height="104" rx="6" fill="#2A3350"/>
        <ellipse cx="130" cy="176" rx="22" ry="12" fill="#1B2338"/>
        <ellipse class="bulb-lit" cx="130" cy="172" rx="16" ry="9" fill="url(#bulbLit)"/>

        <polygon points="82,70 178,70 204,172 56,172" fill="url(#shadeDark)" stroke="#34405F" stroke-width="1.5" stroke-linejoin="round"/>
        <polygon class="shade-lit" points="82,70 178,70 204,172 56,172" fill="url(#shadeLit)" stroke="#FFD9A3" stroke-width="1.5" stroke-linejoin="round"/>
        <ellipse cx="130" cy="70" rx="48" ry="7" fill="#0C1120" opacity="0.55"/>
      </svg>

      <div class="lamp-text">
        <h2 id="heroTitle">Daylight is enough.</h2>
        <p id="heroDescription">The light is off and keeps watching the room. It switches on by itself when the room gets darker than your threshold.</p>

        <div class="chips">
          <span class="chip" id="modeChip"><i></i><span id="modeText">Day mode</span></span>
          <span class="chip" id="ledChip"><i></i><span id="ledText">LED off</span></span>
        </div>
      </div>
    </div>

    <div class="card gauge-card">
      <div class="card-title">Room brightness</div>
      <div class="card-sub">Live reading from the light sensor</div>

      <div class="ring">
        <svg viewBox="0 0 160 160" aria-hidden="true">
          <defs>
            <linearGradient id="ringGrad" x1="0" y1="0" x2="1" y2="1">
              <stop offset="0" stop-color="#6F8FFF"/>
              <stop offset="0.55" stop-color="#A78BFA"/>
              <stop offset="1" stop-color="#FFB15C"/>
            </linearGradient>
          </defs>
          <circle class="track" cx="80" cy="80" r="72"/>
          <circle class="bar" id="ringBar" cx="80" cy="80" r="72"/>
        </svg>
        <div class="ring-center">
          <div class="ring-value"><span id="ringValue">--</span><small>%</small></div>
          <div class="ring-label" id="ringLabel">Waiting</div>
        </div>
      </div>

      <div class="pair">
        <div>Darkness<b id="darkPair">--%</b></div>
        <div>Turns on above<b id="threshPair">--%</b></div>
      </div>
    </div>

  </section>

  <section class="metrics">
    <div class="card metric m-bright">
      <div class="metric-name">Brightness</div>
      <div class="metric-value" id="brightness">--%</div>
      <div class="metric-sub">Ambient light level</div>
    </div>

    <div class="card metric m-dark">
      <div class="metric-name">Darkness</div>
      <div class="metric-value" id="darkness">--%</div>
      <div class="metric-sub">How dark the room is</div>
    </div>

    <div class="card metric m-thresh">
      <div class="metric-name">Threshold</div>
      <div class="metric-value" id="threshold">--%</div>
      <div class="metric-sub">Set with the potentiometer</div>
    </div>

    <div class="card metric m-led">
      <div class="metric-name">LED output</div>
      <div class="metric-value" id="ledPower">--%</div>
      <div class="metric-sub" id="ledSub">Current power</div>
    </div>
  </section>

  <section class="lower">

    <div class="card panel">
      <div class="chart-head">
        <div>
          <div class="card-title">Light history</div>
          <div class="card-sub" style="margin-bottom:0">Last 2 minutes</div>
        </div>
        <div class="legend">
          <span><i></i>Brightness</span>
          <span><i class="dash"></i>Light turns on below</span>
          <span><i class="box"></i>Light on</span>
        </div>
      </div>
      <div class="chart-wrap"><canvas id="chart"></canvas></div>
    </div>

    <div class="card panel">
      <div class="card-title">Night activation</div>
      <div class="card-sub">Your potentiometer sets this point</div>

      <p class="explain">Night mode starts when darkness goes above your threshold.</p>

      <div class="tt">
        <div class="tt-fill" id="ttFill"></div>
        <div class="tt-flag" id="ttFlag">--%</div>
        <div class="tt-marker" id="ttMarker"></div>
      </div>
      <div class="tt-scale"><span>Bright room</span><span>Dark room</span></div>

      <div class="verdict" id="verdict">Waiting for the first reading.</div>
    </div>

  </section>

  <div class="footer">Readings come straight from your ESP32 and refresh about twice a second.</div>

</main>

<script>
// ------------------------------------------------------------
// Element references
// ------------------------------------------------------------
const $ = (id) => document.getElementById(id);

const appEl = $("app");
const connEl = $("connection");
const connState = $("connState");
const connTime = $("connTime");
const heroTitle = $("heroTitle");
const heroDesc = $("heroDescription");
const modeText = $("modeText");
const ledChip = $("ledChip");
const ledText = $("ledText");
const ringBar = $("ringBar");
const ringValue = $("ringValue");
const ringLabel = $("ringLabel");
const darkPair = $("darkPair");
const threshPair = $("threshPair");
const brightnessEl = $("brightness");
const darknessEl = $("darkness");
const thresholdEl = $("threshold");
const ledPowerEl = $("ledPower");
const ledSub = $("ledSub");
const ttFill = $("ttFill");
const ttMarker = $("ttMarker");
const ttFlag = $("ttFlag");
const verdict = $("verdict");
const chart = $("chart");

const RING_LEN = 452.4;     // 2 * PI * 72
const HISTORY_MAX = 240;    // 240 readings x 500 ms = 2 minutes
const POLL_MS = 500;

const history = [];
let lastOk = 0;
let fails = 0;
let hasData = false;

// ------------------------------------------------------------
// Helpers
// ------------------------------------------------------------
function num(v) {
  const n = Number(v);
  return isFinite(n) ? n : 0;
}

function clamp(v, lo, hi) {
  return Math.min(hi, Math.max(lo, v));
}

function normalize(d) {
  return {
    brightness: clamp(num(d.brightness), 0, 100),
    darkness: clamp(num(d.darkness), 0, 100),
    threshold: clamp(num(d.threshold), 0, 100),
    led: clamp(num(d.ledBrightness), 0, 255),
    night: d.nightMode === true || d.nightMode === "true" || d.nightMode === 1
  };
}

function brightnessWord(b) {
  if (b < 15) return "Very dark";
  if (b < 35) return "Dim";
  if (b < 65) return "Balanced";
  if (b < 85) return "Bright";
  return "Very bright";
}

// ------------------------------------------------------------
// Update the dashboard
// ------------------------------------------------------------
function update(d) {
  const ledPct = Math.round((d.led / 255) * 100);
  const ledOn = d.led > 0;

  // Metrics
  brightnessEl.textContent = d.brightness.toFixed(1) + "%";
  darknessEl.textContent = d.darkness.toFixed(1) + "%";
  thresholdEl.textContent = d.threshold.toFixed(1) + "%";
  ledPowerEl.textContent = ledPct + "%";
  ledSub.textContent = ledOn ? "Light is shining" : "Light is off";

  // Ring gauge
  ringBar.style.strokeDashoffset = RING_LEN * (1 - d.brightness / 100);
  ringValue.textContent = Math.round(d.brightness);
  ringLabel.textContent = brightnessWord(d.brightness);
  darkPair.textContent = d.darkness.toFixed(1) + "%";
  threshPair.textContent = d.threshold.toFixed(1) + "%";

  // Threshold track
  ttFill.style.width = d.darkness + "%";
  ttMarker.style.left = d.threshold + "%";
  ttFlag.style.left = clamp(d.threshold, 8, 92) + "%";
  ttFlag.textContent = "Turns on at " + Math.round(d.threshold) + "%";

  if (d.night) {
    verdict.textContent = "Darkness (" + Math.round(d.darkness) + "%) is above your threshold (" + Math.round(d.threshold) + "%), so the light is on.";
  } else {
    verdict.textContent = "Darkness (" + Math.round(d.darkness) + "%) is below your threshold (" + Math.round(d.threshold) + "%), so the light stays off.";
  }

  // Night / day presentation
  document.body.classList.toggle("night", d.night);

  if (d.night) {
    heroTitle.textContent = "The night light is on.";
    heroDesc.textContent = "The room got darker than your threshold, so the light switched on by itself.";
    modeText.textContent = "Night mode";
  } else {
    heroTitle.textContent = "Daylight is enough.";
    heroDesc.textContent = "The light is off and keeps watching the room. It switches on by itself when the room gets darker than your threshold.";
    modeText.textContent = "Day mode";
  }

  // Lamp glow follows the real LED output, not just the mode
  document.documentElement.style.setProperty("--glow", (d.led / 255).toFixed(3));
  ledChip.classList.toggle("on", ledOn);
  ledText.textContent = ledOn ? "LED on at " + ledPct + "%" : "LED off";

  // History
  history.push({ b: d.brightness, t: d.threshold, n: d.night });
  if (history.length > HISTORY_MAX) history.shift();
  drawChart();
}

// ------------------------------------------------------------
// History chart (canvas, no libraries)
// ------------------------------------------------------------
function drawChart() {
  const dpr = window.devicePixelRatio || 1;
  const w = chart.clientWidth;
  const h = chart.clientHeight;
  if (!w || !h) return;

  if (chart.width !== Math.round(w * dpr) || chart.height !== Math.round(h * dpr)) {
    chart.width = Math.round(w * dpr);
    chart.height = Math.round(h * dpr);
  }

  const ctx = chart.getContext("2d");
  ctx.setTransform(dpr, 0, 0, dpr, 0, 0);
  ctx.clearRect(0, 0, w, h);

  const padL = 34, padR = 6, padT = 8, padB = 22;
  const pw = w - padL - padR;
  const ph = h - padT - padB;
  const step = pw / (HISTORY_MAX - 1);

  const X = (i) => padL + pw - (history.length - 1 - i) * step;
  const Y = (v) => padT + ph - (v / 100) * ph;

  // Grid and labels
  ctx.font = "11px system-ui, sans-serif";
  ctx.textBaseline = "middle";
  ctx.textAlign = "right";
  ctx.lineWidth = 1;
  for (let v = 0; v <= 100; v += 25) {
    const y = Math.round(Y(v)) + 0.5;
    ctx.strokeStyle = "rgba(255,255,255,0.06)";
    ctx.beginPath();
    ctx.moveTo(padL, y);
    ctx.lineTo(w - padR, y);
    ctx.stroke();
    ctx.fillStyle = "#8A94A8";
    ctx.fillText(v + "%", padL - 8, y);
  }

  ctx.textAlign = "left";
  ctx.textBaseline = "alphabetic";
  ctx.fillStyle = "#8A94A8";
  ctx.fillText("2 min ago", padL, h - 5);
  ctx.textAlign = "right";
  ctx.fillText("Now", w - padR, h - 5);

  if (history.length < 2) return;

  // Shade the stretches where the light was on
  ctx.fillStyle = "rgba(255,177,92,0.10)";
  let start = -1;
  for (let i = 0; i <= history.length; i++) {
    const on = i < history.length && history[i].n;
    if (on && start < 0) start = i;
    if (!on && start >= 0) {
      const x1 = X(start);
      const x2 = X(i - 1);
      ctx.fillRect(x1, padT, Math.max(x2 - x1, 2), ph);
      start = -1;
    }
  }

  // Area under the brightness line
  const grad = ctx.createLinearGradient(0, padT, 0, padT + ph);
  grad.addColorStop(0, "rgba(111,143,255,0.34)");
  grad.addColorStop(1, "rgba(111,143,255,0)");
  ctx.beginPath();
  ctx.moveTo(X(0), padT + ph);
  for (let i = 0; i < history.length; i++) ctx.lineTo(X(i), Y(history[i].b));
  ctx.lineTo(X(history.length - 1), padT + ph);
  ctx.closePath();
  ctx.fillStyle = grad;
  ctx.fill();

  // Activation level: the light turns on when brightness falls below (100 - threshold)
  ctx.beginPath();
  ctx.setLineDash([5, 5]);
  ctx.strokeStyle = "#FFB15C";
  ctx.lineWidth = 1.5;
  for (let i = 0; i < history.length; i++) {
    const y = Y(100 - history[i].t);
    if (i === 0) ctx.moveTo(X(i), y); else ctx.lineTo(X(i), y);
  }
  ctx.stroke();
  ctx.setLineDash([]);

  // Brightness line
  ctx.beginPath();
  ctx.strokeStyle = "#6F8FFF";
  ctx.lineWidth = 2.2;
  ctx.lineJoin = "round";
  for (let i = 0; i < history.length; i++) {
    const y = Y(history[i].b);
    if (i === 0) ctx.moveTo(X(i), y); else ctx.lineTo(X(i), y);
  }
  ctx.stroke();

  // Latest point
  const last = history[history.length - 1];
  ctx.beginPath();
  ctx.arc(X(history.length - 1), Y(last.b), 4, 0, Math.PI * 2);
  ctx.fillStyle = "#F3F5FA";
  ctx.shadowColor = "#6F8FFF";
  ctx.shadowBlur = 10;
  ctx.fill();
  ctx.shadowBlur = 0;
}

window.addEventListener("resize", drawChart);

// ------------------------------------------------------------
// Connection status
// ------------------------------------------------------------
function setConnection() {
  let cls = "ok";
  let label = "Connected";
  let detail = "";

  if (!hasData && fails === 0) {
    cls = "warn";
    label = "Connecting";
    detail = "Waiting for first reading";
  } else if (fails === 0) {
    const age = (Date.now() - lastOk) / 1000;
    detail = age < 1.5 ? "Updated just now" : "Updated " + Math.round(age) + "s ago";
  } else if (fails <= 3) {
    cls = "warn";
    label = "Reconnecting";
    detail = hasData ? "Last reading " + Math.round((Date.now() - lastOk) / 1000) + "s ago" : "Trying again";
  } else {
    cls = "bad";
    label = "Disconnected";
    detail = hasData ? "Last reading " + Math.round((Date.now() - lastOk) / 1000) + "s ago" : "Check the ESP32 power and Wi-Fi";
  }

  connEl.className = "connection " + cls;
  connState.textContent = label;
  connTime.textContent = detail;
  appEl.classList.toggle("stale", fails > 3);
}

setInterval(setConnection, 1000);

// ------------------------------------------------------------
// Polling (waits for each request to finish before the next)
// ------------------------------------------------------------
async function poll() {
  const ctl = new AbortController();
  const timer = setTimeout(() => ctl.abort(), 2500);

  try {
    const res = await fetch("/api", { cache: "no-store", signal: ctl.signal });
    if (!res.ok) throw new Error("API error");
    const data = await res.json();
    clearTimeout(timer);

    fails = 0;
    lastOk = Date.now();
    hasData = true;
    update(normalize(data));
  } catch (err) {
    clearTimeout(timer);
    fails++;
  }

  setConnection();
  setTimeout(poll, fails ? Math.min(POLL_MS * (fails + 1), 3000) : POLL_MS);
}

poll();
</script>

</body>
</html>
)rawliteral";
