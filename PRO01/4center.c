#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#define MAX_POINTS 2000

typedef struct vec2_s {
    double x, y;
} vec2_type;

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
	    set->num_points += 1;
	    } else {
	    printf ("Too many points in stdin\n");
	    exit(1);
	    }
	}
    }
double vec2_dist_sq (vec2_type u, vec2_type v) {
    double diff_x = u.x - v.x;
    double diff_y = u.y - v.y;
    return diff_x*diff_x + diff_y*diff_y;
}

int main() {
double cost;
int c1;
int c2;
int c3;
int c4;
int n = 0;
double min_dist_sq;
double min_cost = DBL_MAX;
dataset_type set;
dataset_read(&set);
for (int i = 0; i < set.num_points  - 3; i++) {
    for (int j = i +1; j < set.num_points - 2; j++) {
        for(int k = j + 1; k < set.num_points - 1; k++) {
	   for(int m = k + 1; m < set.num_points; m++) {
	       cost = 0;
	       for(int l = 0; l < set.num_points; l++) {
	       if (vec2_dist_sq(set.v[i], set.v[l]) < vec2_dist_sq(set.v[j], set.v[l]) && vec2_dist_sq(set.v[i], set.v[l]) < vec2_dist_sq(set.v[l], set.v[k]) && vec2_dist_sq(set.v[i], set.v[l]) < vec2_dist_sq(set.v[m], set.v[l])) {
        	   min_dist_sq = vec2_dist_sq(set.v[i], set.v[l]);
    	       }
    	       else if (vec2_dist_sq(set.v[j], set.v[l]) < vec2_dist_sq(set.v[k], set.v[l]) && vec2_dist_sq(set.v[j], set.v[l]) < vec2_dist_sq(set.v[m], set.v[l])) {
    	           min_dist_sq = vec2_dist_sq(set.v[j], set.v[l]);
    	       }
    	       else if(vec2_dist_sq(set.v[l], set.v[k]) < vec2_dist_sq(set.v[m], set.v[l])){
         	   min_dist_sq = vec2_dist_sq(set.v[l], set.v[k]);
    		}
		else if(vec2_dist_sq(set.v[m], set.v[l])) {
		    min_dist_sq = vec2_dist_sq(set.v[m], set.v[l]);
		}
	    	if(min_dist_sq > cost) {
		    cost = min_dist_sq;
	        }
	    }
	  if(cost < min_cost) {
	      min_cost = cost;
              c1 = i;
	      c2 = j;		
	      c3 = k;
	      c4 = m;
	  }
	n++;  
       }
    }
}
}
printf("number of 4-tuples checked = %d\n", n);
printf("minimal cost = %lf\n", min_cost);
printf("Optimal centers: %d %d %d %d\n", c1, c2, c3, c4);

}

