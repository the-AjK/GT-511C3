/**
 * File: example.js
 * Module: example
 * Author: Alberto Garbui
 * Created: 05/03/15
 * Version: 1.0
 * Description: example application for GT511C3 module
 * Modification History:
 ==============================================
 * Version | Changes
 ==============================================
 * 1.0 File creation
 ==============================================
 */

var GT511C3 = require('gt511c3');
var async = require('async');
var fs = require('fs');

var fps = new GT511C3('/dev/ttyS1', {
	baudrate: 115200
		//baudrate: 57600,
		//baudrate: 38400,
		//baudrate: 19200,
		//baudrate: 9600,
		//debug: true
});

var ID_TO_USE = 199;
var TEMPLATE = new Buffer(498);
var TEMPLATE_PATH = './';
var IMAGE_PATH = './';

function printHelp() {
	console.log("\n============================================================================================\n");
	console.log("   GT511-C3 Test suite - Alberto Garbui\n");
	console.log("   Usage:\n");
	console.log("   [0]: exit");
	console.log("   [1]: ledON");
	console.log("   [2]: ledOFF");
	console.log("   [3]: getEnrollCount");
	console.log("   [4]: checkEnrolled(" + ID_TO_USE + ")");
	console.log("   [5]: enrollStart(" + ID_TO_USE + ")");
	console.log("   [6]: enroll1");
	console.log("   [7]: enroll2");
	console.log("   [8]: enroll3");
	console.log("   [9]: isPressFinger");
	console.log("   [q]: waitReleaseFinger");
	console.log("   [w]: waitFinger");
	console.log("   [e]: enroll");
	console.log("   [r]: deleteID(" + ID_TO_USE + ")");
	console.log("   [t]: deleteAll");
	console.log("   [y]: verify(" + ID_TO_USE + ")");
	console.log("   [u]: identify");
	console.log("   [i]: verifyTemplate(" + ID_TO_USE + ", TEMPLATE)");
	console.log("   [o]: identifyTemplate(TEMPLATE)");
	console.log("   [p]: captureFinger");
	console.log("   [a]: makeTemplate");
	console.log("   [s]: getImage 258x202 - 52116bytes (image 256x256)");
	console.log("   [d]: getRawImage 240*320/4 = 19200 bytes (image 320x240)");
	console.log("   [f]: getTemplate(" + ID_TO_USE + ")");
	console.log("   [g]: setTemplate(" + ID_TO_USE + ", TEMPLATE)\n");
	console.log("   [b]: changeBaudrate(9600)\n");

	console.log("   [z]: init\n");

	console.log("   [h]: print this HELP\n");

	console.log("============================================================================================\n");
}

var isInit = false;
var stdin = process.stdin;
stdin.setRawMode(true);
stdin.resume();
stdin.setEncoding('utf8');

printHelp();

