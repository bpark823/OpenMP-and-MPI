#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <omp.h>
#include "mat.h"

/* calculate the center cost and arg max */
double center_cost (mat_type* dataset, int* centers, int i, int* arg_max) {
    double cost = 0;
#pragma omp parallel default(none) shared(dataset, centers, i, cost, arg_max)
{  
    int thread_arg_max = 0; 
    double thread_cost = 0;
#pragma omp for schedule(static)    
    for (int j=0;j<dataset->rows;j++) {
	vec_type row_j, row_center;
	double min_dist_sq = DBL_MAX;
	mat_get_row(dataset,&row_j,j);
	for (int m=0;m<i;m++) {
	    mat_get_row(dataset,&row_center,centers[m]);
	    double dist_sq = vec_dist_sq(&row_j,&row_center);
	    if (dist_sq < min_dist_sq) {
		min_dist_sq = dist_sq;
	    }
	}
	if (min_dist_sq > thread_cost) {
	    thread_cost = min_dist_sq;
	    thread_arg_max = j;
	}
    }
#pragma omp critical 
{
    if(thread_cost > cost)
    {
	cost = thread_cost;
	*arg_max = thread_arg_max;
    } 

}
}
return cost;
}
/* find the cluster for the given point */
int find_cluster (mat_type* kmeans, vec_type* point) {
    int cluster;
    double min_dist_sq = DBL_MAX;
    vec_type kmean;
    for (int i=0;i<kmeans->rows;i++) {
	mat_get_row(kmeans,&kmean,i);
	double dist_sq = vec_dist_sq(&kmean,point);
	if (dist_sq < min_dist_sq) {
	    min_dist_sq = dist_sq;
	    cluster = i;
	}
    }
    return cluster;
}

/* calculate the next kmeans */
void calc_kmeans (mat_type* dataset, mat_type* kmeans, mat_type* kmeans_next) {
    int num_points[kmeans->rows];
    for (int i=0;i<kmeans->rows;i++) {
	num_points[i] = 0;
    }
    mat_zero(kmeans_next);
#pragma omp parallel default(none) shared(dataset, num_points, kmeans, kmeans_next)
{
int cluster;
vec_type point, kmean;
int num_points_thread[kmeans->rows];
for(int i = 0; i < kmeans->rows; i++) {
	num_points_thread[i] = 0;
}
mat_type kmeans_next_thread;
double thread_data[kmeans->rows*kmeans->cols];
mat_init(&kmeans_next_thread,thread_data,kmeans->rows,kmeans->cols);
mat_zero(&kmeans_next_thread);
#pragma omp for schedule(static)
    for (int i=0;i<dataset->rows;i++) {
	mat_get_row(dataset,&point,i);
	cluster = find_cluster(kmeans,&point);
	mat_get_row(&kmeans_next_thread,&kmean,cluster);
	vec_add(&kmean,&point,&kmean);
	num_points_thread[cluster] += 1;
    }
#pragma omp critical
{

    mat_add(kmeans_next, &kmeans_next_thread, kmeans_next);
    for(int i =0; i < kmeans->rows; i++) {
        num_points[i] += num_points_thread[i];

    }
    //mat_add(kmeans_next, &kmeans_next_thread, kmeans_next);
}
}
vec_type km;
    for (int i=0;i<kmeans_next->rows;i++) {
	mat_get_row(kmeans_next,&km,i);	
	if (num_points[i] > 0) {
	    vec_mult(&km,1.0/num_points[i],&km);
	} else {
	    printf ("error : cluster has no points!\n");
	    exit(1);
	}
    }

}
int main (int argc, char** argv) {

    /* get k, m, and num_threads from command line */
    if (argc < 3) {
	printf ("Command usage : %s %s %s\n",argv[0],"k","m","num_threads");
	return 1;
    }

    int k = atoi(argv[1]);
    int m = atoi(argv[2]);
    int num_threads = atoi(argv[3]);
    omp_set_num_threads(num_threads);

    mat_type dataset, kmeans, kmeans_next;

    /* start the timer */
    double start_time, end_time;
    start_time = omp_get_wtime();

#ifdef DEBUG
    /* read in the mnist test set of 10000 images */
    int rows = 10000;
    int cols = 784;
    mat_malloc (&dataset,rows,cols);
    matrix_read_bin(&dataset,"t10k-images-idx3-ubyte",16);
#else 
    /* read in the mnist training set of 60000 images */
    int rows = 60000;
    int cols = 784;
    mat_malloc (&dataset,rows,cols);
    matrix_read_bin(&dataset,"train-images-idx3-ubyte",16);
#endif

    /* allocate memory for the (k x cols) kmeans matrices */
    mat_malloc (&kmeans,k,cols);
    mat_malloc (&kmeans_next,k,cols);

    /* find k centers using the farthest first algorithm */
    int centers[k];
    double cost;
    int arg_max;
    centers[0] = 0;
    for (int i=1;i<k;i++) {
	cost = center_cost(&dataset,centers,i,&arg_max);
	centers[i] = arg_max;
    }

    /* initialize kmeans using the k centers */
    for (int i=0;i<k;i++) {
	vec_type center, kmean;
	mat_get_row (&dataset,&center,centers[i]);
	mat_get_row (&kmeans,&kmean,i);
	vec_copy (&kmean,&center);
    }

    /* update kmeans m times */
    for (int iter=0;iter<m;iter++) {
        calc_kmeans (&dataset, &kmeans, &kmeans_next);
	mat_copy (&kmeans,&kmeans_next);
    }

    /* stop the timer */
    end_time = omp_get_wtime();

#ifdef DEBUG
    /* print the wall time used as a comment */
    printf ("# wall time used = %g sec\n",end_time-start_time);
    /* print the kmeans */
    mat_print(&kmeans);
#else
    printf ("(%d,%.4f),",num_threads,(end_time-start_time));
#endif

    /* free the matrices */
    mat_free(&dataset);
    mat_free(&kmeans);
    mat_free(&kmeans_next);

    return 0;

}
