var MessageQueue = (function () {

  var RETRY_MAX = 5;

  var queue = [];
  var sending = false;
  var timer = null;

  return {
    reset: reset,
    sendAppMessage: sendAppMessage,
    size: size
  };

  function reset() {
    queue = [];
    sending = false;
  }

  function sendAppMessage(message, ack, nack) {

    if (! isValidMessage(message)) {
      return false;
    }

    queue.push({
      message: message,
      ack: ack || null,
      nack: nack || null,
      attempts: 0
    });

    setTimeout(function () {
      sendNextMessage();
    }, 1);

    return true;
  }

  function size() {
    return queue.length;
  }

  function isValidMessage(message) {
    // A message must be an object.
    if (message !== Object(message)) {
      return false;
    }
    var keys = Object.keys(message);
    // A message must have at least one key.
    if (! keys.length) {
      return false;
    }
    for (var k = 0; k < keys.length; k += 1) {
      var validKey = /^[0-9a-zA-Z-_]*$/.test(keys[k]);
      if (! validKey) {
        return false;
      }
      var value = message[keys[k]];
      if (! validValue(value)) {
        return false;
      }
    }

    return true;

    function validValue(value) {
      switch (typeof(value)) {
        case 'string':
          return true;
        case 'number':
          return true;
        case 'object':
          if (toString.call(value) == '[object Array]') {
            return true;
          }
      }
      return false;
    }
  }

  function sendNextMessage() {

    if (sending) { return; }
    var message = queue.shift();
    if (! message) { return; }

    message.attempts += 1;
    sending = true;
    Pebble.sendAppMessage(message.message, ack, nack);

    timer = setTimeout(function () {
      timeout();
    }, 1000);

    function ack() {
      clearTimeout(timer);
      setTimeout(function () {
        sending = false;
        sendNextMessage();
      }, 200);
      if (message.ack) {
        message.ack.apply(null, arguments);
      }
    }

    function nack() {
      clearTimeout(timer);
      if (message.attempts < RETRY_MAX) {
        queue.unshift(message);
        setTimeout(function () {
          sending = false;
          sendNextMessage();
        }, 200 * message.attempts);
      }
      else {
        if (message.nack) {
          message.nack.apply(null, arguments);
        }
      }
    }

    function timeout() {
      setTimeout(function () {
        sending = false;
        sendNextMessage();
      }, 1000);
      if (message.ack) {
        message.ack.apply(null, arguments);
      }
    }

  }

}());

(function(){
	'use strict';

	var TYPE = {
		BEER: 0,
		BREWERY: 1,
		STYLE: 2,
		LOADED: 3
	};

	var BreweryDB = {
		server: 'http://api.brewerydb.com/v2',
		apiKey: '',
		error: function(error) {
			appMessageQueue.add({error: error});
			appMessageQueue.send();
		},
		get_beers: function() {
			console.log('Getting beers..');
			var beers_url = this.server + '/beers?key=' + this.apiKey + "&verified=true";
			var xhr = new XMLHttpRequest();
			xhr.open('GET', beers_url);
			xhr.onload = function(e) {
				if (xhr.readyState == 4 && xhr.status == 200) {
					var res = JSON.parse(xhr.responseText);
					Hoppd.beers = res.data;
					Hoppd.beers.forEach(function (element, index, array) {
						MessageQueue.sendAppMessage({type: TYPE.BEER, index: index, name: element.name})
					});
				}
			};
			xhr.send(null);
		},
		get_breweries: function() {
			var breweries_url = this.server + '/breweries?key=' + this.apiKey + "&verified=true";
			console.log('Getting breweries..' + breweries_url);
			var xhr = new XMLHttpRequest();
			xhr.open('GET', breweries_url);
			xhr.onload = function(e) {
				if (xhr.readyState == 4 && xhr.status == 200) {
					var res = JSON.parse(xhr.responseText);
					Hoppd.breweries = res.data;
					Hoppd.breweries.forEach(function (element, index, array) {
						MessageQueue.sendAppMessage({type: TYPE.BREWERY, index: index, name: element.name})
					});
				}
			};
			xhr.send(null);
		},
		get_styles: function() {
			console.log('Getting styles..');
			var styles_url = this.server + '/styles?key=' + this.apiKey + "&verified=true";
			console.log('Getting breweries..' + styles_url);
			var xhr = new XMLHttpRequest();
			xhr.open('GET', styles_url);
			xhr.onload = function(e) {
				if (xhr.readyState == 4 && xhr.status == 200) {
					var res = JSON.parse(xhr.responseText);
					Hoppd.styles = res.data;
					Hoppd.styles.forEach(function (element, index, array) {
						MessageQueue.sendAppMessage({type: TYPE.STYLE, index: index, name: element.name})
					});
				}
			};
			xhr.send(null);
		}
	};

	var Hoppd = {
		beers: [],
		breweries: [],
		styles: []
	};

	Pebble.addEventListener('ready', function(e) {
		console.log('app ready');
	});

	Pebble.addEventListener('appmessage', function(e) {
		if (!isset(e.payload.type)) return;
		switch (e.payload.type) {
			case TYPE.BEER:
				BreweryDB.get_beers();				
				break;
			case TYPE.BREWERY:
				BreweryDB.get_breweries();
				break;
			case TYPE.STYLE:
				BreweryDB.get_styles();
				break;
		}
	});

	function isset(i) {
		return (typeof i != 'undefined');
	}
}());
