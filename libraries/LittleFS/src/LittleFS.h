// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef _LITTLEFS_H_
#define _LITTLEFS_H_

#include "FS.h"

namespace fs
{

struct FSInfo {
    size_t totalBytes;
    size_t usedBytes;
    int blockSize;
    int maxOpenFiles;
    int maxPathLength;
    int pageSize;

    FSInfo(size_t _totalBytes = 0, size_t _usedBytes = 0, int _blockSize = 4096, int _pageSize = 256, int _maxOpenFiles = 10, int _maxPathLength = LFS_NAME_MAX) :
        totalBytes(_totalBytes),
        usedBytes(_usedBytes),
        blockSize(_blockSize),
        maxOpenFiles(_maxOpenFiles),
        maxPathLength(_maxPathLength),
        pageSize(_pageSize)
    {}
};

class LittleFSFS : public FS
{
public:
    LittleFSFS();
    ~LittleFSFS();
    bool begin(bool formatOnFail=false, const char * basePath="/littlefs", uint8_t maxOpenFiles=10, const char * partitionLabel="spiffs");
    bool format();
    size_t totalBytes();
    size_t usedBytes();
    void end();

    void info(FSInfo &info);

private:
    char * partitionLabel_;
};

}

extern fs::LittleFSFS LittleFS;


#endif
