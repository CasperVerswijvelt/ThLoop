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

        

        this.connectToMQTTBroker(this);
    },

    onBatteryStatus: function(status) {
        console.log("Level: " + status.level + " isPlugged: " + status.isPlugged);
        cordova.plugins.CordovaMqTTPlugin.publish({
         topic:"theloop1",
         payload: status.level,
         qos:0,
         retain:false,
        success: function(s){
        console.log("SUCCES!!!");
            },
            error:function(e){
        
        }
});
    },

    onBatteryCritical: function(status) {
        alert("Battery Level Critical " + status.level + "%\nRecharge Soon!");
    },

    onBatteryLow: function(status) {
        alert("Battery Level Low " + status.level + "%");
    },

     connectToMQTTBroker: function(self){
        cordova.plugins.CordovaMqTTPlugin.connect({
    url:"tcp://broker.hivemq.com",
    port:1883,
    clientId:"clientid-theloopmlm",
    connectionTimeout:3000,
    keepAlive:60,
    success:function(s){
        console.log("connect success");

        window.addEventListener("batterystatus", self.onBatteryStatus, false);
        window.addEventListener("batterylow", self.onBatteryLow, false);
        window.addEventListener("batterycritical", self.onBatteryCritical, false);

    },
    error:function(e){
        console.log("connect error");
    },
    onConnectionLost:function (){
        console.log("disconnect");
    }
})
    }
    

};

app.initialize();