cordova.define('cordova/plugin_list', function(require, exports, module) {
module.exports = [
  {
    "id": "cordova-plugin-nativestorage.mainHandle",
    "file": "plugins/cordova-plugin-nativestorage/www/mainHandle.js",
    "pluginId": "cordova-plugin-nativestorage",
    "clobbers": [
      "NativeStorage"
    ]
  },
  {
    "id": "cordova-plugin-nativestorage.LocalStorageHandle",
    "file": "plugins/cordova-plugin-nativestorage/www/LocalStorageHandle.js",
    "pluginId": "cordova-plugin-nativestorage"
  },
  {
    "id": "cordova-plugin-nativestorage.NativeStorageError",
    "file": "plugins/cordova-plugin-nativestorage/www/NativeStorageError.js",
    "pluginId": "cordova-plugin-nativestorage"
  },
  {
    "id": "cordova-plugin-battery-status.battery",
    "file": "plugins/cordova-plugin-battery-status/www/battery.js",
    "pluginId": "cordova-plugin-battery-status",
    "clobbers": [
      "navigator.battery"
    ]
  },
  {
    "id": "cordova-plugin-mqtt.MQTTEmitter",
    "file": "plugins/cordova-plugin-mqtt/www/MQTTEmitter.js",
    "pluginId": "cordova-plugin-mqtt",
    "clobbers": [
      "ME"
    ]
  },
  {
    "id": "cordova-plugin-mqtt.CordovaMqTTPlugin",
    "file": "plugins/cordova-plugin-mqtt/www/cordova-plugin-mqtt.js",
    "pluginId": "cordova-plugin-mqtt",
    "clobbers": [
      "cordova.plugins.CordovaMqTTPlugin"
    ]
  }
];
module.exports.metadata = 
// TOP OF METADATA
{
  "cordova-plugin-whitelist": "1.3.3",
  "cordova-plugin-nativestorage": "2.3.2",
  "cordova-plugin-battery-status": "2.0.2",
  "cordova-plugin-mqtt": "0.3.8"
};
// BOTTOM OF METADATA
});