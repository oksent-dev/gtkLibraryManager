#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
// gcc `pkg-config --cflags gtk+-3.0` -o a main.c `pkg-config --libs gtk+-3.0`
typedef struct ksiazka {
  char *tytul;
  char *autor;
  double cena;
  int ilosc;
  struct ksiazka *nastepny;
} ksiazka;

ksiazka *poczatek = NULL;
GtkWidget *tree_view;
void replace_dot_with_comma(char *str) {
    int i;
    for (i = 0; i < strlen(str); i++) {
        if (str[i] == '.') {
            str[i] = ',';
        }
    }
}
void replace_comma_with_dot(char *str) {
    int i;
    for (i = 0; i < strlen(str); i++) {
        if (str[i] == ',') {
            str[i] = '.';
        }
    }
}
int ilosc_ksiazek() {
  int count = 0;
  ksiazka *current = poczatek;
  if (current == NULL) {
  return count;
  }
  do {
    count++;
    current = current->nastepny;
  } while (current != poczatek);
  return count;
}
void dodajKsiazke(char *tytul, char *autor, double cena, int ilosc) {
  ksiazka *nowa = malloc(sizeof(ksiazka));
  nowa->tytul = strdup(tytul);
  nowa->autor = strdup(autor);
  nowa->cena = cena;
  nowa->ilosc = ilosc;
  nowa->nastepny = NULL;

  if (poczatek == NULL) {
    poczatek = nowa;
    poczatek->nastepny = poczatek;  
  } else {
    ksiazka *obecny = poczatek;
    while (obecny->nastepny != poczatek) {  
      obecny = obecny->nastepny;
    }
    obecny->nastepny = nowa;  
    nowa->nastepny = poczatek;  
  }
}
void usunKsiazke(int position) {
  if (poczatek == NULL)  {
    return; 
  }
  else if (position == 1) 
  {
    ksiazka *toDelete = poczatek;
    ksiazka *obecny = poczatek;
    while (obecny->nastepny != poczatek) {
      obecny = obecny->nastepny;
    }
    obecny->nastepny = poczatek->nastepny;
    poczatek = poczatek->nastepny;
    free(toDelete->tytul);
    free(toDelete->autor);
    free(toDelete);
  } else {
    ksiazka *obecny = poczatek;
    int i;
    for (i = 1; i < position - 1; i++) {
      obecny = obecny->nastepny;
    }
    ksiazka *toDelete = obecny->nastepny;
    obecny->nastepny = toDelete->nastepny;
    free(toDelete->tytul);
    free(toDelete->autor);
    free(toDelete);
  }
}
void zmodyfikujKsiazke(int pozycja, char *tytul, char *autor, double cena, int ilosc) {
  if (poczatek == NULL) {
    printf("Lista jest pusta, brak książek do zmodyfikowania\n");
    return;
  }
  
  ksiazka *obecny = poczatek;
  for (int i = 0; i < pozycja; i++) {
    if (obecny->nastepny == poczatek) {
      printf("Nie ma książki na pozycji %d\n", pozycja);
      return;
    }
    obecny = obecny->nastepny;
  }
  
  free(obecny->tytul);
  free(obecny->autor);
  obecny->tytul = strdup(tytul);
  obecny->autor = strdup(autor);
  obecny->cena = cena;
  obecny->ilosc = ilosc;
  
}
void usunWszystkieKsiazki() {
  if (poczatek == NULL) {
  return; 
  } else {
  ksiazka *obecny = poczatek;
  ksiazka *temp;
  do {
  temp = obecny->nastepny;
  free(obecny->tytul);
  free(obecny->autor);
  free(obecny);
  obecny = temp;
  } while (obecny != poczatek);
  poczatek = NULL;
  }
}
void insert_data_into_tree_view() {
  GtkListStore *list_store = gtk_list_store_new(5, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT);

  ksiazka *obecny = poczatek;
  int position = 1;
  do {
    char formatted_price[10];
    sprintf(formatted_price, "%.2f", obecny->cena);
    gtk_list_store_insert_with_values(list_store, NULL, -1,
                                      0, position,
                                      1, obecny->tytul,
                                      2, obecny->autor,
                                      3, formatted_price,
                                      4, obecny->ilosc,
                                      -1);
    obecny = obecny->nastepny;
    position++;
  } while (obecny != poczatek);
  gtk_tree_view_set_model(GTK_TREE_VIEW(tree_view), GTK_TREE_MODEL(list_store));
}
void delete_data_from_tree_view() {
    GtkListStore *list_store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(tree_view)));
    gtk_list_store_clear(list_store);
}
void on_modify(GtkWidget *button, gpointer data) {
  GtkWidget *grid = GTK_WIDGET(data);

  GtkWidget *position_input = gtk_grid_get_child_at(GTK_GRID(grid), 1, 0);
  GtkWidget *title_input = gtk_grid_get_child_at(GTK_GRID(grid), 1, 1);
  GtkWidget *author_input = gtk_grid_get_child_at(GTK_GRID(grid), 1, 2);
  GtkWidget *price_input = gtk_grid_get_child_at(GTK_GRID(grid), 1, 3);
  GtkWidget *quantity_input = gtk_grid_get_child_at(GTK_GRID(grid), 1, 4);

  const gchar *position = gtk_entry_get_text(GTK_ENTRY(position_input));
  const gchar *title = gtk_entry_get_text(GTK_ENTRY(title_input));
  const gchar *author = gtk_entry_get_text(GTK_ENTRY(author_input));
  const gchar *price = gtk_entry_get_text(GTK_ENTRY(price_input));
  const gchar *quantity = gtk_entry_get_text(GTK_ENTRY(quantity_input));

  
  if (title != NULL && author != NULL && price != NULL && quantity != NULL) {
    if (strcmp(title, "") != 0 && strcmp(author, "") != 0 && strcmp(price, "") != 0 && strcmp(quantity, "") != 0) {
        double double_price = g_ascii_strtod(price, NULL);
        int int_quantity = atoi(quantity);
        int int_position = atoi(position);

        zmodyfikujKsiazke(int_position-1, (char*)title, (char*)author, double_price, int_quantity);
        insert_data_into_tree_view();
        gtk_widget_destroy(gtk_widget_get_toplevel(button));
    }
}
  
  
}

