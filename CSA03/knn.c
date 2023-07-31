#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#define MAX_POINTS 2000

typedef struct vec2_s {
    double x, y;
} vec2_type;

vec2_type vec2_add (vec2_type u, vec2_type v) {
    vec2_type w;
    w.x = u.x + v.x;
    w.y = u.y + v.y;
    return w;
}

typedef struct dataset_s {
    vec2_type v[MAX_POINTS];
    int num_points;
} dataset_type;

void dataset_read(dataset_type* set) {
    vec2_type v[MAX_POINTS];
    vec2_type u;
    set->num_points = 0;
    while (scanf ("%lf %lf",&(u.x),&(u.y)) == 2) {
        if (set->num_points < MAX_POINTS) {
            set->v[set->num_points] = u;
	    //printf("%lf %lf\n", set->v[set->num_points]);
            set->num_points += 1;
        } else {
            printf ("Too many points in stdin\n");
            exit(1);
        }
}
}
void vec2_print (vec2_type v, char* name) {
    printf ("%s = (%lf, %lf)\n",name,v.x,v.y);
}

double vec2_dist_sq (vec2_type u, vec2_type v) {
    double diff_x = u.x - v.x;
    double diff_y = u.y - v.y;
    return diff_x*diff_x + diff_y*diff_y;
}

int main (int argc, char** argv) {
    if (argc < 4) {
        printf ("Command usage : %s %s %s %s\n",argv[0],"k", "x", "y");
        return 1;
    } 
    
    int pos = 0;
    int kpos = 0;
    int k = atoi(argv[1]);
    int n;
    int position[k];
    double dist[k];
    vec2_type arr[k];
    dataset_type set;
    vec2_type center;
    center.x = atof(argv[2]);
    center.y = atof(argv[3]);
    dataset_read(&set);
    double max_dist_sq;
    for (int i=0;i<set.num_points;i++) {
        if(n < k) {
	    arr[n] = set.v[i];
	    dist[n] = vec2_dist_sq(center, set.v[i]);
	    position[n] = i;
	    //printf("%lf %lf\n", arr[n]);
	   
	} else {
	    pos = i;
	    double distance_sq = vec2_dist_sq(center, set.v[i]);
	    max_dist_sq = DBL_MIN;
	    for(int j = 0; j< k; j++) {
		double dist_sq = vec2_dist_sq(center, arr[j]);
                if (dist_sq > max_dist_sq) {
                    max_dist_sq = dist_sq;
                    kpos = j;

                }
            }
	    if(distance_sq < vec2_dist_sq(center, arr[kpos])) {
		arr[kpos] = set.v[i];
		dist[kpos] = distance_sq;
		position[kpos] = pos;
	    }
	} 
	//printf("%d %lf\n", pos, dist[i]);
    n++;/* output the results */
   
}
for(int i = 0; i < k - 1; i++) {
    for(int j = 0; j < k - i - 1; j++) {
        if(sqrt(dist[j]) > sqrt(dist[j + 1])) {
            int temp = position[j]; 
            position[j] = position[j + 1];
            position[j + 1] = temp;
	    double tempest = dist[j];
	    dist[j] = dist[j + 1];
	    dist[j + 1] = tempest;        
        }
    }              
}

    for(int i = 0; i < k; i++) {
	printf("%d %lf\n", position[i], sqrt(dist[i]));
    }

}
