///////////////////////////////////////
// Spells of Fury 
// Chapter 02 CDib Example 01  
// CDib DIB Class Header File  
// File: CDib.hpp
// Author: Mike J. Norton
//
// pixelDyne copyright 1994,1995,1996
// Developer:  Date:     Comments 
// 
///////////////////////////////////////

///////////////////////////////////////
// Include Files
///////////////////////////////////////
#include <windows.h>
#include "cdib.hpp"

///////////////////////////////////////
// default constructor
///////////////////////////////////////
CDib::CDib()
{
    lpbinfo = NULL;
    lpBits = NULL;
    NewDIBObject(640, 480);
}

///////////////////////////////////////
// default destructor
///////////////////////////////////////
CDib::~CDib()
{
    FreeCDib();
}

///////////////////////////////////////
// member function to free memory
///////////////////////////////////////
void CDib::FreeCDib()
{
    // Free the memory.
    if (lpbinfo != NULL) 
        GlobalFreePtr(lpbinfo);
    if (lpBits != NULL) 
        GlobalFreePtr(lpBits);
} 

///////////////////////////////////////
// member function CreateBITMAPINFO
///////////////////////////////////////
BOOL CDib::CreateBITMAPINFO()
{
    UINT biSize;

	// determine the amount of memory needed to hold BITMAPINFO
    biSize = GetBITMAPINFOSize();

    // Allocate the BITMAPINFO data structure
	if (lpbinfo != NULL)
	  GlobalFreePtr(lpbinfo);

    lpbinfo = (LPBITMAPINFO) GlobalAllocPtr(GMEM_MOVEABLE,biSize);

    if (lpbinfo == NULL) 
    {
		SetCDibState(FAIL_LPBI_ALLOC);
		return FALSE;

    }

	// initialize the values to ZERO
    memset(lpbinfo, 0, biSize);

   return TRUE;
}

///////////////////////////////////////
// member function SetCDibState
///////////////////////////////////////
void CDib::SetCDibState( CDibState estate )
{
    state = estate;
} 

///////////////////////////////////////
// function NumberOfColorEntries
///////////////////////////////////////
static int NumberOfColorEntries(LPBITMAPINFO lpBiInfo) 
{
    BITMAPINFOHEADER* pbih;
    int nPalEntries, iBitCount;

 
    pbih = &(lpBiInfo->bmiHeader);
    iBitCount = pbih->biBitCount;

    switch (iBitCount) {
    case 1:
        nPalEntries = 2;
        break;
    case 4:
        nPalEntries = 16;
        break;
    case 8:
        nPalEntries = 256;
        break;
    default:
        nPalEntries = 0;
        break;
    }

	if (pbih->biClrUsed != 0)
       nPalEntries = pbih->biClrUsed;

    return nPalEntries;
}

///////////////////////////////////////
// member function CopyBITMAPINFO
///////////////////////////////////////
BOOL CDib::CopyBITMAPINFO( const CDib& srcDIB )
{
    UINT biSize;

	biSize = GetBITMAPINFOSize();
	CreateBITMAPINFO();

	// 
    memcpy(lpbinfo, srcDIB.lpbinfo, biSize);

	return TRUE;

}

