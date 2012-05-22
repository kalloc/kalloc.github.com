// Set path and prefix for session files
void qsm_init(char *path, char *name, int seconds);

// Start session (do it before printf "Content-Type" and other http headers")
int qsm_init_session();

// Set QPLATFORM session parameter for current session
void qsm_set_param(char *key, char *value);

// Get QPLATFORM session parameter for current session
char *qsm_get_param(char *key);

// Remove QPLATFORM session parameter for current session
int qsm_del_param(char *key);
