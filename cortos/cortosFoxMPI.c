#include <stdio.h>
#include "mpi.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

typedef struct {
  int       p;         /* Número total de procesos     */
  MPI_Comm  comm;      /* Comunicador para la malla    */
  MPI_Comm  row_comm;  /* Comunicador para mi fila     */
  MPI_Comm  col_comm;  /* Comunicador para i columna   */
  int       q;         /* Orden de la malla            */
  int       my_row;    /* Mi número de fila            */
  int       my_col;    /* Mi número de columna         */
  int       my_rank;   /* Mi id en el comunicador para la malla */
} GRID_INFO_T;


#define MAX 5000*5000
typedef struct {
  int     n_bar;			/* Orden de la matriz local 	*/
#define Order(A) ((A)->n_bar)
  short  entries[MAX];	/* Elementos de la matriz local	*/
#define Entry(A,i,j) (*(((A)->entries) + ((A)->n_bar)*(i) + (j)))
} LOCAL_MATRIX_T;

/* Declaraciones de Funciones */
LOCAL_MATRIX_T*  Local_matrix_allocate(int n_bar);
void             Free_local_matrix(LOCAL_MATRIX_T** local_A);
void             Read_matrix(char* prompt, LOCAL_MATRIX_T* local_A, GRID_INFO_T* grid, int n);
void             Print_matrix(char* title, LOCAL_MATRIX_T* local_A, GRID_INFO_T* grid, int n);
void             Set_to_zero(LOCAL_MATRIX_T* local_A);
void             Local_matrix_multiply(LOCAL_MATRIX_T* local_A, LOCAL_MATRIX_T* local_B, LOCAL_MATRIX_T* local_C);
void             Build_matrix_type(LOCAL_MATRIX_T* local_A);
MPI_Datatype     local_matrix_mpi_t;
LOCAL_MATRIX_T*  temp_mat;
void             Print_local_matrices(char* title, LOCAL_MATRIX_T* local_A, GRID_INFO_T* grid);
long obtener_memoria();
/*********************************************************/
int main(int argc, char* argv[]) {
  srand(time(NULL));
  int              p;
  int              my_rank;
  GRID_INFO_T      grid;
  LOCAL_MATRIX_T*  local_A;
  LOCAL_MATRIX_T*  local_B;
  LOCAL_MATRIX_T*  local_C;
  int              n;
  int              n_bar;
  double			 start, end;

  void Setup_grid(GRID_INFO_T*  grid);
  void Fox(int n, GRID_INFO_T* grid, LOCAL_MATRIX_T* local_A, LOCAL_MATRIX_T* local_B, LOCAL_MATRIX_T* local_C);

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  Setup_grid(&grid);			/* Creación de la malla de procesadores */

  if (argc == 2) {
    sscanf(argv[1], "%d", &n);
  }
  else if (my_rank == 0){
    printf("\n¿ Cuál es el orden de las matrices ?\n");
    scanf("%d", &n);
  }

  MPI_Bcast(&n, 1, MPI_SHORT, 0, MPI_COMM_WORLD);
  n_bar = n/grid.q;

  local_A = Local_matrix_allocate(n_bar);
  Order(local_A) = n_bar;
  Read_matrix("", local_A, &grid, n);

  local_B = Local_matrix_allocate(n_bar);
  Order(local_B) = n_bar;
  Read_matrix("", local_B, &grid, n);

  Build_matrix_type(local_A);
  temp_mat = Local_matrix_allocate(n_bar);

  local_C = Local_matrix_allocate(n_bar);
  Order(local_C) = n_bar;
  start = MPI_Wtime();
  Fox(n, &grid, local_A, local_B, local_C);
  double memoria = obtener_memoria();

  end = MPI_Wtime();

  double tiempo;
  tiempo = end - start;
  printf("%lf\n",tiempo);
  printf("%lf\n",memoria);

  FILE *file1;
  //file1 = fopen("datosTiempo.dat","ab+");
  file1 = fopen("datosTiempo.txt","a+");
  if(file1 != NULL){
    //fwrite(&tiempo,sizeof(double), 1, file1);
    fprintf(file1,"%lf\n",tiempo);
  }
  fflush(file1);
  fclose(file1);

  FILE *file2;
  //file2 = fopen("datosMemoria.dat","ab+");
  file2 = fopen("datosMemoria.txt","a+");
  if(file2 != NULL){
    //fwrite(&memoria,sizeof(double), 1, file2);
    fprintf(file2,"%lf\n",memoria);
  }
  fflush(file2);
  fclose(file2);

  Free_local_matrix(&local_A);
  Free_local_matrix(&local_B);
  Free_local_matrix(&local_C);

  MPI_Finalize();
}  /* main */

long obtener_memoria() {
  struct rusage myusage;
  getrusage(RUSAGE_SELF, &myusage);
  return myusage.ru_maxrss;
}

