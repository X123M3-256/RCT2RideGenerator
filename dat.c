#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <ctype.h>
/*#include "renderer.h"*/
#include "dat.h"

/*
typedef struct
{
uint8_t* data;
uint32_t size;
uint32_t allocated;
}buffer_t;
*/
buffer_t* buffer_new()
{
buffer_t* buffer=malloc(sizeof(buffer_t));
buffer->size=0;
buffer->allocated=1024;
buffer->data=malloc(buffer->allocated*sizeof(uint8_t));
return buffer;
}

/*Increases the size of the buffer without writing anything*/
void buffer_expand(buffer_t* buffer,uint32_t num_bytes)
{
uint32_t required_size=buffer->size+num_bytes;
    if(buffer->allocated<required_size)
    {
        do buffer->allocated*=2;
        while(buffer->allocated<required_size);
    buffer->data=realloc(buffer->data,buffer->allocated*sizeof(uint8_t));
    }
buffer->size+=num_bytes;
}

void buffer_write(buffer_t* buffer,uint8_t* bytes,uint32_t num_bytes)
{
uint32_t cur_size=buffer->size;
buffer_expand(buffer,num_bytes);
memcpy(buffer->data+cur_size,bytes,num_bytes);
}

void buffer_free(buffer_t* buffer)
{
free(buffer->data);
free(buffer);
}


uint32_t checksum_process_byte(uint32_t checksum,uint8_t byte)
{
uint32_t checksum_higher_bits=checksum&0xFFFFFF00u;
uint32_t checksum_lower_bits=checksum&0xFFu;
checksum_lower_bits^=byte;
checksum=checksum_higher_bits|checksum_lower_bits;
return (checksum<<11)|(checksum>>21);
}
uint32_t calculate_checksum(uint8_t* header,uint8_t* data,uint32_t size)
{
uint32_t i;
/*Check checksum*/
uint32_t checksum=0xF369A75B;
checksum=checksum_process_byte(checksum,header[0]);/*Do first byte*/
    for(i=4;i<12;i++)checksum=checksum_process_byte(checksum,header[i]);/*Do filename*/
    for(i=0;i<size;i++)checksum=checksum_process_byte(checksum,data[i]);/*Checksum rest file*/;
return checksum;
}



uint8_t count_repeated_bytes(uint8_t* bytes,uint32_t pos,uint32_t size)
{
uint8_t first_char=bytes[pos];
char repeated_bytes=1;
pos++;
    while(pos<size&&bytes[pos]==first_char&&repeated_bytes<125)
    {
    pos++;
    repeated_bytes++;
    }
return repeated_bytes;
}
uint8_t count_differing_bytes(uint8_t* bytes,uint32_t pos,uint32_t size)
{
char last_char;
char differing_bytes;
    if(pos>=size-2)return 1;
last_char=bytes[pos];
differing_bytes=0;
pos++;
    while(pos<size&&bytes[pos]!=last_char&&differing_bytes<125)
    {
    last_char=bytes[pos];
    pos++;
    differing_bytes++;
    }
return differing_bytes;
}
buffer_t* decompress_data(uint8_t* encoded_bytes,uint32_t size)
{
/*Allocate buffer to store decoded data*/
buffer_t* decoded_bytes=buffer_new();
    /*Decode RLE data*/
uint32_t pos=0;
    while(pos<size)
    {
        /*encoded_bytes is left unsigned for the sake of consistency, so test negativity by looking at the high order bit*/
        if(encoded_bytes[pos]&0x80)
        {
        uint8_t num_to_repeat=1-encoded_bytes[pos++];
            while(num_to_repeat>0)
            {
            buffer_write(decoded_bytes,encoded_bytes+pos,1);
            num_to_repeat--;
            }
        pos++;
        }
        else
        {
        uint16_t num_to_copy=(uint16_t)(encoded_bytes[pos++])+1;
        buffer_write(decoded_bytes,encoded_bytes+pos,num_to_copy);
        pos+=num_to_copy;
        }
    }
return decoded_bytes;
}
buffer_t* compress_data(uint8_t* decoded_bytes,uint32_t size)
{
buffer_t* encoded_bytes=buffer_new();

uint32_t pos=0;
    while(pos<size)
    {
    uint8_t repeated_bytes=count_repeated_bytes(decoded_bytes,pos,size);
        if(repeated_bytes>1)
        {
        /*Bytes are repeated*/
        uint8_t byte_to_wryte=~repeated_bytes+2;/*Same as 1-repeated_bytes, but repeated_bytes is unsigned*/
        buffer_write(encoded_bytes,&byte_to_wryte,1);/*Number of bytes to repeat*/
        buffer_write(encoded_bytes,decoded_bytes+pos,1);/*Byte to repeat*/
        pos+=repeated_bytes;
        }
        else
        {
        int bytes_to_copy=count_differing_bytes(decoded_bytes,pos,size);
        assert(bytes_to_copy!=0);
        uint8_t byte_to_wryte=bytes_to_copy-1;
        buffer_write(encoded_bytes,&byte_to_wryte,1);/*Number of bytes to copy*/
        buffer_write(encoded_bytes,decoded_bytes+pos,bytes_to_copy);/*Byte to copy*/
        pos+=bytes_to_copy;
        }
    }
return encoded_bytes;
}

