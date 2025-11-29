#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define MAX_ROWS 5000   // límite de filas
#define K 5             // número de vecinos

// Estructura del punto de datos
typedef struct {
    float age;
    float duration;
    int label;
} DataPoint;

// Distancia euclidiana entre dos puntos
float distance(DataPoint a, DataPoint b) {
    return sqrtf(
        (a.age - b.age) * (a.age - b.age) +
        (a.duration - b.duration) * (a.duration - b.duration)
    );
}

int main() {
    FILE *file = fopen("subset_knn.csv", "r");
    if (!file) {
        printf("Error abriendo archivo.\n");
        return 1;
    }

    DataPoint data[MAX_ROWS];
    int n = 0;
    char line[256];

    fgets(line, sizeof(line), file); // saltar encabezado

    // Lectura del CSV
    while (fgets(line, sizeof(line), file) && n < MAX_ROWS) {
        sscanf(line, "%f,%f,%d",
               &data[n].age,
               &data[n].duration,
               &data[n].label);
        n++;
    }
    fclose(file);

    printf("Datos cargados: %d filas\n", n);

    // División 80% train - 20% test
    int split = (int)(n * 0.8);
    int correct = 0;
    int total = n - split;

    // Clasificación KNN para cada punto de prueba
    for (int i = split; i < n; i++) {

        float dists[MAX_ROWS];
        int labels[MAX_ROWS];

        // Distancias a cada punto de entrenamiento
        for (int j = 0; j < split; j++) {
            dists[j] = distance(data[i], data[j]);
            labels[j] = data[j].label;
        }

        // Ordenamiento parcial: encontrar los K vecinos más cercanos
        for (int a = 0; a < K; a++) {
            for (int b = a + 1; b < split; b++) {
                if (dists[b] < dists[a]) {
                    float tmpd = dists[a];
                    dists[a] = dists[b];
                    dists[b] = tmpd;

                    int tmpl = labels[a];
                    labels[a] = labels[b];
                    labels[b] = tmpl;
                }
            }
        }

        // Votación de los K vecinos
        int votes_yes = 0, votes_no = 0;
        for (int k = 0; k < K; k++) {
            if (labels[k] == 1) votes_yes++;
            else votes_no++;
        }

        int prediction = (votes_yes > votes_no ? 1 : 0);

        if (prediction == data[i].label)
            correct++;
    }

    float accuracy = (correct * 100.0f) / total;
    printf("Accuracy KNN: %.2f%%\n", accuracy);

    return 0;
}
