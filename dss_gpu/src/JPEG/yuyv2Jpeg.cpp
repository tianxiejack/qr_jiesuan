#include <npp.h>
#include <cuda_runtime.h>
#include "Exceptions.h"

#include <math.h>

#include <string.h>
#include <fstream>
#include <iostream>

#include "helper_string.h"
#include "helper_cuda.h"

#include "initTable.hpp"
#include "Endianess.h"

using namespace std;


struct FrameHeader
{
    unsigned char nSamplePrecision;
    unsigned short nHeight;
    unsigned short nWidth;
    unsigned char nComponents;
    unsigned char aComponentIdentifier[3];
    unsigned char aSamplingFactors[3];
    unsigned char aQuantizationTableSelector[3];
};

struct ScanHeader
{
    unsigned char nComponents;
    unsigned char aComponentSelector[3];
    unsigned char aHuffmanTablesSelector[3];
    unsigned char nSs;
    unsigned char nSe;
    unsigned char nA;
};

struct QuantizationTable
{
    unsigned char nPrecisionAndIdentifier;
    unsigned char aTable[64];
};

struct HuffmanTable
{
    unsigned char nClassAndIdentifier;
    unsigned char aCodes[16];
    unsigned char aTable[256];
};


int DivUp(int x, int d)
{
    return (x + d - 1) / d;
}

template<typename T>
T readAndAdvance(const unsigned char *&pData)
{
    T nElement = readBigEndian<T>(pData);
    pData += sizeof(T);
    return nElement;
}

template<typename T>
void writeAndAdvance(unsigned char *&pData, T nElement)
{
    writeBigEndian<T>(pData, nElement);
    pData += sizeof(T);
}


void writeMarker(unsigned char nMarker, unsigned char *&pData)
{
    *pData++ = 0x0ff;
    *pData++ = nMarker;
}

void writeJFIFTag(unsigned char *&pData)
{
    const char JFIF_TAG[] =
    {
        0x4a, 0x46, 0x49, 0x46, 0x00,
        0x01, 0x02,
        0x00,
        0x00, 0x01, 0x00, 0x01,
        0x00, 0x00
    };

    writeMarker(0x0e0, pData);
    writeAndAdvance<unsigned short>(pData, sizeof(JFIF_TAG) + sizeof(unsigned short));
    memcpy(pData, JFIF_TAG, sizeof(JFIF_TAG));
    pData += sizeof(JFIF_TAG);
}

void writeQuantizationTable(const QuantizationTable &table, unsigned char *&pData)
{
    writeMarker(0x0DB, pData);
    writeAndAdvance<unsigned short>(pData, sizeof(QuantizationTable) + 2);
    memcpy(pData, &table, sizeof(QuantizationTable));
    pData += sizeof(QuantizationTable);
}


void writeFrameHeader(const FrameHeader &header, unsigned char *&pData)
{
    unsigned char aTemp[128];
    unsigned char *pTemp = aTemp;

    writeAndAdvance<unsigned char>(pTemp, header.nSamplePrecision);
    writeAndAdvance<unsigned short>(pTemp, header.nHeight);
    writeAndAdvance<unsigned short>(pTemp, header.nWidth);
    writeAndAdvance<unsigned char>(pTemp, header.nComponents);

    for (int c=0; c<header.nComponents; ++c)
    {
        writeAndAdvance<unsigned char>(pTemp,header.aComponentIdentifier[c]);
        writeAndAdvance<unsigned char>(pTemp,header.aSamplingFactors[c]);
        writeAndAdvance<unsigned char>(pTemp,header.aQuantizationTableSelector[c]);
    }

    unsigned short nLength = (unsigned short)(pTemp - aTemp);

    writeMarker(0x0C0, pData);
    writeAndAdvance<unsigned short>(pData, nLength + 2);
    memcpy(pData, aTemp, nLength);
    pData += nLength;
}

void writeHuffmanTable(const HuffmanTable &table, unsigned char *&pData)
{
    writeMarker(0x0C4, pData);

    // Number of Codes for Bit Lengths [1..16]
    int nCodeCount = 0;

    for (int i = 0; i < 16; ++i)
    {
        nCodeCount += table.aCodes[i];
    }

    writeAndAdvance<unsigned short>(pData, 17 + nCodeCount + 2);
    memcpy(pData, &table, 17 + nCodeCount);
    pData += 17 + nCodeCount;
}

