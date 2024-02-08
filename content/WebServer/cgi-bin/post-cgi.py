#!/usr/bin/env python
import sys

print("Content-Type: text/plain")
print()

# Read POST data from standard input
data = sys.stdin.read()
print("Received POST data:")
print(data)