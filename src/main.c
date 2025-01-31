typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;

extern void ioPortOut(const uint16_t port, const uint8_t value);
extern const uint8_t ioPortIn(const uint8_t);

void clearScreen() {
	for (uint8_t row = 0; row < 25; row++) {
		for (uint8_t col = 0; col < 80; col++) {
			*((uint16_t*)0xb8000 + (row * 80) + col) = ' ' | 0x0F << 8;
		}
	}
	ioPortOut(0x3d4, 0x0f);
	ioPortOut(0x3d5, 0);
	ioPortOut(0x3d4, 0x0e);
	ioPortOut(0x3d5, 0);
}

char screenHistory[1024][80] = { {0} };
uint8_t printingRow = 0, printingCol = 0;
uint16_t screenHistoryRow = 0;

void updateCursorPosition() {
	uint16_t cursorPos = (printingRow * 80) + printingCol;
	ioPortOut(0x3d4, 0x0f);
	ioPortOut(0x3d5, (uint8_t)(cursorPos & 0xff));
	ioPortOut(0x3d4, 0x0e);
	ioPortOut(0x3d5, (uint8_t)(cursorPos >> 8));
}

void updateScreenFromHistory() {
	unsigned char attribute = 0x0F; // White text on black background

	uint16_t startingScreenHistoryRow = 0;
	uint8_t endingRow = 24;
	if (screenHistoryRow <= 24) {
		endingRow = screenHistoryRow;
	} else {
		startingScreenHistoryRow = screenHistoryRow - 24;
	}

	printingRow = endingRow;
	printingCol = 80;
	
	char printingChar;
	for (uint8_t row = 0; row <= endingRow; row++) {
		for (uint8_t col = 0; col < 80; col++) {
			printingChar = screenHistory[startingScreenHistoryRow + row][col];
			if (printingChar == 0) {
				printingCol = col;
				for(; col < 80; col++) {
					*((uint16_t*)0xb8000 + (row * 80) + col) = ' ' | 0x0F << 8;
				}
			}
			*((uint16_t*)0xb8000 + (row * 80) + col) = printingChar | attribute << 8;
		}
	}
}

void printChar(const char printingChar) {
	if (printingChar == 0)
		return;

	unsigned char attribute = 0x0F; // White text on black background

	if (printingChar == '\n') {
		printingRow++;
		screenHistoryRow++;
		printingCol = 0;
	} else if (printingChar == '\t') {
		for (uint8_t i = 0; i < 3; i++) {
			*((uint16_t*)0xb8000 + (printingRow * 80) + printingCol) = ' ' | attribute << 8;
			screenHistory[screenHistoryRow][printingCol] = ' ';
			printingCol++;
		}
	} else {
		*((uint16_t*)0xb8000 + (printingRow * 80) + printingCol) = printingChar | attribute << 8;
		screenHistory[screenHistoryRow][printingCol] = printingChar;
		printingCol++;
	}

	if (printingCol >= 80) {
		printingRow++;
		screenHistoryRow++;
		printingCol = printingCol % 80;
	}

	if (printingRow >= 25) {
		updateScreenFromHistory();
	}
		
	updateCursorPosition();
}

void print(const char* currentCharPtr) {	
	while (*currentCharPtr) {
		printChar(*currentCharPtr++);
	}
}

void printDecimal(uint8_t number) {
	//Max value is 256
	uint8_t numberChar[3] = {0};
	uint8_t pos = 0;
	if (number >= 100) {
		pos = 2;
	} else if (number >= 10) {
		pos = 1;
	}

	do {
		numberChar[pos] = '0' + number % 10;
		number /= 10;
		if (pos > 0)
			pos--;
	} while (number > 0);
	print(numberChar);
}

const char scancodeToAsciiTable[] = {
	0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 0, '=', 0,
	'\t', 'a', 'z', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', 0, '$', '\n',
	0, 'q', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm', '%', 0, 0, '*',
	'w', 'x', 'c', 'v', 'b', 'n', ',', ';', '/', '!', 0, 0,
	0, ' '
};

uint8_t startsWith(const char* str1Ptr, const char* str2Ptr) {
	while (*str1Ptr != 0 && *str2Ptr != 0) {
		if (*str1Ptr != *str2Ptr)
			return 0;

		str1Ptr++;
		str2Ptr++;
	}
	return 1;
}

char typingCommand[40] = { 0 };
uint8_t typingCommandCharPos = 0;

void executeCommand() {
	if (typingCommand[0] != 0) {
		printChar('\n');

		if (startsWith(typingCommand, "help")) {
			print("Available commands:\n");
			print("\thelp : Lists all available commands\n");
			print("\tping : Prints \"pong\" back\n");
			print("\treadme : Prints readme message\n");
			print("\ttest : Prints test message\n");
		
		} else if (startsWith(typingCommand, "ping")) {	
			print("pong !!!!\n");
		
		} else if (startsWith(typingCommand, "readme")) {
			print("Welcome on the first Pytom's OS\n");
			print("You're currently in protected mode\n");
			print("Type a command and press Enter\n");

		} else if (startsWith(typingCommand, "test")) {
			print("Printing tests:\n");
			print("Long string........................................................................................................................................................................\n");
			print("\tTabulation\n");
			print("\t\tDouble tabulation\n");
			print("Print decimal test (i: 0 -> 14):\n");
			for (uint8_t i = 0; i < 15; i++) {
				print("\ti: ");
				printDecimal(i);
				print("\n");
			}
			print("Printing tests finished\n");

		} else {
			print("Invalid command, type \"help\" to list all available commands\n");
		}
	}

	for (uint8_t i = 0; i < 80; i++)
		typingCommand[i] = 0;
	typingCommandCharPos = 0;

	print("\n> ");
}

void main() {
	clearScreen();
	print("Welcome on the first Pytom's OS\n");
	print("You're currently in protected mode\n");
	print("Type a command and press Enter\n");
	print ("> ");

	uint8_t value;
	char newChar;
	while (1) {
		value = ioPortIn(0x64);
		if (value & 1) { // 1 is for 00000001 binary mask
			uint8_t scancode = ioPortIn(0x60);
			if (scancode == 28) { // Enter key
				executeCommand();
			} else if (scancode <= 57) { // Normal keys
				//printChar('C');
				//printDecimal(scancode);
				//printChar(',');
				//printDecimal(scancodeToAsciiTable[scancode]);
				//printChar(':');
				newChar = scancodeToAsciiTable[scancode];
				if (newChar != 0 && typingCommandCharPos < (40-1)) {
					printChar(newChar);
					typingCommand[typingCommandCharPos++] = newChar;
				}
			}
			// It doesn't work well, improve it and add other keys
			// Maybe it should be disabled for the moment
			/* else if (scancode == 72) { // Up arrow key
				if (screenHistoryRow > 0) {
					if (printingRow > 0) {
						printingRow--;
						screenHistoryRow--;
					} else {
						updateScreenFromHistory();
					}
					updateCursorPosition();
				}
			}*/
		}
	}
}
