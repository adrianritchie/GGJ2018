var HomeView = function(ws) {

    this.initialize = function() {
        this.$el = $('<div/>');

        ws.addEventListener('open', this.wsConnected);
        ws.addEventListener('close', this.wsDisconnected);
    };

    this.start = function() {};
  
    this.render = function() {
        this.$el.html(this.template());


        if (ws.readyState == 1) { 
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