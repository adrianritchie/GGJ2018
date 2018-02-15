var ConnectView = function() {

    this.initialize = function() {
        this.$el = $('<div/>');

        app.$ws = new WebSocket("ws://"+$("#device-address").val());
        app.$ws.addEventListener('open', this.wsConnected);
        app.$ws.addEventListener('close', this.wsDisconnected);
    };

    this.start = function() {};
  
    this.render = function() {
        this.$el.html(this.template());


        if (app.$ws.readyState == 1) { 
            this.wsConnected();
        } else {
            this.wsDisconnected();
        }
        return this;
    };

    this.wsConnected = function() {
        $('.listening', this.$el).hide();
        $('.received', this.$el).show();
    };

    this.wsDisconnected = function() {
        $('.listening', this.$el).show();
        $('.received', this.$el).hide();
    };
  
    this.initialize();
  
  }; 