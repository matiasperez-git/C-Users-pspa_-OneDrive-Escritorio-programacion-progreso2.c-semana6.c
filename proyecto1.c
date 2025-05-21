#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_PRODUCTOS 5
#define MAX_RECURSOS 10

void convertirMinusculas(char destino[], const char origen[]) {
    int i = 0;
    while (origen[i] != '\0') {
        destino[i] = tolower(origen[i]);
        i++;
    }
    destino[i] = '\0';
}

int buscarRecurso(const char recurso[], char recursosDisponibles[][50], int totalRecursos) {
    char recursoMin[50], temp[50];
    convertirMinusculas(recursoMin, recurso);
    for (int i = 0; i < totalRecursos; i++) {
        convertirMinusculas(temp, recursosDisponibles[i]);
        if (strcmp(temp, recursoMin) == 0) {
            return i;
        }
    }
    return -1;
}

void mostrarListaProductos(char nombres[][50], int n) {
    printf("\n--- Lista de Productos ---\n");
    for (int i = 0; i < n; i++) {
        printf("%d. %s\n", i + 1, nombres[i]);
    }
}

void ingresarProductos(char nombres[][50], float tiempos[], int cantidades[], int recursosCantidad[][MAX_RECURSOS], char recursosNombres[][MAX_RECURSOS][50], int recursosPorProducto[], int n) {
    for (int i = 0; i < n; i++) {
        printf("\nProducto %d:\n", i + 1);

        printf("Nombre del producto: ");
        fgets(nombres[i], 50, stdin);
        nombres[i][strcspn(nombres[i], "\n")] = '\0'; 

        printf("Tiempo de fabricación en horas: ");
        scanf("%f", &tiempos[i]);
        while (getchar() != '\n'); 

        printf("Cantidad de recursos diferentes necesarios: ");
        scanf("%d", &recursosPorProducto[i]);
        while (getchar() != '\n');

        for (int j = 0; j < recursosPorProducto[i]; j++) {
            printf("Nombre del recurso %d: ", j + 1);
            fgets(recursosNombres[i][j], 50, stdin);
            recursosNombres[i][j][strcspn(recursosNombres[i][j], "\n")] = '\0';

            printf("Cantidad del recurso '%s' por unidad: ", recursosNombres[i][j]);
            scanf("%d", &recursosCantidad[i][j]);
            while (getchar() != '\n');
        }

        printf("Cantidad a fabricar: ");
        scanf("%d", &cantidades[i]);
        while (getchar() != '\n');
    }
}


float calcularTiempoTotalProducto(float tiempo, int cantidad) {
    return tiempo * cantidad;
}

float calcularTiempoTotal(float tiempos[], int cantidades[], int n) {
    float total = 0;
    for (int i = 0; i < n; i++) {
        total += calcularTiempoTotalProducto(tiempos[i], cantidades[i]);
    }
    return total;
}

void calcularRecursosTotalesPorNombre(char recursosTotales[][50], int cantidadesTotales[], int *total, char recursosNombres[][MAX_RECURSOS][50], int recursosCantidad[][MAX_RECURSOS], int cantidades[], int recursosPorProducto[], int n) {
    *total = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < recursosPorProducto[i]; j++) {
            int cantidad = recursosCantidad[i][j] * cantidades[i];
            int pos = buscarRecurso(recursosNombres[i][j], recursosTotales, *total);
            if (pos != -1) {
                cantidadesTotales[pos] += cantidad;
            } else {
                strcpy(recursosTotales[*total], recursosNombres[i][j]);
                cantidadesTotales[*total] = cantidad;
                (*total)++;
            }
        }
    }
}

void verificarDemanda(float tiempoTotal, float tiempoDisponible, char recursosTotales[][50], int cantidadesTotales[], char recursosDisponibles[][50], int recursosDispCant[], int totalRecursosUsados, int totalRecursosDisponibles) {
    if (tiempoTotal > tiempoDisponible) {
        printf("\nNo hay suficiente tiempo de producción (Requiere %.2f horas, Disponible %.2f)\n", tiempoTotal, tiempoDisponible);
    } else {
        printf("\nTiempo suficiente (%.2f horas requeridas de %.2f disponibles)\n", tiempoTotal, tiempoDisponible);
    }

    int todosDisponibles = 1;
    for (int i = 0; i < totalRecursosUsados; i++) {
        int pos = buscarRecurso(recursosTotales[i], recursosDisponibles, totalRecursosDisponibles);
        if (pos == -1 || cantidadesTotales[i] > recursosDispCant[pos]) {
            printf("FALTA recurso '%s': necesita %d, disponible %d\n", recursosTotales[i], cantidadesTotales[i], pos == -1 ? 0 : recursosDispCant[pos]);
            todosDisponibles = 0;
        } else {
            printf("OK recurso '%s': necesita %d, disponible %d\n", recursosTotales[i], cantidadesTotales[i], recursosDispCant[pos]);
        }
    }

    if (todosDisponibles && tiempoTotal <= tiempoDisponible) {
        printf("\nLa fábrica PUEDE cumplir con la demanda.\n");
    } else {
        printf("\nLa fábrica NO puede cumplir con la demanda.\n");
    }
}

