var DefusedView = function() {

    this.initialize = function() {
        this.$el = $('<div/>');
    };
  
    this.render = function() {
        this.$el.html(this.template());
        return this;
    };

    this.start = function() {
        $('.continue').off('click', '#nextLevel');
        $('.continue').on("click", '#nextLevel', this.nextLevel );
    };

    this.nextLevel = function(eventObject) {
        eventObject.preventDefault();
        if (app.$ws.readyState != 1) {
            router.load('');
        }
        else {
            app.level--;
            router.load('message');
        }
    }

    this.initialize();
}