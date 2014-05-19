var gameState = null; /*{
    "type" : "STATE",
    "state_id" : 1,
    "field_radius" : 100,
    "player_radius" : 15,
    "coin_radius" : 10,
    "time_delta" : 0.01,
    "velocity_max" : 10,
    "players" : [
        { "id" : 0, "x" : 10, "y" : 10, "v_x" : 0, "v_y" : 0, "score" : 100 }
    ],
    "coins" : [
        { "x" : 30, "y" : -50, "value" : 1 }
    ]
};*/

function drawGameState() {
    if (!gameState) { return; }
    var canvas = document.getElementById('gamefield');
    var ctx = canvas.getContext('2d');
    var cnvWidth = canvas.width, cnvHeight = canvas.height;
    ctx.clearRect(0, 0, cnvWidth, cnvHeight);
    var centerX = canvas.width / 2, centerY = canvas.height / 2, cx = (Math.min(canvas.width, canvas.height) / 2) / gameState.field_radius;
    ctx.fillStyle = "rgb(200,255,200)";
    ctx.beginPath();
    ctx.arc(centerX, centerY, cx * gameState.field_radius, 0, 2 * Math.PI, false);
    ctx.fill();
    for (var i = 0; i < gameState.coins.length; ++i) {
        var coin = gameState.coins[i];
        ctx.fillStyle = "rgb(250,230,100)";
        ctx.beginPath();
        ctx.arc(centerX + coin.x * cx, centerY + coin.y * cx, gameState.coin_radius * cx, 0, 2 * Math.PI, false);
        ctx.fill();
        
    }
    for (var i = 0; i < gameState.players.length; ++i) {
        var player = gameState.players[i];
        ctx.fillStyle = "rgb(250,50,100)";
        ctx.beginPath();
        ctx.arc(centerX + player.x * cx, centerY + player.y * cx, gameState.player_radius * cx, 0, 2 * Math.PI, false);
        ctx.fill();
        
    }
}

function updateCanvasSize() {
    var canvas = document.getElementById('gamefield');
    canvas.width = window.innerWidth - 25;
    canvas.height = window.innerHeight - 100;
    drawGameState();
}
$(function () {
    updateCanvasSize();
});

$(window).on('resize', updateCanvasSize);

var intervalId = null;

function startUpdating() {
    if (intervalId === null) {
        intervalId = setInterval(function () {
            $.ajax('/game_state', { cache: false, dataType: 'json', success: function(data) { gameState = data; drawGameState(); } });
        }, 100);
    }
}

function stopUpdating() {
    if (intervalId !== null) {
        clearInterval(intervalId);
        intervalId = null;
    }
}
