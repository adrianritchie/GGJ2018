var StartView = function(ws) {

    this.initialize = function() {
        this.$el = $('<div/>');
        //ws.addEventListener('message', this.messageReceived, false);
    };
  
    this.render = function() {
        this.$el.html(this.template());
        return this;
    };

    this.messageReceived = function() {
    };
  
    this.initialize();
}