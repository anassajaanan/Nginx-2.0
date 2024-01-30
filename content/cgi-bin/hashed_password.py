#! /usr/bin/python3

import random
import string

def generate_password(length=12):
    characters = string.ascii_letters + string.digits + string.punctuation
    password = ''.join(random.choice(characters) for _ in range(length))
    return password

if __name__ == "__main__":
    password_length = 16  # You can change the length if desired
    generated_password = generate_password(password_length)
    # print("Generated Password:", generated_password)
    response = ""
    response += "<h2>Generated Password:</h2>"
    response += "<p><strong>{generated_password}</strong></p>"
    print(response)