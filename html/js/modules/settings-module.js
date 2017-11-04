app.modules.settings = (function (loader, http, wss, DOM) {

    function createDeviceNameInput(element) {

        var input = document.createElement("input");
        input.id = "device-name";
        input.type = "text";
        input.placeholder = "switch name";
        input.render = function (result) {
            if (result) {
                if (typeof result.name !== "undefined") {
                    input.value = result.name;
                    input.defaultValue = result.name;
                }
            }
        }

        input.onchange = function (e) {

            if (confirm("Rrestarting the device. Are you sure you want to continue?")) {
                input.disabled = true;

                http.get('/api/settings?n=' + input.value, function (result) {
                    input.render(result);
                    input.disabled = false;
                });

                return;
            }

            input.value = input.defaultValue;
        }

        var outerDiv = document.createElement("div");
        outerDiv.appendChild(input);
        element.appendChild(outerDiv);

        return [input];
    }

    function createTimeControl(element) {

        function createHourSelect() {

            var select = DOM.createSelectFromArray(Array.apply(null, Array(24)).map(function (x, y) { return ("0" + y).slice(-2); }));

            select.render = function (state) {
                select.disabled = true;

                if (state) {
                    if (typeof state.time !== "undefined") {
                        var time = state.time;
                        var timeParts = time.split(":");
                        if (timeParts.length > 0) {
                            var hourValue = ("0" + timeParts[0]).slice(-2);
                            for (var i = 0; i < select.options.length; i++) {
                                if (select.options[i].innerText === hourValue) {
                                    select.selectedIndex = i;
                                    break;
                                }
                            }
                        }
                    }
                }

                select.disabled = false;
            }

            select.onchange = function (e) {
                if (!select.disabled) {
                    select.disabled = true;

                    http.get("/api/settings?h=" + select.getValue(), function (state) {
                        select.render(state);
                        select.disabled = false;
                    });
                }
            }

            return select;
        }

        function createMinuteSelect() {

            var select = DOM.createSelectFromArray(Array.apply(null, Array(60)).map(function (x, y) { return ("0" + y).slice(-2); }));
            
            select.render = function (state) {
                select.disabled = true;

                if (state) {
                    if (typeof state.time !== "undefined") {
                        var time = state.time;
                        var timeParts = time.split(":");
                        if (timeParts.length > 1) {
                            var minValue = ("0" + timeParts[1]).slice(-2);
                            for (var i = 0; i < select.options.length; i++) {
                                if (select.options[i].innerText === minValue) {
                                    select.selectedIndex = i;
                                    break;
                                }
                            }
                        }
                    }
                }

                select.disabled = false;
            }

            select.onchange = function (e) {
                if (!select.disabled) {
                    select.disabled = true;

                    http.get("/api/settings?m=" + select.getValue(), function (state) {
                        select.render(state);
                        select.disabled = false;
                    });
                }
            }

            return select;
        }

        var hours = createHourSelect();
        var minutes = createMinuteSelect();

        element.appendChild(hours);
        element.appendChild(DOM.createSpan({ innerText: ":" }));
        element.appendChild(minutes);

        return [hours, minutes];
    }

    return { load: function (el) { loader.create('/api/settings', el, [createDeviceNameInput, createTimeControl]) } };

})(app.modules, Http, Wss, app.DOM);