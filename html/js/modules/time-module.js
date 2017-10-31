if (typeof app === "undefined") { app = {} }
if (typeof app.modules === "undefined") { app.modules = {} }


app.modules.time = (function (http, wss) {

    return {
        load: function (element) {

            function redraw(result) {
                if (result) {
                    if (typeof result.time !== "undefined") {
                        
                        var shift = new Date() - new Date(Date.parse(new Date().toDateString() + " " + result.time));
                        
                        function checkTime(i) {
                            return (i < 10) ?  "0" + i : i; // add zero in front of numbers < 10
                        }
        
                        var now = new Date(new Date() - shift);
                        
                        var h = now.getHours();
                        var m = now.getMinutes();
                        
                        element.innerHTML = h + ":" + checkTime(m);
                        element.classList.remove('hidden');
                    }
                }
            }

            function ping()
            {
                http.get('/api/settings', function (result) {
                    redraw(result);
                });
            }

            window.addEventListener("click", function(){
                ping();
                setTimeout(function(){
                    element.classList.add('hidden');
                }, 5000)
            });
        }
    };

})(Http, Wss);