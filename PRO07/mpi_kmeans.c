#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <mpi.h>
#include "mat.h"

typedef struct double_int_s {
    double cost;
    int arg_max;
} double_int_type;

/* calculate the center cost and arg max */
double center_cost (mat_type* dataset, int* centers, int i, int* arg_max, 
		    int rank, int size) {
    double cost = 0;
    double_int_type in_msg;
    in_msg.cost = 0;
    double_int_type out_msg[size];
    for (int j=0 + rank;j<dataset->rows;j+= size) {
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
	if (min_dist_sq > in_msg.cost) {
	    in_msg.cost = min_dist_sq;
	    in_msg.arg_max = j;
	}
    
   } 
   MPI_Allgather(&in_msg, 1,  MPI_DOUBLE_INT, out_msg, 1, MPI_DOUBLE_INT, MPI_COMM_WORLD); 
   
   for(int i = 0; i < size; i++) {	 
       if(out_msg[i].cost >  cost)
       {
           cost = out_msg[i].cost;
           *arg_max = out_msg[i].arg_max;
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
void calc_kmeans (mat_type* dataset, mat_type* kmeans, mat_type* kmeans_next,
		  int rank, int size) {
    mat_type kmeans_rank;
    mat_calloc(&kmeans_rank, kmeans->rows, kmeans->cols);
    int num_points[kmeans->rows];
    int num_points_rank[kmeans->rows];
    for (int i=0;i<kmeans->rows;i++) {
	num_points[i] = 0;
	num_points_rank[i] = 0;
    }
    vec_type point, kmean;
    int cluster;
    for (int i=0 + rank;i<dataset->rows;i+= size) {
	mat_get_row(dataset,&point,i);
	cluster = find_cluster(kmeans,&point);
	mat_get_row(&kmeans_rank,&kmean,cluster);
	vec_add(&kmean,&point,&kmean);
	num_points_rank[cluster] += 1;
    }
    MPI_Allreduce(kmeans_rank.data, kmeans_next->data, kmeans->rows * kmeans->cols, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    MPI_Allreduce(num_points_rank, num_points, kmeans_next->rows, MPI_INT, MPI_SUM, MPI_COMM_WORLD); 
    for (int i=0;i<kmeans_next->rows;i++) {
	mat_get_row(kmeans_next,&kmean,i);	
	if (num_points[i] > 0) {
	    vec_mult(&kmean,1.0/num_points[i],&kmean);
	} else {
	    printf ("error : cluster has no points!\n");
	    exit(1);
	}
    }
    mat_free(&kmeans_rank);
}

int main (int argc, char** argv) {

    MPI_Init (&argc, &argv);

    /* start the timer */
    double start_time, end_time;
    start_time = MPI_Wtime();

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* get k and m from command line */
    if (argc < 3) {
	printf ("Command usage : %s %s\n",argv[0],"k","m");
	return 1;
    }

    int k = atoi(argv[1]);
    int m = atoi(argv[2]);

    mat_type dataset, kmeans, kmeans_next;

    /* read in the mnist training set of 60000 images */
    int rows = 60000;
    int cols = 784;
    mat_malloc (&dataset,rows,cols);
    matrix_read_bin(&dataset,"train-images-idx3-ubyte",16);

    /* allocate memory for the (k x cols) kmeans matrices */
    mat_malloc (&kmeans,k,cols);
    mat_malloc (&kmeans_next,k,cols);

    /* find k centers using the farthest first algorithm */
    int centers[k];
    double cost;
    int arg_max;
    centers[0] = 0;
    for (int i=1;i<k;i++) {
	cost = center_cost(&dataset,centers,i,&arg_max,rank,size);
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
        calc_kmeans (&dataset, &kmeans, &kmeans_next, rank, size);
	mat_copy (&kmeans,&kmeans_next);
    }

    /* stop the timer */
    end_time = MPI_Wtime();

    if (rank == 0) {
#ifdef DEBUG
	/* print the wall time used as a comment */
	printf ("# wall time used = %g sec\n",end_time-start_time);
	/* print the kmeans */
	mat_print(&kmeans);
#else
	printf ("(%d,%.4f),",size,(end_time-start_time));
#endif
    }

    /* free the matrices */
    mat_free(&dataset);
    mat_free(&kmeans);
    mat_free(&kmeans_next);

    MPI_Finalize();

    return 0;

}
