#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "datastructures.h"
#include "dat.h"


void show_header(unsigned char* header)
{
int i;
    for(i=0;i<0x1C2;i++)
    {
    printf("%x ",header[i]);
        if(i%32==31)putchar('\n');
    }
}


int ChecksumProcessByte(unsigned int Checksum,unsigned char Byte){
unsigned int ChecksumHigherBits=Checksum&0xFFFFFF00;
unsigned char ChecksumLowerBits=(char)(Checksum&0xFF);
ChecksumLowerBits^=Byte;
Checksum=ChecksumHigherBits|(int)ChecksumLowerBits;
return (Checksum<<11)|(Checksum>>21);
}
char CountRepeatedBytes(uint8_t* Bytes,int Pos,int Size){
char FirstChar=Bytes[Pos];
char RepeatedBytes=1;
Pos++;
while(Pos<Size&&Bytes[Pos]==FirstChar&&RepeatedBytes<125)
{
Pos++;
RepeatedBytes++;
}
return RepeatedBytes;
}
char CountDifferingBytes(uint8_t* Bytes,int Pos,int Size){
    if(Pos>=Size-2)return 1;
char LastChar=Bytes[Pos];
char DifferingBytes=0;
Pos++;
    while(Pos<Size&&Bytes[Pos]!=LastChar&&DifferingBytes<125)
    {
    LastChar=Bytes[Pos];
    Pos++;
    DifferingBytes++;
    }
return DifferingBytes;
}

int WriteSimpleBitmap(Image* Bitmap,DynamicBuffer* GraphicData){
int StartAddress=GraphicData->Pos;
int Row;
for(Row=0;Row<Bitmap->Height;Row++)
{
WriteBuffer(Bitmap->Data[Row],Bitmap->Width,GraphicData);
};
return StartAddress;
}
int WriteCompactBitmap(Image* Bitmap,DynamicBuffer* GraphicData){
int StartAddress=GraphicData->Pos;
DynamicBuffer* ScanBuffer=CreateBuffer(64);
int Row;
int RowOffsetEnd=Bitmap->Height*2;
    for(Row=0;Row<Bitmap->Height;Row++)
    {
    unsigned char XOffset=0;
    unsigned char Length=0;
    int RowDataStartOffset=ScanBuffer->Pos+RowOffsetEnd;
    unsigned char LastOne=0;
        while(LastOne==0)
        {
        //Find XOffset
        Length=0;
        while(XOffset<255&&XOffset<Bitmap->Width&&Bitmap->Data[Row][XOffset]==0)XOffset++;

            if(XOffset==Bitmap->Width)
            {
            XOffset=0;
            //Need to set last one flag
            Length|=0x81;
            }
            else
            {
                    //Find Length
                    while(Length<126&&XOffset+Length<Bitmap->Width&&Bitmap->Data[Row][XOffset+Length]!=0)Length++;

                    //Check if this is the last one
                    if(XOffset+Length<Bitmap->Width)
                    {
                    //Check to see if there are any other non-transparent pixels
                    int i=XOffset+Length;
                    while(i<Bitmap->Width&&Bitmap->Data[Row][i]==0)i++;
                    //If at end of row, there are no more non transparent pixels
                    if(i>=Bitmap->Width)Length|=0x80;
                    }
                    else
                    {
                    //printf("Scan ends at end of row\n");
                    //At end of scanline, must be last one
                    Length|=0x80;
                    }
            }

        //Write scan element to buffer
        WriteBuffer(&Length,1,ScanBuffer);
        WriteBuffer(&XOffset,1,ScanBuffer);
        int NumToWrite=Length&0x7F;
        WriteBuffer(Bitmap->Data[Row]+XOffset,NumToWrite,ScanBuffer);
        XOffset+=NumToWrite;
        LastOne=Length&0x80;
        }
    WriteBuffer((unsigned char*)&RowDataStartOffset,2,GraphicData);
    };
int ScanDataSize=ScanBuffer->Pos;
unsigned char* ScanData=FreeBuffer(ScanBuffer);
WriteBuffer(ScanData,ScanDataSize,GraphicData);
free(ScanData);
return StartAddress;
}


