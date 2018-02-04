var GameOverView = function(ws) {

    this.initialize = function() {
        this.$el = $('<div/>');
    };
  
    this.render = function() {
        this.$el.html(this.template());
        return this;
    };

    this.start = function() {
        //$("#restart").unbind("click").bind("click", function() { router.load(''); } );
    };

    this.messageReceived = function() {
    };
  
    this.initialize();
}