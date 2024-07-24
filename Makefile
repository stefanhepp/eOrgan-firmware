

all: keyboard technics piston pedal toestud controller

.PHONY: keyboard technics piston pedal toestud controller

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

controller:
	cd MIDIController && platformio run

.PHONY: flash_keyboard flash_technics flash_piston_keyboard flash_piston_technics flash_pedal flash_toestud flash_controller

flash_keyboard:
	cd KeyboardEncoder