#define HEADER_SIZE 21
ObjectFile* LoadDat(char* filename)//TODO-check that the file is actually a ride
{
int i;
ObjectFile* object=malloc(sizeof(ObjectFile));

FILE* file=fopen(filename,"r");
    if(file==NULL)
    {
    printf("Cannot open file\n");
    return NULL;
    }

//Get file size
fseek(file,0,SEEK_END);
int fileSize=ftell(file);
fseek(file,SEEK_SET,0);

//Allocate object
object=malloc(sizeof(ObjectFile));

//Read data into temp array
char* encodedBytes=malloc(fileSize);
fread(encodedBytes,fileSize,1,file);

fclose(file);


//Allocate buffer to store decoded data
DynamicBuffer* decodedBytes=CreateBuffer(512);

//Check the header to see if the remaining data is compressed
if(encodedBytes[0x10])
    {
    //Decode RLE data
    int pos=HEADER_SIZE;
    while(pos<fileSize)
    {
            if(encodedBytes[pos]>=0)//Copy bytes
            {
            short numToCopy=(short)(encodedBytes[pos])+1;
            pos+=1;
            WriteBuffer((unsigned char*)(encodedBytes+pos),numToCopy,decodedBytes);
            pos+=numToCopy;
            }
            else//Repeat bytes
            {
            char numToRepeat=1-encodedBytes[pos];
            pos++;
            for(;numToRepeat>0;numToRepeat--)WriteBuffer((unsigned char*)(encodedBytes+pos),1,decodedBytes);
            pos++;
            }
        }
    }
    else
    {
    //Data is not compressed, copy it to output
    WriteBuffer((unsigned char*)(encodedBytes+HEADER_SIZE),fileSize-HEADER_SIZE,decodedBytes);
    }
free(encodedBytes);
uint8_t* bytes=(uint8_t*)FreeBuffer(decodedBytes);



//Check checksum
/*
unsigned int Checksum = 0xF369A75B;
Checksum=ChecksumProcessByte(Checksum,RideData->Header[0]);//Do first byte
    for(i=4;i<12;i++)Checksum=ChecksumProcessByte(Checksum,RideData->Header[i]);//Do filename
    for(i=0;i<DecodedFileSize;i++)Checksum=ChecksumProcessByte(Checksum,Bytes[i]);//Checksum rest file;
//printf("Calculated checksum: %x\n",Checksum);
//printf("Actual checksum: %x\n",ReadLittleEndianInt(RideData->Header+12));
    if(Checksum!=ReadLittleEndianInt(RideData->Header+12))
    {
    //printf("Checksum does not match\n");
    return RideData;
    }
*/


//Load object header
uint8_t* objectHeader=bytes;

RideHeader* rideHeader=malloc(sizeof(RideHeader));
//Read flags
rideHeader->Flags=*((uint32_t*)(objectHeader+8));
//Read track style & preview index
rideHeader->PreviewIndex=0;
    for(i=12;i<=14;i++)
    {
        if(objectHeader[i]!=0xFFu)
        {
        rideHeader->TrackStyle=objectHeader[i];
        break;
        }
    rideHeader->PreviewIndex++;
    }

rideHeader->Unknown=*((uint16_t*)(objectHeader+18));
//Read minimum and maximum car length
rideHeader->MinimumCars=objectHeader[15];
rideHeader->MaximumCars=objectHeader[16];
//Read track sections
rideHeader->TrackSections=*((uint64_t*)(objectHeader+438));

rideHeader->Excitement=objectHeader[434];
rideHeader->Intensity=objectHeader[435];
rideHeader->Nausea=objectHeader[436];
rideHeader->MaxHeight=objectHeader[437];
//Read categories
rideHeader->Categories[0]=objectHeader[446];
rideHeader->Categories[1]=objectHeader[447];

//Read car type information
rideHeader->CarTypes[CAR_INDEX_DEFAULT]=objectHeader[20];
rideHeader->CarTypes[CAR_INDEX_FRONT]=objectHeader[21];
rideHeader->CarTypes[CAR_INDEX_SECOND]=objectHeader[22];
rideHeader->CarTypes[CAR_INDEX_REAR]=objectHeader[23];
rideHeader->CarTypes[CAR_INDEX_THIRD]=objectHeader[24];

//Read car types
uint8_t* carData=objectHeader+26;
    for(i=0;i<NUM_CARS;i++)
    {
    Car* car=rideHeader->Cars+i;

    car->HighestRotationIndex=carData[0];
    //Load spacing
    car->Spacing=carData[6];
    //Load riders
    car->RiderPairs=carData[11]&0x80;
    car->RiderSprites=carData[84];
    car->Riders=carData[11]&0x7F;
    //Load flags
    car->Flags=*((uint32_t*)(carData+17));
    //Load parameters
    car->SpinInertia=carData[85];
    car->SpinFriction=carData[86];
    car->PoweredVelocity=carData[91];
    //Load Z value
    car->ZValue=carData[95];

    //Read unknown fields
    car->Unknown[0]=*((uint16_t*)(carData+4));
    car->Unknown[1]=*((uint16_t*)(carData+8));
    car->Unknown[2]=*((uint16_t*)(carData+12));
    car->Unknown[3]=*((uint16_t*)(carData+87));
    car->Unknown[4]=*((uint16_t*)(carData+89));
    car->Unknown[5]=*((uint16_t*)(carData+92));
    car->Unknown[6]=(uint16_t)carData[94];

    carData+=101;
    }

object->ObjectHeader=rideHeader;


//Load string tables
int pos=0x1C2;

    for(i=0;i<NUM_STRING_TABLES;i++)
    {
    object->StringTables[i]=NULL;
    int tableFinished=0;
    int stringNum=0;
        while(!tableFinished)
        {
        //Allocate memory for string
        object->StringTables[i]=realloc(object->StringTables[i],(stringNum+1)*sizeof(String*));
        object->StringTables[i][stringNum]=malloc(sizeof(String));
        object->StringTables[i][stringNum]->Num=bytes[pos];
        pos++;

        //Load string
        int stringLen=strlen((char*)bytes+pos)+1;
        object->StringTables[i][stringNum]->Str=malloc(stringLen);
        memcpy(object->StringTables[i][stringNum]->Str,bytes+pos,stringLen);
        pos+=stringLen;
        if(bytes[pos]==0xFF)tableFinished=1;//There are no more strings
        else stringNum++;//There is another string
        }
    //Add NULL terminator
    stringNum++;
    object->StringTables[i]=realloc(object->StringTables[i],(stringNum+1)*sizeof(String));
    object->StringTables[i][stringNum]=NULL;
    //Skip terminating byte
    pos++;
    }



//Read unknown strutures
RideStructures* structures=malloc(sizeof(RideStructures));
//Sequence of 3 byte structures; I don't know why -1 means 32
structures->NumStructures=bytes[pos]==0xFF?32:bytes[pos];
structures->Structures=malloc(structures->NumStructures*sizeof(struct3byte));
pos++;

	for(i=0;i<structures->NumStructures;i++)
    {
    structures->Structures[i].a=bytes[pos];
    structures->Structures[i].b=bytes[pos+1];
    structures->Structures[i].c=bytes[pos+2];
    pos+=3;
    }

    //Four variable length structures
    for (i=0;i<4;i++)
    {
    uint16_t len=bytes[pos];
    pos++;
        //If the length can't fit in a byte, the length is instead stored in the two bytes following
        if (len==0xFF)
        {
        pos++;
        len=*((uint16_t*)(bytes+pos));
        pos+=2;
        }
    structures->PeepPositions[i].Num=len;
    structures->PeepPositions[i].Positions=malloc(structures->PeepPositions[i].Num);
    int j;
        for(j=0;j<len;j++)
        {
        structures->PeepPositions[i].Positions[j]=bytes[pos];
        pos++;
        }
    }
object->Optional=structures;


//Load images
//Get number of images
object->NumImages=*((uint32_t*)(bytes+pos));
//printf("There are %d Images\n",RideData->NumImages);
pos+=4;
//Get size of graphic data
object->GraphicBytes=*((uint32_t*)(bytes+pos));
pos+=4;
object->Images=malloc(object->NumImages*sizeof(Image));

int bitmapBase=pos+(object->NumImages*16);

    for(i=0;i<object->NumImages;i++)
    {
    Image* image=object->Images+i;
    uint32_t startAddress=*((uint32_t*)(bytes+pos));
    image->Width=*((uint16_t*)(bytes+pos+4));
    image->Height=*((uint16_t*)(bytes+pos+6));
    image->XOffset=*((uint16_t*)(bytes+pos+8));
    image->YOffset=*((uint16_t*)(bytes+pos+10));
    image->Flags=*((uint16_t*)(bytes+pos+12));
    pos+=16;
		if (object->Images[i].Flags== 1) // Simple bitmap.
		{
        int j;
        image->Data=malloc(image->Height*sizeof(char*));
        for (j=0;j<image->Height;j++)image->Data[j]=NULL;

        int offset=bitmapBase+startAddress;
			for (j=0;j<image->Height;j++)//TODO - See if this needs inverting
			{
            image->Data[j]=malloc(image->Width);
			memcpy(image->Data[j],bytes+offset,image->Width);
			offset+=image->Width;
			}
		}
		else if (image->Flags == 5) // compacted bitmap.
		{
        int j;
        image->Data=malloc(image->Height*sizeof(char*));
        for (j=0;j<image->Height;j++)image->Data[j]=NULL;

        int offset=bitmapBase+startAddress;

			for (j=0;j<image->Height;j++)
			{
                int k;
                image->Data[j]=malloc(image->Width);
                for(k=0;k<image->Width;k++)image->Data[j][k]=0;//Set row to transparency
				int rowOffset=offset+*((uint16_t*)(bytes+offset+(j*2)));
                uint8_t lastOne;
				do
				{
					//First byte stores the length of the element
					uint8_t length = bytes[rowOffset];
                    rowOffset++;
                    //Next byte stores distance from start of scanline
					uint8_t xOffset=bytes[rowOffset];
					rowOffset++;
					//The most significant bit of the length is set only if this is the last element
					lastOne=length&0x80;
					length=length&0x7F;
                        if(xOffset+length>image->Width)
                        {
                        printf("File contains invalid bitmaps\n");
                        return NULL;
                        }
                    for(k=0;k<length;k++)
                    {
                    if(xOffset+k<image->Width)image->Data[j][xOffset+k]=bytes[rowOffset];
                    rowOffset++;
                    }
				} while (!lastOne);
			}
		}

    }
return object;
}
void SaveDat(ObjectFile* object,char* filename)
{
int i;
//Check filename
int nameLen=strlen(filename);
int startIndex=nameLen;
int endIndex=nameLen;
while(filename[startIndex]!='/')startIndex--;
startIndex++;
while(filename[endIndex]!='.'&&endIndex>startIndex)endIndex--;
if(startIndex==endIndex)endIndex=nameLen-1;
else endIndex--;

//Convert filename to upper case
char* upperCaseFilename=malloc(endIndex-startIndex+2);
for(i=startIndex;i<=endIndex;i++)upperCaseFilename[i-startIndex]=toupper(filename[i]);
upperCaseFilename[endIndex-startIndex+1]=0;

//Check filename length
    if(strlen(upperCaseFilename)>8)
    {
    printf("Filename too long (8 characters not including extension)\n");
    return;
    }


DynamicBuffer* decodedFile=CreateBuffer(512);

//Write object header
uint8_t objectHeader[0x1C2];
memset(objectHeader,0,0x1C2);
RideHeader* rideHeader=object->ObjectHeader;

//Write flags
*((uint32_t*)(objectHeader+8))=rideHeader->Flags;

//Write track style & preview index
memset(objectHeader+12,0xFFu,3);
objectHeader[12+rideHeader->PreviewIndex]=rideHeader->TrackStyle;
//Write unknown data
*((uint16_t*)(objectHeader+18))=rideHeader->Unknown;
//Write minimum and maximum car length
objectHeader[15]=rideHeader->MinimumCars;
objectHeader[16]=rideHeader->MaximumCars;
//Write track sections
*((uint64_t*)(objectHeader+438))=rideHeader->TrackSections;

objectHeader[434]=rideHeader->Excitement;
objectHeader[435]=rideHeader->Intensity;
objectHeader[436]=rideHeader->Nausea;
objectHeader[437]=rideHeader->MaxHeight;

//Write categories
objectHeader[446]=rideHeader->Categories[0];
objectHeader[447]=rideHeader->Categories[1];

//Write car type information
objectHeader[20]=rideHeader->CarTypes[CAR_INDEX_DEFAULT];
objectHeader[21]=rideHeader->CarTypes[CAR_INDEX_FRONT];
objectHeader[22]=rideHeader->CarTypes[CAR_INDEX_SECOND];
objectHeader[23]=rideHeader->CarTypes[CAR_INDEX_REAR];
objectHeader[24]=rideHeader->CarTypes[CAR_INDEX_THIRD];

//Write values that are known, but not applicable
objectHeader[17]=0xFF;
objectHeader[448]=0xFF;
objectHeader[449]=0xFF;

//Write car types
uint8_t* carData=objectHeader+26;
    for(i=0;i<NUM_CARS;i++)
    {
    Car* car=rideHeader->Cars+i;

    carData[0]=car->HighestRotationIndex;
    //Write spacing
    carData[6]=car->Spacing;
    //Write riders
    carData[11]=car->RiderPairs|car->Riders;
    carData[84]=car->RiderSprites;
    //Write flags
    *((uint32_t*)(carData+17))=car->Flags;
    //Write parameters
    carData[85]=car->SpinInertia;
    carData[86]=car->SpinFriction;
    carData[91]=car->PoweredVelocity;
    //Write Z value
    carData[95]=car->ZValue;

    //Write unknown fields
    *((uint16_t*)(carData+4))=car->Unknown[0];
    *((uint16_t*)(carData+8))=car->Unknown[1];
    *((uint16_t*)(carData+12))=car->Unknown[2];
    *((uint16_t*)(carData+87))=car->Unknown[3];
    *((uint16_t*)(carData+89))=car->Unknown[4];
    *((uint16_t*)(carData+92))=car->Unknown[5];
    carData[94]=(uint8_t)car->Unknown[6];

    carData+=101;
    }
WriteBuffer(objectHeader,0x1C2,decodedFile);

unsigned char terminator=0xFF;
    for(i=0;i<3;i++)
    {
    int stringNum=0;
        while(object->StringTables[i][stringNum]!=NULL)
        {
        uint8_t language=(char)object->StringTables[i][stringNum]->Num;
        WriteBuffer(&language,1,decodedFile);
        WriteBuffer((unsigned char*)object->StringTables[i][stringNum]->Str,strlen(object->StringTables[i][stringNum]->Str)+1,decodedFile);
        stringNum++;
        }
    WriteBuffer(&terminator,1,decodedFile);
    };

//Write mystery structures
RideStructures* structures=object->Optional;
WriteBuffer(&structures->NumStructures,1,decodedFile);
    for(i=0;i<structures->NumStructures;i++)
    {
    WriteBuffer(&structures->Structures[i].a,1,decodedFile);
    WriteBuffer(&structures->Structures[i].b,1,decodedFile);
    WriteBuffer(&structures->Structures[i].c,1,decodedFile);
    }
//Write 4 byte peep positioning structures
    for(i=0;i<4;i++)
    {
        if(structures->PeepPositions[i].Num<0xFFu)
        {
        uint8_t length=(uint8_t)structures->PeepPositions[i].Num;
        WriteBuffer(&length,1,decodedFile);
        }
        else
        {
        uint8_t value=0xFFu;
        WriteBuffer((uint8_t*)&value,1,decodedFile);
        WriteBuffer((uint8_t*)&structures->PeepPositions[i].Num,2,decodedFile);
        }
    WriteBuffer((uint8_t*)structures->PeepPositions[i].Positions,structures->PeepPositions[i].Num,decodedFile);
    }

WriteBuffer((uint8_t*)&object->NumImages,4,decodedFile);
int graphicDataSizeOffset=decodedFile->Pos;
ExpandBuffer(4,decodedFile);

DynamicBuffer* graphicData=CreateBuffer(512);

    for(i=0;i<object->NumImages;i++)
    {
    Image* image=object->Images+i;
    //Write data to data buffer
    int startAddress;
        if(image->Flags==1)startAddress=WriteSimpleBitmap(image,graphicData);
        else startAddress=WriteCompactBitmap(image,graphicData);
    //Write TGraphicRecord to file buffer
    WriteBuffer((unsigned char*)&startAddress,4,decodedFile);
    WriteBuffer((unsigned char*)&image->Width,2,decodedFile);
    WriteBuffer((unsigned char*)&image->Height,2,decodedFile);
    WriteBuffer((unsigned char*)&image->XOffset,2,decodedFile);
    WriteBuffer((unsigned char*)&image->YOffset,2,decodedFile);
    WriteBuffer((unsigned char*)&image->Flags,2,decodedFile);//TODO-Actually check if this is the case
    ExpandBuffer(2,decodedFile);//Padding
    }
//Write size of graphic data
int graphicDataSize=graphicData->Pos;
memcpy(decodedFile->Buffer+graphicDataSizeOffset,&graphicDataSize,4);
//Write data buffer to file buffer
unsigned char* graphicBytes=FreeBuffer(graphicData);
WriteBuffer(graphicBytes,graphicDataSize,decodedFile);
free(graphicBytes);

//Finished writing decoded file
int dataSize=decodedFile->Pos;
unsigned char* bytes=FreeBuffer(decodedFile);

/*
FILE* decomp=fopen("DECOMPRESSED.DAT","w");
fwrite(Bytes,FileSize,1,decomp);
fclose(decomp);
*/
//Encode file with RLE
DynamicBuffer* encodedBytes=CreateBuffer(512);
int pos=0;

    while(pos<dataSize)
    {
    int8_t repeatedBytes=CountRepeatedBytes(bytes,pos,dataSize);
        if(repeatedBytes>1)
        {
        //Bytes are repeated
        int8_t byteToWryte=1-repeatedBytes;
        WriteBuffer((uint8_t*)&byteToWryte,1,encodedBytes);//Number of bytes to repeat
        WriteBuffer((uint8_t*)(bytes+pos),1,encodedBytes);//Byte to repeat
        pos+=repeatedBytes;
        }
        else
        {
        int bytesToCopy=CountDifferingBytes(bytes,pos,dataSize);
        uint8_t byteToWryte=bytesToCopy-1;
        WriteBuffer(&byteToWryte,1,encodedBytes);//Number of bytes to copy
        WriteBuffer(bytes+pos,bytesToCopy,encodedBytes);//Byte to copy
        pos+=bytesToCopy;
            if(bytesToCopy==0)printf("%d\n",pos);
        }
    }

//Finished compressing data
int compressedDataSize=encodedBytes->Pos;
uint8_t* data=FreeBuffer(encodedBytes);


//Create header
uint8_t header[HEADER_SIZE];

//This is a custom object, and it's a ride.
header[0]=0x00;

//Write filename in header
memset(header+4,' ',8);
strncpy((char*)(header+4),upperCaseFilename,strlen(upperCaseFilename));

//Specify that data is compressed
header[0x10]=1;

/*
//Set the animation width and height to 255 (I don't know what this is for (appears to have no effect), probably to do with clipping)
Bytes[0x28+HEADER_SIZE] =255;
Bytes[0x29+HEADER_SIZE] =255;
Bytes[0x2A+HEADER_SIZE] =255;
*/

//Calculate checksum
uint32_t checksum = 0xF369A75B;
checksum=ChecksumProcessByte(checksum,header[0]);//Do first byte
    for(i=4;i<12;i++)checksum=ChecksumProcessByte(checksum,header[i]);//Do filename
for(i=0;i<dataSize;i++)checksum=ChecksumProcessByte(checksum,bytes[i]);//Checksum rest of the file;
//Write checksum
uint8_t checksumByte=(uint8_t)(checksum&0xFF);
header[12]=checksumByte;
checksumByte=(uint8_t)((checksum&0xFF00)>>8);
header[13]=checksumByte;
checksumByte=(uint8_t)((checksum&0xFF0000)>>16);
header[14]=checksumByte;
checksumByte=(uint8_t)((checksum&0xFF000000)>>24);
header[15]=checksumByte;

//Write file size
uint8_t fileSizeByte=(uint8_t)(compressedDataSize&0xFF);
header[17]=fileSizeByte;
fileSizeByte=(uint8_t)((compressedDataSize&0xFF00)>>8);
header[18]=fileSizeByte;
fileSizeByte=(uint8_t)((compressedDataSize&0xFF0000)>>16);
header[19]=fileSizeByte;
fileSizeByte=(uint8_t)((compressedDataSize&0xFF000000)>>24);
header[20]=fileSizeByte;



FILE* file=fopen(filename,"w");
fwrite(header,1,HEADER_SIZE,file);
fwrite(data,1,compressedDataSize,file);
fclose(file);
free(bytes);
free(data);
printf("File saved with filename %s\n",filename);
}

