#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "clahe.h"

/************************** main function CLAHE ******************/   
int CLAHE_CR (kz_pixel_t* pImage, unsigned int uiXRes, unsigned int uiYRes,   
           kz_pixel_t Min, kz_pixel_t Max, unsigned int uiNrX, unsigned int uiNrY,   
           unsigned int uiNrBins, float fCliplimit)   
           /*  pImage - Pointer to the input/output image  
           *   uiXRes - Image resolution in the X direction  
           *   uiYRes - Image resolution in the Y direction  
           *   Min - Minimum greyvalue of input image (also becomes minimum of output image)  
           *   Max - Maximum greyvalue of input image (also becomes maximum of output image)  
           *   uiNrX - Number of contextial regions in the X direction (min 2, max uiMAX_REG_X)  
           *   uiNrY - Number of contextial regions in the Y direction (min 2, max uiMAX_REG_Y)  
           *   uiNrBins - Number of greybins for histogram ("dynamic range")  
           *   float fCliplimit - Normalized cliplimit (higher values give more contrast)  
           *   
           */   
{   
   
    unsigned int uiX, uiY;        /* counters */   
    unsigned int uiXSize, uiYSize, uiSubX, uiSubY; /* size of context. reg. and subimages */   
    unsigned int uiXL, uiXR, uiYU, uiYB;  /* auxiliary variables interpolation routine */   
    unsigned long ulClipLimit, ulNrPixels;/* clip limit and region pixel count */   
    kz_pixel_t* pImPointer;        /* pointer to image */   
    kz_pixel_t aLUT[uiNR_OF_GREY];      /* lookup table used for scaling of input image */   
    unsigned long* pulHist, *pulMapArray; /* pointer to histogram and mappings*/   
    unsigned long* pulLU, *pulLB, *pulRU, *pulRB; /* auxiliary pointers interpolation */   
       
    if (uiNrX > uiMAX_REG_X) return -1;     /* # of regions x-direction too large */   
    if (uiNrY > uiMAX_REG_Y) return -2;     /* # of regions y-direction too large */   
             
    //    if (uiXRes % uiNrX) return -3;      /* x-resolution no multiple of uiNrX */    
    //    if (uiYRes % uiNrY) return -4;      /* y-resolution no multiple of uiNrY */    
       
    if (Max >= uiNR_OF_GREY) return -5;     /* maximum too large */   
    if (Min >= Max) return -6;         /* minimum equal or larger than maximum */   
    if (uiNrX < 2 || uiNrY < 2) return -7;/* at least 4 contextual regions required */   
    if (fCliplimit == 1.0) return 0;      /* is OK, immediately returns original image. */   
    if (uiNrBins == 0) uiNrBins = 128;    /* default value when not specified */   
       
    pulMapArray=(unsigned long *)malloc(sizeof(unsigned long)*uiNrX*uiNrY*uiNrBins);   
    if (pulMapArray == 0) return -8;      /* Not enough memory! (try reducing uiNrBins) */   
       
    uiXSize = uiXRes/uiNrX; uiYSize = uiYRes/uiNrY;  /* Actual size of contextual regions */   
    ulNrPixels = (unsigned long)uiXSize * (unsigned long)uiYSize;   
       
    if(fCliplimit > 0.0) {         /* Calculate actual cliplimit  */   
        ulClipLimit = (unsigned long) (fCliplimit * (uiXSize * uiYSize) / uiNrBins);   
        ulClipLimit = (ulClipLimit < 1UL) ? 1UL : ulClipLimit;   
    }   
    else ulClipLimit = 1UL<<14;         /* Large value, do not clip (AHE) */   
    MakeLut(aLUT, Min, Max, uiNrBins);    /* Make lookup table for mapping of greyvalues */   
    /* Calculate greylevel mappings for each contextual region */   
    for (uiY = 0, pImPointer = pImage; uiY < uiNrY; uiY++)    
    {   
        for (uiX = 0; uiX < uiNrX; uiX++, pImPointer += uiXSize)    
        {   
            pulHist = &pulMapArray[uiNrBins * (uiY * uiNrX + uiX)];   
            MakeHistogram(pImPointer,uiXRes,uiXSize,uiYSize,pulHist,uiNrBins,aLUT);   
            ClipHistogram(pulHist, uiNrBins, ulClipLimit);   
            MapHistogram(pulHist, Min, Max, uiNrBins, ulNrPixels);   
        }   
        pImPointer += (uiYSize - 1) * uiXRes;         /* skip lines, set pointer */   
    }   
       
    /* Interpolate greylevel mappings to get CLAHE image */   
    for (pImPointer = pImage, uiY = 0; uiY <= uiNrY; uiY++)    
    {   
        if (uiY == 0)    
        {                     /* special case: top row */   
            uiSubY = uiYSize >> 1;  uiYU = 0; uiYB = 0;   
        }   
        else    
        {   
            if (uiY == uiNrY) {               /* special case: bottom row */   
                uiSubY = uiYSize >> 1;    uiYU = uiNrY-1;  uiYB = uiYU;   
            }   
            else    
            {                     /* default values */   
                uiSubY = uiYSize; uiYU = uiY - 1; uiYB = uiYU + 1;   
            }   
        }   
        for (uiX = 0; uiX <= uiNrX; uiX++)    
        {   
            if (uiX == 0)    
            {                 /* special case: left column */   
                uiSubX = uiXSize >> 1; uiXL = 0; uiXR = 0;   
            }   
            else    
            {   
                if (uiX == uiNrX)    
                {             /* special case: right column */   
                    uiSubX = uiXSize >> 1;  uiXL = uiNrX - 1; uiXR = uiXL;   
                }   
                else    
                {                     /* default values */   
                    uiSubX = uiXSize; uiXL = uiX - 1; uiXR = uiXL + 1;   
                }   
            }   
               
            pulLU = &pulMapArray[uiNrBins * (uiYU * uiNrX + uiXL)];   
            pulRU = &pulMapArray[uiNrBins * (uiYU * uiNrX + uiXR)];   
            pulLB = &pulMapArray[uiNrBins * (uiYB * uiNrX + uiXL)];   
            pulRB = &pulMapArray[uiNrBins * (uiYB * uiNrX + uiXR)];   
            Interpolate(pImPointer,uiXRes,pulLU,pulRU,pulLB,pulRB,uiSubX,uiSubY,aLUT);   
            pImPointer += uiSubX;             /* set pointer on next matrix */   
        }   
        pImPointer += (uiSubY - 1) * uiXRes;   
    }   
       
    free(pulMapArray);                    /* free space for histograms */   
       
    return 0;                         /* return status OK */   
}   

