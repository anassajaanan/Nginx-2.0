#! /usr/bin/env python3

import requests

# enable cgi debugging
import cgitb
cgitb.enable()

import cgi

# Your API key should be correctly set as a string.
api_key = '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'


# Ensure all values here are simple data types (e.g., strings, lists of dictionaries)
sender_email = "Nginx2.0@mail.com"
receiver_email = cgi.FieldStorage().getvalue("email")
# receiver_email = "anasajaanan.official@gmail.com"
subject = "Feedback Received"
content_value = "Thank you for taking the time to share your feedback with us. We greatly appreciate your input and value your contribution to making our website even better.\nBest regards,\nNginx2.0"

data = {
    "personalizations": [
        {
            "to": [
                {
                    "email": receiver_email
                }
            ]
        }
    ],
    "from": {
        "email": sender_email
    },
    "subject": subject,
    "content": [
        {
            "type": "text/plain",
            "value": content_value
        }
    ]
}

headers = {
    "Authorization": f"Bearer {api_key}",
    "Content-Type": "application/json"
}

response = requests.post("https://api.sendgrid.com/v3/mail/send", json=data, headers=headers, verify=False)

print(response.status_code)
print(response.text)