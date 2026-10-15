#!/usr/bin/env bash
# state-check.sh — answer "what state is this repo in?" in one command.
# Authoritative procedure + expected output: workflows/00-state-check.md
# Convenience layer only: the spec's commands are the source of truth.
#
# Exit 0 = OK, non-zero = needs attention. Usable as a gate.
# Usage: bash scripts/state-check.sh
#
# Perf note: Git Bash forks at ~0.5s, so the link check does ONE grep and
# keeps everything else in memory. Do not reintroduce per-link subprocesses.

set -uo pipefail
cd "$(dirname "$0")/.." || exit 1

FAIL=0
hr() { printf '%s\n' "------------------------------------------------------------"; }

# Resolve "." / ".." segments without spawning anything. Sets NORM.
NORM=""
normalize_into() {
  local out=() part
  local IFS=/
  for part in $1; do
    case "$part" in
      '' | .) continue ;;
      ..) [ ${#out[@]} -gt 0 ] && unset 'out[${#out[@]}-1]' ;;
      *) out+=("$part") ;;
    esac
  done
  local IFS=/
  NORM="${out[*]}"
}

echo "== branch =="
git status -sb 2>/dev/null | head -1
AHEAD=$(git rev-list --count origin/master..HEAD 2>/dev/null || echo '?')
echo "unpushed commits: ${AHEAD}"

hr
echo "== working tree =="
WT=$(git status --porcelain)
if [ -z "$WT" ]; then
  echo "clean"
else
  printf '%s\n' "$WT"
  if printf '%s\n' "$WT" | grep -q '^??'; then
    echo "!! untracked above = possibly uncommitted deliverables"
    FAIL=1
  fi
  if printf '%s\n' "$WT" | grep -qE '^ ?M'; then
    echo "!! uncommitted modifications above"
    FAIL=1
  fi
fi

hr
echo "== tests =="
if [ -f build/CTestTestfile.cmake ]; then
  TEST_OUT=$(ctest --test-dir build 2>&1)
  printf '%s\n' "$TEST_OUT" | tail -3
  printf '%s\n' "$TEST_OUT" | grep -q "100% tests passed" || FAIL=1
else
  echo "!! build/ not configured — run build.bat --test"
  FAIL=1
fi

hr
echo "== guardrail =="
# .git/hooks/ is NOT version-controlled, so a fresh clone gets the tracked
# source (tools/hooks/pre-commit) but no installed hook -- the gate would
# silently not run. Catch that here.
if [ -f tools/hooks/pre-commit ]; then
  if [ ! -f .git/hooks/pre-commit ]; then
    echo "!! pre-commit hook NOT installed — the gate is not running."
    echo "   install: cp tools/hooks/pre-commit .git/hooks/pre-commit && chmod +x .git/hooks/pre-commit"
    FAIL=1
  elif ! cmp -s tools/hooks/pre-commit .git/hooks/pre-commit; then
    echo "!! installed hook differs from tools/hooks/pre-commit — reinstall it"
    FAIL=1
  else
    echo "pre-commit hook installed and matches tools/hooks/pre-commit"
  fi
else
  echo "(no tools/hooks/pre-commit in this repo)"
fi

hr
echo "== doc links (steering + design docs) =="
TRACKED_FILE=$(mktemp)
git -c core.quotePath=false ls-files > "$TRACKED_FILE"

declare -A IS_TRACKED=()
while IFS= read -r line; do IS_TRACKED["$line"]=1; done < "$TRACKED_FILE"
DIRS_TRACKED=""
while IFS= read -r line; do DIRS_TRACKED+="${line%/*}"$'\n'; done < "$TRACKED_FILE"

CHECKED=$(
  {
    ls ./*.md 2>/dev/null
    ls docs/agents/*.md docs/adr/*.md workflows/*.md 2>/dev/null
    find docs/design -name '*.md' 2>/dev/null
  } | sort -u
)

BROKEN=0
while IFS= read -r row; do
  f="${row%%:*}"
  m="${row#*:}"
  p="${m#](}"
  p="${p%)}"
  p="${p%%#*}"
  [ -z "$p" ] && continue
  case "$p" in http* | mailto:* | /*) continue ;; esac
  # pure-bash dirname: no subshell (Git Bash forks at ~0.3s)
  fdir="${f%/*}"
  [ "$fdir" = "$f" ] && fdir="."
  normalize_into "$fdir/$p"
  t="${NORM#./}"
  if [ ! -e "$t" ]; then
    echo "  MISSING: $f -> $p"
    BROKEN=$((BROKEN + 1))
  elif [ -d "$t" ]; then
    case $'\n'"$DIRS_TRACKED" in
      *$'\n'"$t"$'\n'*) ;;
      *)
        echo "  UNTRACKED DIR: $f -> $p"
        BROKEN=$((BROKEN + 1))
        ;;
    esac
  elif [ -z "${IS_TRACKED[$t]:-}" ]; then
    echo "  UNTRACKED TARGET: $f -> $p (exists locally, not committed)"
    BROKEN=$((BROKEN + 1))
  fi
done < <(grep -oH '](\([^)]*\))' $CHECKED 2>/dev/null)

if [ "$BROKEN" -eq 0 ]; then
  echo "all links resolve and are committed"
else
  echo "!! ${BROKEN} problem(s) above"
  FAIL=1
fi

hr
if [ "$FAIL" -eq 0 ]; then
  echo "STATE: OK"
else
  echo "STATE: NEEDS ATTENTION"
fi
exit "$FAIL"