///////////////////////////////////////
// read in the DIB file format (256 color BMP)
///////////////////////////////////////
BOOL CDib::ReadFile(LPSTR lpszFilename)
{
	BITMAPINFOHEADER   bih;
	OFSTRUCT           of;
	HFILE              fh;
	DWORD              dwFileStart;
	BITMAPFILEHEADER   bif;
	UINT               rgbQuadTableSize;
	UINT               lpbitsSize;
	UINT               nPalEntries;
    
	// open the DIB file to initialize OFSTRUCT
	fh = OpenFile(lpszFilename, &of, OF_READ);
	if (fh == -1)
	{ 
	  SetCDibState(FAIL_DIB_OPEN);
	  return FALSE;
	}

    // find the beginning of the DIB file	
 	dwFileStart = _llseek(fh, 0L, FILE_BEGIN);

	// BITMAPFILEHEADER

	// read in the BITMAPFILEHEADER
    if ((_lread(fh,&bif, sizeof(bif))) != sizeof(bif)) 
    {
		SetCDibState(FAIL_FHDR_READ);
		_lclose(fh);
		return FALSE;
    }

    // is the BITMAPFILEHEADER field bfType = 'BM' ?
    if (bif.bfType != 0x4D42) 
    {
		SetCDibState(FAIL_BFTYPE_INVALID);
		_lclose(fh);
		return FALSE;
    }
	

	// BITMAPINFOHEADER

	// read in the BITMAPINFOHEADER
    if ((_lread(fh, &bih, sizeof(bih))) != sizeof(bih)) 
    {
		SetCDibState(FAIL_IHDR_READ);
		_lclose(fh);
		return FALSE;
    }

    // BITMAPINFO

    CreateBITMAPINFO();      

	if (lpbinfo == NULL)
	{
	  _lclose(fh);
	  return FALSE;
	}

    // Copy the header we already have.
    memcpy(lpbinfo, &bih, sizeof(BITMAPINFOHEADER));

	// RGBQUAD

	// determine the RGBQUAD array size
    nPalEntries = NumberOfColorEntries((LPBITMAPINFO) &bih);
    rgbQuadTableSize = nPalEntries * sizeof(RGBQUAD);

    // Read the color table from the file.
    if ((_lread(fh,((LPBYTE) lpbinfo) + sizeof(BITMAPINFOHEADER),rgbQuadTableSize)) != rgbQuadTableSize) 
    {			   // 
        SetCDibState(FAIL_FORMAT_INVALID);
		_lclose(fh);
		return FALSE;
    }

	// assign the LPRGBQUAD pointer to the RGBQUAD array
	lprgb = GetLPRGBPtr();

	// Image Bits

    // Free any previous garbage
	if (lpBits != NULL) 
       GlobalFreePtr(lpBits);

	// determine the size of the image in the file
	lpbitsSize = bif.bfSize - bif.bfOffBits;

	// allocate memory to hold the image
    lpBits = (LPBYTE) GlobalAllocPtr(GMEM_MOVEABLE,lpbitsSize);
    if (lpBits == NULL) 
    {
		SetCDibState(FAIL_BITS_ALLOC);
		_lclose(fh);
		return FALSE;
    }

    // move the file handle to the start of the image
	_llseek(fh, dwFileStart + bif.bfOffBits, FILE_BEGIN);
	
    // read the image into memory
    if ((_lread(fh, lpBits, lpbitsSize)) != lpbitsSize) 
    {
		SetCDibState(FAIL_BITS_READ);
		_lclose(fh);
		return FALSE;
    }

	// all done, close the file
	_lclose(fh);
    return TRUE;
}

///////////////////////////////////////
// Initialize the DIB object's BITMAPINFOHEADER
///////////////////////////////////////
BOOL CDib::SetBitmapHeader(int biWidth, int biHeight)
{
	if (lpbinfo == NULL)
	   return FALSE;

    BITMAPINFOHEADER* lpbi = (BITMAPINFOHEADER*) lpbinfo;
    lpbi->biSize = sizeof(BITMAPINFOHEADER);
    lpbi->biWidth = biWidth;
    lpbi->biHeight = biHeight;
    lpbi->biPlanes = 1;
    lpbi->biBitCount = 8;
    lpbi->biCompression = BI_RGB;
    lpbi->biSizeImage = 0;
    lpbi->biXPelsPerMeter = 0;
    lpbi->biYPelsPerMeter = 0;
    lpbi->biClrUsed = 0;
    lpbi->biClrImportant = 0;

	return TRUE;
}

///////////////////////////////////////
// builds a default Gray scale color palette
///////////////////////////////////////
BOOL CDib::SetDefaultPalette()
{
    lprgb = GetLPRGBPtr();		
    for (int i = 0; i < 256; i++) 
    {
        lprgb->rgbBlue = lprgb->rgbGreen = lprgb->rgbRed = (BYTE) i;
        lprgb->rgbReserved = 0;
        lprgb++;
    }
	return TRUE;
}

///////////////////////////////////////
// Create a DIB object
///////////////////////////////////////
BOOL CDib::NewDIBObject(int biWidth, int biHeight)
{
    int binfoSize;

    // Delete any existing stuff.
    FreeCDib();

    // Allocate memory for the header
	binfoSize = (sizeof(BITMAPINFOHEADER) + 256*sizeof(RGBQUAD));
    lpbinfo = (BITMAPINFO*)GlobalAllocPtr(GMEM_MOVEABLE,binfoSize);

    if (!lpbinfo)
        return FALSE;

    // Fill in the header info.
	SetBitmapHeader(biWidth, biHeight);

	// set the image boundimng rectangle
	SetBounds();

	// Fill in the default color palette
	SetDefaultPalette();

    // Allocate image memory (DWORD) aligned.
    int lpbitsSize = ((biWidth + 3) & ~3) * biHeight;
    lpBits = (BYTE*)GlobalAllocPtr(GMEM_MOVEABLE, lpbitsSize);
    if (!lpBits) 
    {
        FreeCDib();
        return FALSE;
    }

	// Fill the image with ZEROs (black)
	UINT bindex;
	for (bindex = 0; bindex < (UINT)lpbitsSize; bindex++)
	  lpBits[bindex] = 0;

    return TRUE;
}

