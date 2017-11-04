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

        select.getValue = function () {
            var selectedIndex = select.selectedIndex;
            if (selectedIndex != -1) {
                return select.options[selectedIndex].text;
            }

            return 0;
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