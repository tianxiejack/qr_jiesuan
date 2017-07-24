#ifndef _SYSTEM_CONFIGURE_H
#define _SYSTEM_CONFIGURE_H

Int32 syscfg_load(char *configfile, Int32 *data, Int32 dataSize);
Int32 syscfg_save(char *configfile, Int32 *data, Int32 dataSize);
Int32 syscfg_load_block(char *configfile, Int32 *data, Int32 blkId);

#endif
