#include <ESP32SPISlave.h>

#include <ESP32Lib.h>
#include <GfxWrapper.h>

// #include <Wire.h>

//This example acts like a websever. It can create an access point or join an existing WiFI network.
//All text that's entered in the served page will bis displayed on the connected VGA screen.
//You need to connect a VGA screen cable to the pins specified below.
//cc by-sa 4.0 license
//bitluni

#include <stdio.h>
#include <Ressources/Font8x8.h>

//pin configuration, change if you need to
const int hsyncPin = 32;
const int vsyncPin = 33;
const int red0pin = 25;
const int red1pin = 26;
const int green0pin = 12;
const int green1pin = 13;
const int blue0pin = 27;
const int blue1pin = 14;

const unsigned int dataPin[] = { 15, 2, 4, 5, 18, 19, 22, 23 };
//const int inter_clr = 22;
const int inter_dat = 21;
//The VGA Device
VGA6Bit vga;

ESP32SPISlave slave;

static constexpr uint32_t BUFFER_SIZE { 3 };
uint8_t spi_slave_tx_buf[BUFFER_SIZE];
uint8_t spi_slave_rx_buf[BUFFER_SIZE];


void set_buffer() {
    for (uint32_t i = 0; i < BUFFER_SIZE; i++) {
        spi_slave_tx_buf[i] = (0xFF - i) & 0xFF;
    }
    memset(spi_slave_rx_buf, 0, BUFFER_SIZE);
}

volatile int32_t pixel_address = 0;


///Received text will be displayed on the screen

///initialization
void setup()
{
	pinMode(inter_dat, INPUT);
	// pinMode(inter_clr, INPUT);
	 for (int pin = 0; pin <= 3; pin += 1) {
	     pinMode(pin, INPUT);
  	 }
	Serial.begin(115200);
	// Wire.begin(23, 2, 400000);
	// Wire.onReceive(receiveEvent);
	//slave.setDataMode(SPI_MODE0);
	//attachInterrupt(digitalPinToInterrupt(inter_dat), isr_dat, RISING);
	//attachInterrupt(digitalPinToInterrupt(inter_clr), ist_clr, CHANGE);
    //slave.begin(VSPI);
	//set_buffer();
	Serial2.begin(1000000, SERIAL_8N1, 16, 17);
	Mode myMode = vga.MODE640x480.custom(160, 120);
	//start vga on the specified pins
	
	//vga.init(myMode, redPin, greenPin, bluePin, hsyncPin, vsyncPin); // 3 bit
	vga.init(myMode, red0pin, red1pin, green0pin, green1pin, blue0pin, blue1pin, hsyncPin, vsyncPin);
	//make the background blue
	vga.clear(vga.RGBA(0, 0, 0));
	vga.backColor = vga.RGB(0, 0, 0);
	//select the font
	vga.setFont(Font8x8);
	//display some text header on the screen including the ip
	vga.clear(vga.RGBA(0, 0, 0));
	// vga.setCursor(0, 1);
	// vga.println("----------------------");
	// vga.println("bitluni's VGA Terminal");
	// vga.println("----------------------");
}

// void isr_dat() {
// 	 uint8_t data = 0;
//   	 for (int pin = 3; pin >= 0; pin -= 1) {
// 	     data = (data << 1) + digitalRead(dataPin[pin]);
//   	 }
// 	char buf[10];
// 	sprintf(buf, "%04x%02x",pixel_address, data);
// 	Serial.println(buf);
// 	// // printPixel(pixel_address, data);
// 	//Serial.println(pixel_address, DEC);

// 	pixel_address++;

// }

//0000: 21 80 0f 11 00 80 01 80     25 ed b0 21 00 35 11 00
//0010: 80 01 80 25 ed b0 21 80     5a 11 00 80 01 80 25 ed

void ist_clr() {
	pixel_address = 0;
}

void loop()
{
	// if (slave.remained() == 0) {
    //     slave.queue(spi_slave_rx_buf, spi_slave_tx_buf, BUFFER_SIZE);
    // }

    // while (slave.available()) {
	// 	int address = spi_slave_rx_buf[0] * 256 + spi_slave_rx_buf[1];
	// 	printPixel(address, spi_slave_rx_buf[2]);
    //     slave.pop();
    // }

	

	if (Serial2.available() > 0) {
		int bufSize = 3;
		byte pixel[bufSize];
		int pixelLen = Serial2.readBytes(pixel, bufSize);
		// char buf[10];
		// sprintf(buf, "%02x%02x%02x",pixel[0], pixel[1], pixel[2]);
		// Serial.println(buf);
		int address = pixel[0] * 256 + pixel[1];
		// sprintf(buf, "%04x%02x",address, pixel[2]);
		// Serial.println(buf);

		printPixel(address, pixel[2]);

	}
}


// void receiveEvent(int howMany)
// {
//   while(Wire.available() > 0) // loop through all but the last
//   {
// 	int bufSize = 3;
// 	byte pixel[bufSize];
// 	int pixelLen = Wire.readBytes(pixel, bufSize);
// 	int address = pixel[0] * 256 + pixel[1];
//     printPixel(address, pixel[2]);
//   }
// }

void printPixel(uint16_t address, uint8_t data)
{
	int color2 = data & 0xf;
    int color1 = (data >> 4);
	float yd = (address - 0x8000) / (160 / 2);
	float y = (float)floor(yd);
	float x = (float)((address - 0x8000) - (y * (160 / 2)));
	// Serial.println(x);
	// Serial.println(y);
	vga.dot(x*2, y, GetColor(color1));
	vga.dot(x*2+1, y, GetColor(color2));

} 

unsigned char GetColor(int color)
{
	switch (color)
	{
	case 0x0:
		return vga.RGB(0, 0, 0);
		break;
	case 0x1:
		return vga.RGB(0xff, 0x0, 0x0);
		break;
	case 0x2:
		return vga.RGB(0, 0x64, 0);
		break;
	case 0x3:
		return vga.RGB(0xf5, 0xf5, 0xdc);
		break;
	case 0x4:
		return vga.RGB(0, 0, 0x8b);
		break;
	case 0x5:
		return vga.RGB(0xff, 0x14, 0x93);
		break;
	case 0x6:
		return vga.RGB(0, 0, 0xff);
		break;
	case 0x7:
		return vga.RGB(0xd3, 0xd3, 0xd3);
		break;
	case 0x8:
		return vga.RGB(0xa9, 0xa9, 0xa9);
		break;
	case 0x9:
		return vga.RGB(0xff, 0xa5, 0);
		break;
	case 0xA:
		return vga.RGB(0, 0x80, 0);
		break;
	case 0xB:
		return vga.RGB(0xff, 0xff, 0);
		break;
	case 0xC:
		return vga.RGB(0, 0xff, 0xff);
		break;
	case 0xD:
		return vga.RGB(0xff, 0xc0, 0xcb);
		break;
	case 0xE:
		return vga.RGB(0x5f, 0x9e, 0xa0);
		break;
	case 0xF:
		return vga.RGB(0xff, 0xff, 0xff);
		break;
	default:
		return vga.RGB(0, 0, 0);
		break;
	}
	
}
