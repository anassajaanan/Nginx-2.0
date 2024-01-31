#!/usr/bin/env python3
import cgi

# Print the header with Content-type
print("Content-type: text/html\n")

# Get form data
form = cgi.FieldStorage()
name = form.getvalue('username')

# Process the form data
if name:
    print(f"Content-type: text/html\n")
    print(f"<html><head><title>CGI Form Result</title></head><body>")
    print(f"<h1>Hello, {name}!</h1>")
    print("</body></html>")
else:
    print("Content-type: text/html\n")
    print("<html><head><title>CGI Form Error</title></head><body>")
    print("<h1>Please enter your name.</h1>")
    print("</body></html>")