ride_header_t* ride_header_new()
{
int i;
ride_header_t* ride_header=malloc(sizeof(ride_header_t));
memset(ride_header,0,sizeof(ride_header_t));
    for(i=0;i<NUM_CARS;i++)
    {
    ride_header->cars[i].unknown[0]=1;
    ride_header->cars[i].unknown[3]=1;
    ride_header->cars[i].unknown[4]=0x1;

    ride_header->cars[i].unknown[1]=2;
    ride_header->cars[i].unknown[2]=2;
    //object->ride_header->cars[i].unknown[3]=1;
    //object->ride_header->cars[i].unknown[4]=0;
    }
return ride_header;
}
ride_header_t* ride_header_load(uint8_t* bytes,uint32_t* pos_ptr)
{
int i;
ride_header_t* ride_header=malloc(sizeof(ride_header_t));
/*Read flags*/
ride_header->flags=*((uint32_t*)(bytes+8));

/*Read track style & preview index*/
ride_header->preview_index=0;
    for(i=12;i<=14;i++)
    {
        if(bytes[i]!=0xFFu)
        {
        ride_header->track_style=bytes[i];
        break;
        }
    ride_header->preview_index++;
    }

/*Read zero cars*/
ride_header->zero_cars=bytes[18];
/*Read car icon index*/
ride_header->car_icon_index=bytes[19];
/*Read minimum and maximum car length*/
ride_header->minimum_cars=bytes[15];
ride_header->maximum_cars=bytes[16];
/*Read track sections*/
ride_header->track_sections=*((uint64_t*)(bytes+438));
/*Read excitement, intensity, nausea*/
ride_header->excitement=bytes[434];
ride_header->intensity=bytes[435];
ride_header->nausea=bytes[436];
/*Read max height*/
ride_header->max_height=bytes[437];
/*Read categories*/
ride_header->categories[0]=bytes[446];
ride_header->categories[1]=bytes[447];

/*Read car type information*/
ride_header->car_types[CAR_INDEX_DEFAULT]=bytes[20];
ride_header->car_types[CAR_INDEX_FRONT]=bytes[21];
ride_header->car_types[CAR_INDEX_SECOND]=bytes[22];
ride_header->car_types[CAR_INDEX_REAR]=bytes[23];
ride_header->car_types[CAR_INDEX_THIRD]=bytes[24];

/*Read car structures*/
uint8_t* car_data=bytes+26;
    for(i=0;i<NUM_CARS;i++)
    {
    car_t* car=ride_header->cars+i;
    /*Load highest rotation index*/
    car->highest_rotation_index=car_data[0];
    /*Load spacing*/
    car->spacing=*((uint16_t*)(car_data+5));
    /*Load car friction*/
    car->friction=*((uint16_t*)(car_data+8));
    /*Load riders*/
    car->rider_pairs=car_data[11]&0x80;
    car->riders=car_data[11]&0x7F;
    /*Load sprite flags*/
    car->sprites=*((uint16_t*)(car_data+12));
    /*Load rider sprites*/
    car->rider_sprites=car_data[84];
    /*Load flags*/
    car->flags=*((uint32_t*)(car_data+17));
    /*Load spin parameters*/
    car->spin_inertia=car_data[85];
    car->spin_friction=car_data[86];
    /*Load powered velocity*/
    car->powered_acceleration=car_data[91];
    car->powered_velocity=car_data[92];
    /*Load Z value*/
    car->z_value=car_data[95];
    /*Read unknown fields*/
    car->unknown[0]=(uint16_t)car_data[4];
    car->unknown[1]=*((uint16_t*)(car_data+87));
    car->unknown[2]=*((uint16_t*)(car_data+89));
    car->unknown[3]=(uint16_t)car_data[94];
    car->unknown[4]=*((uint16_t*)(car_data+96));
    /*Move to next car structure*/
    car_data+=101;
    }
*pos_ptr+=0x1C2;
return ride_header;
}
void ride_header_write(ride_header_t* header,buffer_t* buffer)
{
int i;
uint8_t header_bytes[0x1C2];
memset(header_bytes,0,0x1C2);

/*Write flags*/
*((uint32_t*)(header_bytes+8))=header->flags;
/*Write track style & preview index*/
memset(header_bytes+12,0xFFu,3);
header_bytes[12+header->preview_index]=header->track_style;
/*Write zero cars*/
header_bytes[18]=header->zero_cars;
/*Write car icon index*/
header_bytes[19]=header->car_icon_index;
/*Write minimum and maximum car length*/
header_bytes[15]=header->minimum_cars;
header_bytes[16]=header->maximum_cars;
/*Write track sections*/
*((uint64_t*)(header_bytes+438))=header->track_sections;

/*Write excitement, intensity, nausea*/
header_bytes[434]=header->excitement;
header_bytes[435]=header->intensity;
header_bytes[436]=header->nausea;
/*Write max height*/
header_bytes[437]=header->max_height;
/*Write categories*/
header_bytes[446]=header->categories[0];
header_bytes[447]=header->categories[1];
/*Write car type information*/
header_bytes[20]=header->car_types[CAR_INDEX_DEFAULT];
header_bytes[21]=header->car_types[CAR_INDEX_FRONT];
header_bytes[22]=header->car_types[CAR_INDEX_SECOND];
header_bytes[23]=header->car_types[CAR_INDEX_REAR];
header_bytes[24]=header->car_types[CAR_INDEX_THIRD];
/*Write values that are known, but not applicable*/
header_bytes[17]=0xFF;/*Flat ride "cars"*/
header_bytes[448]=0xFF;/*First shop item*/
header_bytes[449]=0xFF;/*Second shop item*/

/*Write car types*/
uint8_t* car_data=header_bytes+26;
    for(i=0;i<NUM_CARS;i++)
    {
    car_t* car=header->cars+i;
    /*Write highest rotation index*/
    car_data[0]=car->highest_rotation_index;
    /*Write spacing*/
    *((uint16_t*)(car_data+5))=car->spacing;
    /*Write friction*/
    *((uint16_t*)(car_data+8))=car->friction;
    /*Write riders*/
    car_data[11]=car->rider_pairs|car->riders;
    /*Write sprite flags*/
    *((uint16_t*)(car_data+12))=car->sprites;
    /*Write rider sprites*/
    car_data[84]=car->rider_sprites;
    /*Write flags*/
    *((uint32_t*)(car_data+17))=car->flags;
    /*Write spin parameters*/
    car_data[85]=car->spin_inertia;
    car_data[86]=car->spin_friction;
    /*Write powered velocity*/
    car_data[91]=car->powered_acceleration;
    car_data[92]=car->powered_velocity;
    /*Write Z value*/
    car_data[95]=car->z_value;
    /*Write unknown fields*/
    car_data[4]=(uint8_t)car->unknown[0];
    *((uint16_t*)(car_data+87))=car->unknown[1];
    *((uint16_t*)(car_data+89))=car->unknown[2];
    car_data[94]=(uint8_t)car->unknown[3];
    *((uint16_t*)(car_data+96))=car->unknown[4];
    /*Move to next car*/
    car_data+=101;
    }
buffer_write(buffer,header_bytes,0x1C2);
}
void ride_header_free(ride_header_t* header)
{
free(header);
}