String* SearchStrings(ObjectFile* RideData,int TableNum,int Language)
{
String* Str=NULL;
int StrNum=0;
while(RideData->StringTables[TableNum][StrNum]!=NULL)
{
    if(RideData->StringTables[TableNum][StrNum]->Num==Language)
    {
    Str=RideData->StringTables[TableNum][StrNum];
    break;
    }
StrNum++;
}
return Str;
}
char* GetString(ObjectFile* RideData,int TableNum,int Language)
{
String* Str=SearchStrings(RideData,TableNum,Language);
if(Str==NULL)return NULL;
return Str->Str;
}

void SetString(ObjectFile* RideData,int TableNum,int Language,char* NewStr)
{
String* Str=SearchStrings(RideData,TableNum,Language);
if(Str==NULL)return;
free(Str->Str);
Str->Str=malloc(strlen(NewStr)+1);
strcpy(Str->Str,NewStr);
}

void FreeImage(Image* image)
{
int i;
    for(i=0;i<image->Width;i++)free(image->Data[i]);
free(image->Data);
free(image);
}

void FreeDat(ObjectFile* file)
{
//Free string tables
int i,j;
    for(i=0;i<3;i++)
    {
    int StringNum=0;
        while(file->StringTables[i][StringNum]!=NULL)
        {
        free(file->StringTables[i][StringNum]->Str);
        free(file->StringTables[i][StringNum]);
        StringNum++;
        }
    free(file->StringTables[i]);
    }

//Free images
    for(i=0;i<file->NumImages;i++)
    {
        if(file->Images[i].Data!=NULL)
        {
            for(j=0;j<file->Images[i].Height;j++)
            {
            if(file->Images[i].Data[j]!=NULL)free(file->Images[i].Data[j]);
            }
        free(file->Images[i].Data);
        }
    }
free(file->Images);
free(file);
}
