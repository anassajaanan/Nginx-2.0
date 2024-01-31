let buttonColours = ["red", "blue", "green", "yellow"];
let gamePattern = [];
let userClickedPattern = [];
let started = false;
let level = 0;


$(document).keypress(function () {
    if (!started) {
        $('h1').text("Level " + level);
        nextSequence();
        started = true;
    }
});


$(".btn").click(function () {
    let clickedButtonColor = this.getAttribute("id");
    userClickedPattern.push(clickedButtonColor);
    playSound(clickedButtonColor);
    animatePress(clickedButtonColor);

    if (userClickedPattern.length === level) {
        if (JSON.stringify(gamePattern) === JSON.stringify(userClickedPattern)) {
            userClickedPattern = []
            setTimeout(function () {
                nextSequence();
            }, 1000);

        } else {
            wrongAnswer();
        }
    } else {
        for (let i = 0; i < userClickedPattern.length; i++) {
            if (userClickedPattern[i] !== gamePattern[i]) {
                wrongAnswer();
            }
        }
    }
})


function nextSequence() {
    level++;
    $('h1').text("Level " + level);
    let randomNumber = Math.floor(Math.random() * 4);
    let randomChosenColour = buttonColours[randomNumber];
    playSound(randomChosenColour);
    gamePattern.push(randomChosenColour);
    let activeButton = $("#" + randomChosenColour);
    activeButton.fadeOut(200).fadeIn(200);
}

function playSound(clickedButtonColor) {
    let audio = new Audio('sounds/' + clickedButtonColor + '.mp3');
    audio.play();
}

function animatePress(currentColour) {
    let activeButton = $('#' + currentColour);
    activeButton.addClass('pressed');
    setTimeout(function () {
        activeButton.removeClass('pressed');
    }, 100);
}

function wrongAnswer() {
    $('h1').text("Game Over, Press Any Key to Restart");
    $('body').addClass('game-over');
    let audio = new Audio('sounds/wrong.mp3');
    audio.play();
    setTimeout(function () {
        $('body').removeClass('game-over');
    }, 200);
    startOver();

}

function startOver() {
    gamePattern = [];
    userClickedPattern = [];
    started = false;
    level = 0;
}
