if (typeof app === "undefined") { app = {} }

app.DOM = window.DOM = (function () {

    function geid(id) {
        return document.getElementById(id);
    }

    function ce(tag) {
        return document.createElement(tag);
    }

    function csl(array) {
        var select = ce('select');

        if (array)
        {
            for (var index = 0; index < array.length; index++) {
                var option = ce('option');
                option.innerText = array[index];
                select.appendChild(option);
            }
        }

        select.getText = function () {
            var selectedIndex = select.selectedIndex;
            if (selectedIndex != -1) {
                return select.options[selectedIndex].text;
            }

            return null;
        }

        select.getValue = function () {
            var selectedIndex = select.selectedIndex;
            if (selectedIndex != -1) {
                return select.options[selectedIndex].value;
            }

            return null;
        }

        select.setText = function (val) {
            for (var i = 0; i < select.options.length; i++) {
                if (select.options[i].text === val) {
                    select.selectedIndex = i;
                    break;
                }
            }
        }

        select.setValue = function (val) {
            for (var i = 0; i < select.options.length; i++) {
                if (select.options[i].value === val) {
                    select.selectedIndex = i;
                    break;
                }
            }
        }

        select.addOption = function (text, value) {
            var option = ce('option');
            option.text = text;
            if (value)
            {
                option.value = value;
            }
            select.appendChild(option);
        }

        return select;
    }

    function csp(html) {
        var span = ce('span');
        for (var key in html) {
            if (html.hasOwnProperty(key)) {
                span[key] = html[key];
            }
        }
        return span;
    }

    return {
        createElement: ce, ce: ce,
        createSelect: csl, csl: csl,
        createSpan: csp, csp: csp,
        geid: geid,
    };
})();