int CLAHE2 (kz_pixel_t* pImage, unsigned int uiXRes, unsigned int uiYRes,   
           kz_pixel_t Min, kz_pixel_t Max, unsigned int uiNrX, unsigned int uiNrY,   
           unsigned int uiNrBins, float fCliplimit)   
           /*  pImage - Pointer to the input/output image  
           *   uiXRes - Image resolution in the X direction  
           *   uiYRes - Image resolution in the Y direction  
           *   Min - Minimum greyvalue of input image (also becomes minimum of output image)  
           *   Max - Maximum greyvalue of input image (also becomes maximum of output image)  
           *   uiNrX - Number of contextial regions in the X direction (min 2, max uiMAX_REG_X)  
           *   uiNrY - Number of contextial regions in the Y direction (min 2, max uiMAX_REG_Y)  
           *   uiNrBins - Number of greybins for histogram ("dynamic range")  
           *   float fCliplimit - Normalized cliplimit (higher values give more contrast)  
           *   
           */   
{   
   
    unsigned int uiX, uiY;        /* counters */   
    unsigned int uiXSize, uiYSize, uiSubX, uiSubY; /* size of context. reg. and subimages */   
    unsigned int uiXL, uiXR, uiYU, uiYB;  /* auxiliary variables interpolation routine */   
    unsigned long ulClipLimit, ulNrPixels;/* clip limit and region pixel count */   
    kz_pixel_t* pImPointer;        /* pointer to image */   
    kz_pixel_t aLUT[uiNR_OF_GREY];      /* lookup table used for scaling of input image */   
    unsigned long* pulHist, *pulMapArray; /* pointer to histogram and mappings*/   
    unsigned long* pulLU, *pulLB, *pulRU, *pulRB; /* auxiliary pointers interpolation */   
       
    if (uiNrX > uiMAX_REG_X) return -1;     /* # of regions x-direction too large */   
    if (uiNrY > uiMAX_REG_Y) return -2;     /* # of regions y-direction too large */   
             
    //    if (uiXRes % uiNrX) return -3;      /* x-resolution no multiple of uiNrX */    
    //    if (uiYRes % uiNrY) return -4;      /* y-resolution no multiple of uiNrY */    
       
    if (Max >= uiNR_OF_GREY) return -5;     /* maximum too large */   
    if (Min >= Max) return -6;         /* minimum equal or larger than maximum */   
    if (uiNrX < 2 || uiNrY < 2) return -7;/* at least 4 contextual regions required */   
    if (fCliplimit == 1.0) return 0;      /* is OK, immediately returns original image. */   
    if (uiNrBins == 0) uiNrBins = 128;    /* default value when not specified */   
       
    pulMapArray=(unsigned long *)malloc(sizeof(unsigned long)*uiNrX*uiNrY*uiNrBins);   
    if (pulMapArray == 0) return -8;      /* Not enough memory! (try reducing uiNrBins) */   
       
    uiXSize = uiXRes/uiNrX; uiYSize = uiYRes/uiNrY;  /* Actual size of contextual regions */   
    ulNrPixels = (unsigned long)uiXSize * (unsigned long)uiYSize;   
       
    if(fCliplimit > 0.0) {         /* Calculate actual cliplimit  */   
        ulClipLimit = (unsigned long) (fCliplimit * (uiXSize * uiYSize) / uiNrBins);   
        ulClipLimit = (ulClipLimit < 1UL) ? 1UL : ulClipLimit;   
    }   
    else ulClipLimit = 1UL<<14;         /* Large value, do not clip (AHE) */   
    MakeLut(aLUT, Min, Max, uiNrBins);    /* Make lookup table for mapping of greyvalues */   
    /* Calculate greylevel mappings for each contextual region */   
    for (uiY = 0, pImPointer = pImage; uiY < uiNrY; uiY++)    
    {   
        for (uiX = 0; uiX < uiNrX; uiX++, pImPointer += uiXSize)    
        {   
            pulHist = &pulMapArray[uiNrBins * (uiY * uiNrX + uiX)];   
            MakeHistogram(pImPointer,uiXRes,uiXSize,uiYSize,pulHist,uiNrBins,aLUT);   
            ClipHistogram(pulHist, uiNrBins, ulClipLimit);   
            MapHistogram(pulHist, Min, Max, uiNrBins, ulNrPixels);   
        }   
        pImPointer += (uiYSize - 1) * uiXRes;         /* skip lines, set pointer */   
    }   
       
    /* Interpolate greylevel mappings to get CLAHE image */   
    for (pImPointer = pImage, uiY = 0; uiY <= uiNrY; uiY++)    
    {   
        if (uiY == 0)    
        {                     /* special case: top row */   
            uiSubY = uiYSize >> 1;  uiYU = 0; uiYB = 0;   
        }   
        else    
        {   
            if (uiY == uiNrY) {               /* special case: bottom row */   
                uiSubY = uiYSize >> 1;    uiYU = uiNrY-1;  uiYB = uiYU;   
            }   
            else    
            {                     /* default values */   
                uiSubY = uiYSize; uiYU = uiY - 1; uiYB = uiYU + 1;   
            }   
        }   
        for (uiX = 0; uiX <= uiNrX; uiX++)    
        {   
            if (uiX == 0)    
            {                 /* special case: left column */   
                uiSubX = uiXSize >> 1; uiXL = 0; uiXR = 0;   
            }   
            else    
            {   
                if (uiX == uiNrX)    
                {             /* special case: right column */   
                    uiSubX = uiXSize >> 1;  uiXL = uiNrX - 1; uiXR = uiXL;   
                }   
                else    
                {                     /* default values */   
                    uiSubX = uiXSize; uiXL = uiX - 1; uiXR = uiXL + 1;   
                }   
            }   
               
            pulLU = &pulMapArray[uiNrBins * (uiYU * uiNrX + uiXL)];   
            pulRU = &pulMapArray[uiNrBins * (uiYU * uiNrX + uiXR)];   
            pulLB = &pulMapArray[uiNrBins * (uiYB * uiNrX + uiXL)];   
            pulRB = &pulMapArray[uiNrBins * (uiYB * uiNrX + uiXR)];   
            Interpolate2(pImPointer,uiXRes,pulLU,pulRU,pulLB,pulRB,uiSubX,uiSubY,aLUT);   
            pImPointer += uiSubX;             /* set pointer on next matrix */   
        }   
        pImPointer += (uiSubY - 1) * uiXRes;   
    }   
       
    free(pulMapArray);                    /* free space for histograms */   
       
    return 0;                         /* return status OK */   
}   
void ClipHistogram (unsigned long* pulHistogram, unsigned int   
                    uiNrGreylevels, unsigned long ulClipLimit)                     
{   
    unsigned long* pulBinPointer, *pulEndPointer, *pulHisto;   
    unsigned long ulNrExcess, ulOldNrExcess, ulUpper, ulBinIncr, ulStepSize, i;   
    long lBinExcess;   
       
    ulNrExcess = 0;  pulBinPointer = pulHistogram;   
    for (i = 0; i < uiNrGreylevels; i++)    
    { /* calculate total number of excess pixels */   
        lBinExcess = (long) pulBinPointer[i] - (long) ulClipLimit;   
        if (lBinExcess > 0) ulNrExcess += lBinExcess;      /* excess in current bin */   
    };   
       
    /* Second part: clip histogram and redistribute excess pixels in each bin */   
    ulBinIncr = ulNrExcess / uiNrGreylevels;          /* average binincrement */   
    ulUpper =  ulClipLimit - ulBinIncr;  /* Bins larger than ulUpper set to cliplimit */   
       
    for (i = 0; i < uiNrGreylevels; i++)    
    {   
        if (pulHistogram[i] > ulClipLimit)    
            pulHistogram[i] = ulClipLimit; /* clip bin */   
        else    
        {   
            if (pulHistogram[i] > ulUpper)    
            {       /* high bin count */   
 //               ulNrExcess -= (pulHistogram[i] - ulUpper); pulHistogram[i]=ulClipLimit;   
				ulNrExcess -= (ulClipLimit -pulHistogram[i]); pulHistogram[i]=ulClipLimit;
            }   
            else    
            {                   /* low bin count */   
                ulNrExcess -= ulBinIncr; pulHistogram[i] += ulBinIncr;   
            }   
        }   
    }   
       
    do {   /* Redistribute remaining excess  */   
        pulEndPointer = &pulHistogram[uiNrGreylevels]; pulHisto = pulHistogram;   
           
        ulOldNrExcess = ulNrExcess;     /* Store number of excess pixels for test later. */   
           
        while (ulNrExcess && pulHisto < pulEndPointer)    
        {   
            ulStepSize = uiNrGreylevels / ulNrExcess;   
            if (ulStepSize < 1)    
                ulStepSize = 1;       /* stepsize at least 1 */   
            for (pulBinPointer=pulHisto; pulBinPointer < pulEndPointer && ulNrExcess; pulBinPointer += ulStepSize)    
            {   
                if (*pulBinPointer < ulClipLimit)    
                {   
                    (*pulBinPointer)++;  ulNrExcess--;    /* reduce excess */   
                }   
            }   
            pulHisto++;       /* restart redistributing on other bin location */   
        }   
    } while ((ulNrExcess) && (ulNrExcess < ulOldNrExcess));   
    /* Finish loop when we have no more pixels or we can't redistribute any more pixels */   
}   
   