ride_structures_t* ride_structures_new()
{
int i;
ride_structures_t* structures=malloc(sizeof(ride_structures_t));
structures->num_default_colors=8;
structures->default_colors=malloc(8*sizeof(color_scheme_t));
structures->default_colors[0].colors[0]=18;
structures->default_colors[0].colors[1]=0;
structures->default_colors[0].colors[2]=18;
structures->default_colors[1].colors[0]=0;
structures->default_colors[1].colors[1]=18;
structures->default_colors[1].colors[2]=0;
structures->default_colors[2].colors[0]=7;
structures->default_colors[2].colors[1]=6;
structures->default_colors[2].colors[2]=18;
structures->default_colors[3].colors[0]=7;
structures->default_colors[3].colors[1]=0;
structures->default_colors[3].colors[2]=18;
structures->default_colors[4].colors[0]=11;
structures->default_colors[4].colors[1]=28;
structures->default_colors[4].colors[2]=2;
structures->default_colors[5].colors[0]=9;
structures->default_colors[5].colors[1]=28;
structures->default_colors[5].colors[2]=2;
structures->default_colors[6].colors[0]=28;
structures->default_colors[6].colors[1]=1;
structures->default_colors[6].colors[2]=6;
structures->default_colors[7].colors[0]=23;
structures->default_colors[7].colors[1]=1;
structures->default_colors[7].colors[2]=18;
    for(i=0;i<4;i++)
    {
    structures->peep_positions[i].num=0;
    structures->peep_positions[i].positions=NULL;
    }
return structures;
}
ride_structures_t* ride_structures_load(uint8_t* bytes,uint32_t* pos_ptr)
{
int i;
int pos=*pos_ptr;
ride_structures_t* structures=malloc(sizeof(ride_structures_t));
/*Sequence of 3 byte structures representing the default color schemes; I don't know why -1 means 32*/
structures->num_default_colors=bytes[pos]==0xFF?32:bytes[pos];
structures->default_colors=malloc(structures->num_default_colors*sizeof(color_scheme_t));
pos++;

	for(i=0;i<structures->num_default_colors;i++)
    {
    structures->default_colors[i].colors[0]=bytes[pos];
    structures->default_colors[i].colors[1]=bytes[pos+1];
    structures->default_colors[i].colors[2]=bytes[pos+2];
    pos+=3;
    }

    /*Four variable length structures*/
    for (i=0;i<4;i++)
    {
    uint16_t len=bytes[pos];
    pos++;
        /*If the length can't fit in a byte, the length is instead stored in the two bytes following*/
        if (len==0xFF)
        {
        len=*((uint16_t*)(bytes+pos));
        pos+=2;
        }
    structures->peep_positions[i].num=len;
    structures->peep_positions[i].positions=malloc(structures->peep_positions[i].num);
    int j;
        for(j=0;j<len;j++)
        {
        structures->peep_positions[i].positions[j]=bytes[pos];
        pos++;
        }
    }
*pos_ptr=pos;
return structures;
}
void ride_structures_write(ride_structures_t* structures,buffer_t* buffer)
{
int i;
/*Write number of 3 byte structures*/
buffer_write(buffer,&structures->num_default_colors,1);
/*Write 3 byte structures*/
    for(i=0;i<structures->num_default_colors;i++)
    {
    buffer_write(buffer,&structures->default_colors[i].colors[0],1);
    buffer_write(buffer,&structures->default_colors[i].colors[1],1);
    buffer_write(buffer,&structures->default_colors[i].colors[2],1);
    }
/*Write 4 byte peep positioning structures*/
    for(i=0;i<4;i++)
    {
    /*If the length will fit in a byte, write it*/
        if(structures->peep_positions[i].num<0xFFu)buffer_write(buffer,(uint8_t*)&structures->peep_positions[i].num,1);
    /*Otherwise write as a short*/
        else
        {
        uint8_t value=0xFFu;
        buffer_write(buffer,&value,1);
        buffer_write(buffer,(uint8_t*)&structures->peep_positions[i].num,2);
        }
    /*Write structure*/
    buffer_write(buffer,structures->peep_positions[i].positions,structures->peep_positions[i].num);
    }
}
void ride_structures_free(ride_structures_t* structures)
{
int i;
    for(i=0;i<4;i++)free(structures->peep_positions[i].positions);
free(structures);
}
string_table_t* string_table_new()
{
string_table_t* table=malloc(sizeof(string_table_t));
table->num_strings=1;
table->strings=malloc(sizeof(string_table_entry_t));
table->strings[0].language=LANGUAGE_ENGLISH_UK;
table->strings[0].str=malloc(7);
strcpy(table->strings[0].str,"A Ride");
return table;
}
string_table_t* string_table_load(uint8_t* bytes,uint32_t* pos_ptr)
{
uint32_t pos=*pos_ptr;
/*Allocate string table*/
string_table_t* table=malloc(sizeof(string_table_t));
table->strings=NULL;
table->num_strings=0;
    do
    {
    /*Allocate memory for string*/
    table->strings=realloc(table->strings,(table->num_strings+1)*sizeof(string_table_entry_t));
    /*Load language*/
    table->strings[table->num_strings].language=bytes[pos++];
    /*Load string*/
    uint8_t* start_ptr=bytes+pos;
    /*Get length of string*/
    int length=0;
        while(bytes[pos++]!=0)length++;
    /*Allocate memory for string*/
    table->strings[table->num_strings].str=malloc(length+1);
    /*Load string*/
    memcpy(table->strings[table->num_strings].str,start_ptr,length+1);
    /*Finished loading string*/
    table->num_strings++;
    }while(bytes[pos]!=0xFF);
pos++;
*pos_ptr=pos;
return table;
}
void string_table_write(string_table_t* table,buffer_t* buffer)
{
int i;
    for(i=0;i<table->num_strings;i++)
    {
    uint8_t language=table->strings[i].language;
    buffer_write(buffer,&language,sizeof(uint8_t));
    buffer_write(buffer,(uint8_t*)table->strings[i].str,strlen(table->strings[i].str)+1);
    }
uint8_t terminator=0xFF;
buffer_write(buffer,&terminator,1);
}
char* string_table_get_string_by_language(string_table_t* table,language_t language)
{
int i;
char* str=NULL;
    for(i=0;i<table->num_strings;i++)
    {
        if(table->strings[i].language==language)
        {
        str=table->strings[i].str;
        break;
        }
    }
return str;
}
void string_table_set_string_by_language(string_table_t* table,language_t language,char* str)
{
int i;
    for(i=0;i<table->num_strings;i++)
    {
        if(table->strings[i].language==language)
        {
        table->strings[i].str=realloc(table->strings[i].str,strlen(str)+1);
        strcpy(table->strings[i].str,str);
        return;
        }
    }
}
void string_table_free(string_table_t* table)
{
int i;
    for(i=0;i<table->num_strings;i++)free(table->strings[i].str);
free(table->strings);
free(table);
}

