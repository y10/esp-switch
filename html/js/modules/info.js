if (typeof app.modules === "undefined") { app.modules = {} }

app.modules.info = (function (loader, http, wss, DOM) {

    function createSetting(key, type, placeholder, restart) {

        var input = document.createElement("input");
        input.id = key;
        input.type = type;
        input.placeholder = placeholder;

        var div = document.createElement("div");
        div.render = function (settings) {
            if (settings && typeof settings[key] !== "undefined") {
                input.value = (type == "password") ? window.atob(settings[key]) : settings[key];
                input.defaultValue = input.value;
            }
        };
        div.save = function (settings) {
            if (input.value !== input.defaultValue){
                settings[key] = input.value;
                if (restart) settings.restart = true;
            }
        };
        div.appendChild(input);

        return div;
    }

    return {
        load: function (element) { 
            var section = document.createElement('section');
            section.className = "slide-container";

            var elements = 
            [
                createSetting("disp_name", "text", "display name", true),
                createSetting("upds_addr", "text", "update server", false),
                createSetting("mqtt_addr", "text", "mqtt server", true),
                createSetting("mqtt_user", "text", "mqtt user", true),
                createSetting("mqtt_pwrd", "password", "mqtt pwd", true)
            ];

            elements.forEach(function (el) {
                section.appendChild(el);
            }, this);

            element.render = function (data) {
                elements.forEach(function (el) {
                    if(el["render"]){
                        el.render(data);
                    }
                }, this);
            };
    
            element.activate = function () {
                http.get('/api/settings', function (result) {
                    element.render(result);
                });
            };

            element.deactivate = function () {
                var data = {};

                elements.forEach(function (el) {
                    if(el["save"]){
                        el.save(data);
                    }
                }, this);

                if (data.restart && confirm("Restarting the device. Are you sure you want to continue?") == false) {
                    return; 
                }

                if (Object.keys(data).length !== 0)
                {
                    http.post('/api/settings', data, function (result) {
                        if (data.restart) {
                            Reload(5000);
                        }
                    });
                }
            };
    
            element.appendChild(section);
        }
    };

})(app.modules, Http, Wss, app.DOM);