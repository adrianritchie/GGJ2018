
var app = {

    // Application Constructor
    initialize: function() {
        HomeView.prototype.template = Handlebars.compile($("#home-tpl").html());
        ConnectView.prototype.template = Handlebars.compile($('#connect-tpl').html());
        StartView.prototype.template = Handlebars.compile($('#start-tpl').html());
        MessageView.prototype.template = Handlebars.compile($("#message-tpl").html());
        GameOverView.prototype.template = Handlebars.compile($("#gameover-tpl").html());

        
        

        router.addRoute('', function() { app.showView(new HomeView()); });
        router.addRoute('connect', function() { app.showView(new ConnectView()); });
        router.addRoute('start', function() { app.showView(new StartView()); });
        router.addRoute('message', function() { app.showView(new MessageView()); });
        router.addRoute('gameover', function() { app.showView(new GameOverView()); });
        router.start();

        this.bindEvents();
    },
    // Bind Event Listeners

    showView: function(view) {
        $('.app').html(view.render().$el);
        view.start(); 
    },
    
    // Bind any events that are required on startup. Common events are:
    // 'load', 'deviceready', 'offline', and 'online'.
    bindEvents: function() {
        document.addEventListener('deviceready', this.onDeviceReady, false);
    },
    // deviceready Event Handler
    //
    // The scope of 'this' is the event. In order to call the 'receivedEvent'
    // function, we must explicitly call 'app.receivedEvent(...);'
    onDeviceReady: function() {
        if (!!bluetoothSerial) {
            console.log('we have bt');
            bluetoothSerial.list(app.ondevicelist, ()=>console.log("error")); 
        }
        router.load('');      
    },
    ondevicelist : function(devices) {
        devices.forEach(function(device) {
            if( !!device.name && device.name.length > 5 &&
                 device.name.substr(0, 5) == "bomb-" && device.name != "bomb-offline") {
                app.ipAddress = device.name.substr(5);
            }
            console.log(device.name);
        });
    }
};