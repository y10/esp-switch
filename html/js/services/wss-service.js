Wss = (function () {

    var onSuccess = [];

    var onError = [];

    var websock = null;

    var enabled = false;

    try {
        websock = new WebSocket('ws://' + window.location.hostname + ':80/ws');
        websock.onopen = function (evt) { console.log('WS: open'); };
        websock.onclose = function (evt) { console.log('WS: close'); };
        websock.onerror = function (evt) {
            console.log("WS: error");
            console.log(evt);
            if(enabled)
            {
                onError.forEach(function (callback) {
                    callback(evt);
                }, this);
            }
        };
        websock.onmessage = function (evt) {
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

    return {

        on: function (onSuccessCallback, onErrorCallback) {

            if (onSuccessCallback) {
                onSuccess.push(onSuccessCallback);
            }

            if (onErrorCallback) {
                onError.push(onErrorCallback);
            }
        },

        enable: function()
        {
            if(websock)
            {
                enabled = true;

                return true;
            }

            return false;
        },

        disable: function()
        {
            enabled = false;

            return true;
        }
    }
})();