#if 1
void MakeHistogram (kz_pixel_t* pImage, unsigned int uiXRes,   
                    unsigned int uiSizeX, unsigned int uiSizeY,   
                    unsigned long* pulHistogram,   
                    unsigned int uiNrGreylevels, kz_pixel_t* pLookupTable)   
{   
    kz_pixel_t* pImagePointer;   
    unsigned int i;   
       
    for (i = 0; i < uiNrGreylevels; i++)    
        pulHistogram[i] = 0L; /* clear histogram */   
       
    for (i = 0; i < uiSizeY; i++)    
    {   
        pImagePointer = &pImage[uiSizeX];   
        while (pImage < pImagePointer)    
            pulHistogram[pLookupTable[*pImage++]]++;   
        pImagePointer += uiXRes;   
        pImage = pImagePointer-uiSizeX;   
    }   
}  
#else
void MakeHistogram (kz_pixel_t* pImage, unsigned int uiXRes,   
                    unsigned int uiSizeX, unsigned int uiSizeY,   
                    unsigned long* pulHistogram,   
                    unsigned int uiNrGreylevels, kz_pixel_t* pLookupTable)   

{   
    kz_pixel_t* pImagePointer;   
    unsigned int i;   
       
    for (i = 0; i < uiNrGreylevels; i++)    
        pulHistogram[i] = 0L; /* clear histogram */   
       
    for (i = 0; i < uiSizeY; i++)    
    {   
        pImagePointer = &pImage[uiSizeX];   
        while (pImage < pImagePointer) 
			pulHistogram[pLookupTable[*pImage++]]++;   
        pImagePointer += uiXRes;   
        pImage = pImagePointer-uiSizeX;   
    }   
	for (i=0; i < uiNrGreylevels; i++)
	{
		pulHistogram[i] = (unsigned long)(10 * log((float)pulHistogram[i] + 1.0));
	}
}  
#endif 
   
