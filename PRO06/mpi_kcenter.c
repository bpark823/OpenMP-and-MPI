#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <mpi.h>
#define MAX_POINTS 2000

typedef struct vec2_s {
	double x, y;
} vec2_type;

double vec2_dist_sq (vec2_type u, vec2_type v) {
	double diff_x = u.x - v.x;
	double diff_y = u.y - v.y;
	return diff_x*diff_x + diff_y*diff_y;
}

typedef struct vec2set_s {
	vec2_type v[MAX_POINTS];
	int size;
} vec2set_type;

void vec2set_read_file (vec2set_type* set, char* filename) {
	vec2_type u;
	set->size = 0;
	FILE* file_ptr;
	file_ptr = fopen(filename,"r");
	if (file_ptr == NULL) {
		printf ("error : could not open file %s for reading\n",filename);
		exit(1);
	}
	while (fscanf (file_ptr,"%lf %lf",&(u.x),&(u.y)) == 2) {
		if (set->size < MAX_POINTS) {
			set->v[set->size] = u;
			set->size += 1;
		} else {
			printf ("Too many points in file %s\n",filename);
			fclose (file_ptr);
			exit(1);
		}
	}
	fclose (file_ptr);
}

/* calculate the cost of a given set of center locations */
double center_cost (vec2set_type* set, int* centers, int k) {
	double cost = 0;
	for (int i=0;i<set->size;i++) {
		double min_dist_sq = DBL_MAX;
		for (int j=0;j<k;j++) {
			double dist_sq = vec2_dist_sq(set->v[i],set->v[centers[j]]);
			if (dist_sq < min_dist_sq) {
				min_dist_sq = dist_sq;
			}
		}
		if (min_dist_sq > cost) {
			cost = min_dist_sq;
		}
	}
	return cost;
}

int main (int argc, char** argv) {
	MPI_Init(&argc, &argv);
	double start_time, end_time;
	start_time = MPI_Wtime();

	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	double out_msg[size];
	/* get filename, k, m, and s from command line */
	if (argc < 5) {
		printf ("Command usage : %s %s %s %s %s\n",argv[0],"filename","k","m","s");
		return 1;
	}
	vec2set_type set;
	vec2set_read_file (&set,argv[1]);
	int k = atoi(argv[2]);
	int m = atoi(argv[3]);

	/* seed the random number generator */
	srandom(atoi(argv[4]) + rank);

	/* check the cost of m random sets of k centers */
	int centers[k];
	int optimal_centers[k];
	double optimal_cost = DBL_MAX;
	double cost;
	for (int i=0;i<m;i++) {
		for (int j= 0;j<k;j++) {
			centers[j] = random() % set.size;
		}
		cost = center_cost(&set,centers,k);
		if (cost < optimal_cost) {
			optimal_cost = cost;
			for (int j=0;j<k;j++) {
				optimal_centers[j] = centers[j];
			}

		}
	}
	int all_optimal[k * size];
	double overall_cost = DBL_MAX;
	MPI_Gather(optimal_centers, k, MPI_INT, all_optimal, k, MPI_INT, 0, MPI_COMM_WORLD);  
	m *= size;
	int temp_centers[k];
	for(int i = 0; i < k; i++) {
	   temp_centers[i] = 0;
	}
	if(rank == 0) {
            for (int i=0;i< size;i++) {
		for (int j=0;j<k;j++) {
		    cost = center_cost(&set,&temp_centers[i * k], k);
			if (cost < overall_cost) {
			    overall_cost = cost;
				for (int j=0;j<k;j++) {
				    optimal_centers[j] = temp_centers[i * k + j];
				}
			}
		}
	    }
	}
	end_time = MPI_Wtime();
	if(rank == 0) {
		printf("wall time used = %.4f sec\n", (end_time - start_time));
		/* print out the number of k-tuples checked */
		printf ("number of %d-tuples checked = %d\n",k,m);

		/* print the approximate optimal cost for the k-center problem */
		printf ("approximate optimal cost = %g\n",overall_cost);

		/* print an approx optimal solution to the k-center problem */
		printf ("approx optimal centers : ");
		for (int j=0;j<k;j++) {
			printf ("%d ",optimal_centers[j]);
		}
		printf ("\n");
	} 
	MPI_Finalize();
	return 0;

}
