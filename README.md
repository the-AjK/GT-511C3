# GT511C3 

Copyright © 2015, [Alberto Garbui (aka JK)](mailto:alberto.garbui@gmail.com)

Find me on:
[![alt text][1.1]][1]
[![alt text][2.1]][2]
[![alt text][6.1]][6]
[![alt text][7.1]][7]

[1.1]: http://i.imgur.com/tXSoThF.png (twitter icon with padding)
[2.1]: http://i.imgur.com/P3YfQoD.png (facebook icon with padding)
[6.1]: http://i.imgur.com/0o48UoR.png (github icon with padding)
[7.1]: http://i.imgur.com/VlgBKQ9.png (google plus icon without padding)

[1]: https://twitter.com/albertoajk
[2]: https://www.facebook.com/ajk.alberto
[6]: https://github.com/the-AjK
[7]: https://plus.google.com/u/0/112632073896245208828

## Description

**GT511C3** is a node-js module to control the GT-511C3 fingerprint sensor (also know as **SEN-11792**) through serial port.

<img src="https://raw.githubusercontent.com/the-AjK/GT-511C3/master/GT511-C3.jpg">

Check out the fingerprint sensor datasheet [here](https://github.com/the-AjK/GT-511C3/blob/master/GT-511C3_datasheet_V1_1.pdf) or [download it](https://github.com/the-AjK/GT-511C3/raw/master/GT-511C3_datasheet_V1_1.pdf).

## Repository

[GitHub](https://github.com/the-AjK/GT-511C3)

[NPM](https://www.npmjs.com/package/gt511c3)

## Installation

    $ npm install gt511c3

## Basic usage

```js
var GT511C3 = require('gt511c3');

var fps = new GT511C3('/dev/ttyS1');

fps.init().then(
	function() {
		//init ok
		console.log('firmware version: ' + fps.firmwareVersion);
    	console.log('iso area max: ' + fps.isoAreaMaxSize);
    	console.log('device serial number: ' + fps.deviceSerialNumber);
	},
	function(err) {
		console.log('init err: ' + err);
	}
);
```

## API

### Class GT511C3

  * GT511C3(port [, options]) - constructor

##### GT511C3(port [, options])
Returns a new GT511C3 object that can be used to control the fingerprint sensor.
- port - The serialport name to use.
- options - An object with optional settings

The available optional settings are:
  * baudrate [9600, 19200, 38400, 57600, 115200] - number value of the serialport speed (default 9600 baud)
  * debug - boolean flag, setting it true will make the output more verbose

Example:
```js
var fps = new GT511C3('/dev/ttyS1', {
	baudrate: 115200
	debug: true
});
```

### Global variables and error codes
Device's informations (available after init() or open(EXTRA_INFO) command)
  * firmwareVersion - firmware version of the fingerprint module
  * isoAreaMaxSize - maximum size of ISO CD image
  * deviceSerialNumber - fingerprint module unique serial number

Global variables
  * NO_EXTRA_INFO - to use with open() method
  * EXTRA_INFO - to use with open() method to get full device's informations
  * LED_OFF - to use with ledONOFF() method
  * LED_ON - to use with ledONOFF() method
  * NOT_BEST_IMAGE - to use with captureFinger() method during enrollment procedure
  * BEST_IMAGE - to use with captureFinger() method during identification and verification procedure

Error codes
  * NACK_TIMEOUT
  * NACK_INVALID_BAUDRATE
  * NACK_INVALID_POS
  * NACK_IS_NOT_USED
  * NACK_IS_ALREADY_USED
  * NACK_COMM_ERR
  * NACK_VERIFY_FAILED
  * NACK_IDENTIFY_FAILED
  * NACK_DB_IS_FULL
  * NACK_DB_IS_EMPTY
  * NACK_TURN_ERR
  * NACK_BAD_FINGER
  * NACK_ENROLL_FAILED
  * NACK_IS_NOT_SUPPORTED
  * NACK_DEV_ERR
  * NACK_CAPTURE_CANCELED
  * NACK_INVALID_PARAM
  * NACK_FINGER_IS_NOT_PRESSED

### Methods

  * init()
  * open(extra_info) 
  * close() 
  * closePort()
  * usbInternalCheck()
  * ledONOFF(led_state) 
  * changeBaudRate(baudrate)
  * getEnrollCount()
  * checkEnrolled(ID)
  * enrollStart(ID)
  * enroll1()
  * enroll2()
  * enroll3()
  * isPressFinger()
  * waitFinger(timeout)
  * waitReleaseFinger(timeout)
  * enroll(ID)
  * deleteID(ID)
  * deleteAll()
  * verify(ID)
  * identify()
  * verifyTemplate(ID, template)
  * identifyTemplate(template)
  * captureFinger(image_type)
  * makeTemplate()
  * getImage()
  * getRawImage()
  * getTemplate(ID)
  * setTemplate(ID, template)
  * decodeError(errorCode)

##### init()
Open the serialport and initialize the fingerprint module. Returns a promise.
After the init procedure global variables such as **firmwareVersion**, **isoAreaMaxSize** and **deviceSerialNumber** are available.

Example:
```js
fps.init().then(
  function() {
    //init ok
    console.log('firmware version: ' + fps.firmwareVersion);
    console.log('iso area max: ' + fps.isoAreaMaxSize);
    console.log('device serial number: ' + fps.deviceSerialNumber);
  },
  function(err) {
    console.log('init err: ' + err);
  }
);
```

##### open(extra_info)
Send the **OPEN** command to the fingerprint module. Returns a promise.
- extra_info - specify whether to request extra informations or not. Global variables such as **EXTRA_INFO** or **NO_EXTRA_INFO** may be used.

##### close()
Send the **CLOSE** command to the fingerprint module and close the serial port. Returns a promise.

##### closePort()
Close the serial port. Returns a promise.

##### usbInternalCheck()
Send the **USB_INTERNAL_CHECK** command to the fingerprint module. Returns a promise.

##### ledONOFF(led_state)
Send the **CMOS_LED** command to the fingerprint module. Returns a promise.
- led_state - specify whether to switch the onboard leds on or off. Global variables such as **LED_OFF** or **LED_ON** may be used.

Example:
```js
fps.ledONOFF(fps.LED_ON);
```

##### changeBaudRate(baudrate)
Send the **CHANGE_BAUDRATE** command to the fingerprint module to change the module baudrate, if the command is acknowledged, the serialport baudrate will be updated as well. Returns a promise.
- baudrate - [9600, 19200, 38400, 57600, 115200] - number value of the serialport speed

##### getEnrollCount()
Send the **GET_ENROLL_COUNT** command to the fingerprint module. Returns a promise with the number of enrolled fingerprints in case of success.

Example:
```js
fps.getEnrollCount().then(function(count) {
  console.log("Enrolled fingerprints count: " + count);
});
```

##### checkEnrolled(ID)
Send the **CHECK_ENROLLED** command to the fingerprint module to check if a specified ID is enrolled or not. Returns a promise.
- ID - specify the fingerprint's ID to check [0..199]

##### enrollStart(ID)
Send the **ENROLL_START** command to the fingerprint module to start the enrollment procedure for a specified ID. Returns a promise.
- ID - specify the fingerprint's ID to enroll [0..199]

(note: the onboard leds must be switched ON before sending this command)

##### enroll1()
Send the **ENROLL_1** command to the fingerprint module to execute the first step of the enrollment procedure. Returns a promise.

(note: the onboard leds must be switched ON before sending this command)

##### enroll2()
Send the **ENROLL_2** command to the fingerprint module to execute the second step of the enrollment procedure. Returns a promise.

(note: the onboard leds must be switched ON before sending this command)

##### enroll3()
Send the **ENROLL_3** command to the fingerprint module to execute the third step of the enrollment procedure. Returns a promise.

(note: the onboard leds must be switched ON before sending this command)

##### isPressFinger()
Send the **IS_PRESS_FINGER** command to the fingerprint module to check if a finger is correctly placed on the fingerprint sensor or not. Returns a promise.

(note: the onboard leds must be switched ON before sending this command)

Example:
```js
fps.isPressFinger().then(function() {
  console.log('isPressFinger: pressed!');
}, function(err) {
  console.log('isPressFinger err: ' + fps.decodeError(err));
});
```

##### waitFinger(timeout)
Wait until a finger is correctly placed on the fingerprint sensor. Returns a promise.
- timeout - specify how many ms (milliseconds) to wait before firing a timeout event and reject the promise.

(note: the onboard leds must be switched ON before sending this command)

##### waitReleaseFinger(timeout)
Wait until a finger is removed from the fingerprint sensor. Returns a promise.
- timeout - specify how many ms (milliseconds) to wait before firing a timeout event and reject the promise. 

(note: the onboard leds must be switched ON before sending this command)

##### enroll(ID)
Execute a full enrollment procedure for the specified ID. Returns a promise.
- ID - specify the fingerprint's ID to enroll [0..199]

(note: the onboard leds must be switched ON before sending this command)

##### deleteID(ID)
Send the **DELETE_ID** command to the fingerprint module to delete a specified ID. Returns a promise.
- ID - specify the fingerprint's ID to delete [0..199]

##### deleteAll()
Send the **DELETE_ALL** command to the fingerprint module to delete all the 200 IDs. Returns a promise.

##### verify(ID)
Send the **VERIFY** command to the fingerprint module to verify if the finger placed on the fingerprint sensor match the specified ID. Returns a promise.
- ID - specify the fingerprint's ID to verify [0..199] 

(note: the onboard leds must be switched ON before sending this command)

Example:
```js
var id = 5;
fps.verify(id).then(function() {
  console.log('verify: ' + id + ' verified!');
}, function(err) {
  console.log('verify err: ' + fps.decodeError(err));
});
```

##### identify()
Send the **IDENTIFY** command to the fingerprint module to identify the finger placed on the fingerprint sensor. Returns a promise, in case of success returns the identified ID.

(note: the onboard leds must be switched ON and a captureFinger() method must be called before sending this command)

Example:
```js
fps.identify().then(function(ID) {
  console.log("identify: ID = " + ID);
}, function(err) {
  console.log("identify err: " + fps.decodeError(err));
});
```

##### verifyTemplate(ID, template)
Send the **VERIFY_TEMPLATE** command to the fingerprint module to verify the supplied template match the specified ID. Returns a promise. 
- ID - specify the fingerprint's ID to check [0..199]
- template - 498bytes template to verify (Buffer)

##### identifyTemplate(template)
Send the **IDENTIFY_TEMPLATE** command to the fingerprint module to identify the supplied template. Returns a promise, in case of success returns the matched ID.
- template - 498bytes template to identify (Buffer)

##### captureFinger(image_type)
Send the **CAPTURE_FINGER** command to the fingerprint module. Returns a promise.
- image_type - specify whether to capture the fingerprint at high resolution or not. Global variables such as **BEST_IMAGE** or **NOT_BEST_IMAGE** may be used.

##### makeTemplate()
Send the **MAKE_TEMPLATE** command to the fingerprint module to retrieve the template of the finger placed on the fingerprint sensor. Returns a promise, in case of success returns a 498bytes template Buffer.

##### getImage()
Send the **GET_IMAGE** command to the fingerprint module to retrieve the image of the finger placed on the fingerprint sensor. Returns a promise, in case of success returns a 52116bytes image Buffer.

(note: the onboard leds must be switched ON before sending this command)

##### getRawImage()
Send the **GET_RAW_IMAGE** command to the fingerprint module to retrieve the raw image of the finger placed on the fingerprint sensor. Returns a promise, in case of success returns a 19200bytes image Buffer.

(note: the onboard leds must be switched ON before sending this command)

##### getTemplate(ID)
Send the **GET_TEMPLATE** command to the fingerprint module to retrieve the template that match the specified ID. Returns a promise, in case of success returns a 498bytes template Buffer.
- ID - specify the fingerprint's ID [0..199]

##### setTemplate(ID, template)
Send the **SET_TEMPLATE** command to the fingerprint module to set the template for a specified fingerprint ID. Returns a promise.
- ID - specify the fingerprint's ID [0..199]
- template - 498bytes template (Buffer)

##### decodeError(errorCode)
Returns the string representation of the provided error code.

Example:
```js
console.log(fps.decodeError(fps.NACK_DB_IS_FULL)); //print 'the database is full'
```

## License
GT511C3 - Fingerprint Scanner module
(BSD-3 license)

Copyright © 2015 Alberto Garbui <alberto.garbui@gmail.com>

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

-Redistributions of source code must retain the above copyright notice, this list 
 of conditions and the following disclaimer.
-Redistributions in binary form must reproduce the above copyright notice, this list 
 of conditions and the following disclaimer in the documentation and/or other 
 materials provided with the distribution.
-Neither the name of GT511C3 - Fingerprint Scanner module nor the names of its contributors may be 
 used to endorse or promote products derived from this software without specific prior 
 written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
