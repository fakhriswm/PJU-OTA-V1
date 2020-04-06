#include <arduino.h>
#include <Update.h>

#include "FS.h"
#include "SPIFFS.h"

class update_manager {
    public :
        void printPercent(uint32_t readLength, uint32_t contentLength);
        void performUpdate(Stream &updateSource, size_t updateSize);
        void updateFromFS();
        void listDir(fs::FS &fs, const char *dirname, uint8_t levels);
        void appendFile(fs::FS &fs, const char *path, const char *message);
        void readFile(fs::FS &fs, const char *path);
        void writeFile(fs::FS &fs, const char *path, const char *message);
        void deleteFile(fs::FS &fs, const char *path);
        void spiffs_init();
};