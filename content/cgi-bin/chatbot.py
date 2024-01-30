from chatterbot import ChatBot
from chatterbot.trainers import ChatterBotCorpusTrainer

# Create a new instance of a ChatBot
chatbot = ChatBot("MyBot")

# Create a new trainer for the chatbot
trainer = ChatterBotCorpusTrainer(chatbot)

# Train the chatbot on the English language corpus data
trainer.train("chatterbot.corpus.english")

# Function to get a response from the chatbot
def get_response(user_input):
    response = chatbot.get_response(user_input)
    return str(response)

# Example usage
user_input = input("You: ")
while user_input.lower() != 'exit':
    response = get_response(user_input)
    print(f"Bot: {response}")
    user_input = input("You: ")
