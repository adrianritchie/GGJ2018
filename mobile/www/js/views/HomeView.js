var HomeView = function() {

    this.initialize = function() {
        this.$el = $('<div/>');
    };
  
    this.render = function() {
        this.$el.html(this.template());
        return this;
    };

    this.start = function() {
        $(".app").removeClass("gameover");
        $.getJSON("http://blynk-cloud.com/091b2133b443485dbb2e71686f361c6d/get/V0", function(data) {
            console.log(data);
            $('#device-address').val(data[3]);
        });
    };
  
    this.initialize();
}