void on_row_activated(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer data) {
  GtkTreeModel *model = gtk_tree_view_get_model(tree_view);
  GtkTreeIter iter;
  gtk_tree_model_get_iter(model, &iter, path);

  int position;
  gchar *title, *author, *price;
  gint quantity;

  gtk_tree_model_get(model, &iter, 0, &position, 1, &title, 2, &author, 3, &price, 4, &quantity, -1);
  replace_comma_with_dot(price);
  GtkWidget *add_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(add_window), "modify book");
  gtk_window_set_default_size(GTK_WINDOW(add_window), 300, 200);

  GtkWidget *grid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(grid), 12);
  gtk_grid_set_column_spacing(GTK_GRID(grid), 12);

  GtkWidget *title_label = gtk_label_new("Title:");
  GtkWidget *title_input = gtk_entry_new();
  GtkWidget *author_label = gtk_label_new("Author:");
  GtkWidget *author_input = gtk_entry_new();
  GtkWidget *price_label = gtk_label_new("Price:");
  GtkWidget *price_input = gtk_entry_new();
  GtkWidget *quantity_label = gtk_label_new("Quantity:");
  GtkWidget *quantity_input = gtk_entry_new();
  GtkWidget *position_label = gtk_label_new("Position:");
  GtkWidget *position_input = gtk_entry_new();

  gtk_entry_set_text(GTK_ENTRY(title_input), (char*) title);
  gtk_entry_set_text(GTK_ENTRY(author_input), (char*) author);
  gtk_entry_set_text(GTK_ENTRY(price_input), (char*) price);
  char *quantity_str = g_strdup_printf("%d", quantity);
  gtk_entry_set_text(GTK_ENTRY(quantity_input), quantity_str);
  g_free(quantity_str);
  char *position_str = g_strdup_printf("%d", position);
  gtk_entry_set_text(GTK_ENTRY(position_input), position_str);
  g_free(position_str);
  gtk_grid_attach(GTK_GRID(grid), position_label, 0, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), position_input, 1, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), title_label, 0, 1, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), title_input, 1, 1, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), author_label, 0, 2, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), author_input, 1, 2, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), price_label, 0, 3, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), price_input, 1, 3, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), quantity_label, 0, 4, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), quantity_input, 1, 4, 1, 1);

  gtk_widget_set_sensitive(position_input,FALSE);
  GtkWidget *confirm_button = gtk_button_new_with_label("Confirm");
  g_signal_connect(confirm_button, "clicked", G_CALLBACK(on_modify), grid);


  gtk_grid_attach(GTK_GRID(grid), confirm_button, 0, 5, 2, 1);

  gtk_container_add(GTK_CONTAINER(add_window), grid);

  gtk_widget_show_all(add_window);
}

