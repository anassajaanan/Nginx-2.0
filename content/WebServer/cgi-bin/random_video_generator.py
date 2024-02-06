#!/usr/bin/env python3
import random

def get_random_video():
    videos = [
        # add an iframe here from YouTube
        f'<iframe width="2142" height="1167" src="https://www.youtube.com/embed/YzewsL1kN30" title="ازاي استفيد من مشروع التخرج 🧑‍🎓 ؟" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" allowfullscreen></iframe>',
        f'<iframe width="1280" height="720" src="https://www.youtube.com/embed/7LuwPdp-_4c" title="Recorded CloudFlare Firing And CEO Response" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" allowfullscreen></iframe>',
        f'<iframe src="https://giphy.com/embed/3ohc0QQkTH6YK8g4zS" width="480" height="363" frameBorder="0" class="giphy-embed" allowFullScreen></iframe><p><a href="https://giphy.com/gifs/wwe-wrestling-3ohc0QQkTH6YK8g4zS">via GIPHY</a></p>',
        f'<iframe src="https://giphy.com/embed/e37RbTLYjfc1q" width="480" height="381" frameBorder="0" class="giphy-embed" allowFullScreen></iframe><p><a href="https://giphy.com/gifs/mrw-pull-infront-e37RbTLYjfc1q">via GIPHY</a></p>',
        f'<iframe src="https://giphy.com/embed/JoV6WP96snO0DOnqEz" width="480" height="238" frameBorder="0" class="giphy-embed" allowFullScreen></iframe><p><a href="https://giphy.com/gifs/netflix-alwaysbemymaybe-michellebuteau-netflixfilm-JoV6WP96snO0DOnqEz">via GIPHY</a></p>',
        f'<iframe src="https://giphy.com/embed/14uXQbPS73Y3qU" width="480" height="390" frameBorder="0" class="giphy-embed" allowFullScreen></iframe><p><a href="https://giphy.com/gifs/14uXQbPS73Y3qU">via GIPHY</a></p>',
        f'<iframe src="https://giphy.com/embed/ndeihy7lvAL9C" width="480" height="278" frameBorder="0" class="giphy-embed" allowFullScreen></iframe><p><a href="https://giphy.com/gifs/animal-funny-animals-ndeihy7lvAL9C">via GIPHY</a></p>'
		# Add more video URLs here
    ]
    return random.choice(videos)

def main():
    response = ""

    video_url = get_random_video()


    response += "<html><body>"
    response += "<h2>Random Video Player</h2>"
    # response += f"<video src='{html.escape(video_url)}' controls>"
    # response += "Your browser does not support the video tag."
    # response += "</video>"
    response += video_url
    response += "</body></html>"
    # print("Content-Length: " + str(len(response)))
    # print("Content-Type: text/html\r\n\r\n")
    print(response)

if __name__ == "__main__":
    main()
