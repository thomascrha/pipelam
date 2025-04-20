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

# Get the previous tag to generate release notes
PREVIOUS_TAG=$(git describe --tags --abbrev=0 2>/dev/null || echo "")

# Generate release notes from commit messages
RELEASE_NOTES=""
if [ -n "$PREVIOUS_TAG" ]; then
    echo "Generating release notes from commits since $PREVIOUS_TAG..."
    RELEASE_NOTES=$(git log --pretty=format:"* %s" ${PREVIOUS_TAG}..HEAD)
    echo "Draft release notes:"
    echo "$RELEASE_NOTES"
else
    echo "No previous tag found. Including all commit messages in release notes."
    RELEASE_NOTES=$(git log --pretty=format:"* %s")
fi

# Create a temporary file for the commit message
COMMIT_MSG_FILE=$(mktemp)
echo "Release: Bump version to v$VERSION" > "$COMMIT_MSG_FILE"
echo "" >> "$COMMIT_MSG_FILE"
echo "Changes since previous release:" >> "$COMMIT_MSG_FILE"
echo "$RELEASE_NOTES" >> "$COMMIT_MSG_FILE"

# Open the file in the default editor to allow changes
echo "Opening release notes in editor for review and customization..."
${EDITOR:-vi} "$COMMIT_MSG_FILE"

echo "Committing version change..."
git add "$CONFIG_FILE"
git commit -F "$COMMIT_MSG_FILE"

# Use the same release notes for the tag message
cp "$COMMIT_MSG_FILE" "$COMMIT_MSG_FILE.tag"
TAG_MSG_FILE="$COMMIT_MSG_FILE.tag"

echo "Creating tag v$VERSION..."
git tag -a "v$VERSION" -F "$TAG_MSG_FILE"
rm "$COMMIT_MSG_FILE" "$TAG_MSG_FILE"

echo "Pushing changes and tag to remote repository..."
git push origin main
git push origin "v$VERSION"

echo "Version updated to v$VERSION in $CONFIG_FILE"
echo "Changes committed and tag v$VERSION pushed to GitHub."
echo "GitHub Actions workflow will now build and create a release."
echo "Check the Actions tab on GitHub to monitor progress."