void read_from_file(char *file_name)
{
  FILE *file = fopen(file_name, "r");
  if (file == NULL) {
    printf("Error opening file!\n");
    return;
  }

  char line[256];
  while (fgets(line, sizeof(line), file)) {
    char *tytul, *autor, *ilosc, *cena;
    char buffer[256];
    strcpy(buffer, line);

    tytul = strtok(buffer, ";");
    autor = strtok(NULL, ";");
    ilosc = strtok(NULL, ";");
    cena = strtok(NULL, ";");
    replace_dot_with_comma(cena);

    int int_ilosc = atoi(ilosc);
    double double_cena = atof(cena);

    dodajKsiazke(tytul, autor, double_cena, int_ilosc);
  }
  
  fclose(file);
}
void write_to_file(char *file_name) 
{
  FILE *file = fopen(file_name, "w");
  if (file == NULL) 
  {
    printf("Error opening file!\n");
    return;
  }
  ksiazka *obecny = poczatek;
  do {
    char *formatted_price = g_strdup_printf("%.2f", obecny->cena);
    fprintf(file, "%s;%s;%d;%s;\n", obecny->tytul, obecny->autor, obecny->ilosc, formatted_price);
    g_free(formatted_price);
    obecny = obecny->nastepny;
  } while (obecny != poczatek);
  fclose(file);
}
void on_read_confirm_button_clicked(GtkWidget *widget, gpointer data)
{
    GtkWidget *path_input = GTK_WIDGET(data);
    if (path_input != NULL)
    {
        const gchar *path = gtk_entry_get_text(GTK_ENTRY(path_input));
        if (access((char*)path, F_OK) == -1) {
            printf("File does not exist\n");
        } else {
            usunWszystkieKsiazki();
            delete_data_from_tree_view();
            read_from_file((char*)path);
            insert_data_into_tree_view();
            gtk_widget_destroy(gtk_widget_get_toplevel(widget));
      }
    }
    else
    {
        printf("path_input is NULL\n");
    }
}
void on_read_button_clicked(GtkWidget *widget, gpointer data) 
{

  GtkWidget *new_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(new_window), "Read: file path");
  gtk_window_set_default_size(GTK_WINDOW(new_window), 300, 150);

  GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_container_add(GTK_CONTAINER(new_window), vbox);

  GtkWidget *path_input = gtk_entry_new();
  gtk_box_pack_start(GTK_BOX(vbox), path_input, TRUE, TRUE, 0);

  GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);


  GtkWidget *confirm_button = gtk_button_new_with_label("Confirm");
  
  g_signal_connect(confirm_button, "clicked", G_CALLBACK(on_read_confirm_button_clicked), path_input);
  gtk_box_pack_start(GTK_BOX(hbox), confirm_button, TRUE, TRUE, 0);

  gtk_widget_show_all(new_window);
}

