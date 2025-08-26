#!/bin/bash

echo "=== Cascoin GUI Debug Script ==="
echo "Checking current GUI status..."

# Kill any existing instances
echo "Stopping existing instances..."
killall -9 cascoin-qt 2>/dev/null
sleep 3

echo "Starting GUI with debug output..."
echo "Looking for splash screen messages..."

# Start GUI and capture output
src/qt/cascoin-qt 2>&1 | while read line; do
    echo "[GUI OUTPUT] $line"
    
    # Look for specific patterns
    if echo "$line" | grep -q "Loading\|Verifying\|Done\|Progress"; then
        echo "*** SPLASH MESSAGE DETECTED: $line ***"
    fi
done &

GUI_PID=$!
echo "GUI started with PID: $GUI_PID"

# Monitor for 30 seconds
echo "Monitoring for 30 seconds..."
for i in {1..30}; do
    echo "Time: ${i}s - Checking process..."
    if ps -p $GUI_PID > /dev/null; then
        ps aux | grep cascoin-qt | grep -v grep | head -1
    else
        echo "GUI process ended!"
        break
    fi
    sleep 1
done

echo "Debug completed."
