FLAGS = -mmcu=attiny10 -DF_CPU=8000000UL -Os -std=c99

all: attiny10_bee_blink.hex

%.hex: %.elf
	avr-objcopy -O ihex $< $@

clean:
	rm -f *.elf *.hex

%.elf: %.c
	avr-gcc $(FLAGS) $^ -o $@
	avr-size -C --mcu=attiny10 $@
