var HomeView = function() {

    this.initialize = function() {
        this.$el = $('<div/>');
    };
  
    this.render = function() {
        this.$el.html(this.template());
        return this;
    };

    this.start = function() {
        if (!!app.ipAddress) {
            $('#device-address').text(app.ipAddress);
        }
    };
  
    this.initialize();
}