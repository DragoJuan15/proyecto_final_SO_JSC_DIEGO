#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

extern int errno;

typedef struct Stando
{
   int id;
   char usuario[100];
   char stand[100];
   char habilidad[100];
   int parte;
   struct Stando *siguiente;
} Stando_t;

void insertar_inicio(Stando_t **cabeza, int id, char usuario[], char stand[], char habilidad[], int parte);
void borrar_lista(Stando_t **lista);
void cambiar_stando(Stando_t *usuario_Cambiado, int id, char usuario[], char stand[], char habilidad[], int parte);
void borrar_especifico(Stando_t **cabeza, int id);
void leer_carta(Stando_t *usuario, int id);
void escribir_stando(Stando_t *cabeza, int id, char nombre[]);
void crear_lista(Stando_t **cabeza, char nombre[]);
int buscar_id(Stando_t *cabeza, int id);
void escribir_lista(Stando_t *cabeza, char nombre[]);
void leer_lista(Stando_t *cabeza);
void invertir_lista(Stando_t **cabeza);
int ingresar_numero();

int main()
{
   Stando_t *cabeza = NULL;
   char nombre[] = "Stando_guardados";
   int archivo_stando;
   crear_lista(&cabeza, nombre);
   archivo_stando = open(nombre, O_RDWR | O_CREAT | O_TRUNC, 0644);
   if (archivo_stando == -1)
   {
      printf("Error Number % d\n", errno);
      perror("Program");
      return 0;
   }
   close(archivo_stando);

   insertar_inicio(&cabeza,1,"Jotaro Kujo","Star Platinum","Detener el tiempo",3);
   insertar_inicio(&cabeza,2,"Noriaki Kakyoin","Hierophant Green","Emerald Splash",3);
   insertar_inicio(&cabeza,3,"Muhammad Avdol","Magican Red","Pirokineses",3);
   insertar_inicio(&cabeza,4,"Josuke Higashikata","Crazy Diamond","Reparar y restaurar",4);
   insertar_inicio(&cabeza,5,"Giorno Giovanna","Goldo Experience","Dar vida",5);

   cambiar_stando(cabeza,7,"Jotaro Kujo 4","Star Platino","Detener el tiempo 2 segundos y estar nerfeado",4);

   leer_lista(cabeza);

   printf("\n");
   borrar_especifico(&cabeza,3);

   leer_lista(cabeza);
   escribir_lista(cabeza,nombre);

   borrar_lista(&cabeza);
   printf("\n");

   return 0;
}

void insertar_inicio(Stando_t **cabeza, int id, char usuario[], char stand[], char habilidad[], int parte)
{
   Stando_t *auxiliar = NULL;
   if (buscar_id((*cabeza), id) == 1)
   {
      printf("\nIngrese otro id\n");
      return;
   }
   if ((*cabeza) == NULL)
   {
      auxiliar = (Stando_t *)malloc(sizeof(Stando_t));
      auxiliar->id = id;
      strcpy(auxiliar->usuario, usuario);
      strcpy(auxiliar->stand, stand);
      strcpy(auxiliar->habilidad, habilidad);
      auxiliar->parte = parte;
      (*cabeza) = auxiliar;
   }
   else
   {
      auxiliar = (Stando_t *)malloc(sizeof(Stando_t));
      auxiliar->id = id;
      strcpy(auxiliar->usuario, usuario);
      strcpy(auxiliar->stand, stand);
      strcpy(auxiliar->habilidad, habilidad);
      auxiliar->parte = parte;
      auxiliar->siguiente = (*cabeza);
      (*cabeza) = auxiliar;
   }
}

void borrar_lista(Stando_t **lista)
{
   Stando_t *temporal;
   while ((*lista) != NULL)
   {
      temporal = (*lista);
      (*lista) = (*lista)->siguiente;
      free(temporal);
   }
}

void cambiar_stando(Stando_t *usuario_Cambiado, int id, char usuario[], char stand[], char habilidad[], int parte)
{
   Stando_t *temporal;
   while (usuario_Cambiado->id != id)
   {
      temporal = usuario_Cambiado;
      usuario_Cambiado = usuario_Cambiado->siguiente;
      if (usuario_Cambiado == NULL)
      {
         printf("\n\nNo existe el usuario\n");
         return;
      }
   }
   usuario_Cambiado->id = id;
   strcpy(usuario_Cambiado->usuario, usuario);
   strcpy(usuario_Cambiado->stand, stand);
   strcpy(usuario_Cambiado->habilidad, habilidad);
   usuario_Cambiado->parte = parte;
}

void borrar_especifico(Stando_t **cabeza, int id)
{
   Stando_t *temporal = NULL;
   Stando_t *usuario_Cambiado = (*cabeza);
   while (usuario_Cambiado->id != id)
   {
      temporal = usuario_Cambiado;
      usuario_Cambiado = usuario_Cambiado->siguiente;
      if (usuario_Cambiado == NULL)
      {
         printf("\n\nNo existe el usuario\n");
         return;
      }
   }
   if (temporal == NULL)
   {
      printf("\n2\n");
      (*cabeza) = usuario_Cambiado->siguiente;
      free(usuario_Cambiado);
      return;
   }
   printf("\nt\n");
   temporal->siguiente = usuario_Cambiado->siguiente;
   free(usuario_Cambiado);
}