void MapHistogram (unsigned long* pulHistogram, kz_pixel_t Min, kz_pixel_t Max,   
                   unsigned int uiNrGreylevels, unsigned long ulNrOfPixels)                    
{   
    unsigned int i;  unsigned long ulSum = 0;   
    const float fScale = ((float)(Max - Min)) / ulNrOfPixels;   
    const unsigned long ulMin = (unsigned long) Min;   
       
    for (i = 0; i < uiNrGreylevels; i++)    
    {   
        ulSum += pulHistogram[i];    
        pulHistogram[i]=(unsigned long)(ulMin+ulSum*fScale);   
        if (pulHistogram[i] > Max)    
            pulHistogram[i] = Max;   
    }   
}   
   
void MakeLut (kz_pixel_t * pLUT, kz_pixel_t Min, kz_pixel_t Max, unsigned int uiNrBins)   
{   
    int i;   
    const kz_pixel_t BinSize = (kz_pixel_t) (1 + (Max - Min) / uiNrBins);   
	
    for (i = Min; i <= Max; i++)     
        pLUT[i] = (i - Min) / BinSize;   
}   
   
void Interpolate (kz_pixel_t * pImage, int uiXRes, unsigned long * pulMapLU,   
                  unsigned long * pulMapRU, unsigned long * pulMapLB,  unsigned long * pulMapRB,   
                  unsigned int uiXSize, unsigned int uiYSize, kz_pixel_t* pLUT)   
                  /* pImage      - pointer to input/output image  
                  * uiXRes      - resolution of image in x-direction  
                  * pulMap*     - mappings of greylevels from histograms  
                  * uiXSize     - uiXSize of image submatrix  
                  * uiYSize     - uiYSize of image submatrix  
                  * pLUT           - lookup table containing mapping greyvalues to bins               
                  */   
{   
    const unsigned int uiIncr = uiXRes-uiXSize; /* Pointer increment after processing row */   
    kz_pixel_t GreyValue; unsigned int uiNum = uiXSize*uiYSize; /* Normalization factor */   
       
    unsigned int uiXCoef, uiYCoef, uiXInvCoef, uiYInvCoef, uiShift = 0;   
       
    if (uiNum & (uiNum - 1))   /* If uiNum is not a power of two, use division */  
	{
        for (uiYCoef = 0, uiYInvCoef = uiYSize; uiYCoef < uiYSize;  uiYCoef++, uiYInvCoef--,pImage+=uiIncr)    
        {   
            for (uiXCoef = 0, uiXInvCoef = uiXSize; uiXCoef < uiXSize;  uiXCoef++, uiXInvCoef--)    
            {   
                GreyValue = (unsigned char)pLUT[(unsigned char)(*pImage)];         /* get histogram bin value */   
                *pImage++ = (kz_pixel_t ) ((uiYInvCoef * (uiXInvCoef*pulMapLU[GreyValue] + uiXCoef * pulMapRU[GreyValue])   
										  + uiYCoef * (uiXInvCoef * pulMapLB[GreyValue] + uiXCoef * pulMapRB[GreyValue])) / uiNum);   
            }   
        }  
	}
	else  /* avoid the division and use a right shift instead */   
	{ 
		while (uiNum >>= 1) uiShift++;           /* Calculate 2log of uiNum */   
		for (uiYCoef = 0, uiYInvCoef = uiYSize; uiYCoef < uiYSize;  uiYCoef++, uiYInvCoef--,pImage+=uiIncr)    
		{   
			for (uiXCoef = 0, uiXInvCoef = uiXSize; uiXCoef < uiXSize;  uiXCoef++, uiXInvCoef--)    
			{   
				GreyValue = pLUT[*pImage];    /* get histogram bin value */   
				*pImage++ = (kz_pixel_t)((uiYInvCoef* (uiXInvCoef * pulMapLU[GreyValue]	+ uiXCoef * pulMapRU[GreyValue])   
										+ uiYCoef * (uiXInvCoef * pulMapLB[GreyValue] + uiXCoef * pulMapRB[GreyValue])) >> uiShift);  
			}   
		}   
	}   
}   