/*********************************************************/
void Setup_grid(GRID_INFO_T*  grid) {
  int old_rank;
  int dimensions[2];
  int wrap_around[2];
  int coordinates[2];
  int free_coords[2];

  /* Establecer la información de la malla global */
  MPI_Comm_size(MPI_COMM_WORLD, &(grid->p));
  MPI_Comm_rank(MPI_COMM_WORLD, &old_rank);

  /* Asumimos que p es un cuadrado perfecto */
  grid->q = (int) sqrt((double) grid->p);
  dimensions[0] = dimensions[1] = grid->q;

  /* Queremos un desplazamiento circular en la segunda dimensión */
  /* No nos preocupa la primera dimensión                        */
  wrap_around[0] = wrap_around[1] = 1;
  MPI_Cart_create(MPI_COMM_WORLD, 2, dimensions, wrap_around, 1, &(grid->comm));
  MPI_Comm_rank(grid->comm, &(grid->my_rank));
  MPI_Cart_coords(grid->comm, grid->my_rank, 2, coordinates);
  grid->my_row = coordinates[0];
  grid->my_col = coordinates[1];

  /* Establecer los comunicadores de filas */
  free_coords[0] = 0;
  free_coords[1] = 1;
  MPI_Cart_sub(grid->comm, free_coords, &(grid->row_comm));

  /* Establecer los comunicadores de columnas */
  free_coords[0] = 1;
  free_coords[1] = 0;
  MPI_Cart_sub(grid->comm, free_coords, &(grid->col_comm));
} /* Setup_grid */


/*********************************************************/
void Fox(
    int              n         ,
    GRID_INFO_T*     grid      ,
    LOCAL_MATRIX_T*  local_A   ,
    LOCAL_MATRIX_T*  local_B   ,
    LOCAL_MATRIX_T*  local_C    ) {

  LOCAL_MATRIX_T*  temp_A;
  int              stage;
  int              bcast_root;
  int              n_bar;  /* Orden de la matrix local : n/sqrt(p) */
  int              source;
  int              dest;
  MPI_Status       status;

  n_bar = n/grid->q;
  Set_to_zero(local_C);

  /* Calcula las direccciones para el desplazamiento circular de B */
  source = (grid->my_row + 1) % grid->q;
  dest = (grid->my_row + grid->q - 1) % grid->q;

  /* Aloca el espacio para el envio del bloque de A */
  temp_A = Local_matrix_allocate(n_bar);

  for (stage = 0; stage < grid->q; stage++) {
    bcast_root = (grid->my_row + stage) % grid->q;
    if (bcast_root == grid->my_col) {
      MPI_Bcast(local_A, 1, local_matrix_mpi_t, bcast_root, grid->row_comm);
      Local_matrix_multiply(local_A, local_B, local_C);
    } else {
      MPI_Bcast(temp_A, 1, local_matrix_mpi_t, bcast_root, grid->row_comm);
      Local_matrix_multiply(temp_A, local_B, local_C);
    }
    MPI_Sendrecv_replace(local_B, 1, local_matrix_mpi_t, dest, 0, source, 0, grid->col_comm, &status);
  } /* for */

} /* Fox */


/*********************************************************/
/* Aloca el espacio dinámicamente para el tipo de datos LOCAL_MATRIX_T */
LOCAL_MATRIX_T* Local_matrix_allocate(int local_order) {
  LOCAL_MATRIX_T* temp;

  temp = (LOCAL_MATRIX_T*) malloc(sizeof(LOCAL_MATRIX_T));

  return temp;
}  /* Local_matrix_allocate */


/*********************************************************/
/* Libera el espacio de memoria de unaa bloque de matriz */
void Free_local_matrix(
    LOCAL_MATRIX_T** local_A_ptr ) {
  free(*local_A_ptr);
}  /* Free_local_matrix */


/*********************************************************/
/* Lee y distribuye la matriz para cada fila global de la matriz, luego para cada ccolumna de la malla lee un bloque de orden n_bar en el procesador 0 y luego enviarlos a los procesos adecuados */
void Read_matrix(
    char*            prompt  ,
    LOCAL_MATRIX_T*  local_A ,
    GRID_INFO_T*     grid    ,
    int              n        ) {

  int        mat_row, mat_col;
  int        grid_row, grid_col;
  int        dest;
  int        coords[2];
  short*     temp;
  MPI_Status status;

  /* El procesador con id = 0 reparte los elementos entre los procesadores*/
  if (grid->my_rank == 0) {
    temp = (short*) malloc(Order(local_A)*sizeof(short));
    //printf("%s\n", prompt);
    fflush(stdout);
    for (mat_row = 0;  mat_row < n; mat_row++) {
      grid_row = mat_row/Order(local_A);
      coords[0] = grid_row;
      for (grid_col = 0; grid_col < grid->q; grid_col++) {
        coords[1] = grid_col;
        MPI_Cart_rank(grid->comm, coords, &dest);
        if (dest == 0) {
          for (mat_col = 0; mat_col < Order(local_A); mat_col++) {
            *((local_A->entries)+mat_row*Order(local_A)+mat_col) = rand() % 1001 +1000;

          }
        } else {
          for (mat_col = 0; mat_col < Order(local_A); mat_col++) {
            *(temp + mat_col) = rand() % 1001 +1000;

          }
          MPI_Send(temp, Order(local_A), MPI_SHORT, dest, 0, grid->comm);
        }
      }
    }
    free(temp);
  } else {
    for (mat_row = 0; mat_row < Order(local_A); mat_row++)
      MPI_Recv(&Entry(local_A, mat_row, 0), Order(local_A), MPI_SHORT, 0, 0, grid->comm, &status);
  }

}  /* Read_matrix */