void leer_carta(Stando_t *usuario, int id)
{
   Stando_t *temporal;
   while (usuario->id != id)
   {
      temporal = usuario;
      usuario = usuario->siguiente;
      if (usuario == NULL)
      {
         printf("\n\nNo existe el usuario\n");
         return;
      }
   }
   printf("\n%d%s%s%s%d", usuario->id, usuario->usuario, usuario->stand, usuario->habilidad, usuario->parte);
}

void escribir_stando(Stando_t *cabeza, int id, char nombre[])
{
   int archivo_stando = open(nombre, O_WRONLY | O_APPEND | O_CREAT, 0644);
   if (archivo_stando == -1)
   {
      printf("Error Number % d\n", errno);
      perror("Program");
      return;
   }
   Stando_t *temporal;
   while (cabeza->id != id)
   {
      temporal = cabeza;
      cabeza = cabeza->siguiente;
      if (cabeza == NULL)
      {
         printf("\n\nNo existe el usuario\n");
         return;
      }
   }
   ssize_t comprobador;
   comprobador = write(archivo_stando, cabeza, sizeof(Stando_t));
   close(archivo_stando);

   archivo_stando = open(nombre, O_RDONLY | O_APPEND | O_CREAT);
   Stando_t *stando_leido = NULL;
   stando_leido = (Stando_t *)malloc(sizeof(Stando_t));
   comprobador = read(archivo_stando, stando_leido, sizeof(Stando_t));
   while (id != stando_leido->id)
   {
      comprobador = read(archivo_stando, stando_leido, sizeof(Stando_t));
   }
   // printf("\n..\n%d%s%s%s%d",stando_leido->id,stando_leido->usuario,stando_leido->stand,stando_leido->habilidad,stando_leido->parte);
   free(stando_leido);
   close(archivo_stando);
}

void crear_lista(Stando_t **cabeza, char nombre[])
{
   int archivo_stando = open(nombre, O_RDONLY | O_APPEND | O_CREAT, 0644);
   if (archivo_stando == -1)
   {
      printf("Error Number % d\n", errno);
      perror("Program");
      return;
   }
   ssize_t comprobador;
   Stando_t *stando_leido = NULL;
   int i = 0;
   stando_leido = (Stando_t *)malloc(sizeof(Stando_t));
   comprobador = read(archivo_stando, stando_leido, sizeof(Stando_t));
   if (comprobador <= i)
   {
      printf("No hay standos, empezemos\n\n");
      free(stando_leido);
      return;
   }
   close(archivo_stando);
   archivo_stando = open(nombre, O_RDONLY | O_APPEND | O_CREAT, 0644);
   while (stando_leido->siguiente != NULL)
   {
      comprobador = read(archivo_stando, stando_leido, sizeof(Stando_t));
      insertar_inicio(cabeza, stando_leido->id, stando_leido->usuario, stando_leido->usuario, stando_leido->habilidad, stando_leido->parte);
      // leer_carta(stando_leido,stando_leido->id);
      i++;
   }
   printf("\n\nNumero de Standos = %d\n", i);
   invertir_lista(cabeza);
   free(stando_leido);
   close(archivo_stando);
}

int buscar_id(Stando_t *cabeza, int id)
{
   Stando_t *temporal = NULL;
   while (cabeza != NULL)
   {
      temporal = cabeza;
      cabeza = cabeza->siguiente;
      if (temporal->id == id)
      {
         return 1;
      }
   }
   return 0;
}

void escribir_lista(Stando_t *cabeza, char nombre[])
{
   int archivo_stando;
   archivo_stando = open(nombre, O_RDWR | O_CREAT | O_TRUNC, 0644);
   if (archivo_stando == -1)
   {
      printf("Error Number % d\n", errno);
      perror("Program");
      return;
   }
   close(archivo_stando);
   Stando_t *temporal;
   while (cabeza != NULL)
   {
      temporal = cabeza;
      cabeza = cabeza->siguiente;
      escribir_stando(temporal, temporal->id, nombre);
   }
}

void leer_lista(Stando_t *cabeza)
{
   Stando_t *temporal = cabeza;
   while (temporal != NULL)
   {
      leer_carta(cabeza, temporal->id);
      temporal = temporal->siguiente;
   }
   printf("\n");
}

void invertir_lista(Stando_t **cabeza)
{
   Stando_t *cabeza_temporal = NULL;
   Stando_t *temporal = (*cabeza);
   while (temporal != NULL)
   {
      insertar_inicio(&cabeza_temporal, temporal->id, temporal->usuario, temporal->stand, temporal->habilidad, temporal->parte);
      temporal = temporal->siguiente;
   }
   borrar_lista(cabeza);
   (*cabeza) = cabeza_temporal;
}

int ingresar_numero()
{
   int numero;
   while (scanf("%d", &numero) != 1)
   {
      printf("Error: No es un numero valido. Intenta de nuevo: ");
      while (getchar() != '\n')
         ;
   }
   return numero;
}
