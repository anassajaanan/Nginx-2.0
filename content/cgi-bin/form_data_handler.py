#!/usr/bin/env python3
import os
import sys
import urllib.parse
import html

def sanitize_input(data):
	""" Basic HTML escape to prevent XSS attacks """
	return html.escape(data)

def parse_form_data(raw_data):
	""" Manually parse form data """
	form_data = urllib.parse.parse_qs(raw_data)

	# Decode and sanitize the form data
	processed_data = {}
	for key, value_list in form_data.items():
		# Take the first value in the list for each key
		processed_data[key] = sanitize_input(value_list[0])
	
	return processed_data

def main():

	response = ""
	# Read and store data from stdin
	raw_data = sys.stdin.read()
	form_data = parse_form_data(raw_data)

	
	response += "<html><body>"
	response += "<h2>Processed Form Data:</h2>"
	for key, value in form_data.items():
		response += f"<p><strong>{key}:</strong> {value}</p>"
	response += "</body></html>"

	print(response)
	

if __name__ == "__main__":
	main()

