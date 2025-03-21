#include "COW_library.h"

VersionedFile::VersionedFile(const std::string& filename) : filename(filename), nextBlockID(0) {}

void VersionedFile::create() {
    if (!std::filesystem::exists(filename)) {
        std::ofstream file(filename);
        if (file) {
            file.close();
            std::cout << "Archivo creado: " << filename << std::endl;
        } else {
            std::cerr << "Error al crear el archivo." << std::endl;
        }
    } else {
        std::cout << "El archivo ya existe." << std::endl;
    }
}

void VersionedFile::open() {
    if (std::filesystem::exists(filename)) {
        std::cout << "Archivo abierto: " << filename << std::endl;
    } else {
        std::cerr << "El archivo no existe." << std::endl;
    }
}

void VersionedFile::close() {
    std::cout << "Archivo cerrado: " << filename << std::endl;
}

void VersionedFile::write(const std::string& data) {
    if (!versions.empty() && read(versions.size() - 1) == data) {
        std::cout << "No hay cambios en los datos. No se crea nueva versión." << std::endl;
        return;
    }

    std::string versionedFilename = filename + ".v" + std::to_string(versions.size());
    std::ofstream outFile(versionedFilename);

    std::vector<int> newBlockIDs;
    size_t dataSize = data.size();

    for (size_t i = 0; i < dataSize; i += BLOCK_SIZE) {
        std::vector<char> blockData(data.begin() + i, data.begin() + std::min(i + BLOCK_SIZE, dataSize));

        int existingBlockID = findExistingBlock(blockData);
        if (existingBlockID != -1) {
            newBlockIDs.push_back(existingBlockID);
            dataBlocks[existingBlockID].referenceCount++;
        } else {
            DataBlock newBlock = {nextBlockID++, blockData, 1};
            dataBlocks.push_back(newBlock);
            newBlockIDs.push_back(newBlock.blockID);
        }
        outFile.write(blockData.data(), blockData.size());
    }
    outFile.close();

    time_t now = time(0);
    std::string timestamp = ctime(&now);
    timestamp.pop_back();

    FileVersion newVersion = {static_cast<int>(versions.size()), versionedFilename, dataSize, timestamp, newBlockIDs};
    versions.push_back(newVersion);

    std::cout << "Nueva versión creada: " << versionedFilename << " (" << newVersion.fileSize << " bytes, " << newVersion.timestamp << ")" << std::endl;
}

std::string VersionedFile::read(int versionNumber) {
    if (versionNumber < 0 || versionNumber >= versions.size()) {
        std::cerr << "Número de versión inválido." << std::endl;
        return "";
    }

    std::ifstream file(versions[versionNumber].filename);
    if (file) {
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        return content;
    }
    return "";
}

void VersionedFile::listVersions() {
    std::cout << "Versiones disponibles:" << std::endl;
    for (const auto& version : versions) {
        std::cout << " " << version.versionNumber << ": " << version.filename
                  << " (" << version.fileSize << " bytes, " << version.timestamp << ")" << std::endl;
    }
}

void VersionedFile::listDataBlocks() {
    std::cout << "\nBloques de datos almacenados:" << std::endl;
    for (const auto& block : dataBlocks) {
        std::cout << "Block ID: " << block.blockID
                  << ", Referencias: " << block.referenceCount
                  << ", Contenido: '";
        for (char c : block.data) {
            std::cout << c;
        }
        std::cout << "'" << std::endl;
    }
}

int VersionedFile::findExistingBlock(const std::vector<char>& blockData) {
    for (const auto& block : dataBlocks) {
        if (block.data == blockData) {
            return block.blockID;
        }
    }
    return -1;
}
