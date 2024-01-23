#!/usr/bin/env python3
import cgi

# Print the header with Content-type
print("Content-type: text/html\n")

# Get form data
form = cgi.FieldStorage()
name = form.getvalue('name')

# Process the form data
for i in range(1000):
    for j in range(1000):
        print(0, end="")
    print("\n", end="")
	