void Interpolate2 (kz_pixel_t * pImage, int uiXRes, unsigned long * pulMapLU,   
                  unsigned long * pulMapRU, unsigned long * pulMapLB,  unsigned long * pulMapRB,   
                  unsigned int uiXSize, unsigned int uiYSize, kz_pixel_t* pLUT)   
                  /* pImage      - pointer to input/output image  
                  * uiXRes      - resolution of image in x-direction  
                  * pulMap*     - mappings of greylevels from histograms  
                  * uiXSize     - uiXSize of image submatrix  
                  * uiYSize     - uiYSize of image submatrix  
                  * pLUT           - lookup table containing mapping greyvalues to bins               
                  */   
{   
    const unsigned int uiIncr = uiXRes-uiXSize; /* Pointer increment after processing row */   
    kz_pixel_t GreyValue; unsigned int uiNum = uiXSize*uiYSize; /* Normalization factor */   
       
    unsigned int uiXCoef, uiYCoef, uiXInvCoef, uiYInvCoef, uiShift = 0;   
       
    if (uiNum & (uiNum - 1))   /* If uiNum is not a power of two, use division */  
	{
        for (uiYCoef = 0, uiYInvCoef = uiYSize; uiYCoef < uiYSize;  uiYCoef++, uiYInvCoef--,pImage+=uiIncr)    
        {   
            for (uiXCoef = 0, uiXInvCoef = uiXSize; uiXCoef < uiXSize;  uiXCoef++, uiXInvCoef--)    
            {   
                GreyValue = (unsigned char)pLUT[(unsigned char)(*pImage)];         /* get histogram bin value */   
                *pImage++ = (kz_pixel_t ) ((uiYInvCoef * (uiXInvCoef*pulMapLU[GreyValue] + uiXCoef * pulMapRU[GreyValue])   
										  + uiYCoef * (uiXInvCoef * pulMapLB[GreyValue] + uiXCoef * pulMapRB[GreyValue])) / uiNum);   
            }   
        }  
	}
	else  /* avoid the division and use a right shift instead */   
	{ 
		while (uiNum >>= 1) uiShift++;           /* Calculate 2log of uiNum */   
		for (uiYCoef = 0, uiYInvCoef = uiYSize; uiYCoef < uiYSize;  uiYCoef++, uiYInvCoef--,pImage+=uiIncr)    
		{   
			for (uiXCoef = 0, uiXInvCoef = uiXSize; uiXCoef < uiXSize;  uiXCoef++, uiXInvCoef--)    
			{   
				GreyValue = pLUT[*pImage];    /* get histogram bin value */   
//				*pImage++ = (kz_pixel_t)((uiYInvCoef* (uiXInvCoef * pulMapLU[GreyValue]	+ uiXCoef * pulMapRU[GreyValue])   
//										+ uiYCoef * (uiXInvCoef * pulMapLB[GreyValue] + uiXCoef * pulMapRB[GreyValue])) >> uiShift);  
				*pImage++ = (kz_pixel_t)( (pulMapLU[GreyValue] + pulMapRU[GreyValue] + pulMapLB[GreyValue] + pulMapRB[GreyValue]) >> 2);   
			}   
		}   
	}   
}   

