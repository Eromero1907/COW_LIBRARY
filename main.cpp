#include <iostream>
#include "COW_library.h"

void mostrarMenu() {
    std::cout << "\n===== GESTOR DE VERSIONES =====\n";
    std::cout << "1. Crear archivo\n";
    std::cout << "2. Abrir archivo\n";
    std::cout << "3. Cerrar archivo\n";
    std::cout << "4. Escribir datos (nueva version)\n";
    std::cout << "5. Leer version especifica\n";
    std::cout << "6. Listar versiones\n";
    std::cout << "7. Listar bloques de datos\n";
    std::cout << "8. Salir\n";
    std::cout << "Seleccione una opcion: ";
}

int main() {
    std::string nombreArchivo;
    std::cout << "Ingrese el nombre del archivo a gestionar: ";
    std::cin >> nombreArchivo;

    VersionedFile archivo(nombreArchivo);
    int opcion;

    do {
        mostrarMenu();
        std::cin >> opcion;
        std::cin.ignore();  // Evita problemas con `getline()`

        switch (opcion) {
            case 1:
                archivo.create();
                break;
            case 2:
                archivo.open();
                break;
            case 3:
                archivo.close();
                break;
            case 4: {
                std::cout << "Ingrese los datos a escribir: ";
                std::string datos;
                std::getline(std::cin, datos);
                archivo.write(datos);
                break;
            }
            case 5: {
                std::cout << "Ingrese el numero de version a leer: ";
                int version;
                std::cin >> version;
                std::cin.ignore();
                std::string contenido = archivo.read(version);
                if (!contenido.empty()) {
                    std::cout << "Contenido de la version " << version << ":\n" << contenido << "\n";
                }
                break;
            }
            case 6:
                archivo.listVersions();
                break;
            case 7:
                archivo.listDataBlocks();
                break;
            case 8:
                std::cout << "Saliendo...\n";
                break;
            default:
                std::cout << "Opcion no valida, intente de nuevo.\n";
                break;
        }
    } while (opcion != 8);

    return 0;
}
