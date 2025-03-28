#include "COW_library.h"

VersionedFile::VersionedFile(const std::string& filename) 
    : filename(filename), logFilename(filename + ".log") {
    loadVersions();
}

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

    if (!std::filesystem::exists(logFilename)) {
        std::ofstream logFile(logFilename, std::ios::trunc);
        if (logFile) {
            logFile.close();
            std::cout << "Archivo de versiones creado: " << logFilename << std::endl;
        } else {
            std::cerr << "Error al crear el archivo de versiones." << std::endl;
        }
    }
}

void VersionedFile::open() {
    if (std::filesystem::exists(filename)) {
        std::cout << "Archivo abierto correctamente: " << filename << std::endl;
        loadVersions();
    } else {
        std::cerr << "Error: el archivo no existe." << std::endl;
    }
}

void VersionedFile::write(const std::string& data) {
    std::ofstream outFile(filename, std::ios::app);
    if (!outFile) {
        std::cerr << "Error al abrir el archivo para escritura." << std::endl;
        return;
    }
    outFile << data;
    outFile.close();

    saveVersion();

    if (versionOffsets.size() > MAX_VERSIONS) {
        garbageCollector();
    }
}

void VersionedFile::saveVersion() {
    std::ifstream src(filename, std::ios::binary);
    if (!src) {
        std::cerr << "Error al abrir el archivo fuente." << std::endl;
        return;
    }

    std::ofstream logFile(logFilename, std::ios::app | std::ios::binary);
    if (!logFile) {
        std::cerr << "Error al abrir el archivo de versiones." << std::endl;
        return;
    }

    long offset = logFile.tellp();
    versionOffsets.push_back(offset);

    logFile << "[VERSION " << versionOffsets.size() - 1 << "]\n";
    logFile << src.rdbuf();
    logFile << "\n[END VERSION]\n";

    std::cout << "Nueva versión guardada en " << logFilename << " con offset " << offset << std::endl;
}

void VersionedFile::garbageCollector() {
    if (versionOffsets.empty()) return;

    std::ifstream logFile(logFilename, std::ios::binary);
    if (!logFile) {
        std::cerr << "Error al abrir el archivo de versiones." << std::endl;
        return;
    }

    std::ofstream tempFile("temp.log", std::ios::trunc | std::ios::binary);
    if (!tempFile) {
        std::cerr << "Error al crear archivo temporal." << std::endl;
        return;
    }

    versionOffsets.pop_front();

    tempFile.seekp(0, std::ios::beg);
    for (long offset : versionOffsets) {
        logFile.seekg(offset);
        std::string line;
        while (std::getline(logFile, line)) {
            tempFile << line << "\n";
            if (line == "[END VERSION]") break;
        }
    }

    logFile.close();
    tempFile.close();

    std::filesystem::remove(logFilename);
    std::filesystem::rename("temp.log", logFilename);
}

std::string VersionedFile::read(int version) {
    if (version < 0 || version >= versionOffsets.size()) {
        std::cerr << "Número de versión inválido." << std::endl;
        return "";
    }

    std::ifstream logFile(logFilename, std::ios::binary);
    if (!logFile) {
        std::cerr << "Error al abrir el archivo de versiones." << std::endl;
        return "";
    }

    logFile.seekg(versionOffsets[version]);
    std::string line, content;
    bool reading = false;

    while (std::getline(logFile, line)) {
        if (line.find("[VERSION") != std::string::npos) {
            reading = true;
            continue;
        }
        if (line.find("[END VERSION]") != std::string::npos) {
            break;
        }
        if (reading) {
            content += line + "\n";
        }
    }

    return content;
}

void VersionedFile::listVersions() {
    std::cout << "Versiones disponibles en " << logFilename << ":" << std::endl;
    for (size_t i = 0; i < versionOffsets.size(); i++) {
        std::cout << " - Version " << i << " (Offset: " << versionOffsets[i] << ")" << std::endl;
    }
}

void VersionedFile::listDataBlocks() {
    std::cout << "Mostrando bloques de datos..." << std::endl;
    for (size_t i = 0; i < versionOffsets.size(); i++) {
        std::cout << "Versión " << i << ":" << std::endl;
        std::cout << read(i) << std::endl;
    }
}

void VersionedFile::loadVersions() {
    std::ifstream logFile(logFilename, std::ios::binary);
    if (!logFile) {
        return;
    }

    long pos;
    std::string line;
    while (std::getline(logFile, line)) {
        if (line.find("[VERSION") != std::string::npos) {
            pos = logFile.tellg();
            versionOffsets.push_back(pos);
        }
    }
}

