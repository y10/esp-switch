Wss = (function () {

    var onSuccess = [];

    var onError = [];

    var socket = null;

    var source = null;

    var enabled = false;

    try {

        socket = new WebSocket('ws://' + window.location.hostname + ':80/ws');
        socket.onopen = function (evt) { console.log('WS: open'); };
        socket.onclose = function (evt) { console.log('WS: close'); };
        socket.onerror = function (evt) {
            console.log("WS: error");
            console.log(evt);
            if(enabled)
            {
                onError.forEach(function (callback) {
                    callback(evt);
                }, this);
            }
        };
        socket.onmessage = function (evt) {
            console.log(evt);
            if(enabled)
            {
                var state = JSON.parse(evt.data);
                onSuccess.forEach(function (callback) {
                    callback(state);
                }, this);
            }
        };

        enabled = true;
    }
    catch (error) 
    {
        console.log(error);
    }

    try {
        source = new EventSource('/events');
        source.addEventListener('open', function (evt) {
            console.log("WS: Events Connected");
        }, false);
        source.addEventListener('log', function (evt) 
        {
            console.log(evt.data);

        }, false);
        source.addEventListener('error', function (evt) {
            if (e.target.readyState != EventSource.OPEN) {
                console.log("WS: Events Disconnected");
            }
        }, false);

    } 
    catch (error) 
    {
        console.log(error);
    }

    return {

        on: function (onSuccessCallback, onErrorCallback) {

            if(socket)
            {
                enabled = true;

                if (onSuccessCallback) {
                    onSuccess.push(onSuccessCallback);
                }

                if (onErrorCallback) {
                    onError.push(onErrorCallback);
                }
            }
        },

        off: function()
        {
            enabled = false;
        }
    }
})();
