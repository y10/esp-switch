if (typeof app === "undefined") { app = {} }
if (typeof app.modules === "undefined"){ app.modules = {} }

app.modules.schedule = (function (http) {
    
    return {
        load: function (el, day, caption) {
            
            function createSpan(html) {
                var span = document.createElement('span');
                for (var key in html) {
                    if (html.hasOwnProperty(key)) {
                        span[key] = html[key];
                    }
                }
                return span;
            }

            function createHeader() {

                return createSpan({
                    className: "slide-header",
                    innerText: caption
                });;
            }

            function createSwitch() {

                var label = document.createElement("label");
                label.className = "switch";

                var input = document.createElement("input");
                input.type = "checkbox";
                label.appendChild(input)

                var span = document.createElement("span");
                span.className = "slider round";
                label.appendChild(span)

                label.render = function (state) {
                    if(state) {
                        input.checked = (state.enabled) ? true : false;
                    }
                }

                label.onclick = function (e) {

                    e.preventDefault();
                    label.disabled = true;
                    http.get('/api/schedule' + (day ? "/" + day : "") + "?enabled=" + (input.checked ? "0" : "1"), function (state) {
                        refresh(state);
                        label.disabled = false;
                    });

                }

                return label;
            }

            function createHourSelect() {

                var select = document.createElement('select');

                for (var index = 0; index < 24; index++) {
                    var option = document.createElement('option');
                    option.innerText = ("0" + index).slice(-2);
                    select.appendChild(option);
                }

                select.render = function (state) {
                    select.disabled = true;

                    if (state) {
                        if (typeof state.t !== "undefined") {
                            var time = state.t;
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

                        http.get('/api/schedule' + (day ? "/" + day : "") + "?h=" + select.getValue(), function (state) {

                            refresh(state);

                            select.disabled = false;
                        });
                    }
                }

                select.getValue = function () {
                    var selectedIndex = select.selectedIndex;
                    if (selectedIndex != -1) {
                        return select.options[selectedIndex].text;
                    }

                    return 0;
                }

                return select;
            }

            function createMinuteSelect() {

                var select = document.createElement('select');

                for (var index = 0; index < 60; index++) {
                    var option = document.createElement('option');
                    option.innerText = ("0" + index).slice(-2);
                    select.appendChild(option);
                }

                select.render = function (state) {
                    select.disabled = true;

                    if (state) {
                        if (typeof state.t !== "undefined") {
                            var time = state.t;
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

                        http.get('/api/schedule' + (day ? "/" + day : "") + "?m=" + select.getValue(), function (state) {

                            refresh(state);

                            select.disabled = false;

                        });
                    }
                }

                select.getValue = function () {
                    var selectedIndex = select.selectedIndex;
                    if (selectedIndex != -1) {
                        return select.options[selectedIndex].text;
                    }

                    return 0;
                }

                return select;
            }

            function createDurationSelect() {

                var select = document.createElement('select');

                for (var index = 0; index <= 30; index += 5) {
                    var option = document.createElement('option');
                    option.innerText = ("0" + index).slice(-2);
                    select.appendChild(option);
                }

                select.render = function (state) {
                    select.disabled = true;

                    if (state) {
                        if (typeof state.d !== "undefined") {
                            var durValue = ("0" + state.d).slice(-2);
                            for (var i = 0; i < select.options.length; i++) {
                                if (select.options[i].innerText === durValue) {
                                    select.selectedIndex = i;
                                    break;
                                }
                            }
                        }
                    }

                    select.disabled = false;
                }

                select.getValue = function () {
                    var selectedIndex = select.selectedIndex;
                    if (selectedIndex != -1) {
                        return select.options[selectedIndex].text;
                    }

                    return 0;
                }

                select.onchange = function (e) {
                    if (!select.disabled) {
                        select.disabled = true;

                        http.get('/api/schedule' + (day ? "/" + day : "") + "?d=" + select.getValue(), function (state) {

                            refresh(state);

                            select.disabled = false;

                        });
                    }
                }

                return select;
            }

            function refresh(state)
            {
                if (state) {

                    hours.render(state);
                    
                    minutes.render(state);

                    duration.render(state);
                    
                    swtch.render(state);
                }
            }

            el.appendChild(createHeader());
            var div = document.createElement('section');
            div.className = "slide-container";

            var swtch = createSwitch();
            div.appendChild(swtch);

            var hours = createHourSelect();
            div.appendChild(hours);

            div.appendChild(createSpan({ innerText: ":" }));

            var minutes = createMinuteSelect();
            div.appendChild(minutes);

            div.appendChild(createSpan({ innerHTML: "&nbsp;" }));

            var duration = createDurationSelect();
            div.appendChild(duration);

            el.activate = function () {
                http.get('/api/schedule' + (day ? "/" + day : ""), function (state) {
                    refresh(state);
                });
            }

            el.deactivate = function () { };

            el.appendChild(div);
        }
    };
})(Http);