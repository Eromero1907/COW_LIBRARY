#include "VersionedFile.h"
#include <iostream>
#include <string>

int main() {
    VersionedFile* versionedFile = nullptr;
    std::string filename;
    int opcion;

    do {
        std::cout << "\n--- MENU ---\n";
        std::cout << "1. Crear un nuevo archivo versionado\n";
        std::cout << "2. Abrir un archivo existente\n";
        std::cout << "3. Leer contenido de una version\n";
        std::cout << "4. Crear nueva version\n";
        std::cout << "5. Listar versiones\n";
        std::cout << "6. Listar bloques de datos\n";
        std::cout << "7. Concatenar versiones\n";
        std::cout << "8. Guardar archivo\n";
        std::cout << "9. Salir\n";
        std::cout << "Seleccione una opcion: ";
        std::cin >> opcion;

        switch (opcion) {
            case 1: {
                std::cout << "Ingrese el nombre del archivo a crear: ";
                std::cin >> filename;
                versionedFile = new VersionedFile(filename);
                std::cout << "Archivo creado exitosamente." << std::endl;
                break;
            }
            case 2: {
                std::cout << "Ingrese el nombre del archivo a abrir: ";
                std::cin >> filename;
                versionedFile = new VersionedFile(filename);
                std::cout << "Archivo abierto exitosamente." << std::endl;
                break;
            }
            case 3: {
                if (versionedFile) {
                    int versionNumber;
                    std::cout << "Ingrese el numero de la version a leer: ";
                    std::cin >> versionNumber;
                    std::string content = versionedFile->readContent(versionNumber);
                    std::cout << "Contenido de la version " << versionNumber << ":\n" << content << std::endl;
                } else {
                    std::cout << "Primero debe crear o abrir un archivo." << std::endl;
                }
                break;
            }
            case 4: {
                if (versionedFile) {
                    std::cout << "Ingrese el contenido para la nueva version: ";
                    std::string content;
                    std::cin.ignore();
                    std::getline(std::cin, content);
                    versionedFile->createVersion(content);
                } else {
                    std::cout << "Primero debe crear o abrir un archivo." << std::endl;
                }
                break;
            }
            case 5: {
                if (versionedFile) {
                    versionedFile->listVersions();
                } else {
                    std::cout << "Primero debe crear o abrir un archivo." << std::endl;
                }
                break;
            }
            case 6: {
                if (versionedFile) {
                    versionedFile->listDataBlocks();
                } else {
                    std::cout << "Primero debe crear o abrir un archivo." << std::endl;
                }
                break;
            }
            case 7: {
                if (versionedFile) {
                    std::string concatenatedContent = versionedFile->concatenateVersions();
                    std::cout << "Contenido concatenado:\n" << concatenatedContent << std::endl;
                    versionedFile->createVersion(concatenatedContent); // Crear nueva version con contenido concatenado
                    std::cout << "Nueva version creada con el contenido concatenado." << std::endl;
                } else {
                    std::cout << "Primero debe crear o abrir un archivo." << std::endl;
                }
                break;
            }
            case 8: {
                if (versionedFile) {
                    versionedFile->save();
                    std::cout << "Archivo guardado exitosamente." << std::endl;
                } else {
                    std::cout << "Primero debe crear o abrir un archivo." << std::endl;
                }
                break;
            }
            case 9:
                std::cout << "Saliendo del programa." << std::endl;
                break;
            default:
                std::cout << "Opcion invalida." << std::endl;
        }
    } while (opcion != 9);

    if (versionedFile) {
        delete versionedFile;
    }

    return 0;
}