void on_write_confirm_button_clicked(GtkWidget *widget, gpointer data)
{
    GtkWidget *path_input = GTK_WIDGET(data);
    if (path_input != NULL)
    {
        const gchar *path = gtk_entry_get_text(GTK_ENTRY(path_input));
        write_to_file((char*)path);
        gtk_widget_destroy(gtk_widget_get_toplevel(widget));
    }
    else
    {
        printf("path_input is NULL\n");
    }
}
void on_add_confirm_button_clicked(GtkWidget *widget, gpointer data) 
{

  GtkWidget *title_input = GTK_WIDGET(gtk_grid_get_child_at(GTK_GRID(data), 1, 0));
  const gchar *title = gtk_entry_get_text(GTK_ENTRY(title_input));

  GtkWidget *author_input = GTK_WIDGET(gtk_grid_get_child_at(GTK_GRID(data), 1, 1));
  const gchar *author = gtk_entry_get_text(GTK_ENTRY(author_input));

  GtkWidget *price_input = GTK_WIDGET(gtk_grid_get_child_at(GTK_GRID(data), 1, 2));
  const gchar *price = gtk_entry_get_text(GTK_ENTRY(price_input));

  GtkWidget *quantity_input = GTK_WIDGET(gtk_grid_get_child_at(GTK_GRID(data), 1, 3));
  const gchar *quantity = gtk_entry_get_text(GTK_ENTRY(quantity_input));

  if (title != NULL && author != NULL && price != NULL && quantity != NULL) {
      if (strcmp(title, "") != 0 && strcmp(author, "") != 0 && strcmp(price, "") != 0 && strcmp(quantity, "") != 0) {
          double double_price = g_ascii_strtod(price, NULL);
          int int_quantity = atoi(quantity);

          dodajKsiazke((char*)title, (char*)author, double_price, int_quantity);
          delete_data_from_tree_view();
          insert_data_into_tree_view();
          gtk_widget_destroy(gtk_widget_get_toplevel(widget));
      }
  }
}
void on_add_button_clicked(GtkWidget *widget, gpointer data) {
  GtkWidget *add_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(add_window), "Add Book");
  gtk_window_set_default_size(GTK_WINDOW(add_window), 300, 200);

  GtkWidget *grid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
  gtk_grid_set_column_spacing(GTK_GRID(grid), 10);

  GtkWidget *title_label = gtk_label_new("Title:");
  GtkWidget *title_input = gtk_entry_new();
  GtkWidget *author_label = gtk_label_new("Author:");
  GtkWidget *author_input = gtk_entry_new();
  GtkWidget *price_label = gtk_label_new("Price:");
  GtkWidget *price_input = gtk_entry_new();
  GtkWidget *quantity_label = gtk_label_new("Quantity:");
  GtkWidget *quantity_input = gtk_entry_new();

  gtk_grid_attach(GTK_GRID(grid), title_label, 0, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), title_input, 1, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), author_label, 0, 1, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), author_input, 1, 1, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), price_label, 0, 2, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), price_input, 1, 2, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), quantity_label, 0, 3, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), quantity_input, 1, 3, 1, 1);

  GtkWidget *confirm_button = gtk_button_new_with_label("Confirm");
  g_signal_connect(confirm_button, "clicked", G_CALLBACK(on_add_confirm_button_clicked), grid);


  gtk_grid_attach(GTK_GRID(grid), confirm_button, 0, 4, 2, 1);

  gtk_container_add(GTK_CONTAINER(add_window), grid);

  gtk_widget_show_all(add_window);
}
void on_write_button_clicked(GtkWidget *widget, gpointer data) 
{

  GtkWidget *new_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(new_window), "Write: file path");
  gtk_window_set_default_size(GTK_WINDOW(new_window), 300, 150);

  GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_container_add(GTK_CONTAINER(new_window), vbox);

  GtkWidget *path_input = gtk_entry_new();
  gtk_box_pack_start(GTK_BOX(vbox), path_input, TRUE, TRUE, 0);

  GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

  GtkWidget *confirm_button = gtk_button_new_with_label("Confirm");
  
  g_signal_connect(confirm_button, "clicked", G_CALLBACK(on_write_confirm_button_clicked), path_input);
  gtk_box_pack_start(GTK_BOX(hbox), confirm_button, TRUE, TRUE, 0);

  gtk_widget_show_all(new_window);
}
void on_delete_confirm_button_clicked(GtkWidget *button, gpointer data) {
  GtkWidget *position_input = GTK_WIDGET(data);
  const gchar *position= gtk_entry_get_text(GTK_ENTRY(position_input));
  if (position != NULL) {
    if (strcmp(position, "") != 0) {
        int int_position = atoi(position);
        if (int_position <= ilosc_ksiazek() && int_position > 0 && ilosc_ksiazek() != 1)
        {
          usunKsiazke(int_position);
          delete_data_from_tree_view();
          insert_data_into_tree_view();
          GtkWidget *delete_window = gtk_widget_get_toplevel(button);
          gtk_widget_destroy(delete_window);
        }
      }
    }
} 
void on_delete_button_clicked(GtkWidget *button, gpointer data) {
  GtkWidget *delete_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(delete_window), "Delete Book");
  gtk_window_set_default_size(GTK_WINDOW(delete_window), 300, 100);

  GtkWidget *grid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(grid), 12);
  gtk_grid_set_column_spacing(GTK_GRID(grid), 12);

  GtkWidget *position_label = gtk_label_new("Position:");
  GtkWidget *position_input = gtk_entry_new();

  gtk_grid_attach(GTK_GRID(grid), position_label, 0, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), position_input, 1, 0, 1, 1);

  GtkWidget *confirm_button = gtk_button_new_with_label("Confirm");
  g_signal_connect(confirm_button, "clicked", G_CALLBACK(on_delete_confirm_button_clicked), position_input);

  gtk_grid_attach(GTK_GRID(grid), confirm_button, 0, 1, 2, 1);

  gtk_container_add(GTK_CONTAINER(delete_window), grid);

  gtk_widget_show_all(delete_window);
}
void on_search_confirm_button_clicked(GtkButton *button, gpointer data) {
  GtkGrid *grid = GTK_GRID(data);
  GtkWidget *position_input = gtk_grid_get_child_at(grid, 1, 0);
  GtkWidget *title_input = gtk_grid_get_child_at(grid, 1, 1);
  GtkWidget *author_input = gtk_grid_get_child_at(grid, 1, 2);
  GtkWidget *price_input = gtk_grid_get_child_at(grid, 1, 3);
  GtkWidget *quantity_input = gtk_grid_get_child_at(grid, 1, 4);

  const gchar *position_str = gtk_entry_get_text(GTK_ENTRY(position_input));
  const gchar *title = gtk_entry_get_text(GTK_ENTRY(title_input));
  const gchar *author = gtk_entry_get_text(GTK_ENTRY(author_input));
  const gchar *price = gtk_entry_get_text(GTK_ENTRY(price_input));
  const gchar *quantity_str = gtk_entry_get_text(GTK_ENTRY(quantity_input));

  int position = atoi(position_str);
  int quantity = atoi(quantity_str);
  double price_double = atof(price);

  GtkWidget *list_widget = gtk_grid_get_child_at(grid, 2, 0);
  gtk_container_foreach(GTK_CONTAINER(list_widget), (GtkCallback)gtk_widget_destroy, NULL);
  char *list_text = g_strdup_printf("position; title; author; price; quantity");
  GtkWidget *row = gtk_list_box_row_new();
  GtkWidget *label = gtk_label_new(list_text);
  gtk_container_add(GTK_CONTAINER(row), label);
  gtk_list_box_insert(GTK_LIST_BOX(list_widget), row, -1);
  g_free(list_text);
  ksiazka *current = poczatek;
  int count = 1;
  while(count <= ilosc_ksiazek())
  {
    if (position == count || !strcmp(title, current->tytul) || !strcmp(author, current->autor) || price_double == current->cena || quantity == current->ilosc)
    {
      char *list_text = g_strdup_printf("%d; %s; %s; %.2f; %d", count, current->tytul, current->autor, current->cena, current->ilosc);
      GtkWidget *row = gtk_list_box_row_new();
      GtkWidget *label = gtk_label_new(list_text);
      gtk_container_add(GTK_CONTAINER(row), label);
      gtk_list_box_insert(GTK_LIST_BOX(list_widget), row, -1);
      g_free(list_text);
    }
    count++;
    current = current->nastepny;
  }

  gtk_widget_show_all(list_widget);
}
void on_search_button_clicked(GtkWidget *widget, gpointer data) {
  GtkWidget *search_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(search_window), "Search books");
  gtk_window_set_default_size(GTK_WINDOW(search_window), 800, 600);

  GtkWidget *grid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
  gtk_grid_set_column_spacing(GTK_GRID(grid), 10);

  GtkWidget *position_label = gtk_label_new("Position:");
  GtkWidget *position_input = gtk_entry_new();
  GtkWidget *title_label = gtk_label_new("Title:");
  GtkWidget *title_input = gtk_entry_new();
  GtkWidget *author_label = gtk_label_new("Author:");
  GtkWidget *author_input = gtk_entry_new();
  GtkWidget *price_label = gtk_label_new("Price:");
  GtkWidget *price_input = gtk_entry_new();
  GtkWidget *quantity_label = gtk_label_new("Quantity:");
  GtkWidget *quantity_input = gtk_entry_new();

  gtk_grid_attach(GTK_GRID(grid), position_label, 0, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), position_input, 1, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), title_label, 0, 1, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), title_input, 1, 1, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), author_label, 0, 2, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), author_input, 1, 2, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), price_label, 0, 3, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), price_input, 1, 3, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), quantity_label, 0, 4, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), quantity_input, 1, 4, 1, 1);

  GtkWidget *list = gtk_list_box_new();

  
  gtk_grid_attach(GTK_GRID(grid), list, 2, 0, 1, 5);

  GtkWidget *confirm_button = gtk_button_new_with_label("Confirm");
  g_signal_connect(confirm_button, "clicked", G_CALLBACK(on_search_confirm_button_clicked), grid);

  gtk_grid_attach(GTK_GRID(grid), confirm_button, 0, 5, 2, 1);


  gtk_container_add(GTK_CONTAINER(search_window), grid);

  gtk_widget_show_all(search_window);
  }


