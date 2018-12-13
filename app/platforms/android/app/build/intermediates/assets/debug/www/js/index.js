var app = {
    // Application Constructor
    initialize: function() {
        document.addEventListener('deviceready', this.onDeviceReady.bind(this), false);
    },

    // deviceready Event Handler
    //
    // Bind any cordova events here. Common events are:
    // 'pause', 'resume', etc.
    onDeviceReady: function() {
        this.receivedEvent('deviceready');
    },

    // Update DOM on a Received Event
    receivedEvent: function(id) {
        var parentElement = document.getElementById(id);
        var listeningElement = parentElement.querySelector('.listening');
        var receivedElement = parentElement.querySelector('.received');

        listeningElement.setAttribute('style', 'display:none;');
        receivedElement.setAttribute('style', 'display:block;');

        console.log('Received Event: ' + id);

        window.addEventListener("batterystatus", onBatteryStatus, false);
        window.addEventListener("batterylow", onBatteryLow, false);
        window.addEventListener("batterycritical", onBatteryCritical, false);

        //connectToMQTTBroker();
    },

    function onBatteryStatus(status) {
        console.log("Level: " + status.level + " isPlugged: " + status.isPlugged);
    },

    function onBatteryCritical(status) {
        alert("Battery Level Critical " + status.level + "%\nRecharge Soon!");
    },

    function onBatteryLow(status) {
        alert("Battery Level Low " + status.level + "%");
    }

    /*,

    function connectToMQTTBroker(){
        cordova.plugins.CordovaMqTTPlugin.connect({
    url:"tcp://broker.hivemq.com",
    port:1883,
    clientId:"clientid-theloopmlm",
    connectionTimeout:3000,
    keepAlive:60,
    success:function(s){
        console.log("connect success");

        window.addEventListener("batterystatus", onBatteryStatus, false);
        window.addEventListener("batterylow", onBatteryLow, false);
        window.addEventListener("batterycritical", onBatteryCritical, false);

    },
    error:function(e){
        console.log("connect error");
    },
    onConnectionLost:function (){
        console.log("disconnect");
    }
})
    }
    */

};

app.initialize();