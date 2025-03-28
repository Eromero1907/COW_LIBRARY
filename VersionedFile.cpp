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
    cleanup(); // Limpiar bloques no referenciados

    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "No se pudo abrir el archivo para guardar." << std::endl;
        return;
    }

    // Guardar la cantidad de bloques de datos
    size_t numBlocks = dataBlocks.size();
    file.write(reinterpret_cast<const char*>(&numBlocks), sizeof(numBlocks));

    // Guardar cada bloque de datos
    for (const auto& block : dataBlocks) {
        file.write(reinterpret_cast<const char*>(&block.blockID), sizeof(block.blockID));
        file.write(reinterpret_cast<const char*>(&block.referenceCount), sizeof(block.referenceCount));

        size_t dataSize = block.data.size();
        file.write(reinterpret_cast<const char*>(&dataSize), sizeof(dataSize));
        file.write(block.data.data(), dataSize);
    }

    // Guardar la cantidad de versiones
    size_t numVersions = versions.size();
    file.write(reinterpret_cast<const char*>(&numVersions), sizeof(numVersions));

    // Guardar cada versión
    for (const auto& version : versions) {
        file.write(reinterpret_cast<const char*>(&version.versionNumber), sizeof(version.versionNumber));

        size_t timestampSize = version.timestamp.size();
        file.write(reinterpret_cast<const char*>(&timestampSize), sizeof(timestampSize));
        file.write(version.timestamp.data(), timestampSize);

        file.write(reinterpret_cast<const char*>(&version.fileSize), sizeof(version.fileSize));

        size_t numBlockIDs = version.blockIDs.size();
        file.write(reinterpret_cast<const char*>(&numBlockIDs), sizeof(numBlockIDs));
        file.write(reinterpret_cast<const char*>(version.blockIDs.data()), numBlockIDs * sizeof(int));
    }

    file.close();
    std::cout << "Datos guardados exitosamente en el archivo." << std::endl;
}


void VersionedFile::load() {
    std::ifstream file(filename, std::ios::binary | std::ios::ate); // Abrir en modo binario y posicionar al final
    if (!file) {
        std::cerr << "No se pudo abrir el archivo para cargar." << std::endl;
        return;
    }

    std::streamsize fileSize = file.tellg(); // Obtener tamaño del archivo
    if (fileSize == 0) { // Si el archivo está vacío
        std::cout << "El archivo esta vacio. No hay nada que cargar." << std::endl;
        file.close();
        return;
    }

    file.seekg(0, std::ios::beg); // Volver al inicio del archivo para leerlo

    // Limpiar las listas actuales antes de cargar nuevas
    dataBlocks.clear();
    versions.clear();
    nextBlockID = 0;

    // Cargar la cantidad de bloques de datos
    size_t numBlocks;
    file.read(reinterpret_cast<char*>(&numBlocks), sizeof(numBlocks));

    if (!file) { // Verificar si la lectura fue exitosa
        std::cerr << "Error al leer la cantidad de bloques de datos." << std::endl;
        return;
    }

    // Cargar cada bloque de datos
    for (size_t i = 0; i < numBlocks; ++i) {
        DataBlock block;

        file.read(reinterpret_cast<char*>(&block.blockID), sizeof(block.blockID));
        file.read(reinterpret_cast<char*>(&block.referenceCount), sizeof(block.referenceCount));

        size_t dataSize;
        file.read(reinterpret_cast<char*>(&dataSize), sizeof(dataSize));

        if (!file || dataSize == 0) { // Si falla la lectura o dataSize es inválido
            std::cerr << "Error al leer un bloque de datos. Archivo posiblemente corrupto." << std::endl;
            return;
        }

        block.data.resize(dataSize);
        file.read(block.data.data(), dataSize);

        dataBlocks.push_back(block);

        if (block.blockID >= nextBlockID) {
            nextBlockID = block.blockID + 1;
        }
    }

    // Cargar la cantidad de versiones
    size_t numVersions;
    file.read(reinterpret_cast<char*>(&numVersions), sizeof(numVersions));

    if (!file) {
        std::cerr << "Error al leer la cantidad de versiones." << std::endl;
        return;
    }

    for (size_t i = 0; i < numVersions; ++i) {
        FileVersion version;

        file.read(reinterpret_cast<char*>(&version.versionNumber), sizeof(version.versionNumber));

        size_t timestampSize;
        file.read(reinterpret_cast<char*>(&timestampSize), sizeof(timestampSize));

        if (!file || timestampSize == 0) {
            std::cerr << "Error al leer un timestamp. Archivo posiblemente corrupto." << std::endl;
            return;
        }

        version.timestamp.resize(timestampSize);
        file.read(version.timestamp.data(), timestampSize);

        file.read(reinterpret_cast<char*>(&version.fileSize), sizeof(version.fileSize));

        size_t numBlockIDs;
        file.read(reinterpret_cast<char*>(&numBlockIDs), sizeof(numBlockIDs));

        if (!file) {
            std::cerr << "Error al leer la cantidad de IDs de bloques." << std::endl;
            return;
        }

        version.blockIDs.resize(numBlockIDs);
        file.read(reinterpret_cast<char*>(version.blockIDs.data()), numBlockIDs * sizeof(int));

        if (!file) {
            std::cerr << "Error al leer los IDs de bloques. Archivo posiblemente corrupto." << std::endl;
            return;
        }

        versions.push_back(version);
    }

    file.close();
    std::cout << "Datos cargados exitosamente desde el archivo." << std::endl;
}

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