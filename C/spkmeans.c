#include "spkmeans.h"

// TODO: free memory


///* function declarations */
double euclidean_distance(double *vector1, double *vector2, int max_d);

double **wam(double **vectors, int num_of_vectors, int vector_dimension);

double **ddg(double **wam_matrix, int num_of_vectors);

double **gl(double **ddg_matrix, double **wam_matrix, int num_of_vectors);

double **transpose(double **matrix, int num_of_vectors);

int *find_largest_absolute_value_coordinates(double **matrix, int num_of_vectors);

double **calculate_rotation_matrix(double **mat, int num_of_vectors, int *coordinates);

int check_convergence(double **matrix, double **previous_matrix, int num_of_vectors, double eps);

double **jacobi(double **matrix, int num_of_vectors);

double **multiply_matrices(double **matrix1, double **matrix2, int num_of_vectors);

int sign(double x);

//
//void kmeans(int num_of_clusters, int num_of_iterations, int vector_dimension, int count, double vector_list[][vector_dimension], double eps, double init_centroids[][vector_dimension]);
//

double euclidean_distance(double *vector1, double *vector2, int max_d) {
    double total_sum;
    int i;
    total_sum = 0;
    for (i = 0; i < max_d; i++) {
        total_sum += pow(vector1[i] - vector2[i], 2);
    }
    return sqrt(total_sum);
}


double **wam(double **vectors, int num_of_vectors, int vector_dimension) {
    double **wam_matrix;
    int i;
    int j;
    wam_matrix = (double **) malloc(num_of_vectors * sizeof(double *));
    for (i = 0; i < num_of_vectors; i++) {
        wam_matrix[i] = (double *) malloc(num_of_vectors * sizeof(double));
    }
    for (i = 0; i < num_of_vectors; i++) {
        for (j = 0; j < num_of_vectors; j++) {
            if (i == j) {
                wam_matrix[i][j] = 0;
            } else {
                wam_matrix[i][j] = exp(-pow(euclidean_distance(vectors[i], vectors[j], vector_dimension), 2) / 2);
            }
        }
    }
    return wam_matrix;
}

double **ddg(double **wam_matrix, int num_of_vectors) {
    double **ddg_matrix;
    int i;
    int j;
    double sum;
    ddg_matrix = (double **) malloc(num_of_vectors * sizeof(double *));
    for (i = 0; i < num_of_vectors; i++) {
        ddg_matrix[i] = (double *) malloc(num_of_vectors * sizeof(double));
    }
    for (i = 0; i < num_of_vectors; i++) {
        sum = 0;
        for (j = 0; j < num_of_vectors; j++) {
            sum += wam_matrix[i][j];
        }
        ddg_matrix[i][i] = sum;
    }
    return ddg_matrix;
}

double **gl(double **ddg_matrix, double **wam_matrix, int num_of_vectors) {
    double **gl_matrix;
    int i;
    int j;
    gl_matrix = (double **) malloc(num_of_vectors * sizeof(double *));
    for (i = 0; i < num_of_vectors; i++) {
        gl_matrix[i] = (double *) malloc(num_of_vectors * sizeof(double));
    }
    for (i = 0; i < num_of_vectors; i++) {
        for (j = 0; j < num_of_vectors; j++) {
            gl_matrix[i][j] = ddg_matrix[i][j] - wam_matrix[i][j];
        }
    }
    return gl_matrix;
}


double **transpose(double **matrix, int num_of_vectors) {
    double **transposed_matrix;
    int i;
    int j;
    transposed_matrix = (double **) malloc(num_of_vectors * sizeof(double *));
    for (i = 0; i < num_of_vectors; i++) {
        transposed_matrix[i] = (double *) malloc(num_of_vectors * sizeof(double));
    }
    for (i = 0; i < num_of_vectors; i++) {
        for (j = 0; j < num_of_vectors; j++) {
            transposed_matrix[i][j] = matrix[j][i];
        }
    }
    return transposed_matrix;
}

int *find_largest_absolute_value_coordinates(double **matrix, int num_of_vectors) {
    int *coordinates;
    int i;
    int j;
    double max;
    max = 0;
    coordinates = (int *) malloc(2 * sizeof(int));
    for (i = 0; i < num_of_vectors; i++) {
        for (j = 0; j < num_of_vectors; j++) {
            if (i >= j) {
                continue;
            }
            if (fabs(matrix[i][j]) > max) {
                max = fabs(matrix[i][j]);
                coordinates[0] = i;
                coordinates[1] = j;
            }
        }
    }
    return coordinates;
}

int sign(double x) {
    if (x >= 0) {
        return 1;
    }
    return -1;
}


