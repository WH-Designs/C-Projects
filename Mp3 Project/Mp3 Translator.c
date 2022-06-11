/* CS360 Lab 4: C */
/* Wyatt Haak */

//include statements
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//global variables
FILE * fp;
int binaryArray[32];

//function to determine whether or not the file is Layer 3 MPEG
int determineMPEGLayer(int binaryArray[]){

	if((binaryArray[12] == 1) && (binaryArray[13] == 0) && (binaryArray[14] == 1)){

		return 1;

	}

	return 0;
}

//Convert each byte into an 8-Bit binary number then combine them into a 32-bit binary number
void byteToBinary(unsigned char *data, int syncLocation, unsigned int nextByte){

	//intialize local variables
	int oneByteArray[8];
	int b = 0;

	for(int j = syncLocation; j < syncLocation + 4; j++){
		
		nextByte = data[j];

		//convert byte into 8-bit binary
		for(int l = 0; l < 8; l++){
			
			oneByteArray[l] = nextByte % 2;
			nextByte = nextByte / 2;
			
		}
		
		//combine 8-bit binary numbers into a 32-bit binary number
		for(int a = 7; a >=0; a--){
			if(binaryArray[b] == 0){
				binaryArray[b] = oneByteArray[a];
				b++;
				
			}
		}
	}
}

//initialize the file
int initialize(int argc, char ** argv){

	// Open the file given on the command line
	if( argc != 2 )
	{
		printf( "Usage: %s filename.mp3\n", argv[0] );
		return(EXIT_FAILURE);
	}
	
	fp = fopen(argv[1], "rb");
	if( fp == NULL )
	{
		printf( "Can't open file %s\n", argv[1] );
		return(EXIT_FAILURE);
	}

}

//read in the file
void readFile(){

	// How many bytes are there in the file?  If you know the OS you're
	// on you can use a system API call to find out.  Here we use ANSI standard
	// function calls.
	long size = 0;
	fseek( fp, 0, SEEK_END );		// go to 0 bytes from the end
	size = ftell(fp);				// how far from the beginning?
	rewind(fp);						// go back to the beginning
	
	long largestSize = 10485760;

	if( size < 1 || size > largestSize )
	{
		printf("File size is not within the allowed range\n"); 
		fclose(fp);				// close and free the file
		exit(EXIT_SUCCESS);		// or return 0;
	}
	
	float fileSize = size/(largestSize + 6.0);

	// Allocate memory on the heap for a copy of the file
	unsigned char * data = (unsigned char *)malloc(size);

	// Read it into our block of memory
	size_t bytesRead = fread( data, sizeof(unsigned char), size, fp );
	if( bytesRead != size )
	{
		printf( "Error reading file. Unexpected number of bytes read: %I64d\n",bytesRead );
		fclose(fp);				// close and free the file
		exit(EXIT_SUCCESS);		// or return 0;
	}

	//Varibles
	unsigned int nextByte;
	int syncLocation;
	int frequency = 0;
	int bitRate = 0;



	//Find the sync word location in the data array
	for(int i = 0; i < bytesRead; i++){
		
		nextByte = data[i];
		nextByte >>= 4;

		if((data[i] == 255) && (nextByte == 15)){

			syncLocation = i;

			break;

		}

	}



	//turn each byte from the data into a binary given the data, sync word location, and the next byte number
	//then take those 8-bit binary and combine them into a 32-bit binary number
	byteToBinary(data, syncLocation, nextByte);

	printf("\n\n");

	/*
	Determine all of the attributes of the file:
		MPEG Layer
		Copyright
		Orginiality
		Frequency
		Bit Rate
	*/
	if(determineMPEGLayer(binaryArray) == 1){

		//print statement that prints out the file size in MB form
		printf( "File size: %.02f MB\n\n", fileSize );

		//checking if it is copyright
		if(binaryArray[28] == 1){

			//print statement that prints out if the MP3 is copyrighted
			printf("It is copyrighted\n\n");

		}else{

			//print statement that prints out if the MP3 is not copyrighted
			printf("It is not copyrighted\n\n");

		}



		//checking if it is an original 
		if(binaryArray[29] == 1){
			
			//print statement that prints out if the MP3 is an original version
			printf("It is an original\n\n");

		}else{
			
			//print statement that prints out if the MP3 is a copyed version
			printf("It is a copy\n\n");

		}



		//determine the frequency
		if((binaryArray[21] == 0) && (binaryArray[22] == 0)){
			frequency = 44100;
		}else{
			frequency = 0;
		}

		//print statement to print out the frequency converted into kHz form
		printf("Frequency: %.1f kHz\n\n", frequency / 1000.0);



		//determine the bitrate of file
		if((binaryArray[16] == 1) && (binaryArray[18] == 1)){
			
			bitRate = 160;
		
		}else if((binaryArray[16] == 1) && (binaryArray[19] == 1)){
			
			bitRate = 144;
		
		}else if((binaryArray[17] == 1) && (binaryArray[18] == 1) && (binaryArray[19] == 1)){
			
			bitRate = 112;
		
		}else if((binaryArray[17] == 1) && (binaryArray[18] == 1)){
			
			bitRate = 96;
		
		}else if((binaryArray[17] == 1) && (binaryArray[19] == 1)){
		
			bitRate = 80;
		
		}else if((binaryArray[17] == 1)){
		
			bitRate = 64;
		
		}else if((binaryArray[18] == 1) && (binaryArray[19] == 1)){
		
			bitRate = 48;
		
		}else if((binaryArray[18] == 1)){
		
			bitRate = 32;
		
		}else if((binaryArray[16] == 1)){
		
			bitRate = 128;
		
		}else{

			//print statement if the bit rate is unknown
			printf("Unknown bit rate\n\n");

		}

		//print out the bit rate
		printf("The bit rate is: %i kbps\n\n", bitRate);


	}else{

		//print statement if the MP3 file is not Layer 3
		printf("It is not a Layer 3 MPEG file\n\n");

	}

	//memory dump
	free(data);	
	
}


//Main function
int main( int argc, char ** argv )
{
	
	initialize(argc, argv);
	
	readFile();

}