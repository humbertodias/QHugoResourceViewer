#!/bin/bash
echo "Removing quarantine attribute..."
sudo xattr -rd com.apple.quarantine "/Applications/hugo-resource-viewer.app"
echo "Done."
open /Applications/hugo-resource-viewer.app