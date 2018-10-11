
#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>
#include <jpeglib.h>
#include <jerror.h>
#include <jerror.h>
#include <jerror.h>
#include <string.h>
#include <unistd.h>
#include <lcd.h>

#define width 	800
#define height 	480

extern JSAMPLE * image_buffer;	/* Points to large array of R,G,B-order data */
extern int image_height;	/* Number of rows in image */
extern int image_width;		/* Number of columns in image */

struct my_error_mgr {
  struct jpeg_error_mgr pub;    /* "public" fields */

  jmp_buf setjmp_buffer;        /* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;



METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}


/*
 * Sample routine for JPEG decompression.  We assume that the source file name
 * is passed in.  We want to return 1 on success, 0 on error.
 */



int display_format_jpeg(const char *filename,  unsigned int *lcd_buf_point)
{
  
	struct jpeg_decompress_struct cinfo;
  
	struct my_error_mgr jerr;
	FILE * infile;		/* source file */
	int row_stride;		/* physical row width in output buffer */

	int x = 0, y = 0;
	

 
	if((infile = fopen(filename, "rb")) == NULL) //filename
	{
		fprintf(stderr, "can't open %s\n", filename);
		return 0;
	}

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
  
	if(setjmp(jerr.setjmp_buffer)) 
	{    
		jpeg_destroy_decompress(&cinfo);
		fclose(infile);
		return 0;
	}

	jpeg_create_decompress(&cinfo);

	jpeg_stdio_src(&cinfo, infile);
	
	jpeg_read_header(&cinfo, TRUE);
    
	jpeg_start_decompress(&cinfo);
  
	row_stride = cinfo.output_width * cinfo.output_components;

	char *buffer;		/* Output row buffer */
  
	buffer =(char *)(*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	
	unsigned int color;
	char *buf_save = buffer;

	while(cinfo.output_scanline < cinfo.output_height ) 
	{   
		buffer = buf_save;
		jpeg_read_scanlines(&cinfo, (JSAMPARRAY)&buffer, 1);
		{
			for(x=0; x<width ; x++)
			{
				color = buffer[2] | buffer[1]<<8 |buffer[0]<<16;

				draw_a_point_to_lcd(x, y, color, lcd_buf_point);

				buffer += 3;
			}
		}
		y += 1;
	}


	//jpeg_finish_decompress(&cinfo);
 
	//jpeg_destroy_decompress(&cinfo);
	//fclose(infile)
	return 0;
}


