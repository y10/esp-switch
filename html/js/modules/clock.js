if (typeof app.modules === "undefined") { app.modules = {} }

app.modules.clock = (function (loader, http, wss, DOM) {
    function createSetting(key, options, onrender) {
    
        var select = DOM.createSelect(options);
        select.render = function (state) {
            select.disabled = true;

            if (state && state.hasOwnProperty(key)) {
                for (var i = 0; i < select.options.length; i++) {
                    if (select.options[i].innerText === state[key]) {
                        select.selectedIndex = i;
                        break;
                    }
                }
            }

            select.disabled = false;
        };

        select.onchange = function (e) {
            if (!select.disabled) {
                select.disabled = true;
                var data = {};
                data[key] = parseInt(select.getText());
                http.post("/api/settings", data, function (state) {
                    onrender(state);
                    select.disabled = false;
                });
            }
        };

        return select;
    }

    return {
        load: function (element) { 
            var section = document.createElement('section');
            section.className = "slide-container";

            element.render = function (data) {
                if (data && data.hasOwnProperty("time")) {
                    var parts = data["time"].split(":");

                    if (parts.length > 0) {
                        data.time_hour = ("0" + parts[0]).slice(-2);
                        hours.render(data);
                    }

                    if (parts.length > 1) {
                        data.time_min = ("0" + parts[1]).slice(-2);
                        minutes.render(data);
                    } 
                }           
            };
            
            var hours = createSetting("time_hour", Array.apply(null, Array(24)).map(function (x, y) { return ("0" + y).slice(-2); }), function(data){
                element.render(data);
            });
            section.appendChild(hours);

            section.appendChild(DOM.createSpan({ innerText: ":" }));

            var minutes = createSetting("time_min", Array.apply(null, Array(60)).map(function (x, y) { return ("0" + y).slice(-2); }), function(data){
                element.render(data);
            });
            section.appendChild(minutes);

            element.activate = function () {
                http.get('/api/settings', function (result) {
                    element.render(result);
                });
            };

            element.appendChild(section);
        }
    };

})(app.modules, Http, Wss, app.DOM);