
#ifndef SVERAC_DEBUG_H
#define SVERAC_DEBUG_H

#ifdef DEBUG
#define debug(fmt, ...) printf("%s:%d " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__);
#else
#define debug(fmt, ...) /* (do nothing) */
#endif

#endif // SVERAC_DEBUG_H