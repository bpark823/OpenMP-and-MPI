#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include "mat.h"

/* find the cluster for the given point */
int find_cluster (mat_type* kmeans, vec_type* point) {
    int cluster = 0;
    vec_type row;
 	double min_dist_sq = DBL_MAX;
    for(int i = 0; i < kmeans->rows; i++) {

	mat_get_row(kmeans, &row, i);
	double dist_sq = vec_dist_sq(&row, point);
	if(dist_sq < min_dist_sq) {
	    min_dist_sq = dist_sq;
	    cluster = i;
	}
    }
    return cluster;
}

/* calculate the next kmeans */
void calc_kmeans (mat_type* dataset, mat_type* kmeans, mat_type* kmeans_next) {
    vec_type row1, row2, rows;
    int km[kmeans->rows];
    for(int i = 0; i < kmeans->rows; i++) {
	km[i] = 0;
    }
    mat_zero(kmeans_next);
    for(int i =0; i < dataset->rows; i++) {
        mat_get_row(dataset, &row1, i);
	int closest = find_cluster(kmeans, &row1);
	mat_get_row(kmeans_next, &row2, closest);
        vec_add(&row1, &row2, &row2);	
	km[closest] += 1;
    }
    for(int i =0; i < kmeans_next->rows; i++) {
	mat_get_row(kmeans_next, &rows, i);
   	vec_mult(&rows,1.0/km[i],&rows);	 
    }

}

int main (int argc, char** argv) {

    /* get k and m from command line */
    if (argc < 3) {
	printf ("Command usage : %s %s %s\n",argv[0],"k","m");
	return 1;
    }

    int k = atoi(argv[1]);
    int m = atoi(argv[2]);

    mat_type dataset, kmeans, kmeans_next;
    int rows, cols;

    /* read the shape of the matrix from STDIN */
    if (scanf("%d %d",&rows,&cols) != 2) {
	printf ("error reading the shape of the matrix\n");
	return 1;
    }

    /* allocate memory for the (rows x cols) matrix */
    mat_malloc (&dataset,rows,cols);

    /* allocate memory for the (k x cols) kmeans matrices */
    mat_malloc (&kmeans,k,cols);
    mat_malloc (&kmeans_next,k,cols);

    /* read the dataset from STDIN */
    mat_read (&dataset);

    /* read the initial guess for kmeans from STDIN */
    mat_read (&kmeans);
    /* update kmeans m times */
    for (int iter=0;iter<m;iter++) {
        calc_kmeans (&dataset, &kmeans, &kmeans_next);
	mat_copy (&kmeans,&kmeans_next);
    }

    /* print the kmeans */
    mat_print(&kmeans);

    /* free the matrices */
    mat_free(&dataset);
    mat_free(&kmeans);
    mat_free(&kmeans_next);

    return 0;
}
