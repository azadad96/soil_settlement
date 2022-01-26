#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>

#define PI acos(-1)

struct Foundation {
	double l, b, z, tension;
};

struct Soil {
	double z, weight, E;
};

/**
 * @brief      Reads foundation's and soil's properties from file
 *
 * @param      file          file with data
 * @param[out] foundation    foundation's properties
 * @param[out] soils         soil properties
 * @param[out] number_soils  number of different soils
 */
void read_file(char *file, struct Foundation *foundation, struct Soil **soils, int *number_soils);

/**
 * @brief      Spannungsberechnung nach Steinbrenner
 *
 * @param[in]  l        foundation length
 * @param[in]  b        foundation breadth
 * @param[in]  px       x-coordinate of the point to calculate the tension at
 * @param[in]  py       y-coordinate of the point to calculate the tension at
 * @param[in]  z        depth
 * @param[in]  tension  tension at depth 0
 *
 * @return     tension at depth z
 */
double tension_steinbrenner(double l, double b, double px, double py, double z, double tension);

/**
 * @brief      calculates the foundation's settlement
 *
 * @param[in]  foundation    foundation's properties
 * @param[in]  soils         soil properties
 * @param[in]  number_soils  number of different soils
 *
 * @return     foundation's settlement
 */
double settlement(struct Foundation foundation, struct Soil *soils, int number_soils, FILE *output);

int main(int argc, char *argv[]) {
	char *input_file = NULL, *output_file = NULL;
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-i") == 0 && argc > i + 1)
			input_file = argv[i + 1];
		else if (strcmp(argv[i], "-o") == 0 && argc > i + 1)
			output_file = argv[i + 1];
	}
	if (input_file == NULL)
		input_file = "input";
	if (output_file == NULL)
		output_file = "output";

	struct Foundation foundation;
	struct Soil *soils;
	int number_soils;
	read_file(input_file, &foundation, &soils, &number_soils);

	FILE *output = fopen(output_file, "w");
	settlement(foundation, soils, number_soils, output);
	fclose(output);
}

void read_file(char *file, struct Foundation *foundation, struct Soil **soils, int *number_soils) {
	FILE *f = fopen(file, "r");
	if (f == NULL) {
		fprintf(stderr, "Couldn't open %s\n", file);
		exit(EXIT_FAILURE);
	}

	double l, b, z, tension, weight, E;
	fscanf(f, "%lf, %lf, %lf, %lf\n", &l, &b, &z, &tension);
	foundation->l = l;
	foundation->b = b;
	foundation->z = z;
	foundation->tension = tension;

	fscanf(f, "%i\n", number_soils);
	*soils = (struct Soil*) malloc(*number_soils * sizeof(struct Soil));
	for (int i = 0; i < *number_soils && fscanf(f, "%lf, %lf, %lf\n", &z, &weight, &E) != EOF; i++) {
		(*soils)[i].z = z;
		(*soils)[i].weight = weight;
		(*soils)[i].E = E;
	}

	fclose(f);
}

double tension_steinbrenner(double l, double b, double px, double py, double z, double tension) {
	double tension_p = 0;
	for (int j = 0; j < 4; j++) {
		double x, y;
		if (j < 2)
			x = px;
		else
			x = l - px;
		if (j & 2 == 0)
			y = py;
		else
			y = b - py;
		double R = sqrt(x * x + y * y + z * z);
		double i = (1 / (2 * PI))
			* (atan((x * y) / (z * R)) + ((x * y * z) / R) * (1 / (x * x + z * z) + 1 / (y * y + z * z)));
		tension_p += tension * i;
	}
	return tension_p;
}

double settlement(struct Foundation foundation, struct Soil *soils, int number_soils, FILE *output) {
	double relief = 0;
	for (int i = 1; i < number_soils; i++) {
		double a = relief;
		if (foundation.z > soils[i].z)
			relief += soils[i - 1].weight * (soils[i].z - soils[i - 1].z);
		else {
			relief += soils[i - 1].weight * (foundation.z - soils[i - 1].z);
			break;
		}
	}

	double tension = foundation.tension - relief;

	double layer_thickness = 0.1;
	double thickness = layer_thickness;
	double limit = DBL_MAX;
	double total_settlement = 0;
	for (double z = foundation.z; z < limit; z += thickness) {
		struct Soil soil;
		thickness = layer_thickness;
		for (int i = 0; i < number_soils; i++) {
			if (i + 1 < number_soils)
				if (z < soils[i + 1].z) {
					soil = soils[i];
					if (z + thickness > soils[i + 1].z)
						thickness = soils[i + 1].z - z;
					break;
				}
			else
				soil = soils[number_soils - 1];
		}
		if (thickness < 0.001)
			continue;
		
		double depth = z + thickness / 2;
		double tension_depth = tension_steinbrenner(
			foundation.l,
			foundation.b,
			foundation.l / 2,
			foundation.b / 2,
			depth - foundation.z,
			tension
		);
		double tension_ground = relief;
		for (int i = 1; i < number_soils; i++) {
			if (depth > soils[i].z)
				tension_ground += (soils[i].z - fmax(foundation.z, soils[i - 1].z)) * soils[i - 1].weight;
			else {
				tension_ground += (depth - fmax(foundation.z, soils[i - 1].z)) * soils[i - 1].weight;
				break;
			}
			if (i == number_soils - 1)
				tension_ground += (depth - soils[i].z) * soils[i].weight;
		}

		total_settlement += (tension_depth * thickness) / soil.E;

		fprintf(
			output,
			"%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n",
			z,
			z + thickness,
			thickness,
			depth,
			soil.weight,
			tension_depth,
			tension_ground,
			tension_ground * 0.2,
			soil.E,
			tension_depth / soil.E,
			(tension_depth * thickness) / soil.E
		);

		if (tension_ground / 5 >= tension_depth) {
			limit = depth;
			fprintf(
				output,
				"%lf,%lf,%lf,%lf,,%lf,%lf,%lf,,,%lf",
				foundation.z,
				z + thickness,
				(z + thickness) - foundation.z,
				depth,
				tension_depth,
				tension_ground,
				tension_ground * 0.2,
				total_settlement
			);
		}
	}

	return total_settlement;
}