/*********************************************************/
/* El procesador 0 se encarga de imprimir la matriz, imprimiendo su propio bloque y recibiendo los bloques de los demás procesadores para imprimirlos */
void Print_matrix(
    char*            title    ,
    LOCAL_MATRIX_T*  local_A  ,
    GRID_INFO_T*     grid     ,
    int              n         ) {
  int        mat_row, mat_col;
  int        grid_row, grid_col;
  int        source;
  int        coords[2];
  short*     temp;
  MPI_Status status;

  if (grid->my_rank == 0) {
    temp = (short*) malloc(Order(local_A)*sizeof(short));
    //printf("%s\n", title);
    for (mat_row = 0;  mat_row < n; mat_row++) {
      grid_row = mat_row/Order(local_A);
      coords[0] = grid_row;
      for (grid_col = 0; grid_col < grid->q; grid_col++) {
        coords[1] = grid_col;
        MPI_Cart_rank(grid->comm, coords, &source);
        if (source == 0) {
          for(mat_col = 0; mat_col < Order(local_A); mat_col++)
            printf("%d ", Entry(local_A, mat_row, mat_col));
        } else {
          MPI_Recv(temp, Order(local_A), MPI_SHORT, source, 0, grid->comm, &status);
          for(mat_col = 0; mat_col < Order(local_A); mat_col++)
            printf("%d ", temp[mat_col]);
        }
      }
      //printf("\n");
    }
    free(temp);
  } else {
    for (mat_row = 0; mat_row < Order(local_A); mat_row++)
      MPI_Send(&Entry(local_A, mat_row, 0), Order(local_A), MPI_SHORT, 0, 0, grid->comm);
  }

}  /* Print_matrix */


/*********************************************************/
/* Inicializa a 0.0 un bloque de matriz de tipo LOCAL_MATRIX_T */
void Set_to_zero(
    LOCAL_MATRIX_T*  local_A ) {

  int i, j;

  for (i = 0; i < Order(local_A); i++)
    for (j = 0; j < Order(local_A); j++)
      Entry(local_A,i,j) = 0.0;

}  /* Set_to_zero */


/*********************************************************/
/* Construye el tipo de datos derivado local_matrix_mpi_t, el cual representa a los bloques de matrices LOCAL_MATRIX_T */
void Build_matrix_type(
    LOCAL_MATRIX_T*  local_A ) {
  MPI_Datatype  temp_mpi_t;
  int           block_lengths[2];
  MPI_Aint      displacements[2];
  MPI_Datatype  typelist[2];
  MPI_Aint      start_address;
  MPI_Aint      address;

  MPI_Type_contiguous(Order(local_A)*Order(local_A), MPI_SHORT, &temp_mpi_t);

  block_lengths[0] = block_lengths[1] = 1;

  typelist[0] = MPI_INT;
  typelist[1] = temp_mpi_t;

  MPI_Get_address(local_A, &start_address);
  MPI_Get_address(&(local_A->n_bar), &address);
  displacements[0] = address - start_address;

  MPI_Get_address(local_A->entries, &address);
  displacements[1] = address - start_address;

  MPI_Type_create_struct(2, block_lengths, displacements, typelist, &local_matrix_mpi_t);
  MPI_Type_commit(&local_matrix_mpi_t);
}  /* Build_matrix_type */


/*********************************************************/
/* Multiplica las matrices locales local_A y local_B de cada procesador y luego lo almacena en local_C  */
void Local_matrix_multiply(
    LOCAL_MATRIX_T*  local_A  ,
    LOCAL_MATRIX_T*  local_B  ,
    LOCAL_MATRIX_T*  local_C   ) {
  int i, j, k;

  for (i = 0; i < Order(local_A); i++)
    for (j = 0; j < Order(local_A); j++)
      for (k = 0; k < Order(local_B); k++)
        Entry(local_C,i,j) = Entry(local_C,i,j) + Entry(local_A,i,k)*Entry(local_B,k,j);

}  /* Local_matrix_multiply */

