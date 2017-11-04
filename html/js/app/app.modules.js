if (typeof app === "undefined") { app = {} }

app.modules = (function (http) {

    function create(service, element, factories) {

        var elements = [];
        var section = document.createElement('section');
        section.className = "slide-container";

        factories.forEach(function (createElement) {

            createElement(section).forEach(function (element) {

                elements.push(element);

            }, this);

        }, this);

        element.render = function (settings) {
            elements.forEach(function (el) {
                el.render(settings);
            }, this);
        };

        element.activate = function () {
            http.get(service, function (result) {
                element.render(result);
            });
        };

        element.appendChild(section);
    }

    return { create: create };

})(Http);