image_list_t* image_list_new()
{
image_list_t* list=malloc(sizeof(image_list_t));
list->num_images=0;
list->images=NULL;
return list;
}
image_list_t* image_list_load(uint8_t* bytes,uint32_t* pos_ptr)
{
uint32_t i,j;
uint32_t pos=*pos_ptr;
/*Allocate image list*/
image_list_t* list=image_list_new();
/*Get number of images*/
list->num_images=*((uint32_t*)(bytes+pos));
pos+=4;
/*These four bytes store the size of the graphic data, but I don't use them*/
pos+=4;

list->images=malloc(list->num_images*sizeof(image_t*));

/*Calculate offset of start of bitmap data*/
uint32_t bitmap_base;
bitmap_base=pos+(list->num_images*16);

    for(i=0;i<list->num_images;i++)
    {
    list->images[i]=malloc(sizeof(image_t));
    image_t* image=list->images[i];

    uint32_t data_offset=*((uint32_t*)(bytes+pos));
    /*Load image record*/
    image->width=*((uint16_t*)(bytes+pos+4));
    image->height=*((uint16_t*)(bytes+pos+6));
    image->x_offset=*((uint16_t*)(bytes+pos+8));
    image->y_offset=*((uint16_t*)(bytes+pos+10));
    image->flags=*((uint16_t*)(bytes+pos+12));
    pos+=16;

    /*Allocate rows*/
    image->data=malloc(image->height*sizeof(uint8_t*));
    /*Allocate columns*/
        for (j=0;j<image->height;j++)
        {
        image->data[j]=malloc(image->width*sizeof(uint8_t));
        memset(image->data[j],0,image->width*sizeof(uint8_t));
        }

    /*Compute offset of start of bitmap data for this image*/
    int offset=bitmap_base+data_offset;

		if(image->flags==1)/*Simple bitmap.*/
		{
            for (j=0;j<image->height;j++)
			{
			memcpy(image->data[j],bytes+offset,image->width);
			offset+=image->width;
			}
		}
		else if (image->flags==5) /*Compressed bitmap.*/
		{
			for (j=0;j<image->height;j++)
			{
            /*The data starts with a list of two byte offsets to the individual row data.*/
            uint32_t row_offset=offset+*((uint16_t*)(bytes+offset+(j*2)));
            uint8_t last_one;
				do
				{
                /*First byte stores the length of the element*/
                uint8_t length=bytes[row_offset++];
                /*Next byte stores distance from start of scanline*/
                uint8_t x_displacement=bytes[row_offset++];

                /*The most significant bit of the length is set only if this is the last element*/
                last_one=length&0x80;
                length=length&0x7F;
                /*Check that this element isn't outside the bounds of the image*/
                    if(x_displacement+length>image->width)
                    {
                    printf("File contains invalid bitmaps\n");
                    return NULL;
                    }
                memcpy(image->data[j]+x_displacement,bytes+row_offset,length);
                row_offset+=length;
				}while (!last_one);
			}
		}
    }
*pos_ptr=pos;
return list;
}
void image_list_write(image_list_t* list,buffer_t* buffer)
{
uint32_t i,j;
/*Write number of images*/
buffer_write(buffer,(uint8_t*)&list->num_images,4);
/*Save the offset of the graphic data size field, so we can write it later*/
uint32_t graphic_data_size_offset=buffer->size;
buffer_expand(buffer,4);
/*Save offset of image records to write later*/
int image_records_offset=buffer->size;
buffer_expand(buffer,16*list->num_images);

uint32_t bitmap_base=buffer->size;
/*Write images*/
    for(i=0;i<list->num_images;i++)
    {
    image_t* image=list->images[i];
    /*Write image record*/
    uint32_t start_address=buffer->size-bitmap_base;
    uint8_t* image_record_ptr=buffer->data+image_records_offset+i*16;
    *((uint32_t*)(image_record_ptr))=start_address;
    *((uint16_t*)(image_record_ptr+4))=image->width;
    *((uint16_t*)(image_record_ptr+6))=image->height;
    *((uint16_t*)(image_record_ptr+8))=image->x_offset;
    *((uint16_t*)(image_record_ptr+10))=image->y_offset;
    *((uint16_t*)(image_record_ptr+12))=image->flags;
    *((uint16_t*)(image_record_ptr+14))=0;
    /*Write image data*/
        if(image->flags==1)
        {
            for(j=0;j<image->height;j++)buffer_write(buffer,image->data[j],image->width);
        }
        else /*image->flags==5*/
        {
        /*Store offset of the list of row offsets, and leave space for them in the buffer*/
        uint32_t row_offset_list_offset=buffer->size;
        buffer_expand(buffer,image->height*2);
            for(j=0;j<image->height;j++)
            {
            /*Write offset of row into row offset list*/
            *((uint16_t*)(buffer->data+row_offset_list_offset+j*2))=buffer->size-row_offset_list_offset;

            uint8_t length=0;
            uint8_t x_displacement=0;
            /*Calculate initial x_displacement*/
                while(x_displacement<image->width&&image->data[j][x_displacement]==0)x_displacement++;

                do
                {
                length=0;
                /*Find length. Sections can't be longer that 126 for some reason*/
                    while(x_displacement+length<image->width&&image->data[j][x_displacement+length]!=0&&length<126)length++;
                /*Compute new x_displacement*/
                uint8_t new_x_displacement=x_displacement+length;
                    while(new_x_displacement<image->width&&image->data[j][new_x_displacement]==0)new_x_displacement++;
                /*If there are no more non transparent pixels, set last one flat*/
                    if(new_x_displacement==image->width)length|=0x80;
                /*Write length*/
                buffer_write(buffer,&length,1);
                /*Write x displacement*/
                buffer_write(buffer,&x_displacement,1);
                /*Write data*/
                buffer_write(buffer,image->data[j]+x_displacement,length&0x7F);
                /*Update the x displacement*/
                x_displacement=new_x_displacement;
                }while(!(length&0x80));
            }
        }
    }
/*Write size of graphic data*/
uint32_t graphic_data_size=buffer->size-bitmap_base;
memcpy(buffer->data+graphic_data_size_offset,&graphic_data_size,4);
}
void image_list_set_image(image_list_t* list,int index,image_t* image)
{
    if(list->images[index]!=NULL)image_free(list->images[index]);
list->images[index]=image;
}
void image_list_set_num_images(image_list_t* list,uint32_t num_images)
{
uint32_t i;
    if(num_images<list->num_images)for(i=num_images;i<list->num_images;i++)image_free(list->images[i]);
list->images=realloc(list->images,num_images*sizeof(image_t*));
    if(num_images>list->num_images)for(i=list->num_images;i<num_images;i++)list->images[i]=NULL;
list->num_images=num_images;
}
void image_list_free(image_list_t* list)
{
uint32_t i;
    for(i=0;i<list->num_images;i++)image_free(list->images[i]);
free(list->images);
free(list);
}


