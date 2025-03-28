#ifndef VERSIONEDFILE_H
#define VERSIONEDFILE_H

#include <string>
#include <vector>

struct FileVersion {
    int versionNumber;
    size_t fileSize;
    std::string timestamp;
    std::vector<int> blockIDs;
};

struct DataBlock {
    int blockID;
    std::vector<char> data;
    int referenceCount;
};

class VersionedFile {
public:
    VersionedFile(const std::string& filename);
    ~VersionedFile();

    void create();
    void listVersions() const;
    void listDataBlocks() const;
    void createVersion(const std::string& data);
    std::string readContent(int versionNumber) const;
    std::string concatenateVersions() const;
    int getVersionCount() const;

private:
    std::string currentTimestamp() const;
    void cleanup();
    void save();
    void load();

    std::string filename;
    std::vector<FileVersion> versions;
    std::vector<DataBlock> dataBlocks;
    int nextBlockID;
};

#endif
