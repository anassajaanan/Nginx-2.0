#! /usr/bin/env pyhton3
from chatterbot import ChatBot
from chatterbot.trainers import ChatterBotCorpusTrainer

chatbot = ChatBot('Ron Obvious')

trainer.train("chatterbot.corpus.english")

chatbot.get_response("Hello, how are you today?")