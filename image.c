#include <gtk/gtk.h>
#include "palette.h"
#include "dat.h"

GdkPixbuf* CreateBlankPixbuf()
{
GdkPixbuf* BlankPixbuf=gdk_pixbuf_new(GDK_COLORSPACE_RGB,FALSE,8,256,256);
int rowstride=gdk_pixbuf_get_rowstride(BlankPixbuf);
guchar* pixels=gdk_pixbuf_get_pixels(BlankPixbuf);
int i,j;
    for(i=0;i<256;i++)
    {
        for(j=0;j<256*3;j++)
        {
        pixels[j]=0;
        }
    pixels+=rowstride;
    }
return BlankPixbuf;
}


void ShowImageInPixbuf(GdkPixbuf* pixbuf,Image* RideImage)
{
int rowstride=gdk_pixbuf_get_rowstride(pixbuf);
guchar* pixels=gdk_pixbuf_get_pixels(pixbuf);
//Clear pixbuf
int i,j;
    for(i=0;i<256;i++)
    {
        for(j=0;j<256*3;j++)
        {
        pixels[j]=0;
        }
    pixels+=rowstride;
    }

//Draw new image to pixbuf
int XOffset=128+RideImage->XOffset;//Draw image in centre of screen
int YOffset=128+RideImage->YOffset;
pixels=gdk_pixbuf_get_pixels(pixbuf);
pixels+=YOffset*rowstride;

Color curcol;
    for(i=0;i<RideImage->Height;i++)
    {
        for(j=0;j<RideImage->Width;j++)
        {
        //int index=RideImage->Data[i][j];
        //if(index!=0)printf("Index %d\n",(unsigned char)index);
        curcol=GetColorFromPalette(RideImage->Data[i][j]);
        int pixelindex=(j+XOffset)*3;
        pixels[pixelindex]=curcol.Red;
        pixelindex++;
        pixels[pixelindex]=curcol.Green;
        pixelindex++;
        pixels[pixelindex]=curcol.Blue;
        pixelindex++;
        }
    pixels+=rowstride;
    }

}
