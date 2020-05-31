if (typeof app.modules === "undefined") { app.modules = {} }

app.modules.settings = (function (loader, http, wss, DOM) {

    function createSetting(element, key, placeholder, restart) {

        var input = document.createElement("input");
        input.id = key;
        input.type = "text";
        input.placeholder = placeholder;

        var outerDiv = document.createElement("div");
        outerDiv.appendChild(input);
        element.appendChild(outerDiv);

        return {
            render: function (settings) {
                if (settings && typeof settings[key] !== "undefined") {
                    input.value = settings[key];
                    input.defaultValue = input.value;
                }
            },
            save:  function (settings) {
                if (input.value !== input.defaultValue){
                    settings[key] = input.value;
                    if (restart) settings.restart = true;
                }
            }
        };
    }

    function createSettingForTime(element) {

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

        var hours = createHourSelect();
        var minutes = createMinuteSelect();

        element.appendChild(hours);
        element.appendChild(DOM.createSpan({ innerText: ":" }));
        element.appendChild(minutes);
       
        return {
            render: function (settings) {
               hours.render(settings);
               minutes.render(settings);
            },
            save: function (settings) {
                hours.save(settings);
                minutes.save(settings);
             }
        };
 
    }

    const upgrade_button = '<svg height="50" viewBox="0 0 58 58" width="50" xmlns="http://www.w3.org/2000/svg"><path style="fill: rgb(145, 220, 90);" d="m55 20c-1.104 0-2 .896-2 2v26c0 2.206-1.794 4-4 4h-32c-2.206 0-4-1.794-4-4v-32c0-2.206 1.794-4 4-4h26c1.104 0 2-.896 2-2s-.896-2-2-2h-26c-4.411 0-8 3.589-8 8v32c0 4.411 3.589 8 8 8h32c4.411 0 8-3.589 8-8v-26c0-1.104-.896-2-2-2z"/><path style="fill: rgb(145, 220, 90);" d="m28 23.999c-1.104 0-2 .896-2 2v9c0 2.206 1.794 4 4 4h9c1.104 0 2-.896 2-2s-.896-2-2-2h-9v-9c0-1.104-.896-2-2-2z"/></svg>';

    return {
        load: function (element) { 
            var section = document.createElement('section');
            section.className = "slide-container";

            var settings = 
            [
                createSetting(section, "disp_name", "display name", true),
                createSetting(section, "upds_addr", "update server", false, {
                    title: "Upgrade",
                    innerHtml: upgrade_button,
                    onclick: function(e){
                        http.get('/api/upgrade', function (result) {
                            window.location.reload();
                        });
                    }
                }),
                createSetting(section, "mqtt_addr", "mqtt server", true),
                createSetting(section, "mqtt_user", "mqtt user", true),
                createSetting(section, "mqtt_pwrd", "mqtt password", true),
                createSettingForTime(section)
            ];

            element.render = function (data) {
                settings.forEach(function (el) {
                    el.render(data);
                }, this);
            };
    
            element.activate = function () {
                http.get('/api/settings', function (result) {
                    element.render(result);
                });
            };

            element.deactivate = function () {
                var data = {};

                settings.forEach(function (setting) {
                    setting.save(data);
                }, this);

                if (data.restart && confirm("Restarting the device. Are you sure you want to continue?") == false) {
                    return; 
                }

                if (Object.keys(data).length !== 0)
                {
                    http.post('/api/settings', data, function (result) {
                        if (data.restart) {
                            window.location.reload();
                        }
                    });
                }
            };
    
            element.appendChild(section);
        }
    };

})(app.modules, Http, Wss, app.DOM);