int main(int argc, char *argv[]) {
  gtk_init(&argc, &argv);
  
  GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "Library");
  gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  
  GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  gtk_container_add(GTK_CONTAINER(window), vbox);
  if (access("lista.txt", F_OK) == -1)
  {
    dodajKsiazke("The Hagwood Books", "Robin Jarvis", 25.99, 15);
  }
  else{
    read_from_file("lista.txt");
  }
  tree_view = gtk_tree_view_new();
  

  GtkCellRenderer *position_renderer = gtk_cell_renderer_text_new();
  GtkTreeViewColumn *position_column = gtk_tree_view_column_new_with_attributes("Position", position_renderer, "text", 0, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), position_column);

  GtkCellRenderer *title_renderer = gtk_cell_renderer_text_new();
  GtkTreeViewColumn *title_column = gtk_tree_view_column_new_with_attributes("Title", title_renderer, "text", 1, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), title_column);

  GtkCellRenderer *author_renderer = gtk_cell_renderer_text_new();
  GtkTreeViewColumn *author_column = gtk_tree_view_column_new_with_attributes("Author", author_renderer, "text", 2, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), author_column);
  GtkCellRenderer *price_renderer = gtk_cell_renderer_text_new();
  GtkTreeViewColumn *price_column = gtk_tree_view_column_new_with_attributes("Price", price_renderer, "text", 3, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), price_column);
  
  GtkCellRenderer *quantity_renderer = gtk_cell_renderer_text_new();
  GtkTreeViewColumn *quantity_column = gtk_tree_view_column_new_with_attributes("Quantity", quantity_renderer, "text", 4, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), quantity_column);
  
  insert_data_into_tree_view(tree_view);
  gtk_widget_set_hexpand(tree_view, TRUE);
  gtk_widget_set_vexpand(tree_view, TRUE);

  GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  GtkWidget *search_button = gtk_button_new_with_label("Search");
  GtkWidget *add_button = gtk_button_new_with_label("Add");
  GtkWidget *delete_button = gtk_button_new_with_label("Delete");
  GtkWidget *read_button = gtk_button_new_with_label("Read");
  GtkWidget *write_button = gtk_button_new_with_label("Write");
  GtkWidget *exit_button = gtk_button_new_with_label("Exit");
  
  gtk_box_pack_start(GTK_BOX(hbox), search_button, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), add_button, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), delete_button, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), read_button, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), write_button, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), exit_button, TRUE, TRUE, 0);
  g_signal_connect(exit_button, "clicked", G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(read_button, "clicked", G_CALLBACK(on_read_button_clicked), NULL);
  g_signal_connect(write_button, "clicked", G_CALLBACK(on_write_button_clicked), NULL);
  g_signal_connect(add_button, "clicked", G_CALLBACK(on_add_button_clicked), NULL);
  g_signal_connect(delete_button, "clicked", G_CALLBACK(on_delete_button_clicked), NULL);
  g_signal_connect(search_button, "clicked", G_CALLBACK(on_search_button_clicked), NULL);

  g_signal_connect(tree_view, "row-activated", G_CALLBACK(on_row_activated), NULL);

  GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy ((GtkScrolledWindow*)scrolled_window, GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  

  gtk_widget_set_size_request(scrolled_window, -1, 300);

  gtk_container_add(GTK_CONTAINER(scrolled_window), tree_view);

  gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);

  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

  gtk_widget_show_all(window);
  
  gtk_main();
  
  return 0;
}