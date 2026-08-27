#!/usr/bin/env bash
# Install the Qwythos-27B-v1 GGUF for OCLI's llama.cpp (and ollama) backend.
# Quant override:  QWYTHOS_QUANT=Q6_K ./install_qwythos27b.sh   (Q4_K_M default; also Q5_K_M, Q6_K, Q8_0)
# Note: on a 32GB machine Q4_K_M (~16GB) is the safe choice; Q5_K_M is borderline, Q6_K+ will swap.
set -euo pipefail

REPO="empero-ai/Qwythos-27B-v1-GGUF"
QUANT="${QWYTHOS_QUANT:-Q4_K_M}"
FILE="Qwythos-27B-${QUANT}.gguf"
DEST_DIR="$HOME/models"
DEST="$DEST_DIR/$FILE"
URL="https://huggingface.co/${REPO}/resolve/main/${FILE}?download=true"

mkdir -p "$DEST_DIR"

if [ -f "$DEST" ]; then
  echo "Already present: $DEST ($(du -h "$DEST" | cut -f1))"
else
  echo "Downloading $FILE (~16+ GB) -> $DEST"
  if command -v hf >/dev/null 2>&1; then
    hf download "$REPO" "$FILE" --local-dir "$DEST_DIR"
  elif command -v huggingface-cli >/dev/null 2>&1; then
    huggingface-cli download "$REPO" "$FILE" --local-dir "$DEST_DIR"
  else
    # resumable (-C -) so a dropped multi-GB download continues where it left off
    curl -L --fail --retry 5 --retry-delay 5 -C - -o "$DEST" "$URL"
  fi
  echo "Downloaded: $DEST ($(du -h "$DEST" | cut -f1))"
fi

# verify against the repo's published checksums
echo "Verifying sha256 ..."
EXPECTED="$(curl -sL "https://huggingface.co/${REPO}/resolve/main/SHA256SUMS" | awk -v f="$FILE" '$2==f||$2=="*"f{print $1}')"
if [ -n "$EXPECTED" ]; then
  ACTUAL="$(shasum -a 256 "$DEST" | awk '{print $1}')"
  if [ "$EXPECTED" = "$ACTUAL" ]; then
    echo "sha256 OK."
  else
    echo "sha256 MISMATCH — expected $EXPECTED got $ACTUAL. Delete $DEST and re-run." >&2
    exit 1
  fi
else
  echo "No SHA256SUMS entry for $FILE — skipping verification."
fi

# register with ollama (if installed) so `/backend ollama` model 'qwythos27b' works
if command -v ollama >/dev/null 2>&1; then
  MF="$(mktemp)"
  printf 'FROM %s\nPARAMETER num_ctx 32768\n' "$DEST" > "$MF"
  echo "Registering ollama model 'qwythos27b' ..."
  if ollama create qwythos27b -f "$MF"; then echo "ollama model 'qwythos27b' ready."; else echo "ollama create failed (skipped)."; fi
  rm -f "$MF"
else
  echo "ollama not found — skipping ollama registration (llama-cpp is set up regardless)."
fi

echo
echo "Done."
echo "  llama.cpp:  llama-server -m $DEST -c 32768"
echo "  ollama:     ollama run qwythos27b"
