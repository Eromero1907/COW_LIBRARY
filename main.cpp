#include <iostream>
#include "VersionedFile.h"

int main() {
    VersionedFile file("testfile.db");
    file.create();

    int opcion;
    while (true) {
        std::cout << "\n1. Crear nueva version" << std::endl;
        std::cout << "2. Listar versiones" << std::endl;
        std::cout << "3. Mostrar bloques de datos" << std::endl;
        std::cout << "4. Leer una version" << std::endl;
        std::cout << "5. Juntar todas las versiones" << std::endl;
        std::cout << "6. Salir" << std::endl;
        std::cout << "Seleccione una opcion: ";
        std::cin >> opcion;
        std::cin.ignore();

        if (opcion == 1) {
            std::cout << "Ingrese el contenido de la nueva version: ";
            std::string newData;
            std::getline(std::cin, newData);
            file.createVersion(newData);
        } else if (opcion == 2) {
            file.listVersions();
        } else if (opcion == 3) {
            file.listDataBlocks();
        } else if (opcion == 4) {
            std::cout << "Ingrese el numero de version a leer: ";
            int versionNumber;
            std::cin >> versionNumber;

            if (versionNumber >= 0 && versionNumber < file.getVersionCount()) {
                std::cout << "Contenido de la version " << versionNumber << ":\n" << file.readContent(versionNumber) << std::endl;
            } else {
                std::cout << "Numero de version invalido." << std::endl;
            }
        } else if (opcion == 5) {
            std::string concatenatedContent = file.concatenateVersions();
            std::cout << "Contenido de todas las versiones concatenadas:\n" << concatenatedContent << std::endl;
        } else if (opcion == 6) {
            break;
        } else {
            std::cout << "Opcion invalida." << std::endl;
        }
    }

    return 0;
}
