#include "datastructures.h"
#include "types.h"
#include "backend.h"

static RideFile* RideData=NULL;



short ReadLittleEndianShort(unsigned char* ptr){
return ptr[0]+(ptr[1]*256);
}
int ReadLittleEndianInt(unsigned char* ptr){
return ptr[0]+(ptr[1]*256)+(ptr[2]*65536)+(ptr[3]*16777216);
}
int ChecksumProcessByte(unsigned int Checksum,unsigned char Byte){
unsigned int ChecksumHigherBits=Checksum&0xFFFFFF00;
unsigned char ChecksumLowerBits=(char)(Checksum&0xFF);
ChecksumLowerBits^=Byte;
Checksum=ChecksumHigherBits|(int)ChecksumLowerBits;
return (Checksum<<11)|(Checksum>>21);
}
char CountRepeatedBytes(char* Bytes,int Pos,int Size){
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
char CountDifferingBytes(char* Bytes,int Pos,int Size){
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
void WriteLittleEndianInt(int Num,DynamicBuffer* Buffer){
//TODO-Replace with something that's actually cross platform
WriteBuffer(&Num,4,Buffer);
}
void WriteLittleEndianShort(int Num,DynamicBuffer* Buffer){
//TODO-Replace with something that's actually cross platform
WriteBuffer(&Num,2,Buffer);
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
        while(XOffset<Bitmap->Width&&Bitmap->Data[Row][XOffset]==0)XOffset++;
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
    WriteLittleEndianShort(RowDataStartOffset,GraphicData);
    };
int ScanDataSize=ScanBuffer->Pos;
char* ScanData=FreeBuffer(ScanBuffer);
WriteBuffer(ScanData,ScanDataSize,GraphicData);
free(ScanData);
return StartAddress;
}


void LoadFile(char* filename)//TODO-check that the file is actually a ride
{
if(RideData!=NULL)FreeRideFile();

FILE* DatFile=fopen(filename,"r");
    if(DatFile==NULL)
    {
    printf("Cannot open file\n");
    return NULL;
    }
//Get file size
fseek(DatFile,0,SEEK_END);
int FileSize=ftell(DatFile);
fseek(DatFile,SEEK_SET,0);

//Read header (Not encoded)
RideData=malloc(sizeof(RideFile));
fread(RideData->Header,HEADER_SIZE,1,DatFile);
printf("Read header %d\n",RideData->Header[0x10]);

//Decode remaining data into temp array
char* EncodedBytes=malloc(FileSize-HEADER_SIZE);
DynamicBuffer* DecodedBytes=CreateBuffer(512);
fread(EncodedBytes,FileSize-HEADER_SIZE,1,DatFile);
//Check the header to see if the remaining data is compressed
if(RideData->Header[0x10])//If the data is compressed
    {
    printf("Data is compressed, decoding\n");
    //Decode RLE data
    int Pos=0;
    while(Pos<FileSize-HEADER_SIZE)
    {
            if(EncodedBytes[Pos]>=0)//Copy bytes
            {
            short numToCopy=(short)(EncodedBytes[Pos])+1;
            Pos+=1;
            WriteBuffer(EncodedBytes+Pos,numToCopy,DecodedBytes);
            Pos+=numToCopy;
            }
            else//Repeat bytes
            {
            char numToRepeat=1-EncodedBytes[Pos];
            Pos++;
            for(;numToRepeat>0;numToRepeat--)WriteBuffer(EncodedBytes+Pos,1,DecodedBytes);
            Pos++;
            }
        }
    }
    else
    {
    //Data is not compressed, copy it to output
    WriteBuffer(EncodedBytes,FileSize-HEADER_SIZE,DecodedBytes);
    }
free(EncodedBytes);
int DecodedFileSize=DecodedBytes->Pos;
char* Bytes=FreeBuffer(DecodedBytes);
printf("Decoded data\n\n");
fclose(DatFile);

printf("Byte 1 %x Byte 2 %x\n",(unsigned char)(Bytes[208]),(unsigned char)(Bytes[906]));

FILE* dump=fopen("DUMP.DAT","w");
fwrite(Bytes,DecodedFileSize,1,dump);
fclose(dump);

//Check checksum
unsigned int Checksum = 0xF369A75B;
Checksum=ChecksumProcessByte(Checksum,RideData->Header[0]);//Do first byte
int i;
for(i=4;i<12;i++)Checksum=ChecksumProcessByte(Checksum,RideData->Header[i]);//Do filename
//for(i=16;i<HEADER_SIZE;i++)Checksum=ChecksumProcessByte(Checksum,Bytes[i]);//Checksum rest of the header;
for(i=0;i<DecodedFileSize;i++)Checksum=ChecksumProcessByte(Checksum,Bytes[i]);//Checksum rest of the header;
printf("Calculated checksum: %x\n",Checksum);
printf("Actual checksum: %x\n",ReadLittleEndianInt(RideData->Header+12));
if(Checksum!=ReadLittleEndianInt(RideData->Header+12))
{
printf("Checksum does not match\n");
return;
}



//Bytes now contains the decoded data;
int Pos=0;

//Copy Unknown Data
memcpy(RideData->Unknown,Bytes,0x1C2);
Pos+=0x1C2;

printf("Reading string tables\n");
//Copy string tables
int StringTableNum=0;
    while(StringTableNum<3)//There should be 3 string tables
    {
    RideData->StringTables[StringTableNum]=NULL;
    int TableFinished=0;
    int StringNum=0;
        while(!TableFinished)
        {
        RideData->StringTables[StringTableNum]=realloc(RideData->StringTables[StringTableNum],(StringNum+1)*sizeof(String*));
        RideData->StringTables[StringTableNum][StringNum]=malloc(sizeof(String));
        RideData->StringTables[StringTableNum][StringNum]->Num=Bytes[Pos];
        Pos++;
        int StringLen=strlen(Bytes+Pos)+1;
        RideData->StringTables[StringTableNum][StringNum]->Str=malloc(StringLen);
        memcpy(RideData->StringTables[StringTableNum][StringNum]->Str,Bytes+Pos,StringLen);
        printf("Read string: %s, Number %d\n",RideData->StringTables[StringTableNum][StringNum]->Str,RideData->StringTables[StringTableNum][StringNum]->Num);
        Pos+=StringLen;
        if(Bytes[Pos]==-1)TableFinished=1;//There are no more strings
        else StringNum++;//There is another string
        }
    //Add NULL terminator
    StringNum++;
    RideData->StringTables[StringTableNum]=realloc(RideData->StringTables[StringTableNum],(StringNum+1)*sizeof(String));
    RideData->StringTables[StringTableNum][StringNum]=NULL;

    Pos++;//Skip terminating byte
    StringTableNum++;
    printf("End of string table found\n");
    }
printf("All string tables read\n\n");
//Read unknown strutures
int StructStartPos=Pos;
//Sequence of 3 byte structures; I don't know why -1 means 32
	Pos+=(Bytes[Pos]==-1?32:Bytes[Pos])*3;
		Pos ++;
		//Four variable length structures
		for (i=0;i<4;i++)
		{
			unsigned short Len = (unsigned char)Bytes[Pos];
			//If the length can't fit in a byte, the length is instead stored in the two bytes following
			if (Len == 0xFF)
			{
                Pos++;
				Len=ReadLittleEndianShort(Bytes+Pos);
				//Skip to end of structure
				Pos+=Len+2;
			}
			else
			{
			    //Skip to end of structure
				Pos+=Len+1;
			}
		}
int UnknownStructsLen=Pos-StructStartPos;
RideData->UnknownStructures=malloc(UnknownStructsLen);
memcpy(RideData->UnknownStructures,Bytes+StructStartPos,UnknownStructsLen);
RideData->UnknownStructureSize=UnknownStructsLen;

printf("Reading images\n");
//Get number of images
RideData->NumImages=ReadLittleEndianInt(Bytes+Pos);
printf("There are %d Images\n",RideData->NumImages);
Pos+=4;
//Get size of graphic data
RideData->GraphicBytes=ReadLittleEndianInt(Bytes+Pos);
Pos+=4;
RideData->Images=malloc(RideData->NumImages*sizeof(Image));

int BitmapBase=Pos+(RideData->NumImages*16);

//Read Image data
TGraphicRecord* ImageData=malloc(sizeof(TGraphicRecord));
//Set all data pointers to NULL
for(i=0;i<RideData->NumImages;i++)RideData->Images[i].Data=NULL;
for(i=0;i<RideData->NumImages;i++)
{
//Bytes+Pos points to a TGraphicRecord structure
ImageData->StartAddress=ReadLittleEndianInt(Bytes+Pos);
ImageData->Width=ReadLittleEndianShort(Bytes+Pos+4);
ImageData->Height=ReadLittleEndianShort(Bytes+Pos+6);
ImageData->Xoffset=ReadLittleEndianShort(Bytes+Pos+8);
ImageData->Yoffset=ReadLittleEndianShort(Bytes+Pos+10);
ImageData->Flags=ReadLittleEndianShort(Bytes+Pos+12);
RideData->Images[i].Height=ImageData->Height;
RideData->Images[i].Width=ImageData->Width;
RideData->Images[i].XOffset=ImageData->Xoffset;
RideData->Images[i].YOffset=ImageData->Yoffset;
RideData->Images[i].Flags=ImageData->Flags;

		if (ImageData->Flags== 1) // Simple bitmap.
		{
        int j;
        RideData->Images[i].Data=malloc(ImageData->Height*sizeof(char*));
        for (j=0;j<ImageData->Height;j++)RideData->Images[i].Data[j]=NULL;

        int Offset=BitmapBase+ImageData->StartAddress;
			for (j=0;j<ImageData->Height;j++)//TODO - See if this needs inverting
			{
            RideData->Images[i].Data[j]=malloc(ImageData->Width);
			memcpy(RideData->Images[i].Data[j],Bytes+Offset,ImageData->Width);
			Offset+=ImageData->Width;
			}
		}
		else if (ImageData->Flags == 5) // compacted bitmap.
		{
        int j;
        RideData->Images[i].Data=malloc(ImageData->Height*sizeof(char*));
        for (j=0;j<ImageData->Height;j++)RideData->Images[i].Data[j]=NULL;

        int Offset=BitmapBase+ImageData->StartAddress;

			for (j=0;j<ImageData->Height;j++)
			{
                int k;
                RideData->Images[i].Data[j]=malloc(ImageData->Width);
                for(k=0;k<ImageData->Width;k++)RideData->Images[i].Data[j][k]=0;//Set row to transparency
				int RowOffset=Offset+ReadLittleEndianShort(Bytes+Offset+(j*2));
                unsigned char LastOne;
				do
				{
					//First byte stores the length of the element
					unsigned char Length = Bytes[RowOffset];
                    RowOffset++;
                    //Next byte stores distance from start of scanline
					unsigned char XOffset=Bytes[RowOffset];
					RowOffset++;
					//The most significant bit of the length is set only if this is the last element
					LastOne=Length & 0x80;
					Length=Length&0x7F;
                    if(XOffset+Length>ImageData->Width)
                        {
                        printf("File contains invalid bitmaps\n");
                        FreeRideFile(RideData);
                        return NULL;
                        }
                    for(k=0;k<Length;k++)
                    {
                    if(XOffset+k<ImageData->Width)RideData->Images[i].Data[j][XOffset+k]=Bytes[RowOffset];
                    RowOffset++;
                    }
				} while (!LastOne);
			}
		}
    Pos+=16;
    }
free(ImageData);
printf("Images read\n\n");
printf("Loading complete\n");
}

void SaveFile(char* Filename)
{
    printf("%s\n",Filename);
//Check filename
int NameLen=strlen(Filename);
int StartIndex=NameLen;
int EndIndex=NameLen;
while(Filename[StartIndex]!='/')StartIndex--;
StartIndex++;
while(Filename[EndIndex]!='.'&&EndIndex>StartIndex)EndIndex--;
if(StartIndex==EndIndex)EndIndex=NameLen-1;
else EndIndex--;
printf("StartIndex %d EndIndex %d\n",StartIndex,EndIndex);
//Convert filename to upper case
char* UpperCaseFilename=malloc(EndIndex-StartIndex+2);
int i;
for(i=StartIndex;i<=EndIndex;i++)UpperCaseFilename[i-StartIndex]=toupper(Filename[i]);
UpperCaseFilename[EndIndex-StartIndex+1]=0;
    printf("%s\n",UpperCaseFilename);
//Check filename length
if(strlen(UpperCaseFilename)>8)
    {
    printf("Filename to long (8 characters not including .DAT)\n");
    return;
    }

DynamicBuffer* DecodedFile=CreateBuffer(512);
WriteBuffer(RideData->Header,HEADER_SIZE,DecodedFile);
WriteBuffer(RideData->Unknown,0x1C2,DecodedFile);
printf("Writing string tables\n");
int TableNum=0;
unsigned char Terminator=0xFF;
    for(TableNum=0;TableNum<3;TableNum++)
    {
    int StringNum=0;
        while(RideData->StringTables[TableNum][StringNum]!=NULL)
        {
        char Language=(char)RideData->StringTables[TableNum][StringNum]->Num;
        WriteBuffer(&Language,1,DecodedFile);
        WriteBuffer(RideData->StringTables[TableNum][StringNum]->Str,strlen(RideData->StringTables[TableNum][StringNum]->Str)+1,DecodedFile);
        StringNum++;
        }
    WriteBuffer(&Terminator,1,DecodedFile);
    };
printf("Written string tables\n");

WriteBuffer(RideData->UnknownStructures,RideData->UnknownStructureSize,DecodedFile);

printf("Writing bitmaps\n");
WriteLittleEndianInt(RideData->NumImages,DecodedFile);
int GraphicDataSizeOffset=DecodedFile->Pos;
WriteLittleEndianInt(0,DecodedFile);//Placeholder for size of graphic data;

DynamicBuffer* GraphicData=CreateBuffer(512);
int ImageNum=0;
    for(ImageNum=0;ImageNum<RideData->NumImages;ImageNum++)
    {
    //Write data to data buffer
    int StartAddress;
    if(RideData->Images[ImageNum].Flags==1)
    {
    StartAddress=WriteSimpleBitmap(RideData->Images+ImageNum,GraphicData);
    }
    else//TODO-Handle the case of invalid flags
    {
    StartAddress=WriteCompactBitmap(RideData->Images+ImageNum,GraphicData);
    }
    //Write TGraphicRecord to file buffer
    WriteLittleEndianInt(StartAddress,DecodedFile);
    WriteLittleEndianShort(RideData->Images[ImageNum].Width,DecodedFile);
    WriteLittleEndianShort(RideData->Images[ImageNum].Height,DecodedFile);
    WriteLittleEndianShort(RideData->Images[ImageNum].XOffset,DecodedFile);
    WriteLittleEndianShort(RideData->Images[ImageNum].YOffset,DecodedFile);
    WriteLittleEndianShort(RideData->Images[ImageNum].Flags,DecodedFile);//TODO-Actually check if this is the case
    WriteLittleEndianShort(0,DecodedFile);//Padding
    }

//Write size of graphic data
int PrevPos=DecodedFile->Pos;
DecodedFile->Pos=GraphicDataSizeOffset;
int GraphicDataSize=GraphicData->Pos;
WriteLittleEndianInt(GraphicDataSize,DecodedFile);
DecodedFile->Pos=PrevPos;
//Write data buffer to file buffer
char* Data=FreeBuffer(GraphicData);
WriteBuffer(Data,GraphicDataSize,DecodedFile);
free(Data);



int FileSize=DecodedFile->Pos;
char* Bytes=FreeBuffer(DecodedFile);

//Now adjust requisite header values

//This is a custom object, and it's a ride.
Bytes[0]=0x00;

//Write filename in header
for(i=0;i<8;i++)Bytes[i+4]=' ';
strncpy(Bytes+4,UpperCaseFilename,strlen(UpperCaseFilename));

//Specify that data is compressed
Bytes[0x10]=1;

//Set the animation width and height to 255 (I don't know what this is for, probably to do with clipping)
Bytes[0x28+HEADER_SIZE] = 255;
Bytes[0x29+HEADER_SIZE] = 255;
Bytes[0x2A+HEADER_SIZE] = 255;

//Calculate checksum
int Checksum = 0xF369A75B;
Checksum=ChecksumProcessByte(Checksum,Bytes[0]);//Do first byte
for(i=4;i<12;i++)Checksum=ChecksumProcessByte(Checksum,Bytes[i]);//Do filename
for(i=HEADER_SIZE;i<FileSize;i++)Checksum=ChecksumProcessByte(Checksum,Bytes[i]);//Checksum rest of the file;
//Write little endian checksum
char ChecksumByte=(char)(Checksum&0xFF);
Bytes[12]=ChecksumByte;
ChecksumByte=(char)((Checksum&0xFF00)>>8);
Bytes[13]=ChecksumByte;
ChecksumByte=(char)((Checksum&0xFF0000)>>16);
Bytes[14]=ChecksumByte;
ChecksumByte=(char)((Checksum&0xFF000000)>>24);
Bytes[15]=ChecksumByte;

FILE* decomp=fopen("DECOMPRESSED.DAT","w");
fwrite(Bytes,FileSize,1,decomp);
fclose(decomp);
//Encode file with RLE
DynamicBuffer* EncodedBytes=CreateBuffer(512);
WriteBuffer(Bytes,HEADER_SIZE,EncodedBytes);
int Pos=HEADER_SIZE;

while(Pos<FileSize)
{
char RepeatedBytes=CountRepeatedBytes(Bytes,Pos,FileSize);
if(RepeatedBytes>1)
{
//Bytes are repeated
char ByteToWryte=1-RepeatedBytes;
WriteBuffer(&ByteToWryte,1,EncodedBytes);//Number of bytes to repeat
WriteBuffer(Bytes+Pos,1,EncodedBytes);//Byte to repeat
Pos+=RepeatedBytes;
}
else
{
int BytesToCopy=CountDifferingBytes(Bytes,Pos,FileSize);
char ByteToWryte=BytesToCopy-1;
WriteBuffer(&ByteToWryte,1,EncodedBytes);//Number of bytes to repeat
WriteBuffer(Bytes+Pos,BytesToCopy,EncodedBytes);//Byte to repeat
Pos+=BytesToCopy;
}
}
free(Bytes);
int CompressedSize=EncodedBytes->Pos;

int DataSize=CompressedSize-HEADER_SIZE;

char* FileData=FreeBuffer(EncodedBytes);

//Write file size
char FileSizeByte=(char)(DataSize&0xFF);
FileData[17]=FileSizeByte;
FileSizeByte=(char)((DataSize&0xFF00)>>8);
FileData[18]=FileSizeByte;
FileSizeByte=(char)((DataSize&0xFF0000)>>16);
FileData[19]=FileSizeByte;
FileSizeByte=(char)((DataSize&0xFF000000)>>24);
FileData[20]=FileSizeByte;


FILE* Save=fopen(Filename,"w");
fwrite(FileData,CompressedSize,1,Save);
fclose(Save);
free(FileData);
printf("File saved with filename %s\n",Filename);
}

int GetNumImages()
{
if(RideData==NULL)return 0;
return RideData->NumImages;
}
Image* GetImage(int Index)
{
if(RideData==NULL)return NULL;
if(Index>=RideData->NumImages)return NULL;
return RideData->Images+Index;
}
void SetImage(int Index,Image* Img)
{
if(RideData==NULL)return;
if(Index>=RideData->NumImages)return;
//Free previous image
int i;
for(i=0;i<RideData->Images[Index].Height;i++)free(RideData->Images[Index].Data[i]);
free(RideData->Images[Index].Data);
//Insert new image
RideData->Images[Index]=*Img;
}

String* SearchStrings(int TableNum,int Language)
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
char* GetString(int TableNum,int Language)
{
if(RideData==NULL)return NULL;
String* Str=SearchStrings(TableNum,Language);
if(Str==NULL)return NULL;
return Str->Str;
}
void SetString(int TableNum,int Language,char* NewStr)
{
if(RideData==NULL)return;
String* Str=SearchStrings(TableNum,Language);
if(Str==NULL)return;
free(Str->Str);
Str->Str=malloc(strlen(NewStr)+1);
strcpy(Str->Str,NewStr);
}

char* GetNameString(int Language)
{
return GetString(0,Language);
}
char* GetDescriptionString(int Language)
{
return GetString(1,Language);
}
char* GetCapacityString(int Language)
{
return GetString(2,Language);
}

void SetNameString(int Language,char* Str)
{
return SetString(0,Language,Str);
}
void SetDescriptionString(int Language,char* Str)
{
return SetString(1,Language,Str);
}
void SetCapacityString(int Language,char* Str)
{
return SetString(2,Language,Str);
}

void FreeRideFile()
{
//Free string tables
int i,j;
    for(i=0;i<3;i++)
    {
    int StringNum=0;
        while(RideData->StringTables[i][StringNum]!=NULL)
        {
        free(RideData->StringTables[i][StringNum]->Str);
        free(RideData->StringTables[i][StringNum]);
        StringNum++;
        }
    free(RideData->StringTables[i]);
    }
free(RideData->UnknownStructures);
//Free images
    for(i=0;i<RideData->NumImages;i++)
    {
        if(RideData->Images[i].Data!=NULL)
        {
            for(j=0;j<RideData->Images[i].Height;j++)
            {
            if(RideData->Images[i].Data[j]!=NULL)free(RideData->Images[i].Data[j]);
            }
        free(RideData->Images[i].Data);
        }
    }
free(RideData->Images);
free(RideData);
RideData=NULL;
}
