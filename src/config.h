#ifndef CONFIG_H
#define CONFIG_H
//{
//[SERVER]
//addr = 127.0.0.1
//port = 1100

//[DB]
//addr = 127.0.0.1

//[REDIS]
//addr = 192.188.2.12

//}
int get_INI_value(const char *section, const char *item, char* value);
int set_INI_value(const char *section, const char *item, char* value);
int loadconf(const char *path);
void releaseconf();
#endif // CONFIG_H
