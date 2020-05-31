if (typeof app.modules === "undefined") { app.modules = {} }

app.modules.clock = (function (loader, http, wss, DOM) {

    function createHourSelect() {

        var select = DOM.createSelectFromArray(Array.apply(null, Array(24)).map(function (x, y) { return ("0" + y).slice(-2); }));

        select.render = function (state) {
            select.disabled = true;
            select.selectedIndexDefault = 0;

            if (state) {
                if (typeof state.time !== "undefined") {
                    var time = state.time;
                    var timeParts = time.split(":");
                    if (timeParts.length > 0) {
                        var hourValue = ("0" + timeParts[0]).slice(-2);
                        for (var i = 0; i < select.options.length; i++) {
                            if (select.options[i].innerText === hourValue) {
                                select.selectedIndex = i;
                                select.selectedIndexDefault = i
                                break;
                            }
                        }
                    }
                }
            }

            select.disabled = false;
        };

        select.save = function (settings) {
            if (select.selectedIndex !== select.selectedIndexDefault){
                settings.time_hour = select.getText();
            }
        };

        return select;
    }

    function createMinuteSelect() {

        var select = DOM.createSelectFromArray(Array.apply(null, Array(60)).map(function (x, y) { return ("0" + y).slice(-2); }));
        
        select.render = function (state) {
            select.disabled = true;
            select.selectedIndexDefault = 0

            if (state) {
                if (typeof state.time !== "undefined") {
                    var time = state.time;
                    var timeParts = time.split(":");
                    if (timeParts.length > 1) {
                        var minValue = ("0" + timeParts[1]).slice(-2);
                        for (var i = 0; i < select.options.length; i++) {
                            if (select.options[i].innerText === minValue) {
                                select.selectedIndex = i;
                                select.selectedIndexDefault = i
                                break;
                            }
                        }
                    }
                }
            }

            select.disabled = false;
        };

        select.save = function (settings) {
            if (select.selectedIndex !== select.selectedIndexDefault){
                settings.time_min = select.getText();
            }
        };

        return select;
    }

    return {
        load: function (element) { 
            var section = document.createElement('section');
            section.className = "slide-container";
            
            var hours = createHourSelect();
            section.appendChild(hours);

            section.appendChild(DOM.createSpan({ innerText: ":" }));

            var minutes = createMinuteSelect();
            section.appendChild(minutes);

            element.render = function (data) {
                hours.render(data);
                minutes.render(data);
            };
    
            element.activate = function () {
                http.get('/api/settings', function (result) {
                    element.render(result);
                });
            };

            element.appendChild(section);
        }
    };

})(app.modules, Http, Wss, app.DOM);