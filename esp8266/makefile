update:
	nix-shell --pure --run "arduino-cli lib upgrade"
	cd arduino/user/libraries/; \
	rm -rf PN532*; \
	rm -rf NDEF; \
	git clone https://github.com/elechouse/PN532.git; \
	mv PN532/ PN532-temp/; \
	cd PN532-temp/; \
	rm README.md; \
	mv * ../; \
	cd ../; \
	rm -rf PN532-temp/; \
	cd ../../../../

install:
	nix-shell --pure --run "arduino-cli core update-index --config-file arduino-cli.yaml"
	nix-shell --pure --run "arduino-cli core install esp8266:esp8266"
	nix-shell --pure --run "arduino-cli lib install \"Firebase Arduino Client Library for ESP8266 and ESP32\""
	nix-shell --pure --run "arduino-cli lib install \"ESP8266 Firebase\""
	nix-shell --pure --run "arduino-cli lib install NTPClient"
	nix-shell --pure --run "arduino-cli lib install ArduinoJson"
	nix-shell --pure --run "arduino-cli lib install CustomJWT"
	nix-shell --pure --run "arduino-cli lib install Crypto"
	cd arduino/user/libraries/; \
	git clone https://github.com/elechouse/PN532.git; \
	mv PN532/ PN532-temp/; \
	cd PN532-temp/; \
	rm README.md; \
	mv * ../; \
	cd ../; \
	rm -rf PN532-temp/; \
	cd ../../../

compile:
	nix-shell --pure --run "arduino-cli compile --fqbn esp8266:esp8266:nodemcuv2  --libraries arduino/user/libraries/ --output-dir build/"

upload:
	nix-shell --pure --run "arduino-cli upload -p /dev/ttyUSB0 --fqbn esp8266:esp8266:nodemcuv2"

monitor:
	nix-shell --pure --run "arduino-cli monitor -p /dev/ttyUSB0 --fqbn esp8266:esp8266:nodemcuv2"
