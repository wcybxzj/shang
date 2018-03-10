#include "server.h"

/* ------------------------- Utility functions ------------------------------ */

#ifdef __linux__
/* Returns 1 if Transparent Huge Pages support is enabled in the kernel.
 *  * Otherwise (or if we are unable to check) 0 is returned. */
int THPIsEnabled(void) {
	char buf[1024];

	FILE *fp = fopen("/sys/kernel/mm/transparent_hugepage/enabled","r");
	if (!fp) return 0;
	if (fgets(buf,sizeof(buf),fp) == NULL) {
		fclose(fp);
		return 0;
	}
	fclose(fp);
	return (strstr(buf,"[never]") == NULL) ? 1 : 0;
}
#endif