void editarProducto(char nombres[][50], float tiempos[], int cantidades[], int recursosCantidad[][MAX_RECURSOS], char recursosNombres[][MAX_RECURSOS][50], int recursosPorProducto[], int n, char recursosDisponibles[][50], int totalRecursosDisponibles) {
    char nombreBuscar[50];
    printf("Ingrese el nombre del producto a editar: ");
    fgets(nombreBuscar, 50, stdin);
    nombreBuscar[strcspn(nombreBuscar, "\n")] = '\0';

    int indice = -1;
    for (int i = 0; i < n; i++) {
        if (strcmp(nombres[i], nombreBuscar) == 0) {
            indice = i;
            break;
        }
    }

    if (indice == -1) {
        printf("Producto no encontrado.\n");
        return;
    }

    printf("\nEditando Producto: %s\n", nombres[indice]);

    printf("Nuevo nombre del producto: ");
    fgets(nombres[indice], 50, stdin);
    nombres[indice][strcspn(nombres[indice], "\n")] = '\0';

    printf("Nuevo tiempo de fabricación en horas: ");
    scanf("%f", &tiempos[indice]);
    while (getchar() != '\n');

    printf("Nueva cantidad a fabricar: ");
    scanf("%d", &cantidades[indice]);
    while (getchar() != '\n');

    printf("Nuevos detalles de recursos:\n");
    for (int j = 0; j < recursosPorProducto[indice]; j++) {
        int recursoValido = 0;
        do {
            printf("Nombre del recurso %d: ", j + 1);
            fgets(recursosNombres[indice][j], 50, stdin);
            recursosNombres[indice][j][strcspn(recursosNombres[indice][j], "\n")] = '\0';

            for (int k = 0; k < totalRecursosDisponibles; k++) {
                if (strcmp(recursosNombres[indice][j], recursosDisponibles[k]) == 0) {
                    recursoValido = 1;
                    break;
                }
            }
            if (!recursoValido) {
                printf("Error: Recurso '%s' no es un recurso disponible. Por favor, ingrese un recurso válido.\n", recursosNombres[indice][j]);
            }
        } while (!recursoValido);

        printf("Cantidad del recurso '%s' por unidad: ", recursosNombres[indice][j]);
        scanf("%d", &recursosCantidad[indice][j]);
        while (getchar() != '\n');
    }

    printf("Producto editado exitosamente.\n");
}

void eliminarProducto(char nombres[][50], float tiempos[], int cantidades[], int recursosCantidad[][MAX_RECURSOS], char recursosNombres[][MAX_RECURSOS][50], int recursosPorProducto[], int *n) {
    char nombreEliminar[50];
    printf("Ingrese el nombre del producto a eliminar: ");
    fgets(nombreEliminar, 50, stdin);
    nombreEliminar[strcspn(nombreEliminar, "\n")] = '\0';

    int indice = -1;
    for (int i = 0; i < *n; i++) {
        if (strcmp(nombres[i], nombreEliminar) == 0) {
            indice = i;
            break;
        }
    }

    if (indice == -1) {
        printf("Producto no encontrado.\n");
        return;
    }

    for (int i = indice; i < *n - 1; i++) {
        strcpy(nombres[i], nombres[i + 1]);
        tiempos[i] = tiempos[i + 1];
        cantidades[i] = cantidades[i + 1];
        recursosPorProducto[i] = recursosPorProducto[i + 1];
        for (int j = 0; j < MAX_RECURSOS; j++) {
            recursosCantidad[i][j] = recursosCantidad[i + 1][j];
            strcpy(recursosNombres[i][j], recursosNombres[i + 1][j]);
        }
    }

    (*n)--;

    printf("Producto eliminado exitosamente.\n");
}

void tiempoPorProductoMenu(char nombres[][50], float tiempos[], int cantidades[], int n) {
    int opcionProducto;
    do {
        printf("\n--- Tiempo de Fabricación por Producto ---\n");
        for (int i = 0; i < n; i++) {
            printf("%d. %s\n", i + 1, nombres[i]);
        }
        printf("0. Volver al menú principal\n");
        printf("Seleccione un producto para ver su tiempo de fabricación: ");
        scanf("%d", &opcionProducto);
        while (getchar() != '\n');

        if (opcionProducto > 0 && opcionProducto <= n) {
            float tiempoProducto = calcularTiempoTotalProducto(tiempos[opcionProducto - 1], cantidades[opcionProducto - 1]);
            printf("Tiempo de fabricación para %s: %.2f horas\n", nombres[opcionProducto - 1], tiempoProducto);
        } else if (opcionProducto != 0) {
            printf("Opción inválida.\n");
        }
    } while (opcionProducto != 0);
}

