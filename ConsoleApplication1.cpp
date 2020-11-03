#include <iostream> 
#define N 38 // Packet size 
using namespace std;

// The incoming message and its pointer 
unsigned char buf[N], * ptr;

// Global variables 
unsigned int disableEKF = 0, actionFlags = 0, configFlags = 0, inverseFlags = 0, reserved2 = 0, nomPPDist = 0, nomZero = 0, nomSens = 0, rmsTemp = 0, tempUpdate = 0;

// Bitfield variables 
unsigned int pulsesPerM = 0, useSerWt = 0, invDir = 0, invGyro = 0, clTab = 0, clCalib = 0, nomTacho = 0, nomGyro = 0, setTemp = 0, dir = 0;

// Checksum 
unsigned char CK_A = 0, CK_B = 0;
int i0;

int ascii2Dec(int character) {
	// Converting ASCII code to HEX 
	if (character >= 48 && character <= 57) return character - 48;
	else if (character >= 65 && character <= 70) return character - 55;
	else if (character >= 97 && character <= 102) return character - 87;
	else return 0;
}

int ascii2DecArr(unsigned char arr[], int length) {
	int j;
	int result = 0;
	for (j = 0; j < length; j++) {
		result += ascii2Dec(arr[j]) * pow(16, length - j - 1);
	} return result;
}

int main() {
	while (1) {
		// Reading the incoming buffer
		printf("Enter the incoming message: ");
		scanf_s("%s", buf, N + 1);

		// Evaluating disableEKF field 
		ptr = buf + 6;
		disableEKF = ascii2DecArr(ptr, 2);
		ptr += 2;

		// Evaluating actionFlags bitfield 
		actionFlags = ascii2DecArr(ptr, 2);
		ptr += 2;
		// Skipping the first bit 
		clTab = (actionFlags & 0b10) >> 1;
		clCalib = (actionFlags & 0b100) >> 2;
		// Skipping the third bit 
		nomTacho = (actionFlags & 0b10000) >> 4;
		nomGyro = (actionFlags & 0b100000) >> 5;
		setTemp = (actionFlags & 0b1000000) >> 6;
		dir = (actionFlags & 0b10000000) >> 7;

		// Evaluating configFlags bitfield 
		configFlags = ascii2DecArr(ptr, 2);
		ptr += 2;
		pulsesPerM = (configFlags & 1);
		useSerWt = (configFlags & 0b10) >> 1;

		// Evaluating inverseFlags bitfield 
		inverseFlags = ascii2DecArr(ptr, 2);
		ptr += 2;
		invDir = (inverseFlags & 1);
		invGyro = (inverseFlags & 0b10) >> 1;

		// Skipping 4 of reserved bytes 
		ptr += 8;

		// Evaluating nomPPDist field 
		nomPPDist = ascii2DecArr(ptr, 4);
		ptr += 4;

		// Evaluating nomZero field 
		nomZero = ascii2DecArr(ptr, 4);
		ptr += 4;

		// Evaluating nomSens field 
		nomSens = ascii2DecArr(ptr, 2);
		ptr += 2;

		// Evaluating rmsTemp field 
		rmsTemp = ascii2DecArr(ptr, 2);
		ptr += 2;

		// Evaluating tempUpdate field 
		tempUpdate = ascii2DecArr(ptr, 4);

		// Calculating the checksum 
		for (i0 = 0; i0 < N; i0++) {
			CK_A = CK_A + buf[i0];
			CK_B = CK_B + CK_A;
		} CK_A = (CK_A & 0xFF);
		CK_B = (CK_B & 0xFF);

		// Printing the values 
		printf("\nMap:\n");
		printf("disableEKF: %u\n", disableEKF);
		printf("clTab: %u\n", clTab);
		printf("clCalib: %u\n", clCalib);
		printf("nomTacho: %u\n", nomTacho);
		printf("nomGyro: %u\n", nomGyro);
		printf("setTemp: %u\n", setTemp);
		printf("dir: %u\n", dir);
		printf("pulsesPerM: %u\n", pulsesPerM);
		printf("useSerWt: %u\n", useSerWt);
		printf("invDir: %u\n", invDir);
		printf("invGyro: %u\n", invGyro);
		printf("reserved2: %u\n", reserved2);
		if (pulsesPerM) printf("nomPPDist: %u m\n", nomPPDist);
		else printf("nomPPDist: %u km\n", nomPPDist);
		printf("nomZero: %u mV\n", nomZero);
		printf("nomSens: %u mV/(deg/s)\n", nomSens);
		printf("rmsTemp: %u mV\n", rmsTemp);
		printf("tempUpdate: %u s\n", tempUpdate);
		printf("Checksum values: %u %u\n\n", CK_A, CK_B);

		printf("UBX CFG-EKF Packet: {0x06,0x12,16,%u,%u%u%u%u%u%u,%u%u,%u%u,%u,%u,%u,%u,%u,%u,%u,%u}\n\n", disableEKF, clTab, clCalib, nomTacho, nomGyro, setTemp, dir, useSerWt, pulsesPerM, invGyro, invDir, reserved2, nomPPDist, nomZero, nomSens, rmsTemp, tempUpdate, CK_A, CK_B);
	} return 0;
}