object_t* object_new_ride()
{
object_t* object=malloc(sizeof(object_t));
object->ride_header=ride_header_new();
object->string_tables[0]=string_table_new();
object->string_tables[1]=string_table_new();
object->string_tables[2]=string_table_new();
object->optional=ride_structures_new();
object->images=image_list_new();
return object;
}
#define HEADER_SIZE 21
object_t* object_load_dat(const char* filename)
{
int i;
FILE* file=fopen(filename,"rb");
    if(file==NULL)
    {
    printf("Cannot open file\n");
    return NULL;
    }
/*Get file size*/
    if(fseek(file,0,SEEK_END)!=0)
    {
    printf("Failed to seek file\n");
    fclose(file);
    return NULL;
    };
int file_size=ftell(file);
    if(file_size==-1)
    {
    printf("Failed to determine size of file\n");
    fclose(file);
    return NULL;
    }
    if(fseek(file,SEEK_SET,0)!=0)
    {
    printf("Failed to seek file\n");
    fclose(file);
    return NULL;
    }
/*Read data into array*/
uint8_t* encoded_bytes=malloc(file_size);
fread(encoded_bytes,file_size,1,file);
fclose(file);



/*Load header*/
uint8_t header[HEADER_SIZE];
memcpy(header,encoded_bytes,HEADER_SIZE);


/*Check that this is a ride*/
    if((header[0]&0xF)!=0)
    {
    printf("Not a ride\n");
    free(encoded_bytes);
    return NULL;
    }

/*Check the header to see if the remaining data is compressed- if so, then decode it*/
buffer_t* bytes;
if(header[0x10])
    {
    bytes=decompress_data(encoded_bytes+HEADER_SIZE,file_size-HEADER_SIZE);
    }
    else
    {
    /*Data is not compressed, copy it to output*/
    bytes=buffer_new();
    buffer_write(bytes,encoded_bytes+HEADER_SIZE,file_size-HEADER_SIZE);
    }
free(encoded_bytes);


    if(calculate_checksum(header,bytes->data,bytes->size)!=*((uint32_t*)(header+12)))
    {
    printf("Checksum does not match\n");
    buffer_free(bytes);
    return NULL;
    }


/*Allocate object*/
object_t* object=malloc(sizeof(object_t));

uint32_t pos=0;
/*Load object header*/
object->ride_header=ride_header_load(bytes->data,&pos);
/*Load string tables*/
        for(i=0;i<NUM_STRING_TABLES;i++)
        {
        object->string_tables[i]=string_table_load(bytes->data,&pos);
        }
/*Load unknown strutures*/
object->optional=ride_structures_load(bytes->data,&pos);
/*Load images*/
object->images=image_list_load(bytes->data,&pos);

/*Free decoded data*/
buffer_free(bytes);
return object;
}
void object_save_dat(object_t* object,const char* filename)
{
int i;
/*Extract the filename from the path*/
int filename_length=strlen(filename);
int start_index=filename_length;
int end_index=filename_length;
    while(filename[start_index]!='/'&&filename[start_index]!='\\')start_index--;
    start_index++;
    while(filename[end_index]!='.'&&end_index>start_index)end_index--;
/*If there's no extension, set end_index to the end of the file*/
    if(start_index==end_index)end_index=filename_length-1;
    else end_index--;
/*Check filename length*/
    if(end_index-start_index>8)
    {
    printf("Filename too long (8 characters not including extension)\n");
    return;
    }

buffer_t* decoded_file=buffer_new();
/*Write header*/
ride_header_write(object->ride_header,decoded_file);
/*Write string tables*/
    for(i=0;i<NUM_STRING_TABLES;i++)
    {
    string_table_write(object->string_tables[i],decoded_file);
    }
/*Write unknown structures*/
ride_structures_write(object->optional,decoded_file);
/*Write images*/
image_list_write(object->images,decoded_file);

/*Encode file with RLE*/
buffer_t* encoded_bytes=compress_data(decoded_file->data,decoded_file->size);

/*Create header*/
uint8_t header[HEADER_SIZE];
memset(header,0,HEADER_SIZE);

/*This is a custom object, and it's a ride.*/
header[0]=0x00;
/*Write filename in header*/
uint8_t* header_filename=header+4;
memset(header_filename,' ',8);
    for(i=start_index;i<=end_index;i++)
    {
    header_filename[i-start_index]=toupper(filename[i]);
    }
/*Specify that data is compressed*/
header[0x10]=1;
/*Calculate checksum*/
uint32_t checksum =calculate_checksum(header,decoded_file->data,decoded_file->size);
/*Write checksum*/
*((uint32_t*)(header+12))=checksum;
/*Write file size*/
*((uint32_t*)(header+17))=encoded_bytes->size;

/*Free decode data*/
buffer_free(decoded_file);


FILE* file=fopen(filename,"wb");
fwrite(header,1,HEADER_SIZE,file);
fwrite(encoded_bytes->data,1,encoded_bytes->size,file);
fclose(file);

buffer_free(encoded_bytes);
printf("File saved with filename %s\n",filename);
}
void object_free(object_t* object)
{
ride_header_free(object->ride_header);
string_table_free(object->string_tables[0]);
string_table_free(object->string_tables[1]);
string_table_free(object->string_tables[2]);
ride_structures_free(object->optional);
image_list_free(object->images);
}



buffer_t* track_decode(char* filename)
{
FILE* file=fopen(filename,"rb");
fseek(file,0,SEEK_END);
int file_size=ftell(file);
fseek(file,SEEK_SET,0);
/*Read data into array*/
uint8_t* encoded_bytes=malloc(file_size);
fread(encoded_bytes,file_size,1,file);
fclose(file);
return decompress_data(encoded_bytes,file_size);
}
void track_encode(buffer_t* data,char* filename)
{
buffer_t* buffer=compress_data(data->data,data->size);
FILE* file=fopen(filename,"w");
fwrite(buffer->data,buffer->size,1,file);
fclose(file);
}
