
var MessageView = function() {

    this.state = {
        control : 's1',
        initial : {},
        required : undefined,
        initialising : false,
        completed : false,
        lives : 3,
        difficulty : 1
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

        this.state.lives = 3;
        $('[id^="life"]').show();
        $('[id^="dead"]').hide();

        switch(Math.floor(Math.random() * 3)) {
            case 0:
                this.state.control = 's1';
                this.state.difficulty = 1;
                break;
            case 1:
                this.state.control = 'k1';
                this.state.difficulty = 1.5;
                break;
            case 2:
                this.state.control = 't1';
                this.state.difficulty = 4;
                break;
        }

        $("[id^='trigger-']").hide();
        $("#trigger-"+this.state.control).show();
        
        this.sendInstructions();
    };


    this.sendInstructions = function() {
        if (app.$ws.readyState != 1) {
            router.load('');
        }
        var message = 't'+Math.floor(app.level * this.state.difficulty);
        console.log(message);

        app.$ws.send(message);
        this.state.initialising = true;
        this.state.completed = false;
    };

    this.messageReceived = function(msg) {
        console.log(msg.data);

        if (msg.data.startsWith("r")) {
            var remaining = msg.data.substr(2);
            remaining = '' + (remaining / 1000).toFixed(2);
            this.$display.setValue(remaining.padStart(9, ' '));
        }

        if (msg.data.startsWith('c')) {
            var control = msg.data.substr(2, 2);
            var value = msg.data.substr(5);

            if (this.state.initialising) {
                this.initializeControl(control, value);
            }
            else if (!this.state.completed) {
                 this.checkControl(control, value);
            }
        }

        if (msg.data == 'x:0') {
            this.state.initialising = false;
        }

        if (msg.data == "r:0" && !this.state.completed) {
            this.gameOver();
        }
    };

    this.initializeControl = function(control, value) {
        this.state.initial[control] = value;

        if (control == this.state.control) {
            switch (control) {
                case 's1':
                    this.state.required = value == 'on' ? 'off' : 'on';
                    break;
                case 'k1':
                    this.state.required = "A385*";
                    break;
                case 't1':
                    this.state.require = value == 1 ? 2 : 1;
            }
        }
    };

    this.checkControl = function(control, value) {
        if (control == this.state.control) {
            switch (control) {
                case 's1':
                    this.state.completed = (value == this.state.required);
                    break;
                case 'k1':
                    this.checkKeypadEntry(value);
                    break;
                case 't1':
                    this.checkTracer(value);
                    break;
            }

            if (this.state.completed) {
                app.$ws.send('f:1');
                router.load('defused');
            }
        }
        else if (value != this.state.initial[control]) {
            this.reduceLives();
        }
    };

    this.checkTracer = function(value) {
        if (value == 0) {
            return;
        }
        if (value == this.state.required) {
            this.state.completed = true;
        }
        else {
            this.reduceLives();
        }
    }

    this.checkKeypadEntry = function(value) {
        if (value == '') {
            return;
        }
        if (value == this.state.required.charAt(0)) {
            if (this.state.require.length == 1) {
                this.state.completed = true;
            } 
            else {
                this.state.required = this.state.required.substr(1);
            }
        }
        else {
            this.reduceLives();
        }
    };
  
    this.reduceLives = function() {
        $("#life_"+this.state.lives).hide();
        $("#dead_"+this.state.lives).show();

        this.state.lives--;

        if (this.state.lives == 0) {
            this.gameOver();
        }
    };

    this.gameOver = function() {
        app.$ws.send('f:0');
        this.state.completed = true;
        router.load('gameover');
        console.log('gameover');
    }

    this.initialize();
};