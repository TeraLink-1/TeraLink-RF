#!/usr/bin/env bash
set -e

git submodule update --init --recursive

if ! command -v cmake >/dev/null 2>&1; then
  echo "🔍 CMake not found. Installing…"
  if command -v apt-get >/dev/null 2>&1; then
    sudo apt-get update && sudo apt-get install -y cmake
  elif command -v yum >/dev/null 2>&1; then
    sudo yum install -y cmake
  elif command -v brew >/dev/null 2>&1; then
    brew install cmake
  else
    echo "❌ Could not detect a package manager. Please install CMake manually and re-run this script." >&2
    exit 1
  fi
fi

# 1) Create the Python venv
python3 -m venv fprime-venv

# 2) Activate it
source fprime-venv/bin/activate

# 3) Upgrade pip
pip install --upgrade pip setuptools wheel 



# 4) Install Python deps
pip install -r requirements.txt

echo
echo "✅  Environment ready!"
echo "   - CMake version: $(cmake --version | head -n1)"
echo "   - Activate with: source fprime-venv/bin/activate"
echo " -Mustafa"