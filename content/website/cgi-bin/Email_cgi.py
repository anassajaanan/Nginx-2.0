#! /usr/bin/env python3

import sendgrid
import os
import cgi
from sendgrid.helpers.mail import *

sg = sendgrid.SendGridAPIClient(api_key='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')
sender = Email("Nginx2.0@mail.com")
form = cgi.FieldStorage()
recieverEmail = form.getvalue("email")
reciever = To(recieverEmail)
subject = "Feedback Recieved"
content = Content("text/plain", "Thank you for taking the time to share your feedback with us. We greatly appreciate your input and value your contribution to making our website even better.\nBest regards,\nNginx2.0")
mail = Mail(sender, reciever, subject, content)

try :
	response = sg.client.mail.send.post(request_body=mail.get())
	print("<html>")
	print("<head><title>Email Sent</title></head>")
	print("</html>")
except Exception as e:
	print(f"Error: {e}")