int CLAHE_new(kz_pixel_t* pImage, unsigned int uiXRes, unsigned int uiYRes, kz_pixel_t Min, 
			  kz_pixel_t Max, unsigned int uiNrX, unsigned int uiNrY, 
			  unsigned int uiNrBins, float fCliplimit)
{
    unsigned int uiX, uiY;        /* counters */   
    unsigned int uiXSize, uiYSize, uiSubX, uiSubY; /* size of context. reg. and subimages */   
    unsigned int uiXL, uiXR, uiYU, uiYB;  /* auxiliary variables interpolation routine */   
    unsigned long ulClipLimit, ulNrPixels;/* clip limit and region pixel count */   
    kz_pixel_t* pImPointer, *pIMGBUFFER;        /* pointer to image */   
    kz_pixel_t aLUT[uiNR_OF_GREY];      /* lookup table used for scaling of input image */   
    unsigned long* pulHist, *pulMapArrayCENT,*pulMapArrayUP,*pulMapArrayLEFT,*pulMapArrayLU,*pulMapArrayRB,*pulMapArrayBOT,*pulMapArrayRIGHT; /* pointer to histogram and mappings*/   
    unsigned long* pulLU, *pulLB, *pulRU, *pulRB; /* auxiliary pointers interpolation */   
	kz_pixel_t *pImgBufferCENT, *pImgBufferUP, *pImgBufferLEFT, *pImgBufferLU, *pImgBufferRB, *pImgBufferBOT, *pImgBufferRIGHT;
       
    if (uiNrX > uiMAX_REG_X) return -1;     /* # of regions x-direction too large */   
    if (uiNrY > uiMAX_REG_Y) return -2;     /* # of regions y-direction too large */   
             
    //    if (uiXRes % uiNrX) return -3;      /* x-resolution no multiple of uiNrX */    
    //    if (uiYRes % uiNrY) return -4;      /* y-resolution no multiple of uiNrY */    
       
    if (Max >= uiNR_OF_GREY) return -5;     /* maximum too large */   
    if (Min >= Max) return -6;         /* minimum equal or larger than maximum */   
    if (uiNrX < 2 || uiNrY < 2) return -7;/* at least 4 contextual regions required */   
    if (fCliplimit == 1.0) return 0;      /* is OK, immediately returns original image. */   
    if (uiNrBins == 0) uiNrBins = 128;    /* default value when not specified */   
       
    pulMapArrayLU=(unsigned long *)malloc(sizeof(unsigned long)*uiNrX*uiNrY*uiNrBins);   
    if (pulMapArrayLU == 0) return -8;      /* Not enough memory! (try reducing uiNrBins) */   
	pulMapArrayCENT=(unsigned long *)malloc(sizeof(unsigned long)*uiNrX*uiNrY*uiNrBins);   
    if (pulMapArrayCENT == 0) return -8;      /* Not enough memory! (try reducing uiNrBins) */   
	pulMapArrayUP=(unsigned long *)malloc(sizeof(unsigned long)*uiNrX*uiNrY*uiNrBins);   
	if (pulMapArrayUP == 0) return -8;      /* Not enough memory! (try reducing uiNrBins) */   
	pulMapArrayLEFT=(unsigned long *)malloc(sizeof(unsigned long)*uiNrX*uiNrY*uiNrBins);   
	if (pulMapArrayLEFT == 0) return -8;      /* Not enough memory! (try reducing uiNrBins) */  
	pulMapArrayRB=(unsigned long *)malloc(sizeof(unsigned long)*uiNrX*uiNrY*uiNrBins);   
	if (pulMapArrayRB == 0) return -8;      /* Not enough memory! (try reducing uiNrBins) */  
	pulMapArrayRIGHT=(unsigned long *)malloc(sizeof(unsigned long)*uiNrX*uiNrY*uiNrBins);   
	if (pulMapArrayRIGHT == 0) return -8;      /* Not enough memory! (try reducing uiNrBins) */  
	pulMapArrayBOT=(unsigned long *)malloc(sizeof(unsigned long)*uiNrX*uiNrY*uiNrBins);   
	if (pulMapArrayBOT == 0) return -8;      /* Not enough memory! (try reducing uiNrBins) */  
	
	pImgBufferCENT = (kz_pixel_t*)malloc(sizeof(kz_pixel_t)*uiXRes*uiYRes);
	if (pImgBufferCENT == 0) return -8; 
	pImgBufferUP = (kz_pixel_t*)malloc(sizeof(kz_pixel_t)*uiXRes*uiYRes);
	if (pImgBufferUP == 0) return -8; 
	pImgBufferLEFT = (kz_pixel_t*)malloc(sizeof(kz_pixel_t)*uiXRes*uiYRes);
	if (pImgBufferLEFT == 0) return -8; 
	pImgBufferLU = (kz_pixel_t*)malloc(sizeof(kz_pixel_t)*uiXRes*uiYRes);
	if (pImgBufferLU == 0) return -8; 
	pImgBufferRB = (kz_pixel_t*)malloc(sizeof(kz_pixel_t)*uiXRes*uiYRes);
	if (pImgBufferRB == 0) return -8; 
	pImgBufferBOT = (kz_pixel_t*)malloc(sizeof(kz_pixel_t)*uiXRes*uiYRes);
	if (pImgBufferBOT == 0) return -8; 
	pImgBufferRIGHT = (kz_pixel_t*)malloc(sizeof(kz_pixel_t)*uiXRes*uiYRes);
	if (pImgBufferRIGHT == 0) return -8; 
       
	uiXSize = uiXRes/uiNrX; uiYSize = uiYRes/uiNrY;  /* Actual size of contextual regions */   
    ulNrPixels = (unsigned long)uiXSize * (unsigned long)uiYSize;   
       
    if(fCliplimit > 0.0) {         /* Calculate actual cliplimit  */   
        ulClipLimit = (unsigned long) (fCliplimit * (uiXSize * uiYSize) / uiNrBins);   
        ulClipLimit = (ulClipLimit < 1UL) ? 1UL : ulClipLimit;   
    }   
    else ulClipLimit = 1UL<<14;         /* Large value, do not clip (AHE) */   
    MakeLut(aLUT, Min, Max, uiNrBins);    /* Make lookup table for mapping of greyvalues */   
    /* Calculate greylevel mappings for each contextual region */   
    for (uiY = 0, pImPointer = pImage; uiY < uiNrY; uiY++)    
    {   
        for (uiX = 0; uiX < uiNrX; uiX++, pImPointer += uiXSize)    
        {   
            pulHist = &pulMapArrayCENT[uiNrBins * (uiY * uiNrX + uiX)];   
            MakeHistogram(pImPointer,uiXRes,uiXSize,uiYSize,pulHist,uiNrBins,aLUT);   
            ClipHistogram(pulHist, uiNrBins, ulClipLimit);   
            MapHistogram(pulHist, Min, Max, uiNrBins, ulNrPixels);   
        }   
        pImPointer += (uiYSize - 1) * uiXRes;         /* skip lines, set pointer */   
    }   

	for (uiY = 0, pImPointer = pImage+(uiYSize>>1)*uiXRes+(uiXSize>>1); uiY < uiNrY-1; uiY++)    
    {   
        for (uiX = 0; uiX < uiNrX-1; uiX++, pImPointer += uiXSize)    
        {   
            pulHist = &pulMapArrayLU[uiNrBins * (uiY * uiNrX + uiX)];   
            MakeHistogram(pImPointer,uiXRes,uiXSize,uiYSize,pulHist,uiNrBins,aLUT);   
            ClipHistogram(pulHist, uiNrBins, ulClipLimit);   
            MapHistogram(pulHist, Min, Max, uiNrBins, ulNrPixels);   
        }   
        pImPointer += (uiYSize - 1) * uiXRes;         /* skip lines, set pointer */   
    } 
	
	for (uiY = 0, pImPointer = pImage+(uiYSize>>1)*uiXRes; uiY < uiNrY-1; uiY++)    
    {   
        for (uiX = 0; uiX < uiNrX; uiX++, pImPointer += uiXSize)    
        {   
            pulHist = &pulMapArrayUP[uiNrBins * (uiY * uiNrX + uiX)];   
            MakeHistogram(pImPointer,uiXRes,uiXSize,uiYSize,pulHist,uiNrBins,aLUT);   
            ClipHistogram(pulHist, uiNrBins, ulClipLimit);   
            MapHistogram(pulHist, Min, Max, uiNrBins, ulNrPixels);   
        }   
        pImPointer += (uiYSize - 1) * uiXRes;         /* skip lines, set pointer */   
    } 
	for (uiY = 0, pImPointer = pImage+(uiXSize>>1); uiY < uiNrY; uiY++)    
    {   
        for (uiX = 0; uiX < uiNrX-1; uiX++, pImPointer += uiXSize)
        {   
            pulHist = &pulMapArrayLEFT[uiNrBins * (uiY * uiNrX + uiX)];   
            MakeHistogram(pImPointer,uiXRes,uiXSize,uiYSize,pulHist,uiNrBins,aLUT);   
            ClipHistogram(pulHist, uiNrBins, ulClipLimit);   
            MapHistogram(pulHist, Min, Max, uiNrBins, ulNrPixels);   
        }   
        pImPointer += (uiYSize - 1) * uiXRes;         /* skip lines, set pointer */   
    } 
/************************************************************************************/
	int ix, iy;
	kz_pixel_t GreyValue;
	memcpy(pImgBufferCENT, pImage, uiXRes*uiYRes*sizeof(kz_pixel_t));
	for (uiY = 0, pImPointer = pImage, pIMGBUFFER = pImgBufferCENT; uiY < uiNrY; uiY++)    
    {   
        for (uiX = 0; uiX < uiNrX; uiX++, pImPointer += uiXSize, pIMGBUFFER += uiXSize)    
        {   
            pulHist = &pulMapArrayCENT[uiNrBins * (uiY * uiNrX + uiX)];   
			for(iy = 0; iy<uiYSize; iy++)
			{
				for (ix = 0; ix<uiXSize; ix++)
				{
					GreyValue = aLUT[pImPointer[iy*uiXRes+ix]];    /* get histogram bin value */ 
					pIMGBUFFER[iy*uiXRes+ix] = pulHist[GreyValue];
				}
			}
        }   
        pImPointer += (uiYSize - 1) * uiXRes;         /* skip lines, set pointer */   
		pIMGBUFFER += (uiYSize - 1) * uiXRes;
    }   

	memcpy(pImgBufferLU, pImage, uiXRes*uiYRes*sizeof(kz_pixel_t));
	for (uiY = 0, pImPointer = pImage+(uiYSize>>1)*uiXRes+(uiXSize>>1), 
				  pIMGBUFFER = pImgBufferLU+(uiYSize>>1)*uiXRes+(uiXSize>>1); uiY < uiNrY-1; uiY++)    
    {   
        for (uiX = 0; uiX < uiNrX-1; uiX++, pImPointer += uiXSize, pIMGBUFFER += uiXSize)    
        {   
            pulHist = &pulMapArrayLU[uiNrBins * (uiY * uiNrX + uiX)];   
			for(iy = 0; iy<uiYSize; iy++)
			{
				for (ix = 0; ix<uiXSize; ix++)
				{
					GreyValue = aLUT[pImPointer[iy*uiXRes+ix]];    /* get histogram bin value */ 
					pIMGBUFFER[iy*uiXRes+ix] = pulHist[GreyValue];
				}
			}
        }   
        pImPointer += (uiYSize - 1) * uiXRes;         /* skip lines, set pointer */   
		pIMGBUFFER += (uiYSize - 1) * uiXRes;
    }  

	memcpy(pImgBufferUP, pImage, uiXRes*uiYRes*sizeof(kz_pixel_t));
	for (uiY = 0, pImPointer = pImage+(uiYSize>>1)*uiXRes, 
				  pIMGBUFFER = pImgBufferUP+(uiYSize>>1)*uiXRes; uiY < uiNrY-1; uiY++)    
    {   
        for (uiX = 0; uiX < uiNrX; uiX++, pImPointer += uiXSize, pIMGBUFFER += uiXSize)    
        {   
            pulHist = &pulMapArrayUP[uiNrBins * (uiY * uiNrX + uiX)];   
			for(iy = 0; iy<uiYSize; iy++)
			{
				for (ix = 0; ix<uiXSize; ix++)
				{
					GreyValue = aLUT[pImPointer[iy*uiXRes+ix]];    /* get histogram bin value */ 
					pIMGBUFFER[iy*uiXRes+ix] = pulHist[GreyValue];
				}
			}
        }   
        pImPointer += (uiYSize - 1) * uiXRes;         /* skip lines, set pointer */   
		pIMGBUFFER += (uiYSize - 1) * uiXRes;
    }  

	memcpy(pImgBufferLEFT, pImage, uiXRes*uiYRes*sizeof(kz_pixel_t));
	for (uiY = 0, pImPointer = pImage+(uiXSize>>1), 
				  pIMGBUFFER = pImgBufferLEFT+(uiXSize>>1); uiY < uiNrY; uiY++)    
    {   
        for (uiX = 0; uiX < uiNrX-1; uiX++, pImPointer += uiXSize, pIMGBUFFER += uiXSize)    
        {   
            pulHist = &pulMapArrayLEFT[uiNrBins * (uiY * uiNrX + uiX)];   
			for(iy = 0; iy<uiYSize; iy++)
			{
				for (ix = 0; ix<uiXSize; ix++)
				{
					GreyValue = aLUT[pImPointer[iy*uiXRes+ix]];    /* get histogram bin value */ 
					pIMGBUFFER[iy*uiXRes+ix] = pulHist[GreyValue];
				}
			}
        }   
        pImPointer += (uiYSize - 1) * uiXRes;         /* skip lines, set pointer */   
		pIMGBUFFER += (uiYSize - 1) * uiXRes;
    }  

	for (uiY = 0; uiY < uiYRes; uiY++)
	{
		for (uiX = 0; uiX < uiXRes; uiX++)
		{
			pImage[uiY*uiXRes+uiX] = (kz_pixel_t)((pImgBufferCENT[uiY*uiXRes+uiX]+pImgBufferLEFT[uiY*uiXRes+uiX]+
												   pImgBufferUP[uiY*uiXRes+uiX]+pImgBufferLU[uiY*uiXRes+uiX])>>2);
		}
	}
	
    free(pulMapArrayLU);                    /* free space for histograms */  
	free(pulMapArrayCENT);                    /* free space for histograms */  
	free(pulMapArrayUP);                    /* free space for histograms */  
	free(pulMapArrayLEFT);                    /* free space for histograms */  
	free(pulMapArrayRB);                    /* free space for histograms */  
	free(pulMapArrayBOT);                    /* free space for histograms */  
	free(pulMapArrayRIGHT);                    /* free space for histograms */  

	free(pImgBufferCENT);
	free(pImgBufferUP);
	free(pImgBufferLEFT);
	free(pImgBufferLU);
	free(pImgBufferRB);
	free(pImgBufferBOT);
	free(pImgBufferRIGHT);
       
    return 0;                         /* return status OK */   
}
