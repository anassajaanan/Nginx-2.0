# #!/usr/bin/env python3

# import os
# import cgi
# from openai import OpenAI
# # cgitb.enable()
# client = OpenAI()
# # Set your OpenAI API key here
# OpenAI.api_key = ''
# OpenAI.api_key = os.getenv('OPENAI_API_KEY')
# # print(os.getenv('OPENAI_API_KEY'))
# client = OpenAI()
# def chat_with_gpt(prompt, model="text-davinci-002"):
#     try:
#         response = client.completions.create(
#         model="gpt-3.5-turbo-instruct",
#         prompt=prompt,
#         max_tokens=100,
#         temperature=0
#       )
#         return response.choices[0].text.strip()
#     except Exception as e:
#         return str(e)
# # Example usage
# form = cgi.FieldStorage()
# message = form.getvalue('message')
# user_prompt = message
# response = chat_with_gpt(user_prompt)

# print("Content-Type: application/json")
 # Print a blank line to separate headers from the body

# Print the JSON response
# print(json.dumps(response_data))
print("HI how are you")

# print(f"GPT-3.5 says: {response}")
