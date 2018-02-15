cordova.define("cordova-plugin-zeroconf.ZeroConf", function(require, exports, module) { 'use strict';
var exec = require('cordova/exec');

var ZeroConf = {

    registerAddressFamily : 'any', /* or ipv6 or ipv4 */
    watchAddressFamily : 'any', /* or ipv6 or ipv4 */

    getHostname : function(success, failure) {
        throw "Method not implemented";
    },

    register : function(type, domain, name, port, props, success, failure) {
        throw "Method not implemented";    
    },

    unregister : function(type, domain, name, success, failure) {
        throw "Method not implemented";
    },

    stop : function(success, failure) {
        throw "Method not implemented";
    },

    watch : function(type, domain, success, failure) {
        //return exec(success, failure, "ZeroConf", "watch", [ type, domain, this.watchAddressFamily ]);
        chrome.mdns.onServiceList.addListener(
            function(services) {
                services.forEach(service => {
                    console.log("service resolved", service);
                });
            },
            {
                serviceType: type+domain
            }
        );
    },

    unwatch : function(type, domain, success, failure) {
        return exec(success, failure, "ZeroConf", "unwatch", [ type, domain ]);
    },

    close : function(success, failure) {
        return exec(success, failure, "ZeroConf", "close", []);
    },

    reInit : function(success, failure) {
        throw "Method not implemented";
    }

};

module.exports = ZeroConf;
});
