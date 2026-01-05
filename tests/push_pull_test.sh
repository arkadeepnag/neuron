#!/usr/bin/env bash
set -euo pipefail

ROOT=$(pwd)
TMP=/tmp/neuron_test_remote

rm -rf "$TMP" "$ROOT/.neuron"
mkdir -p "$TMP"

echo "Initializing local repo"
./build/neuron init
echo file > a.txt
./build/neuron add a.txt
./build/neuron commit "test push"

echo "Pushing to local path $TMP"
./build/neuron push "$TMP"

echo "Removing local and pulling back"
rm -rf .neuron
./build/neuron init
./build/neuron pull "$TMP"

echo "Verifying refs were pulled"
if [ -s .neuron/refs/heads/main ]; then
  echo "OK: refs/heads/main present"
else
  echo "ERROR: refs/heads/main missing" >&2
  exit 2
fi

echo "Test completed successfully"
