#!/usr/bin/env python3
"""
Helper script to extract tar archives using Python instead of system tar.
This works around issues with posix_spawn in certain environments.
"""
import sys
import tarfile
import os
import shutil
import warnings

def extract_archive(archive_path, extract_dir, strip_components=1):
    """Extract tar archive with strip-components functionality."""
    
    # Suppress the deprecation warning
    warnings.filterwarnings("ignore", category=DeprecationWarning)
    
    # Create extract directory if it doesn't exist
    os.makedirs(extract_dir, exist_ok=True)
    
    # Open and extract the archive
    with tarfile.open(archive_path, 'r:*') as tar:
        if strip_components == 0:
            tar.extractall(extract_dir)
        else:
            # Implement strip-components
            members = tar.getmembers()
            for member in members:
                # Split the path and remove the first N components
                parts = member.name.split('/')
                if len(parts) > strip_components:
                    member.name = '/'.join(parts[strip_components:])
                    if member.name:  # Only extract if there's something left
                        tar.extract(member, extract_dir)
    
    return 0

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: extract_helper.py <archive> <extract_dir> [strip_components]")
        sys.exit(1)
    
    archive = sys.argv[1]
    extract_dir = sys.argv[2]
    strip = int(sys.argv[3]) if len(sys.argv) > 3 else 1
    
    try:
        exit_code = extract_archive(archive, extract_dir, strip)
        sys.exit(exit_code)
    except Exception as e:
        print(f"Error extracting archive: {e}", file=sys.stderr)
        sys.exit(1)

