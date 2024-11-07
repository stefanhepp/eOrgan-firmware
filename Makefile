

all: keyboard technics piston pedal toestud controller

.PHONY: led keyboard technics piston pedal toestud mainpanel controller

led:
	cd LEDController && platformio run

keyboard:
	cd KeyboardEncoder && platformio run

technics:
	cd TechnicsEncoder && platformio run

piston:
	cd PistonEncoder && platformio run -e PistonTechnics
	cd PistonEncoder && platformio run -e PistonKeyboards

pedal:
	cd PedalEncoder && platformio run

toestud:
	cd ToeStudEncoder && platformio run

mainpanel:
	cd MainPanelEncoder && platformio run

controller:
	cd MIDIController && platformio run

.PHONY: flash_keyboard flash_technics flash_piston_keyboard flash_piston_technics flash_pedal flash_toestud flash_controller flash_mainpanel

update:
	cd LEDController && platformio pkg update
	cd KeyboardEncoder && platformio pkg update
	cd TechnicsEncoder && platformio pkg update
	cd PistonEncoder && platformio pkg update
	cd PedalEncoder && platformio pkg update
	cd ToeStudEncoder && platformio pkg update
	cd MainPanelEncoder && platformio pkg update
	cd MIDIController && platformio pgk update

flash_keyboard:
	cd KeyboardEncoder && platformio run -t upload


