
var app = {

    // Application Constructor
    initialize: function() {
        HomeView.prototype.template = Handlebars.compile($("#home-tpl").html());
        StartView.prototype.template = Handlebars.compile($('#start-tpl').html());
        MessageView.prototype.template = Handlebars.compile($("#message-tpl").html());

        
        this.$ws = new WebSocket("ws://10.120.0.65/");

        router.addRoute('', function() { $('.app').html(new HomeView(app.$ws).render().$el); });
        router.addRoute('start', function() { $('.app').html(new StartView(app.$ws).render().$el); });
        router.addRoute('message', function() { $('.app').html(new MessageView(app.$ws).render().$el); });
        router.start();

        this.bindEvents();
    },
    // Bind Event Listeners
    
    // Bind any events that are required on startup. Common events are:
    // 'load', 'deviceready', 'offline', and 'online'.
    bindEvents: function() {
        document.addEventListener('deviceready', this.onDeviceReady, false);
        this.$ws.addEventListener('close', router.load(''));
        this.$ws.addEventListener('error', router.load(''));
    },
    // deviceready Event Handler
    //
    // The scope of 'this' is the event. In order to call the 'receivedEvent'
    // function, we must explicitly call 'app.receivedEvent(...);'
    onDeviceReady: function() {
        router.load('');
       
    }
};