double **calculate_rotation_matrix(double **mat, int num_of_vectors, int *coordinates) {
    int **rotation_matrix;
    int i;
    int j;
    double aii;
    double ajj;
    double aij;
    double t;
    double c;
    double s;
    double theta;
    rotation_matrix = (int **) malloc(num_of_vectors * sizeof(int *));
    aii = mat[coordinates[0]][coordinates[0]];
    ajj = mat[coordinates[1]][coordinates[1]];
    aij = mat[coordinates[0]][coordinates[1]];
    theta = (ajj - aii) / (2 * aij);
    t = sign(theta) / (fabs(theta) + sqrt(1 + pow(theta, 2)));
    c = 1 / sqrt(1 + pow(t, 2));
    s = c * t;
    for (i = 0; i < num_of_vectors; i++) {
        rotation_matrix[i] = (int *) malloc(num_of_vectors * sizeof(int));
        for (j = 0; j < num_of_vectors; j++) {
            if (i == j) {
                rotation_matrix[i][j] = 1;
            } else {
                rotation_matrix[i][j] = 0;
            }
        }
    }
    rotation_matrix[coordinates[0]][coordinates[0]] = c;
    rotation_matrix[coordinates[1]][coordinates[1]] = c;
    rotation_matrix[coordinates[0]][coordinates[1]] = s;
    rotation_matrix[coordinates[1]][coordinates[0]] = -s;
}

int check_convergence(double **matrix, double **previous_matrix, int num_of_vectors, double eps) {
    int i;
    int j;
    double previous_sum;
    double sum;
    previous_sum = 0;
    sum = 0;
    for (i = 0; i < num_of_vectors; i++) {
        for (j = 0; j < num_of_vectors; j++) {
            if (i == j) {
                continue;
            }
            previous_sum += pow(previous_matrix[i][j], 2);
            sum += pow(matrix[i][j], 2);
        }
    }
    if (sum - previous_sum <= eps) {
        return 1;
    }
    return 0;
}

