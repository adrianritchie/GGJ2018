
var app = {

    // Application Constructor
    initialize: function() {
        HomeView.prototype.template = Handlebars.compile($("#home-tpl").html());
        StartView.prototype.template = Handlebars.compile($('#start-tpl').html());
        MessageView.prototype.template = Handlebars.compile($("#message-tpl").html());
        GameOverView.prototype.template = Handlebars.compile($("#gameover-tpl").html());

        this.$ws = new WebSocket("ws://192.168.2.130/");

        router.addRoute('', function() { app.showView(new HomeView(app.$ws)); });
        router.addRoute('start', function() { app.showView(new StartView(app.$ws)); });
        router.addRoute('message', function() { app.showView(new MessageView(app.$ws)); });
        router.addRoute('gameover', function() { app.showView(new GameOverView(app.$ws)); });
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