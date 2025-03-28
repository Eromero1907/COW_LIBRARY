#ifndef COW_LIBRARY_H
#define COW_LIBRARY_H

#include <string>
#include <vector>
#include <deque>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <ctime>

#define BLOCK_SIZE 4096  // Tamaño de cada bloque de datos
#define MAX_VERSIONS 2 // Número máximo de versiones almacenadas

struct DataBlock {
    int blockID;
    std::vector<char> data;
    int referenceCount;
};

struct FileVersion {
    int versionNumber;
    std::string filename;
    size_t fileSize;
    std::string timestamp;
    std::vector<int> blockIDs;
};

class VersionedFile {
    private:
        std::string filename;         // Nombre del archivo base
        std::string logFilename;      // Nombre del archivo de log donde se guardan las versiones
        std::deque<long> versionOffsets; // Desplazamientos (offsets) de cada versión en el archivo de log
    
        void saveVersion();       // Guarda una nueva versión en el log
        void garbageCollector();  // Elimina versiones antiguas si se supera el límite
        void loadVersions();      // Carga los offsets de las versiones existentes en el log
    
    public:
        VersionedFile(const std::string& filename);  // Constructor
    
        void create();            // Crea un nuevo archivo y su log de versiones
        void open();              // Abre el archivo y carga sus versiones
        void close() {}           // No se necesita manejo especial en esta implementación
        void write(const std::string& data); // Escribe datos y genera una nueva versión
        std::string read(int version); // Lee una versión específica del archivo
        void listVersions();      // Lista todas las versiones disponibles
        void listDataBlocks();    // Muestra los bloques de datos guardados
    };
    
    #endif // COW_LIBRARY_H