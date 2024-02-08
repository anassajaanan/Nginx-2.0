#!/usr/bin/env python3

import os
import cgi
from openai import OpenAI
client = OpenAI()
# Set your OpenAI API key here
OpenAI.api_key = ''
OpenAI.api_key = os.getenv('OPENAI_API_KEY')
# print(os.getenv('OPENAI_API_KEY'))
client = OpenAI()
def chat_with_gpt(prompt, model="gpt-3.5-turbo-instruct"):
    try:
        response = client.completions.create(
        model="gpt-3.5-turbo-instruct",
        prompt=prompt,
        max_tokens=100,
        temperature=0
      )
        return response.choices[0].text.strip()
    except Exception as e:
        return str(e)
# Example usage
form = cgi.FieldStorage()
message = form.getvalue('message')
user_prompt = message
response = chat_with_gpt(user_prompt)

print("<!DOCTYPE html>")
print("<html lang=\"en\">")
print("<head>")
print("  <meta charset=\"UTF-8\">")
print("  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">")
print("  <style>")
print("    body {")
print("      font-family: 'Arial', sans-serif;")
print("      display: flex;")
print("      align-items: center;")
print("      justify-content: center;")
print("      height: 100vh;")
print("      margin: 0;")
print("      background-color: #f5f5f5;")
print("    }")
print("   .box {")
print("      background-color: #fff;")
print("      border: 1px solid #ddd;")
print("      padding: 20px;")
print("      border-radius: 8px;")
print("      box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);")
print("      width: 600px;")
print("      height: 200px;")
print("      text-align: center;")
print("    }")
print("    label {")
print("      display: block;")
print("      margin-bottom: 10px;")
print("      color: #333;")
print("    }")
print("    textarea {")
print("      display: none;")
print("    }")
print("")
print("    input[type=\"submit\"] {")
print("      display: none; ")
print("    }")
print("    #response {")
print("      margin-top: 20px;")
print("    }")
print("    .robot {")
print("      background-image: url(Robot_icon.png);")
print("      width: 4rem;")
print("      background-size: contain;")
print("      height: 4rem;")
print("      background-repeat: no-repeat;")
print("      margin: -170px 17px 1px 1px;")
print("      border-radius: 21px 0px 14px 13px;")
print("      box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);")
print("    }")
print("    .robot-icon {")
print("      margin-right: 10px;")
print("      font-size: 20px;")
print("    }")
print("  </style>")
print("</head>")
print("<body>")
print("  <div class=robot></div>")
print("  <div class=box>")
print(f"    <p>{response}</p>")
print("  </div>")
print("</body>")
print("</html>")