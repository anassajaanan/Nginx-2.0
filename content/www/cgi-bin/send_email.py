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

response = """
	<link href="https://maxcdn.bootstrapcdn.com/font-awesome/4.7.0/css/font-awesome.min.css" rel="stylesheet" />
<section class="mail-seccess section">
	<div class="container">
		<div class="row">
			<div class="col-lg-6 offset-lg-3 col-12">
				<!-- Error Inner -->
				<div class="success-inner">
					<h1><i class="fa fa-envelope"></i><span>Your Mail Sent Successfully!</span></h1>
				</div>
				<!--/ End Error Inner -->
			</div>
		</div>
	</div>
</section>
"""

print(response)