///////////////////////////////////////
// Draw the DIB to a given DC.
///////////////////////////////////////
void CDib::DrawBits(HDC hDC, int x, int y)
{
    ::StretchDIBits(hDC,
                    x,                          // Destination x
                    y,                          // Destination y
                    lpbinfo->bmiHeader.biWidth, // Destination width
                    lpbinfo->bmiHeader.biHeight,// Destination height
                    0,                          // Source x
                    0,                          // Source y
                    lpbinfo->bmiHeader.biWidth, // Source width
                    lpbinfo->bmiHeader.biHeight,// Source height
                    lpBits,                     // Pointer to bits
                    lpbinfo,                    // BITMAPINFO
                    DIB_RGB_COLORS,             // Options
                    SRCCOPY);                   // Raster operator code (ROP)
}

///////////////////////////////////////
// Draw the DIB to a given DC.
///////////////////////////////////////
void CDib::DrawBits(HDC hDC, RECT *r)
{
    DWORD dwWidth, dwHeight;

	dwWidth = r->right - r->left;
	dwHeight = r->bottom - r->top;

    ::StretchDIBits(hDC,
                    r->left,                          // Destination x
                    r->right,                          // Destination y
                    dwWidth, // Destination width
                    dwHeight,// Destination height
                    0,                          // Source x
                    0,                          // Source y
                    dwWidth, // Source width
                    dwHeight,// Source height
                    lpBits,                     // Pointer to bits
                    lpbinfo,                    // BITMAPINFO
                    DIB_RGB_COLORS,             // Options
                    SRCCOPY);                   // Raster operator code (ROP)
}

///////////////////////////////////////
// member function CreateLogicalPalette
///////////////////////////////////////
HPALETTE CDib::CreateLogicalPalette()
{
    UINT numColors = NumberOfColorEntries(lpbinfo) ;
	UINT i;

	struct {
	    WORD Version;
		WORD NumberOfEntries;
		PALETTEENTRY aEntries[256];
	}logicalPalette = { 0x300, 256 };

	for (i = 0; i < numColors; i++)
	{
	    logicalPalette.aEntries[i].peRed = lprgb[i].rgbRed;
	    logicalPalette.aEntries[i].peGreen = lprgb[i].rgbGreen;
	    logicalPalette.aEntries[i].peBlue = lprgb[i].rgbBlue;
		logicalPalette.aEntries[i].peFlags = 0;
	}

	HPALETTE hPalette = CreatePalette((LPLOGPALETTE)&logicalPalette);
	return hPalette;	
}

///////////////////////////////////////
// member function CopyDIBBits
///////////////////////////////////////
BOOL CDib::CopyDIBBits( const CDib& srcDIB, RECT srcRect, RECT dstRect)
{
    LPSTR srcBits, dstBits;			   // pointers to respective image bits
	int srcHeight, srcWidth;		   // dimensions of source pixels
	//int dstHeight, dstWidth;		   // dimensions of destination
	int srcScanLines, dstScanLines;    // number of bytes per scan line
  
	// attributes of source image
	srcHeight = srcRect.bottom - srcRect.top;
	srcWidth = srcRect.right - srcRect.left;
	// 
	srcScanLines = srcDIB.bounds.right - srcDIB.bounds.left;
	dstScanLines = bounds.right - bounds.left;

	srcBits = (LPSTR)srcDIB.lpBits;
	dstBits = (LPSTR)lpBits;
	
	BITMAPINFOHEADER* lpbi = (BITMAPINFOHEADER*) srcDIB.lpbinfo;
    UINT srcDIBBiHeight = (UINT)lpbi->biHeight;
	

    int row,col=0;
    for(row=0; row<srcHeight; row++)
	{
	   for (col=0;col<srcWidth;col++)
	   {
	      dstBits[ col + dstScanLines * (GetCDibBiHeight() - row - 1 - dstRect.top) + dstRect.left ] 
	         = srcBits[col + srcScanLines * (srcDIBBiHeight - row - 1 - srcRect.top) + srcRect.left];
	   }
	}
    return TRUE;
}

