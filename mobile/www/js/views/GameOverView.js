var GameOverView = function(ws) {

    this.initialize = function() {
        this.$el = $('<div/>');
    };
  
    this.render = function() {
        this.$el.html(this.template());
        return this;
    };

    this.start = function() {
        $('.app').addClass('gameover');
        $('.continue').off('click', '#restart');
        $('.continue').on("click", '#restart', this.restart );
    };

    this.restart = function(eventObject) {

        $('.app').removeClass('gameover');
        eventObject.preventDefault();
        if (app.$ws.readyState != 1) {
            router.load('');
        }
        else {
            app.level++;
            router.load('start');
        }
    }

    this.initialize();
}