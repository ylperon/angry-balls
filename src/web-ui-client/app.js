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

function updateScoreBoard() {
    var html = '';
    var players = [];
    if (!gameState) {
        html += '<div>No game state</div>';
    } else {
        html += '<div>State id: ' + gameState.state_id + '</div>';
        if (gameState.players) {
            for (var i = 0; i < gameState.players.length; ++i) {
                var player = gameState.players[i];
                players.push(player);
            }
            players.sort(function (p1, p2) {
                return p1.score < p2.score;
            });
            
            for (var i = 0; i < players.length; ++i) {
                var player = players[i];
                html += '<div>' + player.id + ': ' + Math.round(player.score) + '</div>';
            }
        }
    }
    
    document.getElementById('score-board').innerHTML = html;
}

function drawGameState() {
    updateScoreBoard();
    if (!gameState) { return; }
    var canvas = document.getElementById('gamefield');
    var ctx = canvas.getContext('2d');
    var cnvWidth = canvas.width, cnvHeight = canvas.height;
    ctx.clearRect(0, 0, cnvWidth, cnvHeight);
    var centerX = canvas.width / 2, centerY = canvas.height / 2, cx = (Math.min(canvas.width, canvas.height) / 2) / gameState.field_radius;
    ctx.fillStyle = "rgb(200,255,200)";
    ctx.strokeStyle = "rgb(100,100,100)";
    ctx.beginPath();
    ctx.arc(centerX, centerY, cx * gameState.field_radius, 0, 2 * Math.PI, false);
    ctx.fill();
    ctx.beginPath();
    ctx.arc(centerX, centerY, cx * gameState.field_radius, 0, 2 * Math.PI, false);
    ctx.stroke();
    if (gameState.coins) {
    for (var i = 0; i < gameState.coins.length; ++i) {
        var coin = gameState.coins[i];
        ctx.fillStyle = "rgb(250,230,100)";
        ctx.beginPath();
        ctx.arc(centerX + coin.x * cx, centerY + coin.y * cx, gameState.coin_radius * cx, 0, 2 * Math.PI, false);
        ctx.fill();
        ctx.beginPath();
        ctx.arc(centerX + coin.x * cx, centerY + coin.y * cx, gameState.coin_radius * cx, 0, 2 * Math.PI, false);
        ctx.stroke();
        ctx.fillStyle = "rgb(150, 120, 10)";
        ctx.textAlign = "center";
        ctx.textBaseline = "middle";
        ctx.font = "20px sans-serif bold";
        ctx.fillText('$', centerX + coin.x * cx, centerY + coin.y * cx);
    }
    }
    if (gameState.players) {
    for (var i = 0; i < gameState.players.length; ++i) {
        var player = gameState.players[i];
        ctx.fillStyle = "rgb(250,50,100)";
        ctx.beginPath();
        ctx.arc(centerX + player.x * cx, centerY + player.y * cx, gameState.player_radius * cx, 0, 2 * Math.PI, false);
        ctx.fill();
        ctx.beginPath();
        ctx.arc(centerX + player.x * cx, centerY + player.y * cx, gameState.player_radius * cx, 0, 2 * Math.PI, false);
        ctx.stroke();
        ctx.fillStyle = "rgb(0,0,0)";
        ctx.textAlign = "center";
        ctx.textBaseline = "middle";
        ctx.font = "20px sans-serif";
        ctx.fillText(player.id.toString(), centerX + player.x * cx, centerY + player.y * cx);
    }
    }
}

function updateCanvasSize() {
    var controlWidth = document.getElementById('controls-and-score').offsetWidth;
    var canvas = document.getElementById('gamefield');
    canvas.width = window.innerWidth - controlWidth - 10;
    canvas.height = window.innerHeight - 10;
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
