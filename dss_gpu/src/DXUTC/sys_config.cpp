#include "dx.h"
#include "sys_config.h"

static char *gScriptFile = "/mnt/dxd/config_0.txt";

Int32 syscfg_load(char *configfile, Int32 *data, Int32 dataSize)
{
	FILE *stream;
	char line[128];

	if(configfile == NULL)
		configfile = gScriptFile;

	if( (stream  = fopen( configfile, "r" )) == NULL ){
		printf( "The config file %s was not opened !!!\n", configfile);
		return -1;
	}

	do {		
		if(fgets(line, 128, stream) == NULL){
			break;
		}
		if(strlen(line) > 20){
			char *token;
			char seps[]   = ",:";
			int ident, field;
			Int32 dwData;
			token = strtok(line, seps);
			token = strtok( NULL, seps );
			ident = atoi(token);
			token = strtok( NULL, seps );
			field = atoi(token);
			token = strtok( NULL, seps );
			sscanf(token, " 0x%08X", &dwData);
			if((ident*16+field)*sizeof(Int32) < dataSize)
				data[ident*16+field] = dwData;
		}
	} while(1);

	fclose( stream );

	return 0;
}

Int32 syscfg_save(char *configfile, Int32 *data, Int32 dataSize)
{
	FILE *stream;
	int i;

	if(configfile == NULL)
		configfile = gScriptFile;

	if( (stream  = fopen( configfile, "w+" )) == NULL ){
		printf( "The config file %s was not opened !!!\n", configfile);
		return -1;
	}

	for(i = 0; i<(dataSize>>2); i++)
	{
		fprintf(stream,"VME 55, %03d:%03d : 0x%08X\r\n", i/16, i%16, data[i]);
	}

	fclose( stream );

	return 0;
}

Int32 syscfg_load_block(char *configfile, Int32 *data, Int32 blkId)
{
	FILE *stream;
	char line[128];

	if(configfile == NULL)
		configfile = gScriptFile;

	if( (stream  = fopen( configfile, "r" )) == NULL ){
		printf( "The config file %s was not opened !!!\n", configfile);
		return -1;
	}

	do {		
		if(fgets(line, 128, stream) == NULL){
			break;
		}
		if(strlen(line) > 20){
			char *token;
			char seps[]   = ",:";
			int ident, field;
			Int32 dwData;
			token = strtok(line, seps);
			token = strtok( NULL, seps );
			ident = atoi(token);
			token = strtok( NULL, seps );
			field = atoi(token);
			token = strtok( NULL, seps );
			sscanf(token, " 0x%08X", &dwData);
			if(ident == blkId)
				data[ident*16+field] = dwData;
			if(ident > blkId)
				break;
		}
	} while(1);

	fclose( stream );

	return 0;
}



