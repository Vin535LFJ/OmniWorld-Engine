#!/usr/bin/env node
// fetch_models.mjs — idempotent model fetcher for the real-model adapters
// (stage C). Model files are NOT in git: this script downloads them into
// models/ (git-ignored) and pins SHA256 on first run (--record afterwards
// verifies). Licenses are recorded per entry (research note:
// docs/17-research/2026-10-16-onnx-runtime-integration.md).
//
// Usage:
//   node tools/fetch_models.mjs            # download any missing model, pin
//                                          // new SHA256, report sizes
//   node tools/fetch_models.mjs --record    // verify downloaded files against
//                                          // pinned SHA256 (no downloads)
import { createHash } from 'node:crypto';
import { createReadStream, createWriteStream } from 'node:fs';
import { mkdir, stat, readFile, writeFile } from 'node:fs/promises';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';

const ROOT = join(dirname(fileURLToPath(import.meta.url)), '..');
const MODELS_DIR = join(ROOT, 'models');

// sha256: pinned on first successful download; verified on later runs.
// Empty = not yet pinned (script pins and warns).
const MANIFEST = [
  {
    id: 'rtdetrv2-s-onnx',
    // HEAD-verified 2026-10-16: 82,572,357 B (matches research note).
    url: 'https://huggingface.co/onnx-community/rtdetr_r18vd/resolve/main/onnx/model.onnx',
    size: 82572357,
    license: 'Apache-2.0 (weights; community ONNX export — provenance in research note)',
    backup: 'export per official lyuwenyu/RT-DETR tools/export_onnx.py on Kaggle (free GPU)',
    sha256: '',
  },
  {
    id: 'depth-anything-v2-small-onnx',
    // onnx-community/depth-anything-v2-small (public, apache-2.0 tag; fabio-sim
    // variant is gated/401 — verified 2026-10-16). HEAD: 99,060,839 B.
    url: 'https://huggingface.co/onnx-community/depth-anything-v2-small/resolve/main/onnx/model.onnx',
    size: 99060839,
    license: 'Apache-2.0 (only the Small variant is permissively licensed)',
    backup: 'export per official DepthAnything/Depth-Anything-V2 on Kaggle (free GPU)',
    sha256: '',
  },
];

const MANIFEST_FILE = join(ROOT, 'tools', 'fetch_models.manifest.json');

async function loadManifest() {
  try {
    return JSON.parse(await readFile(MANIFEST_FILE, 'utf8'));
  } catch {
    return { models: {} };
  }
}

async function saveManifest(manifest) {
  await writeFile(MANIFEST_FILE, JSON.stringify(manifest, null, 2) + '\n');
}

async function sha256Of(path) {
  const hash = createHash('sha256');
  const stream = createReadStream(path);
  return new Promise((resolve, reject) => {
    stream.on('data', (c) => hash.update(c));
    stream.on('end', () => resolve(hash.digest('hex')));
    stream.on('error', reject);
  });
}

async function download(entry) {
  const dest = join(MODELS_DIR, entry.id + '.onnx');
  await mkdir(MODELS_DIR, { recursive: true });
  process.stdout.write(`fetching ${entry.id} ...\n`);
  const res = await fetch(entry.url);
  if (!res.ok) throw new Error(`${entry.url} -> HTTP ${res.status} (backup: ${entry.backup})`);
  const file = createWriteStream(dest);
  for await (const chunk of res.body) file.write(chunk);
  await new Promise((r) => file.end(r));
  const bytes = (await stat(dest)).size;
  return { dest, bytes };
}

async function main() {
  const recordOnly = process.argv.includes('--record');
  const manifest = await loadManifest();
  let failures = 0;
  for (const entry of MANIFEST) {
    const dest = join(MODELS_DIR, entry.id + '.onnx');
    const pinned = manifest.models[entry.id]?.sha256;
    try {
      if (recordOnly) {
        if (!pinned) throw new Error('no pinned sha256 yet');
        const actual = await sha256Of(dest);
        if (actual !== pinned) throw new Error(`sha256 mismatch: ${actual}`);
        process.stdout.write(`OK ${entry.id} (${pinned.slice(0, 16)}…)\n`);
        continue;
      }
      let bytes = 0;
      let present = true;
      try {
        bytes = (await stat(dest)).size;
      } catch {
        present = false;  // missing file: stat throws, so probe explicitly
      }
      if (!present || bytes === 0) {
        const r = await download(entry);
        bytes = r.bytes;
      } else {
        process.stdout.write(`present ${entry.id} (${bytes} bytes)\n`);
      }
      if (!manifest.models[entry.id]?.sha256) {
        manifest.models[entry.id] = { sha256: await sha256Of(dest), size: bytes };
        await saveManifest(manifest);
        process.stdout.write(
            `pinned ${entry.id} sha256=${manifest.models[entry.id].sha256.slice(0, 16)}…\n`);
      }
    } catch (e) {
      failures += 1;
      process.stderr.write(`FAIL ${entry.id}: ${e.message}\n`);
    }
  }
  process.exit(failures > 0 ? 1 : 0);
}

main();