void writeScanHeader(const ScanHeader &header, unsigned char *&pData)
{
    unsigned char aTemp[128];
    unsigned char *pTemp = aTemp;

    writeAndAdvance<unsigned char>(pTemp, header.nComponents);

    for (int c=0; c<header.nComponents; ++c)
    {
        writeAndAdvance<unsigned char>(pTemp,header.aComponentSelector[c]);
        writeAndAdvance<unsigned char>(pTemp,header.aHuffmanTablesSelector[c]);
    }

    writeAndAdvance<unsigned char>(pTemp,  header.nSs);
    writeAndAdvance<unsigned char>(pTemp,  header.nSe);
    writeAndAdvance<unsigned char>(pTemp,  header.nA);

    unsigned short nLength = (unsigned short)(pTemp - aTemp);

    writeMarker(0x0DA, pData);
    writeAndAdvance<unsigned short>(pData, nLength + 2);
    memcpy(pData, aTemp, nLength);
    pData += nLength;
}

void InitQuantizationTable(QuantizationTable aQuantizationTables[])
{
	aQuantizationTables[0].nPrecisionAndIdentifier = 0;
	memcpy(aQuantizationTables[0].aTable, Quantization_0_aTable, 64*sizeof(unsigned char));

	aQuantizationTables[1].nPrecisionAndIdentifier = 1;
	memcpy(aQuantizationTables[1].aTable, Quantization_1_aTable, 64*sizeof(unsigned char));

	aQuantizationTables[2].nPrecisionAndIdentifier = 0;
	memcpy(aQuantizationTables[2].aTable, Quantization_2_aTable, 64*sizeof(unsigned char));

	aQuantizationTables[3].nPrecisionAndIdentifier = 0;
	memcpy(aQuantizationTables[3].aTable, Quantization_3_aTable, 64*sizeof(unsigned char));
}

void InitHuffmanTable(HuffmanTable aHuffmanTables[])
{
	aHuffmanTables[0].nClassAndIdentifier = 0;
	memcpy(aHuffmanTables[0].aCodes, HuffmanTable_0_aCodes, 16*sizeof(unsigned char));
	memcpy(aHuffmanTables[0].aTable, HuffmanTable_0_aTable, 256*sizeof(unsigned char));

	aHuffmanTables[1].nClassAndIdentifier = 1;
	memcpy(aHuffmanTables[1].aCodes, HuffmanTable_1_aCodes, 16*sizeof(unsigned char));
	memcpy(aHuffmanTables[1].aTable, HuffmanTable_1_aTable, 256*sizeof(unsigned char));

	aHuffmanTables[2].nClassAndIdentifier = 0x10;
	memcpy(aHuffmanTables[2].aCodes, HuffmanTable_2_aCodes, 16*sizeof(unsigned char));
	memcpy(aHuffmanTables[2].aTable, HuffmanTable_2_aTable, 256*sizeof(unsigned char));

	aHuffmanTables[3].nClassAndIdentifier = 0x11;
	memcpy(aHuffmanTables[3].aCodes, HuffmanTable_3_aCodes, 16*sizeof(unsigned char));
	memcpy(aHuffmanTables[3].aTable, HuffmanTable_3_aTable, 256*sizeof(unsigned char));
}

void initFrameHeader(FrameHeader *pFrameHeader, int nWidth, int nHeight)
{
	pFrameHeader->nWidth = nWidth;
	pFrameHeader->nHeight = nHeight;
	pFrameHeader->nComponents = 3;
	pFrameHeader->aComponentIdentifier[0] = 1;
	pFrameHeader->aComponentIdentifier[1] = 2;
	pFrameHeader->aComponentIdentifier[2] = 3;

	pFrameHeader->aQuantizationTableSelector[0] = 0;
	pFrameHeader->aQuantizationTableSelector[1] = 1;
	pFrameHeader->aQuantizationTableSelector[2] = 1;

	pFrameHeader->aSamplingFactors[0] = 0x22;
	pFrameHeader->aSamplingFactors[1] = 0x12;
	pFrameHeader->aSamplingFactors[2] = 0x12;
}

void initScanHeader(ScanHeader *pScanHeader)
{
	pScanHeader->nComponents = 3;
	pScanHeader->nA = 0;
	pScanHeader->nSe = 63;
	pScanHeader->nSs = 0;

	pScanHeader->aComponentSelector[0] = 1;
	pScanHeader->aComponentSelector[1] = 2;
	pScanHeader->aComponentSelector[2] = 3;

	pScanHeader->aHuffmanTablesSelector[0] = 0x00;
	pScanHeader->aHuffmanTablesSelector[1] = 0x11;
	pScanHeader->aHuffmanTablesSelector[2] = 0x11;
}

