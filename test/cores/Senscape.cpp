#include "Senscape.h"

error_t postTask(void (*function)(void*), void *param) {
    function(param);
    return SUCCESS;
}
