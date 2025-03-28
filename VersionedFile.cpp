#include "VersionedFile.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <filesystem>

VersionedFile::VersionedFile(const std::string& filename) : filename(filename), nextBlockID(0) {
    load();
}

VersionedFile::~VersionedFile() {
    save();
}

void VersionedFile::create() {
    if (!std::filesystem::exists(filename)) {
        std::ofstream file(filename, std::ios::binary);
        file.close();
    } else {
        std::cout << "El archivo ya existe." << std::endl;
    }
}

void VersionedFile::listVersions() const {
    std::cout << "\nVersiones almacenadas:" << std::endl;
    for (const auto& version : versions) {
        std::cout << "Version " << version.versionNumber << " - Timestamp: " << version.timestamp << std::endl;
    }
}

void VersionedFile::listDataBlocks() const {
    std::cout << "\nBloques de datos almacenados:" << std::endl;
    for (const auto& block : dataBlocks) {
        std::cout << "Block ID: " << block.blockID << ", Referencias: " << block.referenceCount << std::endl;
    }
}

std::string VersionedFile::readContent(int versionNumber) const {
    std::string content;
    for (int blockID : versions[versionNumber].blockIDs) {
        for (const auto& block : dataBlocks) {
            if (block.blockID == blockID) {
                content.append(block.data.begin(), block.data.end());
                content.append(" ");
                break;
            }
        }
    }
    return content;
}

void VersionedFile::createVersion(const std::string& data) {
    for (auto& block : dataBlocks) {
        if (std::string(block.data.begin(), block.data.end()) == data) {
            block.referenceCount++;
            versions.back().blockIDs.push_back(block.blockID);
            std::cout << "Se reutilizo un bloque de datos existente." << std::endl;
            return;
        }
    }

    FileVersion newVersion;
    newVersion.versionNumber = versions.size();
    newVersion.timestamp = currentTimestamp();
    newVersion.fileSize = data.size();

    DataBlock newBlock;
    newBlock.blockID = nextBlockID++;
    newBlock.data = std::vector<char>(data.begin(), data.end());
    newBlock.referenceCount = 1;

    dataBlocks.push_back(newBlock);
    newVersion.blockIDs.push_back(newBlock.blockID);
    versions.push_back(newVersion);

    std::cout << "Nueva version creada y registrada." << std::endl;
}

std::string VersionedFile::currentTimestamp() const {
    std::time_t now = std::time(nullptr);
    std::string timestamp = std::ctime(&now);
    timestamp.pop_back();
    return timestamp;
}

void VersionedFile::cleanup() {
    dataBlocks.erase(std::remove_if(dataBlocks.begin(), dataBlocks.end(), [](const DataBlock& block) {
        return block.referenceCount == 0;
    }), dataBlocks.end());
}

void VersionedFile::save() {
    cleanup();
}

void VersionedFile::load() {}

int VersionedFile::getVersionCount() const {
    return versions.size();
}

std::string VersionedFile::concatenateVersions() const {
    std::string concatenatedContent;
    for (int i = 0; i < getVersionCount(); ++i) {
        concatenatedContent += readContent(i);
    }
    return concatenatedContent;
}