stdin.on('data', function(key) {
	if (key == '0') {
		if (isInit) {
			fps.close().then(function() {
				console.log('close OK!');
				exit();
			}, function(err) {
				console.log('close err: ' + fps.decodeError(err));
				fps.closePort();
				exit();
			});
		} else {
			fps.closePort();
			exit();
		}
	} else if (key == '1') {
		fps.ledONOFF(1).then(function() {
			console.log('ledON: OK!');
		}, function(err) {
			console.log('ledON error: ' + fps.decodeError(err));
		});
	} else if (key == '2') {
		fps.ledONOFF(0).then(function() {
			console.log('ledOFF: OK!');
		}, function(err) {
			console.log('ledOFF error: ' + fps.decodeError(err));
		});
	} else if (key == '3') {
		var functionList = [];
		for (var i = 0; i < 200; i++) {
			var fun = function(ID) {
				return function check(callback) {
					fps.checkEnrolled(ID).then(function() {
						callback(null, true);
					}, function(err) {
						if (err == fps.NACK_IS_NOT_USED) {
							callback(null, false);
						} else {
							callback(fps.decodeError(err));
						}
					});
				};
			};
			functionList.push(fun(i));
		}
		fps.getEnrollCount().then(function(count) {
			console.log('getEnrollCount count: ' + count);
			async.series(functionList,
				function(err, results) {
					if (err) {
						console.log('checkEnrolled list error: ' + fps.decodeError(err));
					} else {
						for (var i = 0; i < results.length; i++)
							console.log('[' + i + '] ' + (results[i] ? 'USED' : 'EMPTY'));
					}
				});
		}, function(err) {
			console.log('getEnrollCount error: ' + fps.decodeError(err));
		});
	} else if (key == '4') {
		fps.checkEnrolled(ID_TO_USE).then(function() {
			console.log('checkEnrolled: ' + ID_TO_USE + ' is enrolled!');
		}, function(err) {
			console.log('checkEnrolled error: ' + fps.decodeError(err));
		});
	} else if (key == '5') {
		fps.enrollStart(ID_TO_USE).then(function() {
			console.log('enrollStart: ' + ID_TO_USE + ' enroll started!');
		}, function(err) {
			console.log('enrollStart error: ' + fps.decodeError(err));
		});
	} else if (key == '6') {
		fps.enroll1().then(function() {
			console.log('enroll1 OK!');
		}, function(err) {
			console.log('enroll1 error: ' + fps.decodeError(err));
		});
	} else if (key == '7') {
		fps.enroll2().then(function() {
			console.log('enroll2 OK!');
		}, function(err) {
			console.log('enroll2 error: ' + fps.decodeError(err));
		});
	} else if (key == '8') {
		fps.enroll3().then(function() {
			console.log('enroll3 OK!');
		}, function(err) {
			console.log('enroll3 error: ' + fps.decodeError(err));
		});
	} else if (key == '9') {
		fps.isPressFinger().then(function() {
			console.log('isPressFinger: pressed!');
		}, function(err) {
			console.log('isPressFinger err: ' + fps.decodeError(err));
		});
	} else if (key == 'q') {
		fps.waitReleaseFinger(8000).then(function() {
			console.log('waitReleaseFinger: released!');
		}, function(err) {
			console.log('waitReleaseFinger err: ' + fps.decodeError(err));
		});
	} else if (key == 'w') {
		fps.waitFinger(8000).then(function() {
			console.log('waitFinger: pressed!');
		}, function(err) {
			console.log('waitFinger err: ' + fps.decodeError(err));
		});
	} else if (key == 'e') {
		fps.enroll(ID_TO_USE).then(function() {
			console.log('enroll: enrolled!');
		}, function(err) {
			console.log('enroll err: ' + fps.decodeError(err));
		});
	} else if (key == 'r') {
		fps.deleteID(ID_TO_USE).then(function() {
			console.log('deleteID: ' + ID_TO_USE + ' deleted!');
		}, function(err) {
			console.log('deleteID err: ' + fps.decodeError(err));
		});
	} else if (key == 't') {
		fps.deleteAll().then(function() {
			console.log('deleteAll: OK!');
		}, function(err) {
			console.log('deleteAll err: ' + fps.decodeError(err));
		});
	} else if (key == 'y') {
		fps.verify(ID_TO_USE).then(function() {
			console.log('verify: ' + ID_TO_USE + ' verified!');
		}, function(err) {
			console.log('verify err: ' + fps.decodeError(err));
		});
	} else if (key == 'u') {
		fps.captureFinger(0)
			.then(function() {
				return fps.identify();
			})
			.then(function(ID) {
				console.log("identify: ID = " + ID);
				rele3.writeSync(0);
				setTimeout(function() {
					rele3.writeSync(1);
				}, 1000);
			}, function(err) {
				console.log("identify err: " + fps.decodeError(err));
			});
	} else if (key == 'i') {
		var readStream = fs.createReadStream(TEMPLATE_PATH + '/template_' + ID_TO_USE + '.tpl');
		readStream.on('data', function(template) {
			fps.verifyTemplate(ID_TO_USE, template).then(function() {
				console.log('verifyTemplate: ' + ID_TO_USE + ' template verified!');
			}, function(err) {
				console.log('verifyTemplate err: ' + fps.decodeError(err));
			});
		});
	} else if (key == 'o') {
		var readStream = fs.createReadStream(TEMPLATE_PATH + '/template_' + ID_TO_USE + '.tpl');
		readStream.on('data', function(template) {
			fps.identifyTemplate(template).then(function(ID) {
				console.log('identifyTemplate: ID = ' + ID);
			}, function(err) {
				console.log('identifyTemplate err: ' + fps.decodeError(err));
			});
		});
	} else if (key == 'p') {
		fps.captureFinger().then(function() {
			console.log('captureFinger: OK!');
		}, function(err) {
			console.log('captureFinger err: ' + fps.decodeError(err));
		});
	} else if (key == 'a') {
		fps.makeTemplate().then(function(template) {
			TEMPLATE = new Buffer(template);
			console.log('makeTemplate: [' + template.length + '] ' + (new Buffer(template)).toString('hex'));
			var templateFile = TEMPLATE_PATH + '/template_custom' + '.tpl';
			fs.writeFile(templateFile, template, function(err) {
				if (err) {
					console.log('getTemplate err: ' + fps.decodeError(err));
				} else {
					console.log('getTemplate: template saved! [' + templateFile + ']');
				}
			});
		}, function(err) {
			console.log('makeTemplate err: ' + fps.decodeError(err));
		});
	} else if (key == 's') {
		fps.getImage().then(
			function(image) {
				console.log('getImage: [' + image.length + '] ' + (new Buffer(image)).toString('hex'));
				var imageFile = IMAGE_PATH + '/image' + '.tpl';
				fs.writeFile(imageFile, image, function(err) {
					if (err) {
						console.log('getImage err: ' + fps.decodeError(err));
					} else {
						console.log('getImage: image saved! [' + imageFile + ']');
					}
				});
			},
			function(rawImage) {
				console.log('getImage err: ' + fps.decodeError(err));
			}
		);
	} else if (key == 'd') {
		fps.getRawImage().then(
			function(rawImage) {
				console.log('getRawImage: [' + rawImage.length + '] ' + (new Buffer(rawImage)).toString('hex'));
				var imageFile = IMAGE_PATH + '/rawImage' + '.tpl';
				fs.writeFile(imageFile, rawImage, function(err) {
					if (err) {
						console.log('getRawImage err: ' + fps.decodeError(err));
					} else {
						console.log('getRawImage: image saved! [' + imageFile + ']');
					}
				});
			},
			function(err) {
				console.log('getRawImage err: ' + fps.decodeError(err));
			}
		);
	} else if (key == 'f') {
		fps.getTemplate(ID_TO_USE).then(
			function(template) {
				TEMPLATE = new Buffer(template);
				console.log('getTemplate: ID = ' + ID_TO_USE + ' [' + template.length + '] ' + (new Buffer(template)).toString('hex'));
				var templateFile = TEMPLATE_PATH + '/template_' + ID_TO_USE + '.tpl';
				fs.writeFile(templateFile, template, 'binary', function(err) {
					if (err) {
						console.log('getTemplate err: ' + fps.decodeError(err));
					} else {
						console.log('getTemplate: template saved! [' + templateFile + ']');
					}
				});
			},
			function(err) {
				console.log('getTemplate err: ' + fps.decodeError(err));
			}
		);
	} else if (key == 'g') {

		var readStream = fs.createReadStream(TEMPLATE_PATH + '/template_' + ID_TO_USE + '.tpl');
		readStream.on('data', function(template) {
			fps.setTemplate(ID_TO_USE, template).then(
				function() {
					console.log('setTemplate: ID = ' + ID_TO_USE + ' OK!');
				},
				function(err) {
					console.log('setTemplate err: ' + fps.decodeError(err));
				}
			);
		});
	} else if (key == 'z') {
		fps.init().then(
			function() {
				isInit = true;
				console.log('init: OK!');
				setTimeout(function() {
					console.log('init: OK1!');
					fps.ledONOFF(1);
					setTimeout(function() {
						fps.ledONOFF(0);
					}, 1000);
				}, 1000);
			},
			function(err) {
				console.log('init err: ' + fps.decodeError(err));
			}
		);
	} else if (key == 'b') {
		fps.changeBaudRate(9600).then(
			function() {
				console.log('changeBaudRate: OK!');
			},
			function(err) {
				console.log('changeBaudRate err: ' + fps.decodeError(err));
			}
		);
	} else if (key == 'h') {
		printHelp();
	}
});

fps.init().then(
	function() {
		isInit = true;
		console.log('init: OK!');
		console.log('firmware version: ' + fps.firmwareVersion);
		console.log('iso area max: ' + fps.isoAreaMaxSize);
		console.log('device serial number: ' + fps.deviceSerialNumber);
	},
	function(err) {
	console.log('init err: ' + fps.decodeError(err));
	}
);