int main() {
    char nombres[MAX_PRODUCTOS][50];
    float tiempos[MAX_PRODUCTOS];
    int cantidades[MAX_PRODUCTOS];
    int recursosCantidad[MAX_PRODUCTOS][MAX_RECURSOS];
    char recursosNombres[MAX_PRODUCTOS][MAX_RECURSOS][50];
    int recursosPorProducto[MAX_PRODUCTOS];
    int n = 0;

    char recursosDisponibles[MAX_RECURSOS][50];
    int recursosDispCant[MAX_RECURSOS];
    int totalRecursosDisponibles = 0;

    printf("¿Cuántos productos desea ingresar? (máx %d): ", MAX_PRODUCTOS);
    scanf("%d", &n);
    while (getchar() != '\n');

    ingresarProductos(nombres, tiempos, cantidades, recursosCantidad, recursosNombres, recursosPorProducto, n);

    printf("\n--- Ingreso de Recursos Disponibles en la Fábrica ---\n");
    printf("¿Cuántos tipos de recursos tiene la empresa? (máx %d): ", MAX_RECURSOS);
    scanf("%d", &totalRecursosDisponibles);
    while (getchar() != '\n');

    for (int i = 0; i < totalRecursosDisponibles; i++) {
        printf("Nombre del recurso %d: ", i + 1);
        fgets(recursosDisponibles[i], 50, stdin);
        recursosDisponibles[i][strcspn(recursosDisponibles[i], "\n")] = '\0';

        printf("Cantidad disponible de '%s': ", recursosDisponibles[i]);
        scanf("%d", &recursosDispCant[i]);
        while (getchar() != '\n');
    }

    int opcion;
    do {
        printf("\n--- MENÚ ---\n");
        printf("1. Ver tiempo de fabricación\n");
        printf("2. Ver recursos necesarios por tipo\n");
        printf("3. Verificar si se puede cumplir la demanda\n");
        printf("4. Editar producto\n");
        printf("5. Eliminar producto\n");
        printf("6. Salir\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);
        while (getchar() != '\n');

        switch (opcion) {
            case 1: {
                if (n > 0) {
                    float total = calcularTiempoTotal(tiempos, cantidades, n);
                    printf("Tiempo total de fabricación: %.2f horas\n", total);
                    tiempoPorProductoMenu(nombres, tiempos, cantidades, n); 
                } else {
                    printf("No hay productos ingresados.\n");
                }
                break;
            }
            case 2: {
                if (n > 0) {
                    char recursosTotales[MAX_RECURSOS][50];
                    int cantidadesTotales[MAX_RECURSOS], total;
                    calcularRecursosTotalesPorNombre(recursosTotales, cantidadesTotales, &total, recursosNombres, recursosCantidad, cantidades, recursosPorProducto, n);
                    printf("\n--- Recursos necesarios por tipo ---\n");
                    for (int i = 0; i < total; i++) {
                        printf("%s: %d unidades\n", recursosTotales[i], cantidadesTotales[i]);
                    }
                } else {
                    printf("No hay productos ingresados.\n");
                }
                break;
            }
            case 3: {
                if (n > 0) {
                    float tiempoTotal = calcularTiempoTotal(tiempos, cantidades, n);
                    char recursosTotales[MAX_RECURSOS][50];
                    int cantidadesTotales[MAX_RECURSOS], total;
                    calcularRecursosTotalesPorNombre(recursosTotales, cantidadesTotales, &total, recursosNombres, recursosCantidad, cantidades, recursosPorProducto, n);
                    float tiempoDisponible;
                    printf("\nIngrese el tiempo de producción disponible (en horas): ");
                    scanf("%f", &tiempoDisponible);
                    while (getchar() != '\n');
                    verificarDemanda(tiempoTotal, tiempoDisponible, recursosTotales, cantidadesTotales, recursosDisponibles, recursosDispCant, total, totalRecursosDisponibles);
                } else {
                    printf("No hay productos ingresados.\n");
                }
                break;
            }
            case 4: {
                if (n > 0 ) {
                    editarProducto(nombres, tiempos, cantidades, recursosCantidad, recursosNombres, recursosPorProducto, n, recursosDisponibles, totalRecursosDisponibles);
                } else {
                    printf("No hay productos ingresados para editar.\n");
                }
                break;
            }
            case 5: {
                if (n > 0) {
                    eliminarProducto(nombres, tiempos, cantidades, recursosCantidad, recursosNombres, recursosPorProducto, &n);
                } else {
                    printf("No hay productos ingresados para eliminar.\n");
                }
                break;
            }
            case 6:
                printf("Saliendo del programa.\n");
                break;
            default:
                printf("Opción inválida.\n");
        }
    } while (opcion != 6);

    return 0;
}