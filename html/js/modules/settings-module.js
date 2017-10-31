if (typeof app === "undefined") { app = {} }
if (typeof app.modules === "undefined"){ app.modules = {} }


app.modules.settings = (function (http, wss) {
    
    return {
        load: function (el) {

            var elements = [];

            function bindDeviceName() {

                var element = el.querySelector("#device-name");

                element.redraw = function (result) {
                    if (result) {
                        if (typeof result.name !== "undefined") {
                            element.value = result.name;
                            element.defaultValue = result.name;
                        }
                    }
                }

                element.onchange = function (e) {

                    if (confirm("Restarting the device. Are you sure you want to continue renaming your device?"))
                    {
                        element.disabled = true;
                    
                        http.get('/api/settings?n=' + element.value, function (result) {
                            redraw(result);
                            element.disabled = false;
                        });

                        return;
                    }

                    element.value = element.defaultValue;
                }

                return element;
            }


            function redraw(settings)
            {
                if (settings) {

                    elements.forEach(function(element) {
                        element.redraw(settings);    
                    }, this);
                }
            }

            el.activate = function () {
                http.get('/api/settings', function (result) {
                    redraw(result);
                });
            }
            
            elements.push(bindDeviceName())
            elements.push(bindDeviceName())
        }
    };

})(Http, Wss);