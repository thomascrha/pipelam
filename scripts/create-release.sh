#!/bin/bash

set -eu

if [ $# -ne 1 ]; then
    echo "Usage: $0 <version>"
    echo "Example: $0 1.0.0"
    exit 1
fi

if ! git diff --quiet || ! git diff --staged --quiet || [ -n "$(git ls-files --others --exclude-standard)" ]; then
    echo "Error: You have uncommitted changes or untracked files in your working directory."
    echo "Please commit, stash, or remove all changes before creating a release."
    git status --short
    exit 1
fi

VERSION="$1"
CONFIG_FILE="src/config.h"

if ! [[ $VERSION =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    echo "Error: Version must be in format X.Y.Z (e.g., 1.0.0)"
    exit 1
fi

echo "Updating version in $CONFIG_FILE to v$VERSION..."

if [ ! -f "$CONFIG_FILE" ]; then
    echo "Error: $CONFIG_FILE not found"
    exit 1
fi

# Replace the version string in config.h
sed -i "s/^#define PIPELAM_CURRENT_VERSION \"v[0-9]\+\.[0-9]\+\.[0-9]\+\"/#define PIPELAM_CURRENT_VERSION \"v$VERSION\"/" "$CONFIG_FILE"

echo "Committing version change..."
git add "$CONFIG_FILE"
git commit -m "Release: Bump version to v$VERSION"

echo "Creating tag v$VERSION..."
git tag -a "v$VERSION" -m "Release v$VERSION"

echo "Pushing changes and tag to remote repository..."
git push origin main
git push origin "v$VERSION"

echo "Version updated to v$VERSION in $CONFIG_FILE"
echo "Changes committed and tag v$VERSION pushed to GitHub."
echo "GitHub Actions workflow will now build and create a release."
echo "Check the Actions tab on GitHub to monitor progress."