///////////////////////////////////////
// member function CopyTransDIBBits
///////////////////////////////////////
BOOL CDib::CopyTransDIBBits( const CDib& srcDIB, RECT srcRect, RECT dstRect, BYTE transByte)
{
    LPSTR srcBits, dstBits;			   // pointers to respective image bits
	int srcHeight, srcWidth;		   // dimensions of source pixels
	int srcScanLines, dstScanLines;    // number of bytes per scan line
      
	// attributes of source image
	srcHeight = srcRect.bottom - srcRect.top;
	srcWidth = srcRect.right - srcRect.left;
	// 
	srcScanLines = srcDIB.bounds.right - srcDIB.bounds.left;
	dstScanLines = bounds.right - bounds.left;

	srcBits = (LPSTR)srcDIB.lpBits;
	dstBits = (LPSTR)lpBits;
	
	BITMAPINFOHEADER* lpbi = (BITMAPINFOHEADER*) srcDIB.lpbinfo;
    UINT srcDIBBiHeight = (UINT)lpbi->biHeight;
	
	UINT transColor = 0;
	UINT srcIndex, dstIndex;
    BYTE transParentColor = LOBYTE(LOWORD(transByte));

    int row,col=0;
    for(row=0; row<srcHeight; row++)
	{
	   for (col=0;col<srcWidth;col++)
	   {
	      srcIndex = col + srcScanLines * (srcDIBBiHeight - row - 1 - srcRect.top) + srcRect.left;
		  dstIndex = col + dstScanLines * (GetCDibBiHeight() - row - 1 - dstRect.top) + dstRect.left; 

		  if ( (unsigned char) srcBits[srcIndex]!= (unsigned char)transByte)
		  {
	        dstBits[dstIndex] = srcBits[srcIndex];
		  }
	   }
	}
    return TRUE;
}

///////////////////////////////////////
// member function CopyLRGBQUAD
///////////////////////////////////////
BOOL CDib::CopyLRGBQUAD( const CDib& srcDIB)
{
    UINT i, numColors;
    
    numColors = NumberOfColorEntries(srcDIB.lpbinfo);
        
	for (i = 0; i < numColors; i++)
	{
	    lprgb[i].rgbRed = srcDIB.lprgb[i].rgbRed;
	    lprgb[i].rgbGreen = srcDIB.lprgb[i].rgbGreen;
	    lprgb[i].rgbBlue = srcDIB.lprgb[i].rgbBlue;
		//lprgb[i].peFlags = 0;
	}
	return TRUE;
}

///////////////////////////////////////
// member function GetPixelByte
///////////////////////////////////////
BYTE CDib::GetPixelByte( UINT x, UINT y)
{
   LPSTR srcBits = (LPSTR)lpBits;
   UINT  srcScanLines = bounds.right - bounds.left;
   UINT srcIndex;
   srcIndex = x + srcScanLines * (GetCDibBiHeight() - y - 1); 
   return (BYTE) lpBits[srcIndex];
}

///////////////////////////////////////
// member function GetPixelRGB
///////////////////////////////////////
COLORREF CDib::GetPixelRGB( UINT x, UINT y)
{
   LPSTR srcBits = (LPSTR)lpBits;
   UINT  srcScanLines = bounds.right - bounds.left;
   UINT srcIndex;
   srcIndex = x + srcScanLines * (GetCDibBiHeight() - y - 1); 
   return (COLORREF) lpBits[srcIndex];
}

///////////////////////////////////////
// member function SetBounds
///////////////////////////////////////
void CDib::SetBounds(int left, int top, int right, int bottom)
{
    bounds.left = left;
	bounds.top = top;
	bounds.right = right;
	bounds.bottom = bottom;
}

///////////////////////////////////////
// member function SetBounds
///////////////////////////////////////
void CDib::SetBounds()
{
   if (lpbinfo != NULL)
   {
       BITMAPINFOHEADER* lpbih = (BITMAPINFOHEADER*) lpbinfo;
	   bounds.left = 0;
	   bounds.top = 0;
	   bounds.right = lpbih->biWidth;
	   bounds.bottom = lpbih->biHeight;
   }
   else
   {
       bounds.left = bounds.top = bounds.right = bounds.bottom = 0;
   }
}

///////////////////////////////////////
// member function GetCDibBiHeight
///////////////////////////////////////
UINT CDib::GetCDibBiHeight()
{

  if (lpbinfo != NULL)
  {
    BITMAPINFOHEADER* lpbi = (BITMAPINFOHEADER*) lpbinfo;
    return ((UINT)lpbi->biHeight);
  }
  return 0;
}

///////////////////////////////////////
// member function GetCDibBiWidth
///////////////////////////////////////
UINT CDib::GetCDibBiWidth()
{

  if (lpbinfo != NULL)
  {
    BITMAPINFOHEADER* lpbi = (BITMAPINFOHEADER*) lpbinfo;
    return ((UINT)lpbi->biWidth);
  }
  return 0;
}
