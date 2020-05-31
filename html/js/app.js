if (typeof app === "undefined") { app = {} }

app.DOM = (function () {

    function createSelectFromArray(array) {

        var select = createSelect();

        for (var index = 0; index < array.length; index++) {
            var option = document.createElement('option');
            option.innerText = array[index];
            select.appendChild(option);
        }

        return select;
    }

    function createSelect() {
        var select = document.createElement('select');

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
            var option = document.createElement('option');
            option.text = text;
            if (value)
            {
                option.value = value;
            }
            select.appendChild(option);
        }

        return select;
    }

    function createSpan(html) {
        var span = document.createElement('span');
        for (var key in html) {
            if (html.hasOwnProperty(key)) {
                span[key] = html[key];
            }
        }
        return span;
    }

    return {
        createSelectFromArray: createSelectFromArray,
        createSelect: createSelect,
        createSpan: createSpan
    };
})();