#ifndef VERSIONEDFILE_H
#define VERSIONEDFILE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <ctime>

const size_t BLOCK_SIZE = 4; // Tamaño del bloque en bytes

// Estructura para almacenar metadatos de cada versión
struct FileVersion {
    int versionNumber;         // Número de versión
    std::string filename;      // Nombre del archivo versionado
    size_t fileSize;           // Tamaño del archivo en bytes
    std::string timestamp;     // Fecha y hora de creación
    std::vector<int> blockIDs; // IDs de bloques de datos usados en esta versión
};

// Estructura para representar bloques de datos compartidos
struct DataBlock {
    int blockID;               // ID único del bloque
    std::vector<char> data;    // Datos almacenados en este bloque
    int referenceCount;        // Cuántas versiones están usando este bloque
};

class VersionedFile {
public:
    VersionedFile(const std::string& filename);
    
    void create(); // Check
    void open(); // Falta
    void close(); // Falta
    void write(const std::string& data); // Check
    std::string read(int versionNumber); // Check
    void listVersions(); // Check
    void listDataBlocks(); // Check

private:
    int findExistingBlock(const std::vector<char>& blockData); // Check

    std::string filename;
    std::vector<FileVersion> versions;
    std::vector<DataBlock> dataBlocks;
    int nextBlockID;
};

#endif // VERSIONEDFILE_H
