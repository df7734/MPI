#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

long get_elem_at_index(long index, int rank, int p, long n) {
	long first_elem = floor(rank*(n - 2) / p) + 2;
	return first_elem + index;
}

int main(int argc, char* argv[]) {

	int p; // номер процесу
	int rank;
	int DEBUG = 0;
	unsigned long n;
	unsigned long first_elem;
	unsigned long last_elem;
	unsigned long size;
	unsigned long i;
	unsigned long k;
	unsigned long index_first_multiple;
	unsigned long next_k;
	unsigned int local_sum;
	unsigned int global_sum;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	double time = MPI_Wtime();

	if (argc == 3) DEBUG = atoi(argv[2]);
	else if (!(argc == 2)) {
		if (rank == 1) printf("Error: expected 1 or 2 arguments but got %d \n", argc - 1);
		exit(1);
	}
	n = atol(argv[1]);


	if (!(n / p > sqrt(n))) {
		if (rank == 1) printf("Error: there are too many processes! \n");
		MPI_Finalize();
		exit(1);
	}


	// Ділимо масив
	first_elem = floor(rank*(n - 2) / p) + 2;
	last_elem = floor((rank + 1)*(n - 2) / p) - 1 + 2;
	size = last_elem - first_elem + 1;


	// Заповнюємо масив для окремого процесу
	bool *array = new bool[size];
	for (i = 0; i < size; i++) {
		array[i] = true;
	}

	k = 2;

	while (k*k <= n) {
		// шукаємо перше складне число
		index_first_multiple;
		if (first_elem % k == 0) index_first_multiple = 0;
		else index_first_multiple = k - first_elem % k;



		for (i = index_first_multiple; i < size; i += k) {
			array[i] = false;
		}
		if (rank == 0) array[k - 2] = true;

		if (rank == 0) {
			next_k = k + 1;
			while (!array[next_k - 2]) next_k = next_k + 1;
			k = next_k;

		}
		// Передаємо k іншим потокам
		MPI_Bcast(&k, 1, MPI_INT, 0, MPI_COMM_WORLD);
	}


	local_sum = 0;
	global_sum;
	for (i = 0; i < size; i++) {
		if (array[i]) local_sum++;
	}
	MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	global_sum++;

	if (rank == 0) {
		time = MPI_Wtime() - time;
		printf("Primes: %d \n Total elapsed time: %f \n", global_sum, time);
	}


	delete(array);


	MPI_Finalize();

	exit(0);
}