var MessageView = function(ws) {

    this.initialize = function() {
        this.$el = $('<div/>');
        this.$ws.addEventListener('message', this.messageReceived, false);

        this.sendInstruction();
    };
  
    this.render = function() {
        this.$el.html(this.template());
      
        this.animate();
        return this;
    };

    this.animate = function() {
        if (!this.$display) {
            this.$display = new SegmentDisplay("display");
            this.$display.pattern         = "#######.#";
            this.$display.displayAngle    = 6;
            this.$display.digitHeight     = 20;
            this.$display.digitWidth      = 14;
            this.$display.digitDistance   = 2.5;
            this.$display.segmentWidth    = 2;
            this.$display.segmentDistance = 0.3;
            this.$display.segmentCount    = 14;
            this.$display.cornerType      = 3;
            this.$display.colorOn         = "#090909";
            this.$display.colorOff        = "#b0b0b0";
            this.$display.draw();

            this.$counter = 50;
            this.updateCounter();
        }
  

        if (this.$counter > 0) {
            this.$display.setValue(this.$counter.toString().padStart(9, ' '));
            window.setTimeout(function(s) { 
                s.animate(); 
            }, 250, this);
        }
        else {
            this.$display.setValue('          ');
        }
    };

    this.updateCounter = function() {
        this.$counter = (this.$counter - 0.1).toFixed(1);
        if (this.$counter > 0) {
            window.setTimeout(function(s) {
                s.updateCounter();
            }, 100, this);
        }
    };

    this.sendInstruction = function() {
        while (this.$ws.readyState == )
        this.$ws.send('b1:500');
    };

    this.messageReceived = function(evt) {
        $('#response').text(evt.data);
    };
  
    this.$ws = ws;
    this.initialize();
}