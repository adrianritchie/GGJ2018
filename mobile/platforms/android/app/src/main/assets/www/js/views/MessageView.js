
var MessageView = function() {

    this.state = {
        control : 's1',
        initial : {},
        required : undefined,
        initialising : false,
        completed : false,
        errors : 0,
        messages : 0
    };

    this.initialize = function() {
        this.$el = $('<div/>');
        app.$ws.onmessage = this.messageReceived.bind(this);
    };
  
    this.render = function() {
        this.$el.html(this.template());

        this.$display = new SegmentDisplay("display");
        this.$display.pattern         = "######.##";
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

        return this;
        
    };

    this.start = function() {

        $("#go-again").unbind("click").bind("click", this.start.bind(this) );

        this.state.errors = 0;
        $('[id^="life"]').show();
        $('[id^="dead"]').hide();
        
        this.sendInstructions();
    };


    this.sendInstructions = function() {
        if (app.$ws.readyState != 1) {
            router.load('');
        }
        var message = 't'+app.level;
        console.log(message);

        app.$ws.send(message);
        this.state.initialising = true;
        this.state.completed = false;
    };

    this.messageReceived = function(msg) {
        console.log(msg.data);
        $('#messages').text(++this.state.messages);

        if (msg.data.startsWith("r")) {
            var remaining = msg.data.substr(2);
            remaining = '' + (remaining / 1000).toFixed(2);
            this.$display.setValue(remaining.padStart(9, ' '));
        }

        if (msg.data.startsWith('c')) {
            var control = msg.data.substr(2, 2);
            var value = msg.data.substr(5);

            if (this.state.initialising) {
                this.state.initial[control] = value;

                if (control == this.state.control) {
                    switch (control) {
                        case 's1':
                            this.state.required = value == 'on' ? 'off' : 'on';
                            break;
                    }
                }
            }
            else if (!this.state.completed) {
                 if (control == this.state.control && value == this.state.required) {
                    this.state.completed = true;
                    app.$ws.send('f:1');
                }
                else if (value != this.state.initial[control]) {
                    $('#errors').text(++this.state.errors);
                    this.updateLives();
                }
            }
        }

        if (msg.data == 'x:0') {
            this.state.initialising = false;
        }

        if (msg.data == "r:0" && !this.state.completed) {
            this.state.completed = true;
            router.load('gameover');
            console.log('gameover');
        }
    };
  
    this.updateLives = function() {
        $("#life_"+this.state.errors).hide();
        $("#dead_"+this.state.errors).show();

        if (this.state.errors == 3) {
            app.$ws.send('f:0');
            this.state.completed = true;
            router.load('gameover');
            console.log('gameover');
        }
    };

    this.initialize();
};