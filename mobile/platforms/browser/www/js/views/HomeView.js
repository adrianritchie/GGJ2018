var HomeView = function(ws) {

    this.initialize = function() {
        this.$el = $('<div/>');
    };
  
    this.render = function() {
        this.$el.html(this.template());
        return this;
    };

    this.start = function() {
        debugger;
    };

    this.messageReceived = function() {
    };
  
    this.initialize();
}