void YUYV2Jpeg(  const char *szOutputFile, Npp8u	*nppSrcImage[], Npp32s nppImageStep[], NppiSize nppSrcSize[])
{
		NppiDCTState *pDCTState;
	    NPP_CHECK_NPP(nppiDCTInitAlloc(&pDCTState));

	    // Parsing and Huffman Decoding (on host)
		FrameHeader oFrameHeader;
		QuantizationTable aQuantizationTables[4];
		Npp8u *pdQuantizationTables;
		cudaMalloc(&pdQuantizationTables, 64 * 4);

		HuffmanTable aHuffmanTables[4];
		HuffmanTable *pHuffmanDCTables = aHuffmanTables;
		HuffmanTable *pHuffmanACTables = &aHuffmanTables[2];
		ScanHeader oScanHeader;
		memset(&oFrameHeader,0,sizeof(FrameHeader));
		memset(aQuantizationTables,0, 4 * sizeof(QuantizationTable));
		memset(aHuffmanTables,0, 4 * sizeof(HuffmanTable));
		int nMCUBlocksH = 0;
		int nMCUBlocksV = 0;

		NppiSize aSrcSize[3];
		Npp16s *apdDCT[3] = {0,0,0};
		Npp32s aDCTStep[3];

		InitQuantizationTable(aQuantizationTables);

		InitHuffmanTable(aHuffmanTables);

		initFrameHeader(&oFrameHeader, nppSrcSize[0].width, nppSrcSize[0].height);

		initScanHeader(&oScanHeader);

		// Compute channel sizes as stored in the JPEG (8x8 blocks & MCU block layout)
		for (int i=0; i < oFrameHeader.nComponents; ++i)
		{
			nMCUBlocksV = max(nMCUBlocksV, oFrameHeader.aSamplingFactors[i] & 0x0f );
			nMCUBlocksH = max(nMCUBlocksH, oFrameHeader.aSamplingFactors[i] >> 4 );
		}

		for (int i=0; i < oFrameHeader.nComponents; ++i)
		{
			NppiSize oBlocks;
			NppiSize oBlocksPerMCU = { oFrameHeader.aSamplingFactors[i]  >> 4, oFrameHeader.aSamplingFactors[i] & 0x0f};

			oBlocks.width = (int)ceil((oFrameHeader.nWidth + 7)/8  *
									  static_cast<float>(oBlocksPerMCU.width)/nMCUBlocksH);
			oBlocks.width = DivUp(oBlocks.width, oBlocksPerMCU.width) * oBlocksPerMCU.width;

			oBlocks.height = (int)ceil((oFrameHeader.nHeight+7)/8 *
									   static_cast<float>(oBlocksPerMCU.height)/nMCUBlocksV);
			oBlocks.height = DivUp(oBlocks.height, oBlocksPerMCU.height) * oBlocksPerMCU.height;

			aSrcSize[i].width = oBlocks.width * 8;
			aSrcSize[i].height = oBlocks.height * 8;

			// Allocate Memory
			size_t nPitch;
#if 1
			NPP_CHECK_CUDA(cudaMallocPitch(&apdDCT[i], &nPitch, oBlocks.width * 64 * sizeof(Npp16s), oBlocks.height));
			aDCTStep[i] = static_cast<Npp32s>(nPitch);
			printf("Output width[%d]=%d, height[%d]=%d, pitch[%d]=%d \n", i, aSrcSize[i].width, i, aSrcSize[i].height ,i, (uint)nPitch);
#else
			NPP_CHECK_CUDA(cudaMalloc(&apdDCT[i], oBlocks.width * 64 * sizeof(Npp16s)*oBlocks.height));
			aDCTStep[i] = static_cast<Npp32s>(oBlocks.width * 64 * sizeof(Npp16s));
			printf("Output width[%d]=%d, height[%d]=%d, pitch[%d]=%d \n", i, aSrcSize[i].width, i, aSrcSize[i].height ,i, aDCTStep[i]);
#endif

		}

		// Copy DCT coefficients and Quantization Tables from host to device
		for (int i = 0; i < 4; ++i)
		{
			NPP_CHECK_CUDA(cudaMemcpyAsync(pdQuantizationTables + i * 64, aQuantizationTables[i].aTable, 64, cudaMemcpyHostToDevice));
		}

		for (int i = 0; i < 3; ++i)
		{
			NPP_CHECK_NPP(nppiDCTQuantFwd8x8LS_JPEG_8u16s_C1R_NEW(nppSrcImage[i], nppImageStep[i],
																  apdDCT[i], aDCTStep[i],
																  pdQuantizationTables + oFrameHeader.aQuantizationTableSelector[i] * 64,
																  nppSrcSize[i],
																  pDCTState));
		}

		// Huffman Encoding
		Npp8u *pdScan;
		Npp32s nScanLength;
		NPP_CHECK_CUDA(cudaMalloc(&pdScan, aSrcSize[0].width*aSrcSize[0].height*2));

		Npp8u *pJpegEncoderTemp;
		Npp32s nTempSize;
		NPP_CHECK_NPP(nppiEncodeHuffmanGetSize(aSrcSize[0], 3, &nTempSize));
		NPP_CHECK_CUDA(cudaMalloc(&pJpegEncoderTemp, nTempSize));

		NppiEncodeHuffmanSpec *apHuffmanDCTable[3];
		NppiEncodeHuffmanSpec *apHuffmanACTable[3];

		for (int i = 0; i < 3; ++i)
		{
			nppiEncodeHuffmanSpecInitAlloc_JPEG(pHuffmanDCTables[(oScanHeader.aHuffmanTablesSelector[i] >> 4)].aCodes, nppiDCTable, &apHuffmanDCTable[i]);
			nppiEncodeHuffmanSpecInitAlloc_JPEG(pHuffmanACTables[(oScanHeader.aHuffmanTablesSelector[i] & 0x0f)].aCodes, nppiACTable, &apHuffmanACTable[i]);
		}

		NPP_CHECK_NPP(nppiEncodeHuffmanScan_JPEG_8u16s_P3R(apdDCT, aDCTStep,
														   0, oScanHeader.nSs, oScanHeader.nSe, oScanHeader.nA >> 4, oScanHeader.nA & 0x0f,
														   pdScan, &nScanLength,
														   apHuffmanDCTable,
														   apHuffmanACTable,
														   nppSrcSize,
														   pJpegEncoderTemp));

		for (int i = 0; i < 3; ++i)
		{
			nppiEncodeHuffmanSpecFree_JPEG(apHuffmanDCTable[i]);
			nppiEncodeHuffmanSpecFree_JPEG(apHuffmanACTable[i]);
		}

		  // Write JPEG
		unsigned char *pDstJpeg = new unsigned char[ aSrcSize[0].width*aSrcSize[0].height*2];
		unsigned char *pDstOutput = pDstJpeg;

//		oFrameHeader.nWidth = oDstImageSize.width;
//		oFrameHeader.nHeight = oDstImageSize.height;

		writeMarker(0x0D8, pDstOutput);
		writeJFIFTag(pDstOutput);
		writeQuantizationTable(aQuantizationTables[0], pDstOutput);
		writeQuantizationTable(aQuantizationTables[1], pDstOutput);
		writeFrameHeader(oFrameHeader, pDstOutput);
		writeHuffmanTable(pHuffmanDCTables[0], pDstOutput);
		writeHuffmanTable(pHuffmanACTables[0], pDstOutput);
		writeHuffmanTable(pHuffmanDCTables[1], pDstOutput);
		writeHuffmanTable(pHuffmanACTables[1], pDstOutput);
		writeScanHeader(oScanHeader, pDstOutput);
		NPP_CHECK_CUDA(cudaMemcpy(pDstOutput, pdScan, nScanLength, cudaMemcpyDeviceToHost));
		pDstOutput += nScanLength;
		writeMarker(0x0D9, pDstOutput);

		{
			// Write result to file.
			std::ofstream outputFile(szOutputFile, ios::out | ios::binary);
			outputFile.write(reinterpret_cast<const char *>(pDstJpeg), static_cast<int>(pDstOutput - pDstJpeg));
		}

		delete [] pDstJpeg;

		cudaFree(pJpegEncoderTemp);
		cudaFree(pdQuantizationTables);
		cudaFree(pdScan);

		 nppiDCTFree(pDCTState);

		for (int i = 0; i < 3; ++i)
		{
			cudaFree(apdDCT[i]);
		}
}
