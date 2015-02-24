#include "plot.h"

#include "Graphics.h"

//! Plot a single pixel on a 8-bit buffer
/*!
	\param x		X-coord.
	\param y		Y-coord.
	\param clr		Color.
	\param dstBase	Canvas pointer (halfword-aligned plz).
	\param dstP		Canvas pitch in bytes.
	\note	Slow as fuck. Inline plotting functionality if possible.
*/
void bmp8_plot(int x, int y, u32 clr, Graphics* g)
{
	if(x < 0 || x > g->width * 2 - 1 || y < 0 || y > g->height)
	{
		return;	
	} 

	void* dstBase = g->currentBuffer;
	u32 dstP = g->width * 2;

	u16 *dstD= (u16*)(dstBase+y*dstP+(x&~1));

	if(x&1)
	   *dstD= (*dstD& 0xFF) | (clr<<8); 
	else
	   *dstD= (*dstD&~0xFF) | (clr&0xFF);
}

int sign(int x)
{
	return (x > 0) ? 1 : ( (x < 0) ? -1 : 0 );
}

int absolute(int x)
{
	if(x < 0)
	{
		return -1 * x;	
	}
	
	return x;
}

void slow_line(int x1, int y1, int x2, int y2, u32 clr, 
	Graphics* g)
{
	int deltax = x2 - x1;
	int deltay = y2 - y1;

	int fixed_deltax = deltax << 8;
	int fixed_deltay = deltay << 8;

	if(deltax == 0)
	{
		int y;
		for(y = y1; y != y2; y = y + sign(deltay))
		{
			bmp8_plot(x1, y, clr, g);
		}
		return;	
	}
	
	int error = 0;
	int deltaerr = absolute((fixed_deltay * 0x100) / fixed_deltax);    // Assume deltax != 0 (line is not vertical),
						  // note that this division needs to be done in a way that preserves the fractional part
	int y = y1;
	int x;

	for(x = x1; x != x2; x = x + sign(deltax))
	{
		bmp8_plot(x, y, clr, g);
		error = error + deltaerr;
		while(error >= 0x080)
		{
			bmp8_plot(x, y, clr, g);
			y = y + sign(deltay);
			error = error - 0x100;
		}
	}
}
