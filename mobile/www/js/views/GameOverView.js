var GameOverView = function(ws) {

    this.initialize = function() {
        this.$el = $('<div/>');
    };
  
    this.render = function() {
        this.$el.html(this.template());
        return this;
    };

    this.start = function() {
        $('.continue').off('click', '#restart');
        $('.continue').on("click", '#restart', this.restart );
    };

    this.restart = function(eventObject) {
        eventObject.preventDefault();
        if (app.$ws.readyState != 1) {
            router.load('');
        }
        else {
            router.load('start');
        }
    }

    this.initialize();
}