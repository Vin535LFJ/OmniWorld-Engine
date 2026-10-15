#!/usr/bin/env node
// mermaid_lint.mjs — pattern-level checks for ```mermaid blocks, guarding the
// failure modes this repo has actually hit (problem-log 001: participant
// names colliding with sequence-diagram keywords; ASCII ()[]{} in message
// text; unterminated blocks). NOT a full parser — the boundary is stated
// honestly: it catches what we have broken, not everything mermaid can break.
// Usage: node tools/mermaid_lint.mjs <file.md> [more.md ...]
import { readFileSync } from 'node:fs';

const RESERVED = new Set([
  'loop', 'alt', 'opt', 'par', 'rect', 'break', 'critical', 'note', 'end',
  'else', 'and', 'box', 'autonumber', 'activate', 'deactivate',
]);

let failures = 0;

for (const file of process.argv.slice(2)) {
  const lines = readFileSync(file, 'utf8').split('\n');
  let inBlock = false;
  let blockStart = 0;
  let kind = '';
  for (let i = 0; i < lines.length; ++i) {
    const line = lines[i];
    const opens = /^\s*```mermaid\s*$/i.test(line);
    const closes = /^\s*```/.test(line);
    if (!inBlock && opens) {
      inBlock = true;
      kind = '';
      blockStart = i + 1;
      continue;
    }
    if (inBlock && closes && !opens) {
      inBlock = false;
      continue;
    }
    if (!inBlock) continue;
    if (kind === '' && line.trim() !== '') {
      const m = line.trim().match(/^(sequenceDiagram|flowchart|classDiagram|stateDiagram)/i);
      kind = m ? m[1].toLowerCase() : 'unknown';
    }
    if (kind !== 'sequencediagram') continue;
    const p = line.match(/^\s*(?:participant|actor)\s+([A-Za-z_][\w]*)/);
    if (p && RESERVED.has(p[1].toLowerCase())) {
      console.error(`${file}:${i + 1}: participant name '${p[1]}' collides with a mermaid keyword (problem-log 001)`);
      failures++;
    }
    const msg = line.match(/^\s*\S+\s*(?:->>|-->>|->|-->|-x|--x|<<-|<<--)\s*\S+\s*:\s*(.*)$/);
    if (msg && /[()[\]{}]/.test(msg[1])) {
      console.error(`${file}:${i + 1}: ASCII ()[]{} in sequence message text - use full-width equivalents (problem-log 001)`);
      failures++;
    }
  }
  if (inBlock) {
    console.error(`${file}: unterminated mermaid block starting near line ${blockStart}`);
    failures++;
  }
}

if (failures > 0) {
  console.error(`mermaid-lint: ${failures} issue(s) found`);
  process.exit(1);
}
console.log('mermaid-lint: OK');