double **multiply_matrices(double **matrix1, double **matrix2, int num_of_vectors) {
    double **result_matrix;
    int i;
    int j;
    int k;
    result_matrix = (double **) malloc(num_of_vectors * sizeof(double *));
    for (i = 0; i < num_of_vectors; i++) {
        result_matrix[i] = (double *) malloc(num_of_vectors * sizeof(double));
    }
    for (i = 0; i < num_of_vectors; i++) {
        for (j = 0; j < num_of_vectors; j++) {
            result_matrix[i][j] = 0;
            for (k = 0; k < num_of_vectors; k++) {
                result_matrix[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }
    return result_matrix;
}

double **jacobi(double **matrix, int num_of_vectors) {
    double **jacobi_matrix;
    double **rot_mat;
    double **final_matrix;
    double **previous_matrix;
    double eps;
    int num_of_iterations;
    double s;
    double c;
    int i;
    int j;
    int k;
    int *coordinates;
    previous_matrix = matrix;
    eps = 0.00001;
    num_of_iterations = 0;
    final_matrix = (double **) malloc(num_of_vectors * sizeof(double *));
    for (k = 0; k < num_of_vectors; k++) {
        final_matrix[k] = (double *) malloc(num_of_vectors * sizeof(double));
    }
    for (k = 0; k < num_of_vectors; k++) {
        final_matrix[k][k] = 1;
    }
    while (num_of_iterations < 100) {
        num_of_iterations++;
        coordinates = find_largest_absolute_value_coordinates(matrix, num_of_vectors);
        i = coordinates[0];
        j = coordinates[1];
        rot_mat = calculate_rotation_matrix(matrix, num_of_vectors, coordinates);
        s = rot_mat[i][j];
        c = rot_mat[i][i];
        for (k = 0; k < num_of_vectors; k++) {
            matrix[i][k] = c * previous_matrix[i][k] - s * previous_matrix[j][k];
            matrix[j][k] = s * previous_matrix[i][k] + c * previous_matrix[j][k];
            matrix[k][i] = matrix[i][k];
            matrix[k][j] = matrix[j][k];
        }
        matrix[i][i] = pow(c, 2) * previous_matrix[i][i] - 2 * c * s * previous_matrix[i][j] +
                       pow(s, 2) * previous_matrix[j][j];
        matrix[j][j] = pow(s, 2) * previous_matrix[i][i] + 2 * c * s * previous_matrix[i][j] +
                       pow(c, 2) * previous_matrix[j][j];
        matrix[i][j] = (pow(c, 2) - pow(s, 2)) * previous_matrix[i][j] +
                       c * s * (previous_matrix[i][i] - previous_matrix[j][j]);
        matrix[j][i] = matrix[i][j];
        final_matrix = multiply_matrices(final_matrix, rot_mat, num_of_vectors);
        if (check_convergence(matrix, previous_matrix, num_of_vectors, eps)) {
            break;
        }
    }
    jacobi_matrix = (double **) malloc(num_of_vectors + 1 * sizeof(double *));
    for (i = 0; i < num_of_vectors + 1; i++) {
        jacobi_matrix[i] = (double *) malloc(num_of_vectors * sizeof(double));
    }
    for (i = 0; i < num_of_vectors; i++) {
        jacobi_matrix[0][i] = matrix[i][i];
    }
    for (i = 1; i < num_of_vectors + 1; i++) {
        for (j = 0; j < num_of_vectors; j++) {
            jacobi_matrix[i][j] = final_matrix[i - 1][j];
        }
    }
    return jacobi_matrix;
}


int main() {
    printf("Hello, World!");
    return 0;
}







//
//void kmeans(int num_of_clusters, int num_of_iterations, int vector_dimension, int count, double vector_list[][vector_dimension], double eps, double init_centroids[][vector_dimension]) {
//
//    double **centroids = calloc(num_of_clusters, sizeof(double *));
//    int *cluster_sizes_copy = calloc(num_of_clusters, sizeof(int));
//    double *temp_centroid = calloc(vector_dimension, sizeof(double));
//    int *cluster_sizes = calloc(num_of_clusters, sizeof(int));
//    double ***clusters = calloc(num_of_clusters, sizeof(double **));
//    double max_distance = -1;
//    int min_index1;
//    int min_index2;
//    int i;
//    int j;
//    int m;
//    int n;
//    int l;
//    int k;
//    int p;
//    int q;
//    double min_distance1;
//    double min_distance2;
//    double distance1;
//    double distance2;
//    double sum;
//
//
//    for (i = 0; i < num_of_clusters; i++) {
//        centroids[i] = calloc(vector_dimension, sizeof(double));
//    }
//    for (i = 0; i < num_of_clusters; i++) {
//        for (j = 0; j < vector_dimension; j++) {
//            centroids[i][j] = init_centroids[i][j];
//        }
//    }
//    for (i = 0; i < num_of_iterations; i++) {
//
//        for (j = 0; j < count; j++) {
//            min_distance1 = euclidean_distance(vector_list[j], centroids[0], vector_dimension);
//            min_index1 = 0;
//            for (k = 1; k < num_of_clusters; k++) {
//                distance1 = euclidean_distance(vector_list[j], centroids[k], vector_dimension);
//                if (distance1 < min_distance1) {
//                    min_distance1 = distance1;
//                    min_index1 = k;
//                }
//            }
//            cluster_sizes[min_index1] += 1;
//        }
//        for (j = 0; j < num_of_clusters; j++) {
//            cluster_sizes_copy[j] = cluster_sizes[j];
//        }
//        for (j = 0; j < num_of_clusters; j++) {
//            clusters[j] = calloc(cluster_sizes[j], sizeof(double *));
//            for (m = 0; m < cluster_sizes[j]; m++) {
//                clusters[j][m] = calloc(vector_dimension, sizeof(double));
//            }
//        }
//        for (j = 0; j < count; j++) {
//            min_distance2 = euclidean_distance(vector_list[j], centroids[0], vector_dimension);
//            min_index2 = 0;
//            for (p = 1; p < num_of_clusters; p++) {
//                distance2 = euclidean_distance(vector_list[j], centroids[p], vector_dimension);
//                if (distance2 < min_distance2) {
//                    min_distance2 = distance2;
//                    min_index2 = p;
//                }
//            }
//            for (q = 0; q < vector_dimension; q++) {
//                clusters[min_index2][cluster_sizes[min_index2] - 1][q] = vector_list[j][q];
//            }
//            cluster_sizes[min_index2] -= 1;
//        }
//        max_distance = -1;
//        for (j = 0; j < num_of_clusters; j++) {
//            for (l = 0; l < vector_dimension; l++) {
//                temp_centroid[l] = centroids[j][l];
//            }
//            for (m = 0; m < vector_dimension; m++) {
//                sum = 0;
//                for (n = 0; n < cluster_sizes_copy[j]; n++) {
//                    sum += clusters[j][n][m];
//                }
//                centroids[j][m] = sum / cluster_sizes_copy[j];
//
//            }
//            if (euclidean_distance(temp_centroid, centroids[j], vector_dimension) > max_distance) {
//                max_distance = euclidean_distance(temp_centroid, centroids[j], vector_dimension);
//            }
//        }
//        if (max_distance <= eps) {
//            break;
//        }
//        for (i = 0; i < num_of_clusters; i++) {
//            for (j = 0; j < cluster_sizes_copy[i]; j++) {
//                free(clusters[i][j]);
//            }
//            free(clusters[i]);
//        }
//    }
//    for (i = 0; i < num_of_clusters; i++) {
//        for (j = 0; j < vector_dimension; j++) {
//            if (j == vector_dimension - 1)
//                printf("%.4f\n", centroids[i][j]);
//            else
//                printf("%.4f%c", centroids[i][j], ',');
//        }
//    }
//
//    /* free memory */
//    for (i = 0; i < num_of_clusters; i++) {
//        free(centroids[i]);
//    }
//    free(centroids);
//    free(temp_centroid);
//    free(cluster_sizes);
//    for (i = 0; i < num_of_clusters; i++) {
//        for (j = 0; j < cluster_sizes_copy[i]; j++) {
//            free(clusters[i][j]);
//        }
//        free(clusters[i]);
//    }
//    free(clusters);
//    free(cluster_sizes_copy);
//
//}





